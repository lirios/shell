/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 * Copyright (C) 2013 Marco Martin <mart@kde.org>
 * Copyright (C) 2014 Sebastian Kügler <sebas@kde.org>
 * Copyright (C) 2014 David Edmundson <davidedmunsdon@kde.org>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:LGPL2.1+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

#ifndef UNITS_H
#define UNITS_H

#include <QtCore/QObject>
#include <QtGui/QFont>
#include <QtGui/QGuiApplication>
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlPropertyMap>

class Units : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qreal devicePixelRatio READ devicePixelRatio NOTIFY devicePixelRatioChanged)
    Q_PROPERTY(qreal gridUnit READ gridUnit NOTIFY gridUnitChanged)
    Q_PROPERTY(QObject *iconSizes READ iconSizes NOTIFY iconSizesChanged)
    Q_PROPERTY(qreal smallSpacing READ smallSpacing NOTIFY spacingChanged)
    Q_PROPERTY(qreal largeSpacing READ largeSpacing NOTIFY spacingChanged)
    Q_PROPERTY(int shortDuration READ shortDuration NOTIFY durationChanged)
    Q_PROPERTY(int mediumDuration READ mediumDuration NOTIFY durationChanged)
    Q_PROPERTY(int longDuration READ longDuration NOTIFY durationChanged)
    Q_ENUMS(IconSize)
public:
    enum IconSize {
        IconSizeTiny = 8,
        IconSizeSmall = 16,
        IconSizeSmallMedium = 22,
        IconSizeMedium = 32,
        IconSizeLarge = 48,
        IconSizeHuge = 64,
        IconSizeEnormous = 128
    };

    Units(QObject *parent = 0);

    qreal devicePixelRatio() const;

    qreal gridUnit() const;

    Q_INVOKABLE qreal dp(qreal value) const;
    Q_INVOKABLE qreal gu(qreal value) const;

    QQmlPropertyMap *iconSizes() const;

    qreal smallSpacing() const;
    qreal largeSpacing() const;

    int shortDuration() const;
    int mediumDuration() const;
    int longDuration() const;

    Q_INVOKABLE int roundToIconSize(int size);

protected:
    bool eventFilter(QObject *watched, QEvent *event);

Q_SIGNALS:
    void devicePixelRatioChanged();
    void gridUnitChanged();
    void iconSizesChanged();
    void spacingChanged();
    void durationChanged();

private:
    qreal m_devicePixelRatio;
    qreal m_gridUnit;
    QQmlPropertyMap *m_iconSizes;
    qreal m_smallSpacing;
    qreal m_largeSpacing;
    int m_duration;

    int devicePixelIconSize(const int size) const;

    void updateDevicePixelRatio();
    void updateIconSizes();
    void updateSpacing();
};

#endif // UNITS_H
