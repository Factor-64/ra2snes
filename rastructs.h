#ifndef RASTRUCTS_H
#define RASTRUCTS_H

#include <QUrl>
#include <QDateTime>

enum AchievementInfoType {
    Primed,
    Value,
    Percent,
    Target
};

struct AchievementInfo {
    QUrl badge_locked_url;
    QString badge_name;
    QUrl badge_url;
    //QDateTime created;
    QString description;
    unsigned int flags;
    unsigned int id;
    QString mem_addr;
    //QDateTime modified;
    unsigned int points;
    unsigned int rarity;
    unsigned int rarity_hardcore;
    QString title;
    QString type;
    QString author;
    QString time_unlocked_string;
    QDateTime time_unlocked;
    bool unlocked;
    QUrl achievement_link;
    bool primed;
    int value;
    int percent;
    int target;
};

struct LeaderboardInfo {
    QString title;
    QString description;
    int format;
    unsigned int id;
    int lower_is_better;
    QString mem_addr;
    unsigned int score;
    unsigned int seconds_since_completion;
    QUrl leaderboard_link;
    unsigned int placement;
};

struct UserInfo {
    QString username;
    QString token;
    int softcore_score;
    int hardcore_score;
    QUrl pfp;
    bool hardcore;
    QUrl link;
    int width;
    int height;
    bool savestates;
    bool cheats;
    bool patched;
    bool autohardcore;
};

struct GameInfo {
    QString title;
    QString md5hash;
    unsigned int id;
    QString image_icon;
    QUrl image_icon_url;
    QUrl game_link;
    QString console;
    QUrl console_icon;
    unsigned int completion_count;
    bool beaten;
    bool mastered;
    int point_total;
    unsigned int missable_count;
    int point_count;
    int achievement_count;
};

#endif // RASTRUCTS_H
