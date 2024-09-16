#include "achievementsortfilterproxymodel.h"
#include "achievementmodel.h"

AchievementSortFilterProxyModel::AchievementSortFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent), missableFilterEnabled(false), unlockedFilterEnabled(false) {}

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
