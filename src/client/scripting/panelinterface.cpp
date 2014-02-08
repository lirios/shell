/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include "panelinterface.h"
#include "panelview.h"
#include "shellmanager.h"

static const struct {
    const char *string;
    Hawaii::Shell::Types::FormFactor value;
} formFactors[] = {
    { "plane", Hawaii::Shell::Types::Plane },
    { "horizontal", Hawaii::Shell::Types::Horizontal },
    { "vertical", Hawaii::Shell::Types::Vertical }
};

static const struct {
    const char *string;
    Hawaii::Shell::Types::Location value;
} locations[] = {
    { "floating", Hawaii::Shell::Types::Floating },
    { "desktop", Hawaii::Shell::Types::Desktop },
    { "fullscreen", Hawaii::Shell::Types::FullScreen },
    { "top", Hawaii::Shell::Types::TopEdge },
    { "bottom", Hawaii::Shell::Types::BottomEdge },
    { "left", Hawaii::Shell::Types::LeftEdge },
    { "right", Hawaii::Shell::Types::RightEdge }
};

static const struct {
    const char *string;
    Qt::Alignment value;
} alignments[] = {
    { "left", Qt::AlignLeft },
    { "center", Qt::AlignCenter },
    { "right", Qt::AlignRight }
};

PanelInterface::PanelInterface(QScreen *screen, QObject *parent)
    : QObject(parent)
{
    m_view = new PanelView(ShellManager::instance()->corona(), screen);
    m_view->show();

    ShellUi *corona = qobject_cast<ShellUi *>(m_view->corona());
    if (corona)
        corona->addPanelView(m_view);
}

QString PanelInterface::formFactor() const
{
    for (int i = 0; i < int(sizeof(formFactors) / sizeof(formFactors[0])); i++) {
        if (m_view->formFactor() == formFactors[i].value)
            return formFactors[i].string;
    }

    return QString();
}

void PanelInterface::setFormFactor(const QString &value)
{
    const QString lower = value.toLower();

    for (int i = 0; i < int(sizeof(formFactors) / sizeof(formFactors[0])); i++) {
        if (lower == formFactors[i].string)
            m_view->setFormFactor(formFactors[i].value);
    }
}

bool PanelInterface::isMaximized() const
{
    return m_view->isMaximized();
}

void PanelInterface::setMaximized(bool value)
{
    m_view->setMaximized(value);
}

QString PanelInterface::location() const
{
    for (int i = 0; i < int(sizeof(locations) / sizeof(locations[0])); i++) {
        if (m_view->location() == locations[i].value)
            return locations[i].string;
    }

    return QString();
}

void PanelInterface::setLocation(const QString &value)
{
    const QString lower = value.toLower();

    for (int i = 0; i < int(sizeof(locations) / sizeof(locations[0])); i++) {
        if (lower == locations[i].string)
            m_view->setLocation(locations[i].value);
    }
}

QString PanelInterface::alignment() const
{
    for (int i = 0; i < int(sizeof(alignments) / sizeof(alignments[0])); i++) {
        if (m_view->alignment() == alignments[i].value)
            return alignments[i].string;
    }

    return QString();
}

void PanelInterface::setAlignment(const QString &value)
{
    const QString lower = value.toLower();

    for (int i = 0; i < int(sizeof(alignments) / sizeof(alignments[0])); i++) {
        if (lower == alignments[i].string)
            m_view->setAlignment(alignments[i].value);
    }
}

int PanelInterface::offset() const
{
    return m_view->offset();
}

void PanelInterface::setOffset(int value)
{
    m_view->setOffset(value);
}

int PanelInterface::thickness() const
{
    return m_view->thickness();
}

void PanelInterface::setThickness(int value)
{
    m_view->setThickness(value);
}

void PanelInterface::addElement(const QString &element)
{
    m_view->addElement(element);
}

#include "moc_panelinterface.cpp"
