#pragma once

#include <QSize>
#include <QString>

// Ask Windows for a video's width and height. These are the same "Frame width"
// and "Frame height" values File Explorer shows under Properties > Details,
// so it's instant and doesn't need to open the video in the player.
// Returns an empty QSize if Windows doesn't know.
QSize videoResolution(const QString& path);
