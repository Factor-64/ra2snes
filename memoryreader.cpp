#include "memoryreader.h"
#include <QDebug>
#include <memory>

MemoryReader::MemoryReader(QObject *parent) : QObject(parent) {
    consoleMemory = nullptr;
}

void MemoryReader::initTriggers(const QList<AchievementInfo> achievements, const QList<LeaderboardInfo> leaderboards) {
    uniqueMemoryAddresses.clear();
    achievementTriggers.clear();
    leaderboardTriggers.clear();
    consoleMemorySize = 0;
    QMap<int, int> uniqueAddresses;

    for(const auto& achievement : achievements)
    {
        if(!achievement.unlocked)
        {
            QByteArray data = achievement.mem_addr.toLocal8Bit();
            const char* mem_addr = data.constData();
            size_t trigger_size = rc_trigger_size(mem_addr);
            std::unique_ptr<void, decltype(&free)> trigger_buffer(malloc(trigger_size), free);
            rc_trigger_t* trigger = rc_parse_trigger(trigger_buffer.get(), mem_addr, NULL, 0);
            achievementTriggers[achievement.id] = trigger;
            for(rc_memref_t* nextref = trigger->memrefs; nextref != nullptr; nextref = nextref->next)
            {
                uniqueAddresses[nextref->address] = std::max(uniqueAddresses[nextref->address], nextref->value.size + 1);
            }
        }
    }

    for(const auto& leaderboard : leaderboards)
    {
        QByteArray data = leaderboard.mem_addr.toLocal8Bit();
        const char* mem_addr = data.constData();
        size_t lboard_size = rc_lboard_size(mem_addr);
        std::unique_ptr<void, decltype(&free)> lboard_buffer(malloc(lboard_size), free);
        rc_lboard_t* lboard = rc_parse_lboard(lboard_buffer.get(), mem_addr, NULL, 0);
        leaderboardTriggers[leaderboard.id] = lboard;
        for(rc_memref_t* nextref = lboard->memrefs; nextref != nullptr; nextref = nextref->next)
        {
            uniqueAddresses[nextref->address] = std::max(uniqueAddresses[nextref->address], nextref->value.size + 1);
        }
    }

    for(auto it = uniqueAddresses.begin(); it != uniqueAddresses.end(); ++it)
        uniqueMemoryAddresses.append(qMakePair(it.key(), it.value()));

    remapTriggerAddresses();
}

void MemoryReader::remapTriggerAddresses()
{
    QMap<int, int> memoryOffsets;
    int memoryOffset = 0;
    for(const auto& pair : uniqueMemoryAddresses)
    {
        memoryOffsets[pair.first] = memoryOffset;
        memoryOffset += pair.second;
        consoleMemorySize += pair.second;
    }

    for(auto& trigger : achievementTriggers)
    {
        for(rc_memref_t* nextref = trigger->memrefs; nextref != nullptr; nextref = nextref->next)
        {
            nextref->address = memoryOffsets[nextref->address];
        }
    }

    for(auto& lboard : leaderboardTriggers)
    {
        for(rc_memref_t* nextref = lboard->memrefs; nextref != nullptr; nextref = nextref->next)
        {
            nextref->address = memoryOffsets[nextref->address];
        }
    }

    setupConsoleMemory();
}

void MemoryReader::setupConsoleMemory()
{
    delete[] consoleMemory;
    consoleMemory = (consoleMemorySize != 0) ? new uint8_t[consoleMemorySize] : nullptr;
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
