// SPDX-FileCopyrightText: 2021 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QtConcurrentRun>
#include <QDir>
#include <QFutureWatcher>
#include <QSettings>
#include <QStandardPaths>

#include "extensionsmodel.h"

Q_LOGGING_CATEGORY(lcExtensions, "liri.shell.extensions", QtInfoMsg)

/*
 * ExtensionItem
 */

ExtensionItem::ExtensionItem(QObject *parent)
    : QObject(parent)
{
}

QString ExtensionItem::name() const
{
    return m_name;
}

void ExtensionItem::setName(const QString &name)
{
    m_name = name;
}

QUrl ExtensionItem::url() const
{
    return m_url;
}

void ExtensionItem::setUrl(const QUrl &url)
{
    m_url = url;
}

quint32 ExtensionItem::order() const
{
    return m_order;
}

void ExtensionItem::setOrder(quint32 value)
{
    m_order = value;
}

/*
 * ExtensionsModel
 */

ExtensionsModel::ExtensionsModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_roleNames.insert(NameRole, "name");
    m_roleNames.insert(UrlRole, "url");
    m_roleNames.insert(OrderRole, "order");

    QtConcurrent::run(ExtensionsModel::populate, this);
}

ExtensionsModel::~ExtensionsModel()
{
    qDeleteAll(m_items);
}

QHash<int, QByteArray> ExtensionsModel::roleNames() const
{
    return m_roleNames;
}

int ExtensionsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_items.count();
}

QVariant ExtensionsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    ExtensionItem *item = m_items[index.row()];

    switch (role) {
    case NameRole:
        return item->name();
    case UrlRole:
        return item->url();
    case OrderRole:
        return item->order();
    default:
        break;
    }

    return QVariant();
}

void ExtensionsModel::sort(int column, Qt::SortOrder order)
{
    Q_UNUSED(column);

    std::sort(m_items.begin(), m_items.end(),
              [order](ExtensionItem *item1, ExtensionItem *item2) {
        if (order == Qt::AscendingOrder) {
            if (item1->order() == UINT32_MAX && item2->order() == UINT32_MAX)
                return item1->name().localeAwareCompare(item2->name()) < 0;
            return item1->order() < item2->order();
        } else {
            if (item1->order() == UINT32_MAX && item2->order() == UINT32_MAX)
                return item1->name().localeAwareCompare(item2->name()) > 0;
            return item1->order() > item2->order();
        }
    });
}

void ExtensionsModel::populate(ExtensionsModel *model)
{
    const auto directories =
            QStandardPaths::locateAll(QStandardPaths::GenericDataLocation,
                                      QStringLiteral("liri-shell/statusarea"),
                                      QStandardPaths::LocateDirectory);
    for (const auto &directory : directories) {
        const QDir dataDir(directory);
        auto entries = dataDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
        for (const auto &info : entries) {
            const QDir indicatorDir(info.absoluteFilePath());
            const QString metadataFileName = QStringLiteral("metadata.desktop");

            if (!indicatorDir.exists(metadataFileName))
                continue;

            QSettings metadata(indicatorDir.absoluteFilePath(metadataFileName), QSettings::IniFormat);
            metadata.beginGroup(QStringLiteral("Desktop Entry"));

            // Only services (which is our custom type)
            if (metadata.value(QStringLiteral("Type")).toString() != QStringLiteral("X-Liri-Service"))
                continue;

            // Check service type
            if (metadata.value(QStringLiteral("X-Liri-ServiceType")).toString() != QStringLiteral("StatusAreaExtension"))
                continue;

            // Don't add the same indicator twice, might happen when developing while
            // there is already a system-wide installation
            const auto name = metadata.value(QStringLiteral("X-Liri-PluginInfo-Name")).toString();
            if (model->m_extensionNames.contains(name)) {
                qCWarning(lcExtensions, "Skipping \"%s\" status area extension: duplicate name",
                          qPrintable(name));
                continue;
            } else {
                model->m_extensionNames.append(name);
            }

            const auto mainScriptPath = indicatorDir.absoluteFilePath(metadata.value(QStringLiteral("X-Liri-StatusAreaExtension-Script")).toString());
            QFileInfo mainScriptInfo(mainScriptPath);
            if (!mainScriptInfo.exists() || !mainScriptInfo.isFile()) {
                qCWarning(lcExtensions, "Skipping \"%s\" status area extension: path \"%s\" doesn't exist or it's not a file",
                          qPrintable(name), qPrintable(mainScriptPath));
                continue;
            }

            const auto order = qBound<quint32>(0, metadata.value(QStringLiteral("X-Liri-StatusAreaExtension-Order"), UINT32_MAX).toUInt(), UINT32_MAX);

            ExtensionItem *item = new ExtensionItem();
            item->setName(name);
            item->setUrl(QUrl::fromLocalFile(mainScriptPath));
            item->setOrder(order);
            item->moveToThread(model->thread());
            model->beginInsertRows(QModelIndex(), model->m_items.count(), model->m_items.count());
            model->m_items.append(item);
            model->endInsertRows();
        }
    }

    model->sort(0);
}
