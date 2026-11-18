#include "clips/clip_browser.h"
#include "clips/decode_errors.h"
#include "clips/video_info.h"

#include <QAudioOutput>
#include <QButtonGroup>
#include <QDirIterator>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QLocale>
#include <QMediaPlayer>
#include <QPushButton>
#include <QStandardPaths>
#include <QStyle>
#include <QTime>
#include <QTimer>
#include <QUrl>
#include <QVBoxLayout>
#include <QVideoWidget>

namespace {

// Each list item stores hidden extra data in "roles".
constexpr int PathRole       = Qt::UserRole;
constexpr int ResolutionRole = Qt::UserRole + 1;
constexpr int SizeRole       = Qt::UserRole + 2;
constexpr int CorruptedRole  = Qt::UserRole + 3;   // why it's broken; empty = clip is fine

// How many FFmpeg errors while playing before a clip counts as damaged.
// Tested on real clips: healthy ones log 4 errors once at startup (Qt checking
// for hardware encoders), a damaged one logged 818 in its first 4 seconds.
constexpr int ErrorsBeforeFlagging = 20;

QString formatSize(qint64 bytes)
{
    return QLocale().formattedDataSize(bytes, 1, QLocale::DataSizeTraditionalFormat);
}

// Milliseconds -> "3:42", or "1:03:42" for an hour or more
QString formatTime(qint64 ms)
{
    return QTime(0, 0).addMSecs(int(ms)).toString(ms >= 3600000 ? "h:mm:ss" : "m:ss");
}

} // namespace


ClipBrowser::ClipBrowser(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QHBoxLayout(this);

    // ---- Left side: filter buttons, clip list, totals ----
    auto* leftSide = new QVBoxLayout;
    layout->addLayout(leftSide, 1);

    auto* filterRow = new QHBoxLayout;
    leftSide->addLayout(filterRow);

    // Each button's id is the frame height it keeps (0 = keep everything)
    auto* filters = new QButtonGroup(this);
    filters->addButton(new QPushButton("All"), 0);
    filters->addButton(new QPushButton("1080p"), 1080);
    filters->addButton(new QPushButton("4K"), 2160);
    for (QAbstractButton* button : filters->buttons()) {
        button->setCheckable(true);
        button->setFocusPolicy(Qt::NoFocus);
        filterRow->addWidget(button);
    }
    filters->button(0)->setChecked(true);

    m_clipList = new QListWidget;
    leftSide->addWidget(m_clipList);

    m_totals = new QLabel;
    leftSide->addWidget(m_totals);

    // ---- Right side: video, clip details, play/pause ----
    auto* rightSide = new QVBoxLayout;
    layout->addLayout(rightSide, 3);

    auto* video = new QVideoWidget;
    rightSide->addWidget(video, 1);

    // Red warning banner, only visible when the selected clip is corrupted
    m_warning = new QLabel;
    m_warning->setWordWrap(true);
    m_warning->setStyleSheet("background-color: #b3261e; color: white; font-weight: bold;"
                             "padding: 8px; border-radius: 4px;");
    m_warning->hide();
    rightSide->addWidget(m_warning);

    m_info = new QLabel("Select a clip");
    rightSide->addWidget(m_info);

    m_playPause = new QPushButton("Play");
    m_playPause->setFocusPolicy(Qt::NoFocus);
    rightSide->addWidget(m_playPause);

    m_player = new QMediaPlayer(this);
    auto* audio = new QAudioOutput(this);
    m_player->setVideoOutput(video);
    m_player->setAudioOutput(audio);

    // ---- Connections: signal -> the member function that handles it ----
    connect(m_clipList, &QListWidget::currentItemChanged, this, &ClipBrowser::playClip);
    connect(filters, &QButtonGroup::idClicked, this, &ClipBrowser::showOnly);
    connect(m_playPause, &QPushButton::clicked, this, &ClipBrowser::togglePlayPause);
    connect(m_player, &QMediaPlayer::playbackStateChanged, this,
            [this](QMediaPlayer::PlaybackState state) {
        m_playPause->setText(state == QMediaPlayer::PlayingState ? "Pause" : "Play");
    });

    // The player couldn't open or decode the clip: flag it as corrupted
    connect(m_player, &QMediaPlayer::errorOccurred, this,
            [this](QMediaPlayer::Error, const QString& errorString) {
        QListWidgetItem* item = m_clipList->currentItem();
        // Only blame the clip if it's still the one the player was loading
        if (!item || item->data(PathRole).toString() != m_player->source().toLocalFile())
            return;
        flagWhilePlaying(item, "This video can't be played: " + errorString);
    });

    // The player keeps going through damaged data without reporting an error,
    // so count FFmpeg's decoding errors ourselves and check them twice a second
    DecodeErrors::install();
    auto* errorCheck = new QTimer(this);
    connect(errorCheck, &QTimer::timeout, this, &ClipBrowser::checkDecodeErrors);
    errorCheck->start(500);

    scanVideosFolder();
}

void ClipBrowser::scanVideosFolder()
{
    // C:\Users\<you>\Videos - QStandardPaths finds it for whoever runs the app
    const QString videosDir = QStandardPaths::writableLocation(QStandardPaths::MoviesLocation);

    QDirIterator it(videosDir,
                    {"*.mp4", "*.mkv", "*.mov", "*.webm"},
                    QDir::Files,
                    QDirIterator::Subdirectories);

    while (it.hasNext()) {
        const QFileInfo file = it.nextFileInfo();
        const QString game = file.dir().dirName();

        const QSize resolution = videoResolution(file.absoluteFilePath());

        auto* item = new QListWidgetItem(game + "  |  " + file.fileName());
        item->setData(PathRole, file.absoluteFilePath());
        item->setData(ResolutionRole, resolution);
        item->setData(SizeRole, file.size());
        m_clipList->addItem(item);

        // First: was this clip found broken in an earlier session?
        QString problem = m_statusStore.corruptionReason(file);

        // If not, run the quick checks. A healthy video always has a resolution
        // Windows can read. If it doesn't, the file is empty, not really a video,
        // or a recording that got cut off (the recorder writes the video's
        // header last, so a crash loses it).
        if (problem.isEmpty() && file.size() == 0)
            problem = "This file is empty (0 bytes). The recording probably failed.";
        else if (problem.isEmpty() && resolution.isEmpty())
            problem = "Windows can't read this video's details. It may be incomplete or corrupted.";

        if (!problem.isEmpty())
            markCorrupted(item, problem);
    }
    m_clipList->sortItems();
    updateTotals();
}

void ClipBrowser::showOnly(int height)
{
    for (int i = 0; i < m_clipList->count(); ++i) {
        QListWidgetItem* item = m_clipList->item(i);
        const int clipHeight = item->data(ResolutionRole).toSize().height();
        item->setHidden(height != 0 && clipHeight != height);
    }
    updateTotals();
}

// Count the clips that are showing and add up how much space they take
void ClipBrowser::updateTotals()
{
    int shown = 0;
    qint64 bytes = 0;
    for (int i = 0; i < m_clipList->count(); ++i) {
        QListWidgetItem* item = m_clipList->item(i);
        if (item->isHidden()) continue;
        ++shown;
        bytes += item->data(SizeRole).toLongLong();
    }
    m_totals->setText(QString("%1 of %2 clips  |  %3")
                          .arg(shown).arg(m_clipList->count()).arg(formatSize(bytes)));
}

void ClipBrowser::playClip(QListWidgetItem* item)
{
    if (!item) return;

    const QFileInfo file(item->data(PathRole).toString());
    const QSize resolution = item->data(ResolutionRole).toSize();
    m_info->setText(file.fileName() + "   |   "
                    + QString("%1 x %2").arg(resolution.width()).arg(resolution.height()) + "   |   "
                    + formatSize(file.size()) + "   |   "
                    + file.lastModified().toString("yyyy-MM-dd hh:mm"));
    updateWarning(item);

    // Start counting decoding errors from zero for this clip
    DecodeErrors::reset();
    m_firstErrorAt = -1;

    m_player->setSource(QUrl::fromLocalFile(file.absoluteFilePath()));
    m_player->play();
}

// Runs twice a second: has FFmpeg hit enough errors to call this clip damaged?
void ClipBrowser::checkDecodeErrors()
{
    QListWidgetItem* item = m_clipList->currentItem();
    if (!item || !item->data(CorruptedRole).toString().isEmpty())
        return;   // nothing selected, or it's already flagged

    const int errors = DecodeErrors::count();
    if (errors == 0)
        return;

    if (m_firstErrorAt < 0)
        m_firstErrorAt = m_player->position();   // roughly where the damage starts

    if (errors < ErrorsBeforeFlagging)
        return;

    flagWhilePlaying(item, QString("Damaged video data around %1: the decoder reported %2 errors "
                                   "(missing or broken frames).")
                               .arg(formatTime(m_firstErrorAt)).arg(errors));
}

// A problem found while playing: show it now, and save it to clip_status.json
// so the clip is flagged next time without having to play it again
void ClipBrowser::flagWhilePlaying(QListWidgetItem* item, const QString& reason)
{
    markCorrupted(item, reason);
    updateWarning(item);
    m_statusStore.markCorrupted(QFileInfo(item->data(PathRole).toString()), reason);
}

// Put a warning sign next to the clip in the list, and remember why
void ClipBrowser::markCorrupted(QListWidgetItem* item, const QString& reason)
{
    item->setData(CorruptedRole, reason);
    item->setIcon(style()->standardIcon(QStyle::SP_MessageBoxWarning));
    item->setToolTip(reason);   // shows when you hover over the clip
}

// Show the red banner if the selected clip is corrupted, hide it if not
void ClipBrowser::updateWarning(QListWidgetItem* item)
{
    const QString reason = item ? item->data(CorruptedRole).toString() : QString();
    m_warning->setText("⚠  " + reason);   // ⚠ is the ⚠ symbol
    m_warning->setVisible(!reason.isEmpty());
}

void ClipBrowser::togglePlayPause()
{
    if (m_player->playbackState() == QMediaPlayer::PlayingState)
        m_player->pause();
    else
        m_player->play();
}
