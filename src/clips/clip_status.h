#pragma once

#include <QFileInfo>
#include <QJsonObject>
#include <QString>

// Remembers which clips are broken, in
//   C:\Users\<you>\AppData\Roaming\Framewatch\clip_status.json
// so a problem found once still shows up the next time Framewatch starts,
// without checking the clip again.
//
// Each entry also stores the file's size and modified time. If the file has
// changed since (for example, deleted and re-recorded with the same name),
// the old entry is ignored.
class ClipStatusStore
{
public:
    ClipStatusStore();   // loads the JSON file, if there is one

    // Why this clip was found broken, or an empty string if it wasn't
    // (or if the file has changed since it was checked)
    QString corruptionReason(const QFileInfo& file) const;

    // Remember that this clip is broken, and save to disk right away
    void markCorrupted(const QFileInfo& file, const QString& reason);

private:
    void save() const;

    QString     m_path;    // full path of clip_status.json
    QJsonObject m_clips;   // clip path -> { size, modified, status, reason }
};
