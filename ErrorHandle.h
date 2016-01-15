#ifndef ERRORHANDLE_H_INCLUDED
#define ERRORHANDLE_H_INCLUDED

#include <stdint.h>
// ErrorMessage

// 1xx : WinAPI Failure
#define JVERR_GetSystemPowerStatus				100
#define JVERR_LoadIcon							101
#define JVERR_LoadCursor						102
#define JVERR_CreateWindowEx 					103
#define JVERR_RegisterPowerSettingNotification 	104
#define JVERR_SetLayeredWindowAttributes		105
#define JVERR_FindWindow						106
#define JVERR_GetWindowRect						107
#define JVERR_GetSystemMetrics					108
#define JVERR_SetWindowPos						109
#define JVERR_UpdateWindow						110
#define JVERR_GetClientRect						111
#define JVERR_CreateFile						112
#define JVERR_WriteFile							113
#define JVERR_ReadFile							114
#define JVERR_GetCurrentDirectory				115
// 2xx : File IO, INI Parsing
#define JVERR_FILEIO_WRITTEN_BYTES				200
#define JVERR_FILEIO_READ_BYTES					201
#define JVERR_OPT_INI_IMPERFECT_OPTIONS			210
#define JVERR_OPT_INI_INVALID_SHOWCHARGE		211
#define JVERR_OPT_INI_INVALID_MONITOR			212
#define JVERR_OPT_INI_NOT_EXIST_MONITOR			213
#define JVERR_OPT_INI_INVALID_POSITION			214
#define JVERR_OPT_INI_INVALID_TASKBAR			215
#define JVERR_OPT_INI_INVALID_TRANSPARENCY		216
#define JVERR_OPT_INI_INVALID_HEIGHT			217
#define JVERR_OPT_INI_INVALID_DEFAULTCOLOR		218
#define JVERR_OPT_INI_INVALID_CHARGECOLOR		219
#define JVERR_OPT_INI_INVALID_FULLCOLOR			220
#define JVERR_OPT_INI_INVALID_COLOR				221
#define JVERR_OPT_INI_TOO_MUCH_COLOR			222
// 25x : Invalid Option
#define JVERR_OPT_INVALID_TASKBAR				250
#define JVERR_OPT_INVALID_POSITION				251
// 3xx : Battery Status
#define JVERR_ACLineStatus_UNKNOWN				300
#define JVERR_BATTERY_NOT_EXIST					301
// 4xx : etc
#define JVERR_MALLOC_FAILURE					400

// WarningMessage
#define JVWARN_NOT_ENOUGH_ARGV			10000

void JV_ErrorHandle(int code, int iswinapi);
void JV_WarnHandle(int code, int iswinapi);
void BinaryDump(const uint8_t buf[], const uint32_t bufsize);

#endif // ERRORHANDLE_H_INCLUDED
