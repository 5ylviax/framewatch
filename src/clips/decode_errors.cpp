#include "clips/decode_errors.h"

#include <atomic>
#include <cstdarg>

#include <windows.h>
#include <tlhelp32.h>   // CreateToolhelp32Snapshot: list the DLLs loaded in this process

namespace {

// FFmpeg's log function types, from libavutil/log.h
using LogCallback      = void (*)(void* context, int level, const char* format, va_list args);
using SetLogCallbackFn = void (*)(LogCallback);

// FFmpeg log levels: lower numbers are more serious.
// 0 = panic, 8 = fatal, 16 = error, 24 = warning, 32 = info
constexpr int FFmpegLogError = 16;

// std::atomic because FFmpeg logs from its own decoding threads, while the
// UI thread reads the count. Plain int could give wrong numbers.
std::atomic<int> g_errorCount{0};
LogCallback      g_originalCallback = nullptr;

// FFmpeg calls this for every message it logs
void onFFmpegLog(void* context, int level, const char* format, va_list args)
{
    if (level <= FFmpegLogError)
        ++g_errorCount;

    // Pass it on, so the messages still show in the console like before
    if (g_originalCallback)
        g_originalCallback(context, level, format, args);
}

// Qt ships FFmpeg as avutil-NN.dll, where NN changes with each FFmpeg version.
// Look through the DLLs this process has loaded for one starting with "avutil-".
HMODULE findAvutil()
{
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());
    if (snapshot == INVALID_HANDLE_VALUE)
        return nullptr;

    MODULEENTRY32W module{};
    module.dwSize = sizeof(module);
    HMODULE found = nullptr;
    for (BOOL ok = Module32FirstW(snapshot, &module); ok; ok = Module32NextW(snapshot, &module)) {
        if (_wcsnicmp(module.szModule, L"avutil-", 7) == 0) {
            found = module.hModule;
            break;
        }
    }
    CloseHandle(snapshot);
    return found;
}

} // namespace


bool DecodeErrors::install()
{
    HMODULE avutil = findAvutil();
    if (!avutil)
        return false;

    // Look up FFmpeg's functions by name inside the DLL
    auto setCallback   = reinterpret_cast<SetLogCallbackFn>(GetProcAddress(avutil, "av_log_set_callback"));
    g_originalCallback = reinterpret_cast<LogCallback>(GetProcAddress(avutil, "av_log_default_callback"));
    if (!setCallback)
        return false;

    setCallback(&onFFmpegLog);
    return true;
}

int DecodeErrors::count()
{
    return g_errorCount.load();
}

void DecodeErrors::reset()
{
    g_errorCount = 0;
}
