#ifndef GAMEINFOMODEL_H
#define GAMEINFOMODEL_H

#include "rastructs.h"

class GameInfoModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString title READ title NOTIFY dataChanged)
    Q_PROPERTY(QString md5hash READ md5hash NOTIFY dataChanged)
    Q_PROPERTY(unsigned int id READ id NOTIFY dataChanged)
    Q_PROPERTY(QString image_icon READ image_icon NOTIFY dataChanged)
    Q_PROPERTY(QUrl image_icon_url READ image_icon_url NOTIFY dataChanged)
    Q_PROPERTY(QUrl game_link READ game_link NOTIFY dataChanged)
    Q_PROPERTY(QString console READ console NOTIFY dataChanged)
    Q_PROPERTY(QUrl console_icon READ console_icon NOTIFY dataChanged)
    Q_PROPERTY(unsigned int completion_count READ completion_count NOTIFY dataChanged)
    Q_PROPERTY(bool mastered READ mastered NOTIFY dataChanged)
    Q_PROPERTY(bool beaten READ beaten NOTIFY dataChanged)
    Q_PROPERTY(int point_total READ point_total NOTIFY dataChanged)
    Q_PROPERTY(unsigned int missable_count READ missable_count NOTIFY dataChanged)
    Q_PROPERTY(int point_count READ point_count NOTIFY dataChanged)
    Q_PROPERTY(int achievement_count READ achievement_count NOTIFY dataChanged)

public:
    explicit GameInfoModel(QObject *parent = nullptr);

    void setGameInfo(const GameInfo &gameInfo);
    void updateCompletionCount();

    QString title() const;
    QString md5hash() const;
    unsigned int id() const;
    QString image_icon() const;
    QUrl image_icon_url() const;
    QUrl game_link() const;
    QString console() const;
    QUrl console_icon() const;
    unsigned int completion_count() const;
    bool beaten() const;
    bool mastered() const;
    void clearGame();
    int point_total() const;
    unsigned int missable_count() const;
    int point_count() const;
    int achievement_count() const;

signals:
    void dataChanged();

private:
    GameInfo m_gameInfo;
};

#endif // GAMEINFOMODEL_H
