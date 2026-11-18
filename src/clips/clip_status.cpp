#include "clips/clip_status.h"

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QSaveFile>
#include <QStandardPaths>

namespace {

// Saved in UTC so daylight-saving changes don't make a file look "changed"
QString modifiedTime(const QFileInfo& file)
{
    return file.lastModified().toUTC().toString(Qt::ISODateWithMs);
}

} // namespace


ClipStatusStore::ClipStatusStore()
{
    // C:\Users\<you>\AppData\Roaming\Framewatch
    const QString folder = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(folder);
    m_path = folder + "/clip_status.json";

    QFile file(m_path);
    if (file.open(QIODevice::ReadOnly))
        m_clips = QJsonDocument::fromJson(file.readAll()).object();
}

QString ClipStatusStore::corruptionReason(const QFileInfo& file) const
{
    const QJsonObject entry = m_clips.value(file.absoluteFilePath()).toObject();
    if (entry.isEmpty())
        return {};

    const bool sameFile = entry.value("size").toInteger() == file.size()
                       && entry.value("modified").toString() == modifiedTime(file);
    if (!sameFile)
        return {};

    return entry.value("reason").toString();
}

void ClipStatusStore::markCorrupted(const QFileInfo& file, const QString& reason)
{
    QJsonObject entry;
    entry["size"]     = file.size();
    entry["modified"] = modifiedTime(file);
    entry["status"]   = "corrupted";
    entry["reason"]   = reason;

    m_clips[file.absoluteFilePath()] = entry;
    save();
}

void ClipStatusStore::save() const
{
    // QSaveFile writes to a temporary file and only replaces the real one at
    // commit(), so a crash halfway through can't leave a half-written JSON file.
    QSaveFile file(m_path);
    if (!file.open(QIODevice::WriteOnly))
        return;
    file.write(QJsonDocument(m_clips).toJson());   // indented, easy to read
    file.commit();
}
