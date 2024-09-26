#include "userinfomodel.h"

UserInfoModel::UserInfoModel(QObject *parent)
    : QObject(parent) {
}

void UserInfoModel::updateHardcoreScore(int score)
{
    m_userInfo.hardcore_score += score;
    emit dataChanged();
}

void UserInfoModel::updateSoftcoreScore(int score)
{
    m_userInfo.softcore_score += score;
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

bool UserInfoModel::savestates() const {
    return m_userInfo.savestates;
}

bool UserInfoModel::cheats() const {
    return m_userInfo.cheats;
}

bool UserInfoModel::patched() const {
    return m_userInfo.patched;
}

bool UserInfoModel::autohardcore() const {
    return m_userInfo.autohardcore;
}

void UserInfoModel::username(QString u)
{
    m_userInfo.username = u;
    emit dataChanged();
}

void UserInfoModel::token(QString t)
{
    m_userInfo.token = t;
    emit dataChanged();
}

void UserInfoModel::softcore_score(int s)
{
    m_userInfo.softcore_score = s;
    emit dataChanged();
}

void UserInfoModel::hardcore_score(int hs)
{
    m_userInfo.hardcore_score = hs;
    emit dataChanged();
}

void UserInfoModel::pfp(QUrl p)
{
    m_userInfo.pfp = p;
    emit dataChanged();
}

void UserInfoModel::hardcore(bool h)
{
    m_userInfo.hardcore = h;
    emit dataChanged();
}

void UserInfoModel::link(QUrl l)
{
    m_userInfo.link = l;
    emit dataChanged();
}

void UserInfoModel::width(int w)
{
    m_userInfo.width = w;
    emit dataChanged();
}

void UserInfoModel::height(int h)
{
    m_userInfo.height = h;
    emit dataChanged();
}

void UserInfoModel::savestates(bool s)
{
    m_userInfo.savestates = s;
    emit dataChanged();
}

void UserInfoModel::cheats(bool c)
{
    m_userInfo.cheats = c;
    emit dataChanged();
}

void UserInfoModel::patched(bool p)
{
    m_userInfo.patched = p;
    emit dataChanged();
}

void UserInfoModel::autohardcore(bool a)
{
    m_userInfo.autohardcore = a;
    emit dataChanged();
}

void UserInfoModel::clearUser() {
    m_userInfo = UserInfo();
}
