#include "clips/video_info.h"

#include <QDir>
#include <string>

#include <windows.h>
#include <shobjidl.h>   // SHGetPropertyStoreFromParsingName: read a file's Windows properties
#include <propkey.h>    // PKEY_Video_FrameWidth / PKEY_Video_FrameHeight

QSize videoResolution(const QString& path)
{
    QSize size;
    IPropertyStore* store = nullptr;
    const std::wstring nativePath = QDir::toNativeSeparators(path).toStdWString();

    if (SUCCEEDED(SHGetPropertyStoreFromParsingName(nativePath.c_str(), nullptr,
                                                    GPS_DEFAULT, IID_PPV_ARGS(&store)))) {
        PROPVARIANT width, height;
        PropVariantInit(&width);
        PropVariantInit(&height);

        if (SUCCEEDED(store->GetValue(PKEY_Video_FrameWidth, &width)) && width.vt == VT_UI4 &&
            SUCCEEDED(store->GetValue(PKEY_Video_FrameHeight, &height)) && height.vt == VT_UI4) {
            size = QSize(int(width.ulVal), int(height.ulVal));
        }

        PropVariantClear(&width);
        PropVariantClear(&height);
        store->Release();
    }
    return size;
}
