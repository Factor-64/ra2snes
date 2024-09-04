#ifndef RA_CLIENT_OLD_H
#define RA_CLIENT_OLD_H

#include <QCoreApplication>
#include <QWebSocket>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QEventLoop>
#include <QPair>
#include <QList>
#include <QThread>
#include <QWaitCondition>
#include <QMutex>
#include "ra2snes.h"

#define RC_CLIENT_SUPPORTS_HASH 1

extern "C" {
#include "rc_client.h"
#include "rcheevos.h"

#if defined(_WIN32)
#include <Windows.h>
#elif defined(__unix__) && __STDC_VERSION__ >= 199309L
#include <unistd.h>
#else
#define RC_NO_SLEEP
#endif

#include <stdint.h>
}

typedef struct {
    rc_client_server_callback_t callback;
    void* callback_data;
} async_callback_data;

typedef QPair<uint32_t, uint32_t> AddressPair;

extern QList<AddressPair> memoryAddresses;
extern rc_client_t* g_client;
extern bool loggedin;
extern Usb2Snes* usb2snes;
extern uint32_t readMemoryOffset;
extern uint8_t* snesMemory;
extern size_t snesMemorySize;

typedef void (*http_callback_t)(int status_code, const char* response_data, size_t response_size, void* userdata, const char* error_message);

static uint32_t read_memory(uint32_t address, uint8_t* buffer, uint32_t num_bytes, rc_client_t* client);
static void http_callback(int status_code, const char* content, size_t content_size, void* userdata, const char* error_message);
static void server_call(const rc_api_request_t* request, rc_client_server_callback_t callback, void* callback_data, rc_client_t* client);
void async_http_post(const char* url, const char* post_data, const char* content_type, const char* user_agent, http_callback_t callback, void* userdata);
void async_http_get(const char* url, const char* user_agent, http_callback_t callback, void* userdata);
static void log_message(const char* message, const rc_client_t* client);
static void achievement_triggered(const rc_client_achievement_t* achievement);
static void leaderboard_started(const rc_client_leaderboard_t* leaderboard);
static void leaderboard_failed(const rc_client_leaderboard_t* leaderboard);
static void leaderboard_submitted(const rc_client_leaderboard_t* leaderboard);
static void leaderboard_tracker_update(const rc_client_leaderboard_tracker_t* tracker);
static void leaderboard_tracker_show(const rc_client_leaderboard_tracker_t* tracker);
static void leaderboard_tracker_hide(const rc_client_leaderboard_tracker_t* tracker);
static void challenge_indicator_show(const rc_client_achievement_t* achievement);
static void challenge_indicator_hide(const rc_client_achievement_t* achievement);
static void progress_indicator_update(const rc_client_achievement_t* achievement);
static void progress_indicator_show(const rc_client_achievement_t* achievement);
static void progress_indicator_hide(void);
static void game_mastered(void);
static void server_error(const rc_client_server_error_t* error);
static void event_handler(const rc_client_event_t* event, rc_client_t* client);
void initialize_retroachievements_client(void);
void shutdown_retroachievements_client(void);
static void login_callback(int result, const char* error_message, rc_client_t* client, void* userdata);
void login_retroachievements_user(const char* username, const char* password);
void login_remembered_retroachievements_user(const char* username, const char* token);
static void load_game_callback(int result, const char* error_message, rc_client_t* client, void* userdata);
void load_snes_game(const uint8_t* rom, size_t rom_size);
void load_gameboy_game(const uint8_t* rom, size_t rom_size);
static void show_game_placard();

#endif // RA_CLIENT_OLD_H
