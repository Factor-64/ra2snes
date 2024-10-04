#ifndef MEMORYREADER_H
#define MEMORYREADER_H

#include <QObject>
#include <QList>
#include "rc_runtime_types.h"
#include "rastructs.h"

class MemoryReader : public QObject {
    Q_OBJECT

public:
    explicit MemoryReader(QObject *parent = nullptr);

    void initTriggers(const QList<AchievementInfo> achievements, const QList<LeaderboardInfo> leaderboards);
    void remapTriggerAddresses();
    void setupConsoleMemory();
    uint8_t* getConsoleMemory();
    QList<QPair<int, int>> getUniqueMemoryAddresses();
    void checkAchievements(unsigned int frames);
    void checkLeaderboards(unsigned int frames);
    void freeConsoleMemory();

signals:
    void finishedMemorySetup();
    void achievementsChecked();
    void achievementUnlocked(unsigned int id, QDateTime time);
    void leaderboardsChecked();
    void leaderboardCompleted(unsigned int id, QDateTime time);

private:
    QList<QPair<int, int>> uniqueMemoryAddresses;
    QMap<unsigned int, rc_trigger_t*> achievementTriggers;
    QMap<unsigned int, rc_lboard_t*> leaderboardTriggers;
    QList<unsigned int> achievementsToRemove;
    uint8_t* consoleMemory;
    unsigned int consoleMemorySize;
};


#endif // MEMORYREADER_H
