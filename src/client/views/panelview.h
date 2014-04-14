/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2013-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 * Copyright (C) 2013 Marco Martin <mart@kde.org>
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

#ifndef PANELVIEW_H
#define PANELVIEW_H

#include <QtCore/QPointer>

#include <QtConfiguration/QStaticConfiguration>

#include <HawaiiShell/Element>
#include <HawaiiShell/HawaiiShell>
#include <HawaiiShell/QuickView>

class QScreen;
class PanelConfigView;
class PanelSurface;
class ShellUi;

class PanelView : public Hawaii::QuickView
{
    Q_OBJECT
    Q_PROPERTY(bool maximized READ isMaximized WRITE setMaximized NOTIFY maximizedChanged)
    Q_PROPERTY(Qt::Alignment alignment READ alignment WRITE setAlignment NOTIFY alignmentChanged)
    Q_PROPERTY(int offset READ offset WRITE setOffset NOTIFY offsetChanged)
    Q_PROPERTY(int thickness READ thickness WRITE setThickness NOTIFY thicknessChanged)
    Q_PROPERTY(int length READ length WRITE setLength NOTIFY lengthChanged)
    Q_PROPERTY(int minimumLength READ minimumLength WRITE setMinimumLength NOTIFY minimumLengthChanged)
    Q_PROPERTY(int maximumLength READ maximumLength WRITE setMaximumLength NOTIFY maximumLengthChanged)
public:
    explicit PanelView(ShellUi *mantle, QScreen *screen);
    virtual ~PanelView();

    bool isMaximized() const;
    void setMaximized(bool value);

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

protected:
    void resizeEvent(QResizeEvent *event);

    void showConfigurationWindow();
    void hideConfigurationWindow();

Q_SIGNALS:
    void maximizedChanged();
    void alignmentChanged();
    void offsetChanged();
    void thicknessChanged();
    void lengthChanged();
    void minimumLengthChanged();
    void maximumLengthChanged();

private:
    QScreen *m_screen;
    int m_screenNumber;
    QRect m_screenGeometry;

    int m_panelNum;

    bool m_maximized;
    Qt::Alignment m_alignment;
    int m_offset;
    int m_thickness;
    int m_length;
    int m_minimumLength;
    int m_maximumLength;
    QStaticConfiguration *m_settings;
    PanelSurface *m_surface;
    QPointer<PanelConfigView> m_configView;

    QString settingsCategory() const;

    void loadSettings();
    void saveSettings();

    void setWindowType();

    void updateCurrentScreen();

    void applySizeChanges();
    void recalculateLength();

private Q_SLOTS:
    void containmentLocationChanged();
    void containmentImmutableChanged();

    void dockPanel();
};

#endif // PANELVIEW_H
