/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:GPL2+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

#ifndef PANELINTERFACE_H
#define PANELINTERFACE_H

#include <QtCore/QObject>

#include "panelview.h"

class PanelInterface : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString formFactor READ formFactor WRITE setFormFactor)
    Q_PROPERTY(QString location READ location WRITE setLocation)
    Q_PROPERTY(bool maximized READ isMaximized WRITE setMaximized)
    Q_PROPERTY(QString alignment READ alignment WRITE setAlignment)
    Q_PROPERTY(int offset READ offset WRITE setOffset)
    Q_PROPERTY(int thickness READ thickness WRITE setThickness)
public:
    explicit PanelInterface(QScreen *screen, QObject *parent = 0);

    QString formFactor() const;
    void setFormFactor(const QString &value);

    bool isMaximized() const;
    void setMaximized(bool value);

    QString location() const;
    void setLocation(const QString &value);

    QString alignment() const;
    void setAlignment(const QString &value);

    int offset() const;
    void setOffset(int value);

    int thickness() const;
    void setThickness(int value);

public Q_SLOTS:
    void addElement(const QString &element);

private:
    PanelView *m_view;
};

#endif // PANELINTERFACE_H
