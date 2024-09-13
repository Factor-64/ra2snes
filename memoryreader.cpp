#include "memoryreader.h"
#include <QDebug>

MemoryReader::MemoryReader(QObject *parent) : QObject(parent) {
    consoleMemory = nullptr;
}

void MemoryReader::initTriggers(const QList<AchievementInfo> achievements, const QList<LeaderboardInfo> leaderboards) {
    uniqueMemoryAddresses.clear();
    achievementTriggers.clear();
    leaderboardTriggers.clear();
    consoleMemorySize = 0;
    QMap<int, int> uniqueAddresses;
    for(const AchievementInfo& achievement : achievements)
    {
        if(!achievement.unlocked)
        {
            QByteArray data = achievement.mem_addr.toLocal8Bit();
            const char* mem_addr = data.constData();
            rc_trigger_t* trigger;
            size_t trigger_size = rc_trigger_size(mem_addr);
            void* trigger_buffer = malloc(trigger_size);;
            trigger = rc_parse_trigger(trigger_buffer, mem_addr, NULL, 0);
            achievementTriggers[achievement.id] = trigger;
            rc_memref_t* nextref = trigger->memrefs;
            while(nextref != nullptr)
            {
                if(uniqueAddresses[nextref->address] < nextref->value.size + 1)
                    uniqueAddresses[nextref->address] = nextref->value.size + 1;
                nextref = nextref->next;
            }
        }
    }

    if(!leaderboards.empty())
    {
            for(const LeaderboardInfo& leaderboard : leaderboards)
            {
                QByteArray data = leaderboard.mem_addr.toLocal8Bit();
                const char* mem_addr = data.constData();
                rc_lboard_t* lboard;
                size_t lboard_size = rc_lboard_size(mem_addr);
                void* lboard_buffer = malloc(lboard_size);
                lboard = rc_parse_lboard(lboard_buffer, mem_addr, NULL, 0);
                rc_memref_t* nextref = lboard->memrefs;
                leaderboardTriggers[leaderboard.id] = lboard;
                while(nextref != nullptr)
                {
                    if(uniqueAddresses[nextref->address] < nextref->value.size + 1)
                        uniqueAddresses[nextref->address] = nextref->value.size + 1;
                    nextref = nextref->next;
                }
            }
    }

    //qDebug() << uniqueAddresses;

    for(auto it = uniqueAddresses.begin(); it != uniqueAddresses.end(); ++it)
        uniqueMemoryAddresses.append(qMakePair(it.key(), it.value()));

    remapTriggerAddresses();
}

void MemoryReader::remapTriggerAddresses()
{
    for(auto it = achievementTriggers.begin(); it != achievementTriggers.end(); ++it)
    {
        rc_memref_t* nextref = it.value()->memrefs;
        while(nextref != nullptr)
        {
            int memoryOffset = 0;
            for(const auto& pair : uniqueMemoryAddresses)
            {
                if(pair.first == nextref->address)
                {
                    //qDebug() << "Unique Address: " << pair.first;
                    //qDebug() << "Trigger Address:" << nextref->address;
                    nextref->address = memoryOffset;
                    //qDebug() << "Memory Offset: " << memoryOffset;
                    //qDebug() << "New Trigger Address: " << nextref->address;
                }
                memoryOffset += pair.second;
            }
            nextref = nextref->next;
        }
    }

    if(!leaderboardTriggers.empty())
    {
        for(auto it = leaderboardTriggers.begin(); it != leaderboardTriggers.end(); ++it)
        {
            rc_memref_t* nextref = it.value()->memrefs;
            while(nextref != nullptr)
            {
                int memoryOffset = 0;
                for(const auto& pair : uniqueMemoryAddresses)
                {
                    if(pair.first == nextref->address)
                    {
                        //qDebug() << "Unique Address: " << pair.first;
                        //qDebug() << "Trigger Address:" << nextref->address;
                        nextref->address = memoryOffset;
                        //qDebug() << "Memory Offset: " << memoryOffset;
                        //qDebug() << "New Trigger Address: " << nextref->address;
                    }
                    memoryOffset += pair.second;
                }
                nextref = nextref->next;
            }
        }
    }

    setConsoleMemorySize();
}

void MemoryReader::setConsoleMemorySize()
{
    for(const auto& pair : std::as_const(uniqueMemoryAddresses))
        consoleMemorySize += pair.second;

    delete[] consoleMemory;
    if(consoleMemorySize != 0)
        consoleMemory = new uint8_t[consoleMemorySize];
    else
        consoleMemory = nullptr;

    emit finishedMemorySetup();
}

uint8_t* MemoryReader::getConsoleMemory()
{
    return consoleMemory;
}

QList<QPair<int, int>> MemoryReader::getUniqueMemoryAddresses()
{
    return uniqueMemoryAddresses;
}

void MemoryReader::checkAchievements()
{
    QList<unsigned int> ids;
    for(auto it = achievementTriggers.cbegin(); it != achievementTriggers.cend(); ++it)
    {
        const auto& trigger = it.value();
        rc_test_trigger(trigger, peek, consoleMemory, nullptr);

        if (trigger->state == RC_TRIGGER_STATE_TRIGGERED)
        {
            qDebug() << "Achievement Unlocked: " << it.key();
            ids.append(it.key());
            emit achievementUnlocked(it.key());
        }
    }
    for(const auto& id : ids)
        achievementTriggers.remove(id);
    emit achievementsChecked();
}

void MemoryReader::checkLeaderboards()
{
    emit leaderboardsChecked();
}

void MemoryReader::freeConsoleMemory()
{
    delete[] consoleMemory;
}
