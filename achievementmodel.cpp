#include "achievementmodel.h"

AchievementModel::AchievementModel(QObject *parent)
    : QAbstractListModel(parent) {}

void AchievementModel::setAchievements(const QList<AchievementInfo> &achievements) {
    beginResetModel();
    m_achievements = achievements;
    endResetModel();
    qDebug() << "Achievements set:" << m_achievements.count();
}

int AchievementModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return m_achievements.count();
}

QVariant AchievementModel::data(const QModelIndex &index, int role) const {
    if (index.row() < 0 || index.row() >= m_achievements.count())
        return QVariant();

    const AchievementInfo &achievement = m_achievements[index.row()];

    switch (role) {
    case BadgeLockedUrlRole:
        return achievement.badge_locked_url;
    case BadgeNameRole:
        return achievement.badge_name;
    case BadgeUrlRole:
        return achievement.badge_url;
    case DescriptionRole:
        return achievement.description;
    case FlagsRole:
        return achievement.flags;
    case IdRole:
        return achievement.id;
    case MemAddrRole:
        return achievement.mem_addr;
    case PointsRole:
        return achievement.points;
    case RarityRole:
        return achievement.rarity;
    case RarityHardcoreRole:
        return achievement.rarity_hardcore;
    case TitleRole:
        return achievement.title;
    case TypeRole:
        return achievement.type;
    case AuthorRole:
        return achievement.author;
    case TimeUnlockedRole:
        return achievement.time_unlocked;
    case UnlockedRole:
        return achievement.unlocked;
    case AchievementLinkRole:
        return achievement.achievement_link;
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> AchievementModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[BadgeLockedUrlRole] = "badgeLockedUrl";
    roles[BadgeNameRole] = "badgeName";
    roles[BadgeUrlRole] = "badgeUrl";
    roles[DescriptionRole] = "description";
    roles[FlagsRole] = "flags";
    roles[IdRole] = "id";
    roles[MemAddrRole] = "memAddr";
    roles[PointsRole] = "points";
    roles[RarityRole] = "rarity";
    roles[RarityHardcoreRole] = "rarityHardcore";
    roles[TitleRole] = "title";
    roles[TypeRole] = "type";
    roles[AuthorRole] = "author";
    roles[TimeUnlockedRole] = "timeUnlocked";
    roles[UnlockedRole] = "unlocked";
    roles[AchievementLinkRole] = "achievementLink";
    return roles;
}

void AchievementModel::setUnlockedState(unsigned int id, bool unlocked, QString time) {
    for (int i = 0; i < m_achievements.size(); ++i) {
        if (m_achievements[i].id == id) {
            m_achievements[i].unlocked = unlocked;
            m_achievements[i].time_unlocked = time;
            QModelIndex index = createIndex(i, 0);
            qDebug() << "Data changed for index:" << index << "Unlocked state:" << unlocked;
            emit dataChanged(index, index, {UnlockedRole});
            emit dataChanged(index, index, {TimeUnlockedRole});
            break;
        }
    }
}

void AchievementModel::clearAchievements() {
    beginResetModel();
    m_achievements.clear();
    endResetModel();
}
