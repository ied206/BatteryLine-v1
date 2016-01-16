#include "Var.h"

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#undef __CRT__NO_INLINE
#include <strsafe.h>
#define __CRT__NO_INLINE

#include "ErrorHandle.h"

#define ERR_BUF_SIZE 512

void JV_ErrorHandle(int code, int iswinapi)
{
	char *msg = NULL;
	#ifndef _DEBUG_CONSOLE
	wchar_t buf[ERR_BUF_SIZE], bufapi[ERR_BUF_SIZE], msgbuf[ERR_BUF_SIZE];
	#endif

	switch (code)
	{
	// 1xx : WinAPI Failure
	case JVERR_GetSystemPowerStatus:
		msg =	"ErrorMessage : JVERR_GetSystemPowerStatus\n"
				"WinAPI GetSystemPowerStatus() Failed\n";
		break;
	case JVERR_LoadIcon:
		msg =	"ErrorMessage : JVERR_LoadIcon\n"
				"WinAPI LoadIcon() Failed\n";
		break;
	case JVERR_LoadCursor:
		msg =	"ErrorMessage : JVERR_LoadCursor\n"
				"WinAPI LoadCursor() Failed\n";
		break;
	case JVERR_CreateWindowEx:
		msg =	"ErrorMessage : JVERR_CreateWindowEx\n"
				"WinAPI CreateWindowEx() Failed\n";
		break;
	case JVERR_RegisterPowerSettingNotification:
		msg =	"ErrorMessage : JVERR_RegisterPowerSettingNotification\n"
				"WinAPI RegisterPowerSettingNotification() Failed\n";
		break;
	case JVERR_SetLayeredWindowAttributes:
		msg =	"ErrorMessage : JVERR_SetLayeredWindowAttributes\n"
				"WinAPI SetLayeredWindowAttributes() Failed\n";
		break;
	case JVERR_FindWindow:
		msg =	"ErrorMessage : JVERR_FindWindow\n"
				"WinAPI FindWindow() Failed\n";
		break;
	case JVERR_GetWindowRect:
		msg =	"ErrorMessage : JVERR_GetWindowRect\n"
				"WinAPI GetWindowRect() Failed\n";
		break;
	case JVERR_GetSystemMetrics:
		msg =	"ErrorMessage : JVERR_GetSystemMetrics\n"
				"WinAPI GetSystemMetrics() Failed\n";
		break;
	case JVERR_SetWindowPos:
		msg =	"ErrorMessage : JVERR_SetWindowPos\n"
				"WinAPI SetWindowPos() Failed\n";
		break;
	case JVERR_UpdateWindow:
		msg =	"ErrorMessage : JVERR_UpdateWindow\n"
				"WinAPI UpdateWindow() Failed\n";
		break;
	case JVERR_GetClientRect:
		msg =	"ErrorMessage : JVERR_GetClientRect\n"
				"WinAPI GetClientRect() Failed\n";
		break;
	case JVERR_CreateFile:
		msg =	"ErrorMessage : JVERR_CreateFile\n"
				"WinAPI CreateFile() Failed\n";
		break;
	case JVERR_WriteFile:
		msg =	"ErrorMessage : JVERR_WriteFile\n"
				"WinAPI WriteFile() Failed\n";
		break;
	case JVERR_ReadFile:
		msg =	"ErrorMessage : JVERR_ReadFile\n"
				"WinAPI ReadFile() Failed\n";
		break;
	case JVERR_GetCurrentDirectory:
		msg =	"ErrorMessage : JVERR_GetCurrentDirectory\n"
				"WinAPI GetCurrentDirectory() Failed\n";
		break;
	// 2xx : File IO, INI Parsing
	case JVERR_FILEIO_WRITTEN_BYTES:
		msg =	"ErrorMessage : JVERR_FILEIO_WRITTEN_BYTES\n"
				"In WriteFile, nNumberOfBytesToWrite != lpNumberOfBytesWritten\n";
		break;
	case JVERR_FILEIO_READ_BYTES:
		msg =	"ErrorMessage : JVERR_FILEIO_READ_BYTES\n"
				"In ReadFile, nNumberOfBytesToRead != lpNumberOfBytesRead\n";
		break;
	case JVERR_OPT_INI_MISSING_OPTIONS:
		msg =	"ErrorMessage : JVERR_OPT_INI_MISSING_OPTIONS\n"
				"Some options in BatteryLine.ini is missing.\n"
				"Try deleting BatteryLine.ini and run BatteryLine again.\n";
		break;
	case JVERR_OPT_INI_INVALID_SHOWCHARGE:
		msg =	"ErrorMessage : JVERR_OPT_INI_INVALID_SHOWCHARGE\n"
				"Invalid Option in BetteryLine.ini. Check \'showcharge\'.\n";
		break;
	case JVERR_OPT_INI_INVALID_MONITOR:
		msg =	"ErrorMessage : JVERR_OPT_INI_INVALID_MONITOR\n"
				"Invalid Option in BetteryLine.ini. Check \'monitor\'.\n";
		break;
	case JVERR_OPT_INI_NOT_EXIST_MONITOR:
		msg =	"ErrorMessage : JVERR_OPT_INI_NOT_EXIST_MONITOR\n"
				"\'monitor\' option in BetteryLine.ini points to monitor do not exists.\n"
				"Set \'monitor\' option to \'primary\' and run BatteryLine again.\n";
		break;
	case JVERR_OPT_INI_INVALID_POSITION:
		msg =	"ErrorMessage : JVERR_OPT_INI_INVALID_POSITION\n"
				"Invalid Option in BetteryLine.ini. Check \'position\'.\n";
		break;
	case JVERR_OPT_INI_INVALID_TASKBAR:
		msg =	"ErrorMessage : JVERR_OPT_INI_INVALID_TASKBAR\n"
				"Invalid Option in BetteryLine.ini. Check \'taskbar\'.\n";
		break;
	case JVERR_OPT_INI_INVALID_TRANSPARENCY:
		msg =	"ErrorMessage : JVERR_OPT_INI_INVALID_TRANSPARENCY\n"
				"Invalid Option in BetteryLine.ini. Check \'transparency\'.\n";
		break;
	case JVERR_OPT_INI_INVALID_HEIGHT:
		msg =	"ErrorMessage : JVERR_OPT_INI_INVALID_HEIGHT\n"
				"Invalid Option in BetteryLine.ini. Check \'height\'.\n";
		break;
	case JVERR_OPT_INI_INVALID_DEFAULTCOLOR:
		msg =	"ErrorMessage : JVERR_OPT_INI_INVALID_DEFAULTCOLOR\n"
				"Invalid Option in BetteryLine.ini. Check \'defaultcolor\'.\n";
		break;
	case JVERR_OPT_INI_INVALID_CHARGECOLOR:
		msg =	"ErrorMessage : JVERR_OPT_INI_INVALID_CHARGECOLOR\n"
				"Invalid Option in BetteryLine.ini. Check \'chargecolor\'.\n";
		break;
	case JVERR_OPT_INI_INVALID_FULLCOLOR:
		msg	=	"ErrorMessage : JVERR_OPT_INI_INVALID_FULLCOLOR\n"
				"Invalid Option in BetteryLine.ini. Check \'fullcolor\'.\n";
	case JVERR_OPT_INI_INVALID_COLOR:
		msg =	"ErrorMessage : JVERR_OPT_INI_INVALID_COLOR\n"
				"Invalid Option in BetteryLine.ini. Check \'color\'.\n";
		break;
	case JVERR_OPT_INI_TOO_MUCH_COLOR:
		msg =	"ErrorMessage : JVERR_OPT_INI_TOO_MUCH_COLOR\n"
				"Too much color in BetteryLine.ini. Check \'[Color] Section\'.\n";
		break;
	// 25x
	case JVERR_OPT_INVALID_TASKBAR:
		msg =	"ErrorMessage : JVERR_OPT_INVALID_TASKBAR\n"
				"option.taskbar in invalid.\n";
		break;
	// 3xx
	case JVERR_ACLineStatus_UNKNOWN:
		msg =	"ErrorMessage : JVERR_ACLineStatus_UNKNOWN\n"
				"ACLineStatus has value Unknown (-1)\n";
		break;
	case JVERR_BATTERY_NOT_EXIST:
		msg =	"ErrorMessage : JVERR_BATTERY_NOT_EXIST\n"
				"There is no battery in this system (128)\n";
		break;
	// Undefined
	default:
		msg = 	"ErrorMessage : UNDEFINDED ERROR\n"
				"Undefined Error\n";
		break;
	}

	#ifdef _DEBUG_CONSOLE
    printf("ErrorCode : %d\n%s", code, msg);
    if (iswinapi)
		printf("WinAPI LastError = %lu\n", GetLastError());
	#else
	if (iswinapi)
		StringCchPrintfW(bufapi, ERR_BUF_SIZE, L"WinAPI LastError = %lu\n", GetLastError());
	else
		bufapi[0] = L'\0';
	MultiByteToWideChar(CP_ACP, 0, msg, -1, msgbuf, ERR_BUF_SIZE);
	StringCchPrintfW(buf, ERR_BUF_SIZE, L"ErrorCode : %d\n%ws%ws", code, msgbuf, bufapi);
	MessageBoxW(NULL, buf, L"Error", MB_ICONERROR | MB_OK);
	#endif

	exit(code);
}

void JV_WarnHandle(int code, int iswinapi)
{
	char *msg = NULL;
	#ifndef _DEBUG_CONSOLE
	wchar_t buf[ERR_BUF_SIZE], bufapi[ERR_BUF_SIZE], msgbuf[ERR_BUF_SIZE];
	#endif

	switch (code)
	{
	case JVWARN_NOT_ENOUGH_ARGV:
		msg = 	"WarnMessage : JVWARN_NOT_ENOUGH_ARGV\n"
				"Not enough argv\n";
		break;
	case JVWARN_OPT_INVALID_MONITOR:
		msg = 	"WarnMessage : JVWARN_OPT_INVALID_MONITOR\n"
				"Specified monitor is no longer valid.\n"
				"Fall back to PRIMARY monitor.\n";
		break;
	default:
		msg = "WarnMessage : UNDEFINDED WARNING\nUndefined Warning\n";
		break;
	}

	#ifdef _DEBUG_CONSOLE
    printf("WarnCode : %d\n%s", code, msg);
    if (iswinapi)
		printf("WinAPI LastError = %lu\n", GetLastError());
	putchar('\n');
	#else
	if (iswinapi)
		StringCchPrintfW(bufapi, ERR_BUF_SIZE, L"WinAPI LastError = %lu\n", GetLastError());
	else
		bufapi[0] = 0x0000;
	MultiByteToWideChar(CP_ACP, 0, msg, -1, msgbuf, ERR_BUF_SIZE);
	StringCchPrintfW(buf, ERR_BUF_SIZE, L"WarnCode : %d\n%ws%ws", code, msgbuf, bufapi);
	MessageBoxW(NULL, buf, L"Warning", MB_ICONWARNING | MB_OK);
	#endif
}
