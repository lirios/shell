/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2013-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 * Copyright (C) 2013 Marco Martin <mart@kde.org>
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

#ifndef PANELVIEW_H
#define PANELVIEW_H

#include <QtQml/QQmlPropertyMap>

#include <QtConfiguration/QConfiguration>

#include <HawaiiShell/Element>
#include <HawaiiShell/HawaiiShell>
#include <HawaiiShell/QuickView>

class QScreen;
class PanelSurface;
class ShellUi;

class PanelView : public Hawaii::Shell::QuickView
{
    Q_OBJECT
    Q_PROPERTY(Qt::Alignment alignment READ alignment WRITE setAlignment NOTIFY alignmentChanged)
    Q_PROPERTY(int offset READ offset WRITE setOffset NOTIFY offsetChanged)
    Q_PROPERTY(int thickness READ thickness WRITE setThickness NOTIFY thicknessChanged)
    Q_PROPERTY(int length READ length WRITE setLength NOTIFY lengthChanged)
    Q_PROPERTY(int minimumLength READ minimumLength WRITE setMinimumLength NOTIFY minimumLengthChanged)
    Q_PROPERTY(int maximumLength READ maximumLength WRITE setMaximumLength NOTIFY maximumLengthChanged)
    Q_PROPERTY(QStringList elements READ elements NOTIFY elementsChanged)
public:
    explicit PanelView(ShellUi *corona, QScreen *screen);
    virtual ~PanelView();

    Qt::Alignment alignment() const;
    void setAlignment(Qt::Alignment alignment);

    int offset() const;
    void setOffset(int value);

    int thickness() const;
    void setThickness(int value);

    int length() const;
    void setLength(int value);

    int minimumLength() const;
    void setMinimumLength(int value);

    int maximumLength() const;
    void setMaximumLength(int value);

    QStringList elements() const;

public Q_SLOTS:
    void addElement(const QString &name);

Q_SIGNALS:
    void alignmentChanged();
    void offsetChanged();
    void thicknessChanged();
    void lengthChanged();
    void minimumLengthChanged();
    void maximumLengthChanged();

    void elementsChanged();
    void elementAdded(Hawaii::Shell::Element *element);
    void elementRemoved();

private:
    Qt::Alignment m_alignment;
    int m_offset;
    int m_minimumLength;
    int m_maximumLength;
    QConfiguration *m_configuration;
    QQmlPropertyMap m_settings;
    QSet<QString> m_elementsSet;
    QList<Hawaii::Shell::Element *> m_elements;
    PanelSurface *m_surface;

    void setWindowType();
    void restore();

private Q_SLOTS:
    void dockPanel();
};

#endif // PANELVIEW_H
