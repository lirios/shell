/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#ifndef THEME_H
#define THEME_H

#include <QtCore/QObject>
#include <QtGui/QFont>
#include <QtGui/QGuiApplication>
#include <QtQml/QQmlEngine>

#include "themesettings.h"

class QUrl;

class Theme : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(QString controlsStyle READ controlsStyle NOTIFY controlsStyleChanged)
    Q_PROPERTY(QObject *palette READ palette NOTIFY paletteChanged)
    Q_PROPERTY(QFont defaultFont READ defaultFont NOTIFY defaultFontChanged)
    Q_PROPERTY(QFont smallestFont READ smallestFont NOTIFY smallestFontChanged)
    Q_PROPERTY(QFont monospaceFont READ monospaceFont NOTIFY monospaceFontChanged)
public:
    enum FontType {
        DefaultFont = 0,
        SmallestFont,
        MonospaceFont
    };

    Theme(QQmlEngine *engine, QObject *parent = 0);

    QString name() const;

    QString controlsStyle();

    QObject *palette();

    QFont defaultFont() const;
    QFont smallestFont() const;
    QFont monospaceFont() const;

    Q_INVOKABLE QSizeF mSize(const QFont &font = QGuiApplication::font());

Q_SIGNALS:
    void nameChanged();
    void controlsStyleChanged();
    void paletteChanged();
    void defaultFontChanged();
    void smallestFontChanged();
    void monospaceFontChanged();

private:
    QQmlEngine *m_engine;
    ThemeSettings m_themeSettings;
    QString m_controlsStyle;
    QObject *m_palette;
    QFont m_defaultFont;
    QFont m_smallestFont;
    QFont m_monospaceFont;

    QObject *createQmlObject(const QUrl &url, QQmlEngine *engine);
    QUrl resolveFileName(const QString &fileName);

    void loadControlsStyle(bool changed = true);
    void loadPalette(bool changed = true);
    void loadFont(const FontType &type, bool changed = true);

    void setDefaultFont(const QString &family, int pointSize, int weight, bool changed = true);
    void setSmallestFont(const QString &family, int pointSize, int weight, bool changed = true);
    void setMonospaceFont(const QString &family, int pointSize, int weight, bool changed = true);

private Q_SLOTS:
    void themePathChanged();
};

#endif // THEME_H
