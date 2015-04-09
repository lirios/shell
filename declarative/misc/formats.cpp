/*
 * Copyright 2014  Bhushan Shah <bhush94@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#include "formats.h"

Formats::Formats(QObject* parent)
    : QObject(parent)
    , m_format()
{
}

QString Formats::formatByteSize(double size, int precision) const
{
    return m_format.formatByteSize(size, precision);
}

QString Formats::formatDuration(quint64 msecs, KFormat::DurationFormatOptions options) const
{
    return m_format.formatDuration(msecs, options);
}

QString Formats::formatDuration(quint64 msecs, int options) const
{
    return m_format.formatDuration(msecs, static_cast<KFormat::DurationFormatOptions>(options));
}

QString Formats::formatDecimalDuration(quint64 msecs, int decimalPlaces) const
{
    return m_format.formatDecimalDuration(msecs, decimalPlaces);
}

QString Formats::formatSpelloutDuration(quint64 msecs) const
{
    return m_format.formatSpelloutDuration(msecs);
}

QString Formats::formatRelativeDate(const QDate &date, QLocale::FormatType format) const
{
    return m_format.formatRelativeDate(date, format);
}

QString Formats::formatRelativeDateTime(const QDateTime &dateTime, QLocale::FormatType format) const
{
    return m_format.formatRelativeDateTime(dateTime, format);
}
