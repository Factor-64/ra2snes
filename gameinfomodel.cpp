#include "gameinfomodel.h"

GameInfoModel::GameInfoModel(QObject *parent) : QObject(parent) {}

void GameInfoModel::setGameInfo(const GameInfo &gameInfo) {
    m_gameInfo = gameInfo;
    emit dataChanged();
}

QString GameInfoModel::title() const {
    return m_gameInfo.title;
}

QString GameInfoModel::md5hash() const {
    return m_gameInfo.md5hash;
}

unsigned int GameInfoModel::id() const {
    return m_gameInfo.id;
}

QString GameInfoModel::image_icon() const {
    return m_gameInfo.image_icon;
}

QUrl GameInfoModel::image_icon_url() const {
    return m_gameInfo.image_icon_url;
}

QUrl GameInfoModel::game_link() const {
    return m_gameInfo.game_link;
}

QString GameInfoModel::console() const {
    return m_gameInfo.console;
}

QUrl GameInfoModel::console_icon() const {
    return m_gameInfo.console_icon;
}

unsigned int GameInfoModel::completion_count() const {
    return m_gameInfo.completion_count;
}

int GameInfoModel::achievement_count() const {
    return m_gameInfo.achievement_count;
}

bool GameInfoModel::beaten() const {
    return m_gameInfo.beaten;
}

bool GameInfoModel::mastered() const {
    return m_gameInfo.mastered;
}

int GameInfoModel::point_total() const {
    return m_gameInfo.point_total;
}

unsigned int GameInfoModel::missable_count() const {
    return m_gameInfo.missable_count;
}

int GameInfoModel::point_count() const {
    return m_gameInfo.point_count;
}

void GameInfoModel::updateCompletionCount() {
    m_gameInfo.completion_count++;
    emit dataChanged();
}

void GameInfoModel::updatePointCount(unsigned int points) {
    m_gameInfo.point_count += points;
    emit dataChanged();
}

void GameInfoModel::title(QString t)
{
    m_gameInfo.title = t;
    emit dataChanged();
}
void GameInfoModel::md5hash(QString md5)
{
    m_gameInfo.md5hash = md5;
    emit dataChanged();
}

void GameInfoModel::id(unsigned int i)
{
    m_gameInfo.id = i;
    emit dataChanged();
}

void GameInfoModel::image_icon(QString ii)
{
    m_gameInfo.image_icon = ii;
    emit dataChanged();
}

void GameInfoModel::image_icon_url(QUrl iu)
{
    m_gameInfo.image_icon_url = iu;
    emit dataChanged();
}

void GameInfoModel::game_link(QUrl gl)
{
    m_gameInfo.game_link = gl;
    emit dataChanged();
}

void GameInfoModel::console(QString c)
{
    m_gameInfo.console = c;
    emit dataChanged();
}

void GameInfoModel::console_icon(QUrl ci)
{
    m_gameInfo.console_icon = ci;
    emit dataChanged();
}

void GameInfoModel::completion_count(unsigned int cc)
{
    m_gameInfo.completion_count = cc;
    emit dataChanged();
}

void GameInfoModel::beaten(bool b)
{
    m_gameInfo.beaten = b;
    if(b)
        emit beatenGame();
    emit dataChanged();
}

void GameInfoModel::mastered(bool m)
{
    m_gameInfo.mastered = m;
    if(m)
        emit masteredGame();
    emit dataChanged();
}

void GameInfoModel::point_total(int pt)
{
    m_gameInfo.point_total = pt;
    emit dataChanged();
}

void GameInfoModel::missable_count(unsigned int mc)
{
    m_gameInfo.missable_count = mc;
    emit dataChanged();
}

void GameInfoModel::point_count(int pc)
{
    m_gameInfo.point_count = pc;
    emit dataChanged();
}

void GameInfoModel::achievement_count(int ac)
{
    m_gameInfo.achievement_count = ac;
    emit dataChanged();
}

void GameInfoModel::clearGame() {
    m_gameInfo = GameInfo();
}
