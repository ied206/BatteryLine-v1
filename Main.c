// This program is licensed under MIT license.

#include "Var.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <windows.h>
#include <commctrl.h>
#ifdef _DEBUG  // without this, in Debug mode, produces a lot of warnings
#undef __CRT__NO_INLINE
#endif
#include <strsafe.h>
#ifdef _DEBUG
#define __CRT__NO_INLINE
#endif


#include "BasicIO.h"
#include "BatStat.h"
#include "ErrorHandle.h"
#include "DrawLine.h"

HANDLE not_power_src, not_bat_per;
SYSTEM_POWER_STATUS stat;
BL_OPTION option;
HINSTANCE g_hInst;
int g_nMon; // Number of monitors installed on this system
int g_nPriMon;
BL_MONINFO g_monRes[BL_MAX_MONITOR];
MONITORINFO g_monInfo[BL_MAX_MONITOR];

LRESULT CALLBACK WndProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// http://www.functionx.com/win32/Lesson01c.htm
int WINAPI WinMain(	HINSTANCE hInstance, HINSTANCE hPrevInstance,
					LPSTR lpCmdLine, int nCmdShow)
{
	HWND hWnd;
	MSG	Msg;

	ZeroMemory(&g_monInfo, sizeof(BL_MONINFO) * BL_MAX_MONITOR);
	ZeroMemory(&option, sizeof(BL_OPTION));
	g_hInst = hInstance;

	// Find if BatteryLine is already running.
	hWnd = FindWindowW(BL_ClassName, 0);
	if (hWnd != NULL) // Running BatteryLine found? Terminate it.
	{
		BLDL_AddTrayIcon(hWnd, BL_SysTrayID_OFF, NIF_INFO, 0, L"BatteryLine OFF");
		BLDL_DelTrayIcon(hWnd, BL_SysTrayID_OFF);
		SendMessageW(hWnd, WM_CLOSE, 0, 0);
		return 0;
	}

	// Load settings from BatteryLine.ini
	BLBS_ReadSetting();

	// Decode and treat the messages as long as the application is running
	hWnd = BLDL_InitWindow(hInstance);
	while (GetMessage(&Msg, NULL, 0, 0))
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}

	// Destroy Window
	BLCB_WM_CLOSE(hWnd, FALSE);

	return Msg.wParam;
}

LRESULT CALLBACK WndProcedure(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	wchar_t msgbox[BL_MSGBOX_BUF_SIZE];

    switch (Msg)
    {
	case WM_CREATE:
		#ifdef _DEBUG_CONSOLE
		puts("WM_CREATE");
		#endif // _DEBUG_CONSOLE
		BLDL_AddTrayIcon(hWnd, BL_SysTrayID_ON, NIF_MESSAGE | NIF_TIP | NIF_INFO, WM_APP_SYSTRAY_POPUP, L"BatteryLine On");
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
				StringCchPrintfW(msgbox, BL_MSGBOX_BUF_SIZE, L"Joveler's BatteryLine %d.%d (%dbit)\nVisit https://joveler.kr/batteryline\n\nCompile Date : %04d.%02d.%02d\n", BL_MAJOR_VER, BL_MINOR_VER, WhatBitOS(FALSE), CompileYear(), CompileMonth(), CompileDay());
				MessageBoxW(hWnd, msgbox, L"BatteryLine", MB_ICONINFORMATION | MB_OK);
				break;
			case ID_SETTING:
				if (MessageBoxW(hWnd, L"Edit BatteryLine.ini and run BatteryLine again.\n", L"BatteryLine", MB_ICONINFORMATION | MB_OKCANCEL) == IDOK)
					BLCB_OpenSettingIni(hWnd);
				break;
			case ID_POWERINFO:
				BLBS_GetBatteryStat();
				break;
			case ID_EXIT:
				#ifdef _DEBUG_CONSOLE
				puts("  ID_EXIT");;
				#endif // _DEBUG_CONSOLE
				PostMessage(hWnd, WM_CLOSE, 0, 0);
				break;
        }
        break;
	case WM_CLOSE: // 0x0010
	case WM_DESTROY:
		#ifdef _DEBUG_CONSOLE
		if (Msg == WM_CLOSE)
			puts("WM_CLOSE");
		else if (Msg == WM_DESTROY)
			puts("WM_DESTROY");
		#endif // _DEBUG_CONSOLE
		BLCB_WM_CLOSE(hWnd, TRUE);
        break;
	case WM_ERASEBKGND: // 0x0014
		#ifdef _DEBUG_CONSOLE
		puts("WM_ERASEBKGND");
		#endif // _DEBUG_CONSOLE
		// BLCB_WM_PAINT(hWnd);
        break;
    case WM_SETTINGCHANGE: // 0x001A
	case WM_POWERBROADCAST: // 0x0218
		#ifdef _DEBUG_CONSOLE
		if (Msg == WM_SETTINGCHANGE)
			puts("WM_SETTINGCHANGE");
		else if (Msg == WM_POWERBROADCAST)
			puts("WM_POWERBROADCAST");
		#endif // _DEBUG_CONSOLE
		BLCB_SetWindowPos(hWnd);
		break;
	case WM_DISPLAYCHANGE: // Monitor is attached or detached, Screen resolution changed, etc. Check for HMONITOR is valid.
		#ifdef _DEBUG_CONSOLE
		puts("WM_DISPLAYCHANGE");
		#endif // _DEBUG_CONSOLE
		// BringWindowToTop(hWnd);
		BLCB_SetWindowPos(hWnd); // Redraw windows fit changed resolution
		break;
	default:
		return DefWindowProc(hWnd, Msg, wParam, lParam);
		break;
    }

    return 0;
}
