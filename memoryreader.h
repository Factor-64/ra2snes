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

    void initTriggers(const QList<AchievementInfo> achievements, const QList<LeaderboardInfo> leaderboards, unsigned int ramSize);
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
    void updateAchievementInfo(unsigned int id, AchievementInfoType infotype, int value);
    void modifiedAddresses();

private:
    void decrementAddressCounts(rc_memref_t* nextref);
    QList<QPair<int, int>> uniqueMemoryAddresses;
    QMap<unsigned int, rc_trigger_t*> achievementTriggers;
    QMap<unsigned int, rc_lboard_t*> leaderboardTriggers;
    QQueue<QPair<QByteArray, int>> achievementFrames;
    QMap<int, int> uniqueMemoryAddressesCounts;
    QMap<int, int> addressMap;
    bool modified;
    //QQueue<QPair<QByteArray, int>> leaderboardFrames;
};


#endif // MEMORYREADER_H
