# Framewatch

A background game recorder for Apex Legends, built in C++ on Windows.

## Why this exists

Existing recorders force a choice that doesn't make sense: clip the last
2 minutes, or clip the last 20 minutes, or pay for a subscription, or watch ads.
None of them let you decide *after the fact* exactly how much footage to keep.

Framewatch records continuously into a rolling buffer while the game is open.
When something interesting happens, you save out exactly the slice you want —
any length, any time, no pre-decision required. Free, open source, no ads.

## Why I'm building it the hard way

I want to step away from web development and learn how programs actually
talk to the operating system: threads, screen capture APIs, video encoding,
process management. Native C++ on Windows is the most direct path to that
knowledge, even though the toolchain takes longer to set up.

## Project status

Phase 0 — toolchain validation. A Hello-Qt window appears. Foundation works.

### Roadmap

- [x] Phase 0 — Build system and Qt window working end-to-end
- [ ] Phase 1 — Detect Apex Legends process start and stop
- [ ] Phase 2 — Continuous screen capture with hardware encoding
- [ ] Phase 3 — Rolling buffer to disk, manual clip extraction
- [ ] Phase 4 — Kill feed and death detection via computer vision
- [ ] Phase 5 — Tray icon, settings UI, configurable hotkeys

## Anti-cheat policy

This project stays fully external to the game. No DLL injection, no memory
reads, no API hooks against `r5apex.exe`. Screen capture and computer vision
only. EAC will not flag external screen recording.

## Building

See [BUILD.md](BUILD.md) for the exact build and run procedure.
The short version:

1. Open "x64 Native Tools Command Prompt for VS"
2. `cd C:\dev\framewatch && code .`
3. F1 → `CMake: Configure`
4. F7 to build
5. Run `windeployqt.exe` once after first build (see BUILD.md)
6. F5 to run

## Platform support

Windows 10 (1809+) / Windows 11 only. Cross-platform support is not a goal —
the capture and encoder APIs this project targets are Windows-specific.

## License

TBD.