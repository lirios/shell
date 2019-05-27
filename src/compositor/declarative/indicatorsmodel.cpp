/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * $BEGIN_LICENSE:GPL3+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
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

#include <QtConcurrent/QtConcurrentRun>
#include <QDir>
#include <QFutureWatcher>
#include <QSettings>
#include <QStandardPaths>

#include "declarative/indicatorsmodel.h"

/*
 * IndicatorItem
 */

IndicatorItem::IndicatorItem(QObject *parent)
    : QObject(parent)
{
}

QString IndicatorItem::name() const
{
    return m_name;
}

void IndicatorItem::setName(const QString &name)
{
    m_name = name;
}

QUrl IndicatorItem::url() const
{
    return m_url;
}

void IndicatorItem::setUrl(const QUrl &url)
{
    m_url = url;
}

/*
 * IndicatorsModel
 */

IndicatorsModel::IndicatorsModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_roleNames.insert(NameRole, QByteArrayLiteral("name"));
    m_roleNames.insert(UrlRole, QByteArrayLiteral("url"));

    QFutureWatcher<void> *watcher = new QFutureWatcher<void>(this);
    QFuture<void> future = QtConcurrent::run(IndicatorsModel::populate, this);
    watcher->setFuture(future);
    connect(watcher, SIGNAL(finished()), this, SLOT(modelReady()));
}

IndicatorsModel::~IndicatorsModel()
{
    qDeleteAll(m_items);
}

QHash<int, QByteArray> IndicatorsModel::roleNames() const
{
    return m_roleNames;
}

int IndicatorsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_items.count();
}

QVariant IndicatorsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    IndicatorItem *item = m_items[index.row()];

    switch (role) {
    case NameRole:
        return item->name();
    case UrlRole:
        return item->url();
    default:
        break;
    }

    return QVariant();
}

void IndicatorsModel::sort(int column, Qt::SortOrder order)
{
    Q_UNUSED(column);
    Q_UNUSED(order);
    std::sort(m_items.begin(), m_items.end(), IndicatorsModel::itemLessThan);
}

bool IndicatorsModel::itemLessThan(const IndicatorItem *i1, const IndicatorItem *i2)
{
    return i1->name() < i2->name();
}

void IndicatorsModel::populate(IndicatorsModel *model)
{
    QStringList directories =
            QStandardPaths::locateAll(
                QStandardPaths::GenericDataLocation,
                QStringLiteral("liri-shell/indicators"),
                QStandardPaths::LocateDirectory);
    for (const QString &directory : directories) {
        const QDir dataDir(directory);
        QFileInfoList entries = dataDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
        for (const QFileInfo &info : entries) {
            const QDir indicatorDir(info.absoluteFilePath());
            const QString metadataFileName = QLatin1String("metadata.desktop");

            if (!indicatorDir.exists(metadataFileName))
                continue;

            QSettings metadata(indicatorDir.absoluteFilePath(metadataFileName), QSettings::IniFormat);
            metadata.beginGroup(QLatin1String("Desktop Entry"));
            if (metadata.value(QLatin1String("Type")).toString() != QLatin1String("Service"))
                continue;

            // Don't add the same indicator twice, might happen when developing while
            // there is already a system-wide installation
            const QString name = metadata.value(QStringLiteral("X-Liri-PluginInfo-Name")).toString();
            if (model->m_indicatorNames.contains(name))
                continue;
            else
                model->m_indicatorNames.append(name);

            const QString mainScriptPath = indicatorDir.absoluteFilePath(metadata.value(QLatin1String("X-Liri-MainScript")).toString());
            if (!indicatorDir.exists(mainScriptPath))
                continue;

            IndicatorItem *item = new IndicatorItem();
            item->setName(info.baseName());
            item->setUrl(QUrl::fromLocalFile(mainScriptPath));
            item->moveToThread(model->thread());
            QMetaObject::invokeMethod(model, "addNewItem", Q_ARG(QObject*, qobject_cast<QObject*>(item)));
        }
    }
}

void IndicatorsModel::addNewItem(QObject *object)
{
    beginInsertRows(QModelIndex(), m_items.count(), m_items.count());
    IndicatorItem *item = qobject_cast<IndicatorItem *>(object);
    if (item)
        m_items.append(item);
    endInsertRows();
}

void IndicatorsModel::modelReady()
{
    beginResetModel();
    sort(0);
    endResetModel();

    Q_EMIT ready();
}
