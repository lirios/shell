/*  This file is part of the KDE Frameworks

    Copyright (C) 2013 Alex Merry <alex.merry@kdemail.net>
    Copyright (C) 2013 John Layt <jlayt@kde.org>
    Copyright (C) 2010 Michael Leupold <lemma@confuego.org>
    Copyright (C) 2009 Michael Pyne <mpyne@kde.org>
    Copyright (C) 2008 Albert Astals Cid <aacid@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef KFORMATPRIVATE_P_H
#define KFORMATPRIVATE_P_H

#include "kformat.h"

#include <QCoreApplication> // for Q_DECLARE_TR_FUNCTIONS

class KFormatPrivate : public QSharedData
{
    Q_DECLARE_TR_FUNCTIONS(KFormat)

public:

    explicit KFormatPrivate(const QLocale &locale);
    virtual ~KFormatPrivate();

    QString formatByteSize(double size,
                           int precision,
                           KFormat::BinaryUnitDialect dialect,
                           KFormat::BinarySizeUnits units) const;

    QString formatDuration(quint64 msecs,
                           KFormat::DurationFormatOptions options) const;

    QString formatDecimalDuration(quint64 msecs,
                                  int decimalPlaces) const;

    QString formatSpelloutDuration(quint64 msecs) const;

    QString formatRelativeDate(const QDate &date,
                               QLocale::FormatType format) const;

    QString formatRelativeDateTime(const QDateTime &dateTime,
                                   QLocale::FormatType format) const;

private:

    QLocale m_locale;
};

#endif // KFORMATPRIVATE_P_H
