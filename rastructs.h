#ifndef RASTRUCTS_H
#define RASTRUCTS_H

#include <QUrl>
#include <QDateTime>

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
    QString time_unlocked;
    bool unlocked;
    QUrl achievement_link;
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
};

struct GameInfo {
    QString title;
    QString md5hash;
    unsigned int id;
    QString image_icon;
    QUrl image_icon_url;
    QList<AchievementInfo> achievements;
    QList<LeaderboardInfo> leaderboards;
    QUrl game_link;
    QString console;
    QUrl console_icon;
    unsigned int completion_count;
};

static uint32_t peek(uint32_t address, uint32_t num_bytes, void* ud) {
    uint8_t* memory = (uint8_t*)ud;

    switch (num_bytes) {
    case 1: return memory[address];

    case 2: return memory[address] |
               memory[address + 1] << 8;

    case 4: return memory[address] |
               memory[address + 1] << 8 |
               memory[address + 2] << 16 |
               memory[address + 3] << 24;
    }

    return 0;
}

#endif // RASTRUCTS_H
