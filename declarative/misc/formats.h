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

#ifndef FORMATS_H
#define FORMATS_H

#include "kformat.h"

class Formats : public QObject
{
    Q_OBJECT

    public:

	explicit Formats (QObject* parent = 0);

	/**
	* Converts size from bytes to the appropriate string representation
	*/
	Q_INVOKABLE QString formatByteSize(double size, int precision = 1) const;

	/**
	 * Given a number of milliseconds, converts that to a string containing
	 * the localized equivalent, e.g. 1:23:45
	 */
    Q_INVOKABLE QString formatDuration(quint64 msecs, KFormat::DurationFormatOptions options = KFormat::DefaultDuration) const;

    Q_DECLARE_FLAGS(DurationFormatOptions, KFormat::DurationFormatOption)

    /**
     * This overload exists so it can be called from QML, which does
     * not support calling Q_INVOKABLEs with Q_ENUMS from different classes
     *
     * This is mentioned in the docs and also in https://bugreports.qt.io/browse/QTBUG-20639
     * Until that bug is fixed, we'll need this
     */
    Q_INVOKABLE QString formatDuration(quint64 msecs, int options) const;

	/**
	 * Given a number of milliseconds, converts that to a string containing
	 * the localized equivalent to the requested decimal places.
	 *
	 * e.g. given formatDuration(60000), returns "1.0 minutes"
	 */
	Q_INVOKABLE QString formatDecimalDuration(quint64 msecs, int decimalPlaces = 2) const;

	/**
	 * Given a number of milliseconds, converts that to a spell-out string containing
	 * the localized equivalent.
	 *
	 * e.g. given formatSpelloutDuration(60001) returns "1 minute"
	 *      given formatSpelloutDuration(62005) returns "1 minute and 2 seconds"
	 *      given formatSpelloutDuration(90060000) returns "1 day and 1 hour"
	 *
	 * Units not interesting to the user, for example seconds or minutes when the first
	 * unit is day, are not returned because they are irrelevant. The same applies for
	 * seconds when the first unit is hour.
	 *
	 */
	Q_INVOKABLE QString formatSpelloutDuration(quint64 msecs) const;

	/**
	 * Returns a string formatted to a relative date style.
	 *
	 * If the date falls within one week before or after the current date
	 * then a relative date string will be returned, such as:
	 * * Yesterday
	 * * Today
	 * * Tomorrow
	 * * Last Tuesday
	 * * Next Wednesday
	 *
	 * If the date falls outside this period then the format is used
	 */
	Q_INVOKABLE QString formatRelativeDate(const QDate &date, QLocale::FormatType format) const;

	/**
	 * Returns a string formatted to a relative datetime style.
	 *
	 * If the dateTime falls within one week before or after the current date
	 * then a relative date string will be returned, such as:
	 * * Yesterday, 3:00pm
	 * * Today, 3:00pm
	 * * Tomorrow, 3:00pm
	 * * Last Tuesday, 3:00pm
	 * * Next Wednesday, 3:00pm
	 *
	 * If the datetime falls outside this period then the format is used
	 */
	Q_INVOKABLE QString formatRelativeDateTime(const QDateTime &dateTime, QLocale::FormatType format) const;

    private:

	KFormat m_format;
};

Q_DECLARE_METATYPE(QLocale::FormatType)

#endif
