// SPDX-FileCopyrightText: 2022 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
// SPDX-FileCopyrightText: 2008 Dmitry Suzdalev <dimsuz@gmail.com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <QtCore/QDebug>

#include "notificationsimageutils.h"

inline void copyLineRGB32(QRgb *dst, const char *src, int width)
{
    const char *end = src + width * 3;
    for (; src != end; ++dst, src += 3)
        * dst = qRgb(src[0], src[1], src[2]);
}

inline void copyLineARGB32(QRgb *dst, const char *src, int width)
{
    const char *end = src + width * 4;
    for (; src != end; ++dst, src += 4)
        * dst = qRgba(src[0], src[1], src[2], src[3]);
}

QImage decodeImageHint(const QDBusArgument &arg)
{
    int width, height, stride, hasAlpha, bitsPerSample, channels;
    QByteArray pixels;

    // Decode hint
    arg.beginStructure();
    arg >> width >> height >> stride >> hasAlpha
        >> bitsPerSample >> channels >> pixels;
    arg.endStructure();

    char *ptr, *end;

    // Sanity check
    if ((width <= 0) || (width >= 2048) || (height <= 0) ||
            (height >= 2048) || (stride <= 0)) {
        qWarning() << "Image hint is not valid!";
        return QImage();
    }

    QImage::Format format = QImage::Format_Invalid;
    void (*function)(QRgb *, const char *, int) = 0;
    if (bitsPerSample == 8) {
        if (channels == 4) {
            format = QImage::Format_ARGB32;
            function = copyLineARGB32;
        } else if (channels == 3) {
            format = QImage::Format_RGB32;
            function = copyLineRGB32;
        }
    }

    if (format == QImage::Format_Invalid) {
        qWarning() << "Unsupported image format received from hint (hasAlpha:"
                   << hasAlpha << "bitsPerSample:" << bitsPerSample
                   << "channels:" << channels << ")";
        return QImage();
    }

    QImage image(width, height, format);
    ptr = pixels.data();
    end = ptr + pixels.length();
    for (int y = 0; y < height; ++y, ptr += stride) {
        if (ptr + channels * width > end) {
            qWarning() << "Image data is incomplete. y:" << y << "height:" << height;
            break;
        }
        function((QRgb *)(void *)image.scanLine(y), ptr, width);
    }

    return image;
}
