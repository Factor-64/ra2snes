#ifndef ACHIEVEMENTSORTFILTERPROXYMODEL_H
#define ACHIEVEMENTSORTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>

class AchievementSortFilterProxyModel : public QSortFilterProxyModel {
    Q_OBJECT
public:
    explicit AchievementSortFilterProxyModel(QObject *parent = nullptr);

    Q_INVOKABLE void sortByNormal();
    Q_INVOKABLE void sortByPoints();
    Q_INVOKABLE void showOnlyMissable();
    Q_INVOKABLE void sortByTitle();
    Q_INVOKABLE void hideUnlocked();
    Q_INVOKABLE void clearMissableFilter();
    Q_INVOKABLE void clearUnlockedFilter();
    Q_INVOKABLE void sortByType();
    Q_INVOKABLE void sortByTime();
    Q_INVOKABLE void sortByPrimed();
    Q_INVOKABLE void sortByPercent();

protected:
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
    bool missableFilterEnabled;
    bool unlockedFilterEnabled;
};

#endif // ACHIEVEMENTSORTFILTERPROXYMODEL_H
