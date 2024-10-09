#include "achievementmodel.h"

AchievementModel::AchievementModel(QObject *parent)
    : QAbstractListModel(parent) {}

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
    case TimeUnlockedStringRole:
        return achievement.time_unlocked_string;
    case UnlockedRole:
        return achievement.unlocked;
    case AchievementLinkRole:
        return achievement.achievement_link;
    case PrimedRole:
        return achievement.primed;
    case ValueRole:
        return achievement.value;
    case PercentRole:
        return achievement.percent;
    case TargetRole:
        return achievement.target;
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
    roles[TimeUnlockedStringRole] = "timeUnlockedString";
    roles[UnlockedRole] = "unlocked";
    roles[AchievementLinkRole] = "achievementLink";
    roles[PrimedRole] = "primed";
    roles[ValueRole] = "value";
    roles[PercentRole] = "percent";
    roles[TargetRole] = "target";
    return roles;
}

void AchievementModel::setUnlockedState(unsigned int id, bool unlocked, QDateTime time) {
    for (int i = 0; i < m_achievements.size(); ++i) {
        if (m_achievements[i].id == id) {
            m_achievements[i].unlocked = unlocked;
            m_achievements[i].time_unlocked = time;
            m_achievements[i].time_unlocked_string = time.toString("MMMM d yyyy, h:mmap");
            QModelIndex index = createIndex(i, 0);
            //qDebug() << "Data changed for index:" << index << "Unlocked state:" << unlocked;
            emit dataChanged(index, index, {UnlockedRole, TimeUnlockedRole, TimeUnlockedStringRole});
            emit unlockedChanged();
            break;
        }
    }
}

void AchievementModel::primeAchievement(unsigned int id, bool p)
{
    for (int i = 0; i < m_achievements.size(); ++i) {
        if (m_achievements[i].id == id) {
            m_achievements[i].primed = p;
            QModelIndex index = createIndex(i, 0);
            emit dataChanged(index, index, {PrimedRole});
            break;
        }
    }
}

void AchievementModel::updateAchievementValue(unsigned int id, int value)
{
    for (int i = 0; i < m_achievements.size(); ++i) {
        if (m_achievements[i].id == id) {
            m_achievements[i].value = value;
            QModelIndex index = createIndex(i, 0);
            emit dataChanged(index, index, {ValueRole});
            break;
        }
    }
}

void AchievementModel::updateAchievementPercent(unsigned int id, int percent)
{
    for (int i = 0; i < m_achievements.size(); ++i) {
        if (m_achievements[i].id == id) {
            m_achievements[i].percent = percent;
            QModelIndex index = createIndex(i, 0);
            emit dataChanged(index, index, {PercentRole});
            break;
        }
    }
}

void AchievementModel::updateAchievementTarget(unsigned int id, int target)
{
    for (int i = 0; i < m_achievements.size(); ++i) {
        if (m_achievements[i].id == id) {
            m_achievements[i].target = target;
            QModelIndex index = createIndex(i, 0);
            emit dataChanged(index, index, {TargetRole});
            break;
        }
    }
}

void AchievementModel::clearAchievements() {
    beginResetModel();
    m_achievements.clear();
    endResetModel();
}

QList<AchievementInfo> AchievementModel::getAchievements()
{
    return m_achievements;
}

void AchievementModel::appendAchievement(AchievementInfo a) {
    beginInsertRows(QModelIndex(), m_achievements.size(), m_achievements.size());
    m_achievements.append(a);
    endInsertRows();
}
