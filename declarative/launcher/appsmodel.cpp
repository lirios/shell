/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2015 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusInterface>
#include <QtGui/QIcon>

#include <qt5xdg/xdgmenu.h>
#include <qt5xdg/xmlhelper.h>

#include "appsmodel.h"

Q_LOGGING_CATEGORY(APPSMODEL, "hawaii.qml.launcher.appsmodel")

class AppEntry
{
public:
    AppEntry() {}

    QString name;
    QString comment;
    QString filterInfo;
    QString iconName;
    QString desktopFile;

    static bool lessThan(AppEntry *e1, AppEntry *e2) {
        return e1->name < e2->name;
    }
};

AppsModel::AppsModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_nameFormat(NameOnly)
{
    refresh();
}

AppsModel::~AppsModel()
{
    qDeleteAll(m_list);
}

AppsModel::NameFormat AppsModel::appNameFormat() const
{
    return m_nameFormat;
}

void AppsModel::setAppNameFormat(NameFormat format)
{
    if (m_nameFormat == format)
        return;

    m_nameFormat = format;
    Q_EMIT appNameFormatChanged();

    refresh();
}

QString AppsModel::categoryFilter() const
{
    return m_categoryFilter;
}

void AppsModel::setCategoryFilter(const QString &filter)
{
    if (m_categoryFilter == filter)
        return;

    m_categoryFilter = filter;
    Q_EMIT categoryFilterChanged();

    refresh();
}

QHash<int, QByteArray> AppsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles.insert(Qt::DisplayRole, "display");
    roles.insert(Qt::DecorationRole, "decoration");
    roles.insert(NameRole, "name");
    roles.insert(CommentRole, "comment");
    roles.insert(IconNameRole, "iconName");
    return roles;
}

int AppsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_list.size();
}

QVariant AppsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    AppEntry *item = m_list.at(index.row());

    switch (role) {
    case Qt::DecorationRole:
        return QIcon::fromTheme(item->iconName);
    case Qt::DisplayRole:
    case NameRole:
        return item->name;
    case CommentRole:
        return item->comment;
    case IconNameRole:
        return item->iconName;
    case FilterInfoRole:
        return item->filterInfo;
    default:
        break;
    }

    return QVariant();
}

bool AppsModel::trigger(const QModelIndex &index)
{
    if (!index.isValid())
        return false;

    AppEntry *entry = m_list.at(index.row());
    if (!entry)
        return false;

    const QDBusConnection bus = QDBusConnection::sessionBus();
    QDBusInterface interface(QStringLiteral("org.hawaiios.Session"),
                             QStringLiteral("/ProcessLauncher"),
                             QStringLiteral("org.hawaiios.ProcessLauncher"),
                             bus);
    QDBusMessage msg = interface.call("launchDesktopFile", entry->desktopFile);
    bool ran = msg.arguments().at(0).toBool();

    if (ran)
        Q_EMIT appLaunched(entry->desktopFile);

    return ran;
}

void AppsModel::refresh()
{
    beginResetModel();

    Q_EMIT refreshing();

    qDeleteAll(m_list);
    m_list.clear();

    XdgMenu xdgMenu;
    xdgMenu.setLogDir("/tmp/");
    xdgMenu.setEnvironments(QStringList() << QStringLiteral("Hawaii") << QStringLiteral("X-Hawaii"));
    const QString menuFileName = XdgMenu::getMenuFileName();
    qCDebug(APPSMODEL) << "Menu file name:" << menuFileName;
    if (!xdgMenu.read(menuFileName)) {
        qCWarning(APPSMODEL,
                  "Failed to read menu \"%s\": %s", qPrintable(menuFileName),
                  qPrintable(xdgMenu.errorString()));
        return;
    }

    QDomElement xml = xdgMenu.xml().documentElement();

    DomElementIterator it(xml, QString());
    while (it.hasNext()) {
        QDomElement xml = it.next();

        if (xml.tagName() == QStringLiteral("Menu")) {
            if (m_categoryFilter.isEmpty()) {
                readMenu(xml);
            } else {
                QString name = xml.attribute(QStringLiteral("name"));
                if (name == QStringLiteral("Applications") || name == m_categoryFilter)
                    readMenu(xml);
            }
        }
    }

    qSort(m_list.begin(), m_list.end(), AppEntry::lessThan);

    endResetModel();

    Q_EMIT refreshed();
}

void AppsModel::readMenu(const QDomElement &xml)
{
    DomElementIterator it(xml, QString());
    while (it.hasNext()) {
        QDomElement xml = it.next();

        if (xml.tagName() == QStringLiteral("AppLink"))
            readAppLink(xml);
    }
}

void AppsModel::readAppLink(const QDomElement &xml)
{
    QString desktopFile = xml.attribute(QStringLiteral("desktopFile"));

    // Do not duplicate applications
    Q_FOREACH (AppEntry *entry, m_list) {
        if (entry->desktopFile == desktopFile)
            return;
    }

    AppEntry *entry = new AppEntry();

    QString name;
    if (!xml.attribute(QStringLiteral("title")).isEmpty())
        name = xml.attribute(QStringLiteral("title"));
    else
        name = xml.attribute(QStringLiteral("name"));
    QString genericName = xml.attribute(QStringLiteral("genericName"));

    switch (m_nameFormat) {
    case GenericNameOnly:
        entry->name = genericName;
        break;
    case NameAndGenericName:
        entry->name = QStringLiteral("%1 (%2)").arg(name).arg(genericName);
        break;
    case GenericNameAndName:
        entry->name = QStringLiteral("%1 (%2)").arg(genericName).arg(name);
        break;
    default:
        entry->name = name;
        break;
    }

    entry->comment = xml.attribute(QStringLiteral("comment"));
    entry->filterInfo = QStringLiteral("%1 %2").arg(entry->name).arg(entry->comment);
    entry->desktopFile = desktopFile;
    entry->iconName = xml.attribute(QStringLiteral("icon"));
    m_list.append(entry);
}

#include "moc_appsmodel.cpp"
