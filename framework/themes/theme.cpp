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

#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QUrl>
#include <QtCore/QVariant>
#include <QtCore/QSettings>
#include <QtGui/QFontMetrics>
#include <QtQml/QQmlComponent>
#include <QtQml/QQmlContext>

#include "theme.h"

#define THEME_FILE_FORMAT QStringLiteral("%1/theme.ini")
#define THEME_CONTROLS_STYLE_KEY QStringLiteral("ControlsStyle")
#define THEME_FONT_FAMILY_KEY QStringLiteral("Family")
#define THEME_FONT_POINT_SIZE_KEY QStringLiteral("PointSize")
#define THEME_FONT_WEIGHT_KEY QStringLiteral("Weight")

#define CONTROLS_STYLE QStringLiteral("Wind")

#define DEFAULT_FONT_FAMILY QStringLiteral("Droid Sans")
#define DEFAULT_FONT_POINT_SIZE 11
#define DEFAULT_FONT_WEIGHT QFont::Light

#define SMALLEST_FONT_FAMILY QStringLiteral("Droid Sans")
#define SMALLEST_FONT_POINT_SIZE 8
#define SMALLEST_FONT_WEIGHT QFont::Normal

#define MONOSPACE_FONT_FAMILY QStringLiteral("Droid Sans Mono")
#define MONOSPACE_FONT_POINT_SIZE 10
#define MONOSPACE_FONT_WEIGHT QFont::Normal

Theme::Theme(QQmlEngine *engine, QObject *parent)
    : QObject(parent)
    , m_engine(engine)
    , m_palette(Q_NULLPTR)
{
    // Set default fonts
    setDefaultFont(DEFAULT_FONT_FAMILY, DEFAULT_FONT_POINT_SIZE,
                   DEFAULT_FONT_WEIGHT, false);
    setSmallestFont(SMALLEST_FONT_FAMILY, SMALLEST_FONT_POINT_SIZE,
                    SMALLEST_FONT_WEIGHT, false);
    setMonospaceFont(MONOSPACE_FONT_FAMILY, MONOSPACE_FONT_POINT_SIZE,
                     MONOSPACE_FONT_WEIGHT, false);

    // Receive theme name and path changes
    connect(&m_themeSettings, &ThemeSettings::nameChanged,
            this, &Theme::nameChanged,
            Qt::UniqueConnection);
    connect(&m_themeSettings, &ThemeSettings::pathChanged,
            this, &Theme::pathChanged);
    connect(&m_themeSettings, &ThemeSettings::pathChanged,
            this, &Theme::themePathChanged);
}

QString Theme::name() const
{
    return m_themeSettings.name();
}

QString Theme::path() const
{
    return m_themeSettings.path();
}

QString Theme::controlsStyle()
{
    if (m_controlsStyle.isEmpty())
        loadControlsStyle(false);
    return m_controlsStyle;
}

QObject *Theme::palette()
{
    if (!m_palette)
        loadPalette(false);
    return m_palette;
}

QFont Theme::defaultFont() const
{
    return m_defaultFont;
}

QFont Theme::smallestFont() const
{
    return m_smallestFont;
}

QFont Theme::monospaceFont() const
{
    return m_monospaceFont;
}

QSizeF Theme::mSize(const QFont &font)
{
    return QFontMetrics(font).boundingRect("M").size();
}

QQmlComponent *Theme::createStyleComponent(const QString &fileName, QObject *parent)
{
    QUrl url = QUrl::fromLocalFile(QDir(m_themeSettings.path()).absoluteFilePath(fileName));
    QQmlComponent *component = new QQmlComponent(m_engine, url,
                                                 QQmlComponent::PreferSynchronous, parent);
    if (component->status() != QQmlComponent::Ready) {
        qWarning("Unable to load style \"%s\" from theme \"%s\": %s",
                 qPrintable(fileName), qPrintable(m_themeSettings.path()),
                 qPrintable(component->errorString()));
        return Q_NULLPTR;
    }

    return component;
}

QObject *Theme::createQmlObject(const QUrl &url, QQmlEngine *engine)
{
    QQmlComponent *component = new QQmlComponent(engine, url, QQmlComponent::PreferSynchronous);
    if (component->status() != QQmlComponent::Ready)
        qWarning("Unable to load component from \"%s\": %s",
                 qPrintable(url.toString()),
                 qPrintable(component->errorString()));
    QObject *object = component->create();
    delete component;
    return object;
}

QUrl Theme::resolveFileName(const QString &fileName)
{
    QDir dir(m_themeSettings.path());
    if (QFile::exists(dir.absoluteFilePath(fileName)))
        return QUrl::fromLocalFile(dir.absoluteFilePath(fileName));

    return QUrl();
}

void Theme::loadControlsStyle(bool changed)
{
    QSettings settings(THEME_FILE_FORMAT.arg(m_themeSettings.path()), QSettings::IniFormat);
    settings.beginGroup(QStringLiteral("General"));
    m_controlsStyle = settings.value(THEME_CONTROLS_STYLE_KEY, QVariant(CONTROLS_STYLE)).toString();
    if (changed)
        Q_EMIT controlsStyleChanged();

    // FIXME: This env var need to be set when the shell start not like this!
    //qputenv("QT_QUICK_CONTROLS_STYLE", m_controlsStyle.toUtf8());
}

void Theme::loadPalette(bool changed)
{
    // Delete previously loaded palette
    delete m_palette;

    // Create a new object
    m_palette = createQmlObject(resolveFileName("Palette.qml"), m_engine);
    if (changed)
        Q_EMIT paletteChanged();
}

void Theme::loadFont(const FontType &type, bool changed)
{
    QString family;
    int pointSize;
    int weight;

    QSettings settings(THEME_FILE_FORMAT.arg(m_themeSettings.path()), QSettings::IniFormat);

    switch (type) {
    case Theme::DefaultFont:
        settings.beginGroup(QStringLiteral("DefaultFont"));
        family = settings.value(THEME_FONT_FAMILY_KEY, QVariant(DEFAULT_FONT_FAMILY)).toString();
        pointSize = settings.value(THEME_FONT_POINT_SIZE_KEY, QVariant(DEFAULT_FONT_POINT_SIZE)).toInt();
        weight = settings.value(THEME_FONT_WEIGHT_KEY, QVariant(DEFAULT_FONT_WEIGHT)).toInt();
        setDefaultFont(family, pointSize, weight, changed);
        break;
    case Theme::SmallestFont:
        settings.beginGroup(QStringLiteral("SmallestFont"));
        family = settings.value(THEME_FONT_FAMILY_KEY, QVariant(SMALLEST_FONT_FAMILY)).toString();
        pointSize = settings.value(THEME_FONT_POINT_SIZE_KEY, QVariant(SMALLEST_FONT_POINT_SIZE)).toInt();
        weight = settings.value(THEME_FONT_WEIGHT_KEY, QVariant(SMALLEST_FONT_WEIGHT)).toInt();
        setSmallestFont(family, pointSize, weight, changed);
        break;
    case Theme::MonospaceFont:
        settings.beginGroup(QStringLiteral("MonospaceFont"));
        family = settings.value(THEME_FONT_FAMILY_KEY, QVariant(MONOSPACE_FONT_FAMILY)).toString();
        pointSize = settings.value(THEME_FONT_POINT_SIZE_KEY, QVariant(MONOSPACE_FONT_POINT_SIZE)).toInt();
        weight = settings.value(THEME_FONT_WEIGHT_KEY, QVariant(MONOSPACE_FONT_WEIGHT)).toInt();
        setMonospaceFont(family, pointSize, weight, changed);
        break;
    default:
        break;
    }
}

void Theme::setDefaultFont(const QString &family, int pointSize, int weight, bool changed)
{
    m_defaultFont.setFamily(family);
    m_defaultFont.setPointSize(pointSize);
    m_defaultFont.setWeight(weight);
    QGuiApplication::setFont(m_defaultFont);
    if (changed)
        Q_EMIT defaultFontChanged();
}

void Theme::setSmallestFont(const QString &family, int pointSize, int weight, bool changed)
{
    m_smallestFont.setFamily(family);
    m_smallestFont.setPointSize(pointSize);
    m_smallestFont.setWeight(weight);
    if (changed)
        Q_EMIT smallestFontChanged();
}

void Theme::setMonospaceFont(const QString &family, int pointSize, int weight, bool changed)
{
    m_monospaceFont.setFamily(family);
    m_monospaceFont.setPointSize(pointSize);
    m_monospaceFont.setWeight(weight);
    if (changed)
        Q_EMIT monospaceFontChanged();
}

void Theme::themePathChanged()
{
    // Reload settings
    loadControlsStyle();
    loadPalette();
    loadFont(Theme::DefaultFont);
    loadFont(Theme::SmallestFont);
    loadFont(Theme::MonospaceFont);
}

#include "moc_theme.cpp"
