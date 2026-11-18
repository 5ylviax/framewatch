#pragma once

#include <QWidget>

#include "clips/clip_status.h"

// "Forward declarations": they tell the compiler these classes exist, so the
// pointers below compile. The full headers are only included in the .cpp,
// which keeps this header light for every file that includes it.
class QLabel;
class QListWidget;
class QListWidgetItem;
class QMediaPlayer;
class QPushButton;

// The "Clips" tab: lists every video in the Videos folder, filters them by
// resolution, and plays the one you select.
class ClipBrowser : public QWidget
{
    Q_OBJECT

public:
    explicit ClipBrowser(QWidget* parent = nullptr);

private:
    void scanVideosFolder();
    void showOnly(int height);              // 0 = show every clip
    void updateTotals();
    void playClip(QListWidgetItem* item);
    void togglePlayPause();
    void markCorrupted(QListWidgetItem* item, const QString& reason);
    void updateWarning(QListWidgetItem* item);
    void checkDecodeErrors();
    void flagWhilePlaying(QListWidgetItem* item, const QString& reason);

    // Widgets the functions above need to reach. Qt deletes them
    // automatically because they're children of this widget.
    QListWidget*  m_clipList  = nullptr;
    QLabel*       m_totals    = nullptr;
    QLabel*       m_warning   = nullptr;
    QLabel*       m_info      = nullptr;
    QPushButton*  m_playPause = nullptr;
    QMediaPlayer* m_player    = nullptr;

    ClipStatusStore m_statusStore;   // broken clips found in earlier sessions

    qint64 m_firstErrorAt = -1;      // playback position (ms) of the first decoding error, -1 = none yet
};
