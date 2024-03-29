﻿// This program is licensed under MIT license.

// Custom Constants
#include "Var.h"

// C Runtime Headers
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>

// Windows SDK Headers
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shlwapi.h>
#include <commctrl.h>
#include <strsafe.h>

// Custom Headers
#include "BasicIO.h"
#include "BatStat.h"
#include "ErrorHandle.h"
#include "DrawLine.h"

HANDLE not_power_src, not_bat_per;
SYSTEM_POWER_STATUS stat;
HWND g_hWnd = NULL;
HINSTANCE g_hInst;
BL_OPTION option;
int g_nMon; // Number of monitors installed on this system
int g_nPriMon;
BL_MONINFO g_monRes[BL_MAX_MONITOR];
MONITORINFO g_monInfo[BL_MAX_MONITOR];
BL_ARG g_arg;
UINT_PTR g_timerId;

LRESULT CALLBACK WndProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
bool BL_ParseArg(int argc, LPWSTR* argv, BL_ARG* arg);


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	HWND hWnd;
	MSG	msg;
	int argc = 0;
	LPWSTR* argv = NULL;

	ZeroMemory(&g_monInfo, sizeof(BL_MONINFO) * BL_MAX_MONITOR);
	ZeroMemory(&option, sizeof(BL_OPTION));
	g_hInst = hInstance;

	// Get Command line arguments
	argv = CommandLineToArgvW(GetCommandLineW(), &argc);
	if (BL_ParseArg(argc, argv, &g_arg))
		JV_ErrorHandle(JVERR_INVALID_ARGUMENT, FALSE);

	// Print Help Message and exit if '-h' '/?' is used
	if (g_arg.help)
	{
		BLDL_PrintHelp(NULL);
		exit(0);
	}

	// Check if this system has battery. If not, terminate
	if (GetSystemPowerStatus(&stat) == 0)
		JV_ErrorHandle(JVERR_GetSystemPowerStatus, TRUE);
	if (stat.BatteryFlag & 0x80) // Is battery exists?
	{
		if (!g_arg.quiet)
			JV_ErrorHandle(JVERR_BATTERY_NOT_EXIST, FALSE);
		else
			exit(JVERR_BATTERY_NOT_EXIST);
	}

	// Find if BatteryLine is already running.
	hWnd = FindWindowW(BL_ClassName, 0);
	if (hWnd != NULL) // Running BatteryLine found? Terminate it.
	{
		if (!g_arg.quiet)
		{
			BLDL_AddTrayIcon(hWnd, BL_SysTrayID_OFF, NIF_INFO, 0, L"BatteryLine OFF");
			BLDL_DelTrayIcon(hWnd, BL_SysTrayID_OFF);
		}
		SendMessageW(hWnd, WM_CLOSE, 0, 0);
		return 0;
	}

	// Load settings from BatteryLine.ini
	BLBS_ReadSetting();

	// Init BatteryLine window
	g_hWnd = hWnd = BLDL_InitWindow(hInstance);

	// Set BatteryLine timer
	BLDL_SetTimer(hWnd);

	// Decode and treat the messages as long as the application is running
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// Destroy Window
	BLCB_WM_CLOSE(hWnd, FALSE);

	return (int)msg.wParam;
}

LRESULT CALLBACK WndProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE:
#ifdef _DEBUG_CONSOLE
		puts("WM_CREATE");
#endif // _DEBUG_CONSOLE
		// popup ballon notification only when quiet option is not used
		BLDL_AddTrayIcon(hWnd, BL_SysTrayID_ON, NIF_MESSAGE | NIF_TIP | (g_arg.quiet ? 0 : NIF_INFO), WM_APP_SYSTRAY_POPUP, L"BatteryLine On");
		break;
	case WM_PAINT:
#ifdef _DEBUG_CONSOLE
		puts("WM_PAINT");
#endif // _DEBUG_CONSOLE
		BLCB_WM_PAINT(hWnd);
		break;
	// http://everything2.com/title/Win32%2520system%2520tray%2520icon
	case WM_APP_SYSTRAY_POPUP: // systray msg callback
#ifdef _DEBUG_CONSOLE
		puts("WM_APP_SYSTRAY_POPUP");
#endif
		switch (lParam)
		{
		case WM_LBUTTONDBLCLK:
#ifdef _DEBUG_CONSOLE
			puts("  WM_LBUTTONDBLCLK");
#endif // _DEBUG_CONSOLE
			SendMessage(hWnd, WM_COMMAND, ID_ABOUT, 0);
			break;
		case WM_RBUTTONUP:
#ifdef _DEBUG_CONSOLE
			puts("  WM_RBUTTONUP");
#endif // _DEBUG_CONSOLE
			SetForegroundWindow(hWnd);
			BLDL_ShowPopupMenu(hWnd, NULL, -1 );
			PostMessage(hWnd, WM_APP_SYSTRAY_POPUP, 0, 0);
			break;
		}
		break;
	case WM_COMMAND: // systray msg callback
#ifdef _DEBUG_CONSOLE
		puts("WM_COMMAND");
#endif // _DEBUG_CONSOLE
		switch (LOWORD(wParam))
		{
		case ID_ABOUT:
#ifdef _DEBUG_CONSOLE
			puts("  ID_ABOUT");
#endif // _DEBUG_CONSOLE
			BLDL_PrintBanner(g_hWnd);
			break;
		case ID_HELP:
#ifdef _DEBUG_CONSOLE
			puts("  ID_HELP");
#endif // _DEBUG_CONSOLE
			BLDL_PrintHelp(g_hWnd);
			break;
		case ID_HOMEPAGE:
#ifdef _DEBUG_CONSOLE
			puts(" ID_HOMEPAGE");
#endif // _DEBUG_CONSOLE
			BLDL_OpenHomepage(g_hWnd);
			break;
		case ID_LICENSE:
#ifdef _DEBUG_CONSOLE
			puts("  ID_LICENSE");
#endif // _DEBUG_CONSOLE
			BLDL_OpenLicense(g_hWnd);
			break;
		case ID_SETTING:
#ifdef _DEBUG_CONSOLE
			puts("  ID_SETTING");
#endif // _DEBUG_CONSOLE
			if (MessageBoxW(hWnd, L"Edit BatteryLine.ini and run BatteryLine again.\n", L"BatteryLine", MB_ICONINFORMATION | MB_OKCANCEL) == IDOK)
				BLCB_OpenSettingIni(hWnd);
			break;
		case ID_POWERINFO:
#ifdef _DEBUG_CONSOLE
			puts("  ID_POWERINFO");
#endif // _DEBUG_CONSOLE
			BLBS_GetBatteryStat();
			break;
		case ID_EXIT:
#ifdef _DEBUG_CONSOLE
			puts("  ID_EXIT");
#endif // _DEBUG_CONSOLE
			PostMessage(hWnd, WM_CLOSE, 0, 0);
			break;
		}
		break;
	case WM_CLOSE:
#ifdef _DEBUG_CONSOLE
		puts("WM_CLOSE");
#endif // _DEBUG_CONSOLE
		BLCB_WM_CLOSE(hWnd, TRUE);
		break;
	case WM_ERASEBKGND:
#ifdef _DEBUG_CONSOLE
		puts("WM_ERASEBKGND");
#endif // _DEBUG_CONSOLE
		// BLCB_WM_PAINT(hWnd);
		break;
	case WM_SETTINGCHANGE:
	case WM_POWERBROADCAST:
#ifdef _DEBUG_CONSOLE
		if (msg == WM_SETTINGCHANGE)
			puts("WM_SETTINGCHANGE");
		else if (msg == WM_POWERBROADCAST)
			puts("WM_POWERBROADCAST");
#endif // _DEBUG_CONSOLE
		BLCB_SetWindowPos(hWnd);
		break;
	case WM_DISPLAYCHANGE: // Monitor is attached or detached, Screen resolution changed, etc. Check for HMONITOR's validity.
#ifdef _DEBUG_CONSOLE
		if (msg == WM_DISPLAYCHANGE)
			puts("WM_DISPLAYCHANGE");
#endif // _DEBUG_CONSOLE
		// BringWindowToTop(hWnd);
		BLCB_SetWindowPos(hWnd); // Redraw windows if changed resolution
		break;
	case WM_DPICHANGED:
#ifdef _DEBUG_CONSOLE
		puts("WM_DPICHANGED");
#endif // _DEBUG_CONSOLE
		BLCB_SetWindowPos(hWnd); // Redraw windows if DPI is changed
		break;
	case WM_TIMER:
#ifdef _DEBUG_CONSOLE
		puts("WM_TIMER");
#endif // _DEBUG_CONSOLE
		BLCB_SetWindowPos(hWnd); // Redraw windows if a timer was called
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
		break;
	}

	return 0;
}

bool BL_ParseArg(int argc, LPWSTR* argv, BL_ARG* arg)
{
	bool flag_err = false;

	memset(arg, 0, sizeof(BL_ARG));
	// set to default value
	arg->quiet = BLA_QUIET_OFF;
	arg->help = BLA_HELP_OFF;

	if (2 <= argc)
	{
		for (int i = 1; i < argc; i++)
		{
			flag_err = false;
			if (StrCmpIW(argv[i], L"-q") == 0 || StrCmpIW(argv[i], L"--quiet") == 0 || StrCmpIW(argv[i], L"/q") == 0)
				arg->quiet = BLA_QUIET_ON;
			else if (StrCmpIW(argv[i], L"-h") == 0 || StrCmpIW(argv[i], L"--help") == 0 || StrCmpIW(argv[i], L"/?") == 0 || StrCmpIW(argv[i], L"-?") == 0)
				arg->help = BLA_HELP_ON;
			else
				flag_err = true;
		}
	}

	// return Zero when success
	return flag_err;
}
