/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */


#include "xdgdirs.h"
#include <stdlib.h>
#include <QDir>
#include <QDebug>


// Helper functions prototypes
void fixBashShortcuts(QString &s);
QString xdgSingleDir(const QString &envVar, const QString &def, bool createDir);
QStringList xdgDirList(const QString &envVar, const QString &postfix);

/************************************************
 Helper func.
 ************************************************/
void fixBashShortcuts(QString &s)
{
    if (s.startsWith('~'))
        s = QString(getenv("HOME")) + (s).mid(1);
}


/************************************************
 Helper func.
 ************************************************/
QString xdgSingleDir(const QString &envVar, const QString &def, bool createDir)
{
#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
    QString s(getenv(envVar.toAscii()));
#else
    QString s(getenv(envVar.toLatin1()));
#endif

    if (!s.isEmpty())
        fixBashShortcuts(s);
    else
        s = QString("%1/%2").arg(getenv("HOME"), def);

    QDir d(s);
    if (createDir && !d.exists())
    {
        if (!d.mkpath("."))
            qWarning() << QString("Can't create %1 directory.").arg(d.absolutePath());
    }

    return d.absolutePath();
}


/************************************************
 Helper func.
 ************************************************/
QStringList xdgDirList(const QString &envVar, const QString &postfix)
{
#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
    QStringList dirs = QString(getenv(envVar.toAscii())).split(':', QString::SkipEmptyParts);
#else
    QStringList dirs = QString(getenv(envVar.toLatin1())).split(':', QString::SkipEmptyParts);
#endif
    for (QStringList::Iterator i=dirs.begin(); i!=dirs.end(); ++i)
    {
        fixBashShortcuts((*i));
        *i += postfix;
    }
    return dirs;
}

/************************************************

 ************************************************/
QString XdgDirs::userDir(XdgDirs::UserDirectory dir)
{
    // possible values for UserDirectory
    if (dir < 0 || dir > 7)
        return QString();

    QString folderName = userDirectoryString[dir];

    QString fallback;
    if (getenv("HOME") == NULL)
        return QString("/tmp");
    else if (dir == XdgDirs::Desktop)
        fallback = QString("%1/%2").arg(getenv("HOME")).arg("Desktop");
    else
        fallback = QString(getenv("HOME"));

    QString configDir(configHome());
    QFile configFile(configDir + "/user-dirs.dirs");
    if (!configFile.exists())
        return fallback;

    if (!configFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return fallback;

    QString userDirVar("XDG_" + folderName.toUpper() + "_DIR");
    QTextStream in(&configFile);
    QString line;
    while (!in.atEnd())
    {
        line = in.readLine();
        if (line.contains(userDirVar))
        {
            configFile.close();

            // get path between quotes
            line = line.section('"', 1, 1);
            line.replace("$HOME", "~");
            fixBashShortcuts(line);
            return line;
        }
    }

    configFile.close();
    return fallback;
}


/************************************************

 ************************************************/
bool XdgDirs::setUserDir(XdgDirs::UserDirectory dir, const QString& value, bool createDir)
{
    // possible values for UserDirectory
    if (dir < 0 || dir > 7)
        return false;

    if (!(value.startsWith("$HOME") || value.startsWith("~/") || value.startsWith(QString(getenv("HOME")))))
        return false;

    QString folderName = userDirectoryString[dir];

    QString configDir(configHome());
    QFile configFile(configDir + "/user-dirs.dirs");

    // create the file if doesn't exist and opens it
    if (!configFile.open(QIODevice::ReadWrite | QIODevice::Text))
        return false;

    QTextStream stream(&configFile);
    QVector<QString> lines;
    QString line;
    bool foundVar = false;
    while (!stream.atEnd())
    {
        line = stream.readLine();
        if (line.indexOf("XDG_" + folderName.toUpper() + "_DIR") == 0)
        {
            foundVar = true;
            QString path = line.section('"', 1, 1);
            line.replace(path, value);
            lines.append(line);
        }
        else if (line.indexOf("XDG_") == 0)
        {
            lines.append(line);
        }
    }

    stream.reset();
    configFile.resize(0);
    if (!foundVar)
        stream << QString("XDG_%1_DIR=\"%2\"\n").arg(folderName.toUpper()).arg(value);

    for (QVector<QString>::iterator i = lines.begin(); i != lines.end(); ++i)
        stream << *i << "\n";

    configFile.close();

    if (createDir) {
        QString path = QString(value).replace("$HOME", "~");
        fixBashShortcuts(path);
        QDir().mkpath(path);
    }

    return true;
}


/************************************************

 ************************************************/
QString XdgDirs::dataHome(bool createDir)
{
    return xdgSingleDir("XDG_DATA_HOME", ".local/share", createDir);
}


/************************************************

 ************************************************/
QString XdgDirs::configHome(bool createDir)
{
    return xdgSingleDir("XDG_CONFIG_HOME", ".config", createDir);
}


/************************************************

 ************************************************/
QStringList XdgDirs::dataDirs(const QString &postfix)
{
    QStringList dirs = xdgDirList("XDG_DATA_DIRS", postfix);
    if (dirs.isEmpty())
    {
        dirs << "/usr/local/share/" + postfix;
        dirs << "/usr/share/" + postfix;
    }

    return dirs;
}


/************************************************

 ************************************************/
QStringList XdgDirs::configDirs(const QString &postfix)
{
    QStringList dirs = xdgDirList("XDG_CONFIG_DIRS", postfix);
    if (dirs.isEmpty())
    {
        dirs << "/etc/xdg" << postfix;
    }

    return dirs;
}


/************************************************

 ************************************************/
QString XdgDirs::cacheHome(bool createDir)
{
    return xdgSingleDir("XDG_CACHE_HOME", ".cache", createDir);
}


/************************************************

 ************************************************/
QString XdgDirs::runtimeDir()
{
    QString result(getenv("XDG_RUNTIME_DIR"));
    fixBashShortcuts(result);
    return result;
}


/************************************************

 ************************************************/
QString XdgDirs::autostartHome(bool createDir)
{
    QDir dir(QString("%1/autostart").arg(configHome(createDir)));

    if (createDir && !dir.exists())
    {
        if (!dir.mkpath("."))
            qWarning() << QString("Can't create %1 directory.").arg(dir.absolutePath());
    }

    return dir.absolutePath();
}


/************************************************

 ************************************************/
QStringList XdgDirs::autostartDirs(const QString &postfix)
{
    QStringList dirs;
    foreach(QString dir, configDirs())
        dirs << QString("%1/autostart").arg(dir) + postfix;

    return dirs;
}
