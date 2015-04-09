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

#include <QtGui/QGuiApplication>
#include <QtGui/QFontMetrics>
#include <QtGui/QScreen>
#include <QDebug>

#include "units.h"

Units::Units(QObject *parent)
    : QObject(parent)
    , m_devicePixelRatio(-1)
    , m_gridUnit(-1)
    , m_iconSizes(new QQmlPropertyMap)
    , m_smallSpacing(-1)
    , m_largeSpacing(-1)
    , m_duration(400)
{
    updateDevicePixelRatio();
    updateIconSizes();
    updateSpacing();

    QCoreApplication::instance()->installEventFilter(this);
}

qreal Units::devicePixelRatio() const
{
    return m_devicePixelRatio;
}

qreal Units::gridUnit() const
{
    return m_gridUnit;
}

qreal Units::dp(qreal value) const
{
    return qRound(value * m_devicePixelRatio);
}

qreal Units::gu(qreal value) const
{
    return qRound(value * m_gridUnit);
}

QQmlPropertyMap *Units::iconSizes() const
{
    return m_iconSizes;
}

qreal Units::smallSpacing() const
{
    return m_smallSpacing;
}

qreal Units::largeSpacing() const
{
    return m_largeSpacing;
}

int Units::shortDuration() const
{
    return m_duration / 4;
}

int Units::mediumDuration() const
{
    return m_duration / 2;
}

int Units::longDuration() const
{
    return m_duration;
}

int Units::roundToIconSize(int size)
{
    if (size <= 0) {
        return 0;
    } else if (size < IconSizeTiny) {
        return IconSizeTiny;
    } else if (size < IconSizeSmall) {
        return IconSizeTiny;
    } else if (size < IconSizeSmallMedium) {
        return IconSizeSmall;
    } else if (size < IconSizeMedium) {
        return IconSizeSmallMedium;
    } else if (size < IconSizeLarge) {
        return IconSizeMedium;
    } else if (size < IconSizeHuge) {
        return IconSizeLarge;
    } else if (size < IconSizeEnormous) {
        return IconSizeHuge;
    }

    return size;
}

bool Units::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == QCoreApplication::instance()) {
        if (event->type() == QEvent::ApplicationFontChange || event->type() == QEvent::FontChange)
            updateSpacing();
    }

    return QObject::eventFilter(watched, event);
}

int Units::devicePixelIconSize(const int size) const
{
    if (m_devicePixelRatio < 1.5)
        return size;
    else if (m_devicePixelRatio < 2.0)
        return size * 1.5;
    else if (m_devicePixelRatio < 2.5)
        return size * 2.0;
    else if (m_devicePixelRatio < 3.0)
        return size * 3.0;
    return size * m_devicePixelRatio;
}

void Units::updateDevicePixelRatio()
{
    m_devicePixelRatio = (qreal)QGuiApplication::primaryScreen()->physicalDotsPerInchX() / (qreal)96;
    Q_EMIT devicePixelRatioChanged();
}

void Units::updateIconSizes()
{
    m_iconSizes->insert("tiny", devicePixelIconSize(IconSizeTiny));
    m_iconSizes->insert("small", devicePixelIconSize(IconSizeSmall));
    m_iconSizes->insert("smallMedium", devicePixelIconSize(IconSizeSmallMedium));
    m_iconSizes->insert("medium", devicePixelIconSize(IconSizeMedium));
    m_iconSizes->insert("large", devicePixelIconSize(IconSizeLarge));
    m_iconSizes->insert("huge", devicePixelIconSize(IconSizeHuge));
    m_iconSizes->insert("enormous", devicePixelIconSize(IconSizeEnormous));
    Q_EMIT iconSizesChanged();
}

void Units::updateSpacing()
{
    int gridUnit = QFontMetrics(QGuiApplication::font()).boundingRect("M").height();

    if (gridUnit % 2 != 0)
        gridUnit++;

    if (gridUnit != m_gridUnit) {
        m_gridUnit = gridUnit;
        Q_EMIT gridUnitChanged();
    }

    if (gridUnit != m_largeSpacing) {
        m_smallSpacing = qMax(2, (int)(gridUnit / 4)); // 1/4 of gridUnit, at least 2
        m_largeSpacing = gridUnit; // mSize.height
        Q_EMIT spacingChanged();
    }
}

#include "moc_units.cpp"
