#include "achievementsortfilterproxymodel.h"
#include "achievementmodel.h"

AchievementSortFilterProxyModel::AchievementSortFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent), missableFilterEnabled(false), unlockedFilterEnabled(false) {}

void AchievementSortFilterProxyModel::sortByNormal()
{
    sort(-1);
}

void AchievementSortFilterProxyModel::sortByPoints() {
    setSortRole(AchievementModel::PointsRole);
    sort(0, Qt::AscendingOrder);
}

void AchievementSortFilterProxyModel::sortByTitle() {
    setSortRole(AchievementModel::TitleRole);
    sort(0, Qt::AscendingOrder);
}

void AchievementSortFilterProxyModel::sortByType() {
    setSortRole(AchievementModel::TypeRole);
    sort(0, Qt::AscendingOrder);
}

void AchievementSortFilterProxyModel::sortByTime() {
    sort(-1);
    setSortRole(AchievementModel::TimeUnlockedRole);
    sort(0, Qt::DescendingOrder);
}

void AchievementSortFilterProxyModel::showOnlyMissable() {
    missableFilterEnabled = true;
    invalidateFilter();
}

void AchievementSortFilterProxyModel::hideUnlocked() {
    unlockedFilterEnabled = true;
    invalidateFilter();
}

void AchievementSortFilterProxyModel::clearMissableFilter() {
    missableFilterEnabled = false;
    invalidateFilter();
}

void AchievementSortFilterProxyModel::clearUnlockedFilter() {
    unlockedFilterEnabled = false;
    invalidateFilter();
}

bool AchievementSortFilterProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const {
    if (sortRole() == AchievementModel::TypeRole) {
        QString leftType = sourceModel()->data(left, AchievementModel::TypeRole).toString();
        QString rightType = sourceModel()->data(right, AchievementModel::TypeRole).toString();

        static const QStringList typeOrder = {"progression", "win_condition", "missable", ""};

        int leftIndex = typeOrder.indexOf(leftType);
        int rightIndex = typeOrder.indexOf(rightType);

        if (leftIndex == -1) leftIndex = typeOrder.size();
        if (rightIndex == -1) rightIndex = typeOrder.size();

        return leftIndex < rightIndex;
    } else {
        return QSortFilterProxyModel::lessThan(left, right);
    }
}

bool AchievementSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);

    bool typeFilter = true;
    if (missableFilterEnabled) {
        typeFilter = sourceModel()->data(index, AchievementModel::TypeRole).toString() == "missable";
    }

    bool unlockedFilter = true;
    if (unlockedFilterEnabled) {
        unlockedFilter = sourceModel()->data(index, AchievementModel::UnlockedRole).toBool() == false;
    }

    return typeFilter && unlockedFilter;
}
