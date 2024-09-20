#ifndef USERINFOMODEL_H
#define USERINFOMODEL_H

#include "rastructs.h"

class UserInfoModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString username READ username NOTIFY dataChanged)
    Q_PROPERTY(QString token READ token NOTIFY dataChanged)
    Q_PROPERTY(int softcore_score READ softcore_score NOTIFY dataChanged)
    Q_PROPERTY(int hardcore_score READ hardcore_score NOTIFY dataChanged)
    Q_PROPERTY(QUrl pfp READ pfp NOTIFY dataChanged)
    Q_PROPERTY(bool hardcore READ hardcore NOTIFY dataChanged)
    Q_PROPERTY(QUrl link READ link NOTIFY dataChanged)
    Q_PROPERTY(int width READ width CONSTANT)
    Q_PROPERTY(int height READ height CONSTANT)
    Q_PROPERTY(bool savestates READ savestates NOTIFY dataChanged)
    Q_PROPERTY(bool cheats READ cheats NOTIFY dataChanged)
    Q_PROPERTY(bool patched READ patched NOTIFY dataChanged)

public:
    explicit UserInfoModel(QObject *parent = nullptr);

    void setUserInfo(const UserInfo &userInfo);
    void setHardcore(bool h);
    void setSaveStates(bool s);
    void setCheats(bool c);
    void setPatched(bool p);

    QString username() const;
    QString token() const;
    int softcore_score() const;
    int hardcore_score() const;
    QUrl pfp() const;
    bool hardcore() const;
    QUrl link() const;
    int width() const;
    int height() const;
    bool savestates() const;
    bool cheats() const;
    bool patched() const;
    void clearUser();

signals:
    void dataChanged();

private:
    UserInfo m_userInfo;
};

#endif // USERINFOMODEL_H
