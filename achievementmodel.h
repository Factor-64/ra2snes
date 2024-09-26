#ifndef ACHIEVEMENTMODEL_H
#define ACHIEVEMENTMODEL_H

#include <QAbstractListModel>
#include <QList>
#include <QUrl>
#include <QDateTime>
#include "rastructs.h"

class AchievementModel : public QAbstractListModel {
    Q_OBJECT

public:
    enum AchievementRoles {
        BadgeLockedUrlRole = Qt::UserRole + 1,
        BadgeNameRole,
        BadgeUrlRole,
        DescriptionRole,
        FlagsRole,
        IdRole,
        MemAddrRole,
        PointsRole,
        RarityRole,
        RarityHardcoreRole,
        TitleRole,
        TypeRole,
        AuthorRole,
        TimeUnlockedRole,
        UnlockedRole,
        AchievementLinkRole
    };

    AchievementModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QHash<int, QByteArray> roleNames() const override;

    void setUnlockedState(unsigned int id, bool unlocked, QString time);

    void clearAchievements();

    QList<AchievementInfo> getAchievements();

    void appendAchievement(AchievementInfo a);

private:
    QList<AchievementInfo> m_achievements;
};

#endif // ACHIEVEMENTMODEL_H
