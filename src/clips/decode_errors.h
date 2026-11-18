#pragma once

// Counts the errors FFmpeg reports while it decodes video.
//
// Qt's media player uses FFmpeg under the hood. When a clip has damaged data,
// FFmpeg logs errors like "missing picture in access unit" (the red lines in
// the console) but keeps playing, so QMediaPlayer never reports a problem.
// This hooks into FFmpeg's log to count those errors.
namespace DecodeErrors {

// Start counting. Call after a QMediaPlayer exists, because that's what loads
// FFmpeg. Returns false if FFmpeg couldn't be found (counting stays at 0).
bool install();

int  count();    // errors since the last reset()
void reset();    // call when a new clip starts loading

} // namespace DecodeErrors
