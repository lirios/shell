#include "frequentmodel.h"

#include <QtCore/QDebug>
#include <QtCore/QFileInfo>

#include "appsmodel.h"
#include "usagetracker.h"

QString appIdFromDesktopFile(const QString &desktopFile)
{
    return QFileInfo(desktopFile).completeBaseName();
}

FrequentAppsModel::FrequentAppsModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    sort(0, Qt::DescendingOrder);

    connect(UsageTracker::instance(), &UsageTracker::updated, this, &FrequentAppsModel::invalidate);
}

bool FrequentAppsModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex sourceIndex = sourceModel()->index(sourceRow, 0, sourceParent);
    QString appId = sourceModel()->data(sourceIndex, AppsModel::AppIdRole).toString();

    AppUsage *app = UsageTracker::instance()->usageForAppId(appId);

    return app != nullptr && app->score > 0;
}

bool FrequentAppsModel::lessThan(const QModelIndex &source_left,
                                 const QModelIndex &source_right) const
{
    QString leftAppId = sourceModel()->data(source_left, AppsModel::AppIdRole).toString();
    QString rightAppId = sourceModel()->data(source_right, AppsModel::AppIdRole).toString();

    AppUsage *leftApp = UsageTracker::instance()->usageForAppId(leftAppId);
    AppUsage *rightApp = UsageTracker::instance()->usageForAppId(rightAppId);

    if (leftApp == nullptr) {
        return false;
    } else if (rightApp == nullptr) {
        return true;
    } else {
        return leftApp->score < rightApp->score;
    }
}
