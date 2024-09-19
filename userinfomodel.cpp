#include "userinfomodel.h"

UserInfoModel::UserInfoModel(QObject *parent)
    : QObject(parent) {
}

void UserInfoModel::setUserInfo(const UserInfo &userInfo) {
    m_userInfo = userInfo;
    emit dataChanged();
}

QString UserInfoModel::username() const {
    return m_userInfo.username;
}

QString UserInfoModel::token() const {
    return m_userInfo.token;
}

int UserInfoModel::softcore_score() const {
    return m_userInfo.softcore_score;
}

int UserInfoModel::hardcore_score() const {
    return m_userInfo.hardcore_score;
}

QUrl UserInfoModel::pfp() const {
    return m_userInfo.pfp;
}

bool UserInfoModel::hardcore() const {
    return m_userInfo.hardcore;
}

QUrl UserInfoModel::link() const {
    return m_userInfo.link;
}

int UserInfoModel::width() const {
    return m_userInfo.width;
}

int UserInfoModel::height() const {
    return m_userInfo.height;
}
