#ifndef MEMORYREADER_H
#define MEMORYREADER_H

#include <QObject>
#include <QList>
#include <QQueue>
#include "rc_runtime_types.h"
#include "rastructs.h"

class MemoryReader : public QObject {
    Q_OBJECT

public:
    explicit MemoryReader(QObject *parent = nullptr);

    void initTriggers(const QList<AchievementInfo> achievements, const QList<LeaderboardInfo> leaderboards);
    void remapTriggerAddresses();
    QList<QPair<int, int>> getUniqueMemoryAddresses();
    void checkAchievements();
    void checkLeaderboards();
    void freeConsoleMemory();
    void addFrameToQueues(QByteArray data, int frames);
    int achievementQueueSize();

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
    QQueue<QPair<QByteArray, int>> achievementFrames;
    //QQueue<QPair<QByteArray, int>> leaderboardFrames;
};


#endif // MEMORYREADER_H
