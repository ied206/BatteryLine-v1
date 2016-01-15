#include "Var.h"
#include "rc\resource.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <windows.h>
#include <shellapi.h>
#undef __CRT__NO_INLINE
#include <strsafe.h>
#define __CRT__NO_INLINE
#include <commctrl.h>

#include "DrawLine.h"
#include "BatStat.h"
#include "ErrorHandle.h"

// http://soen.kr/
// www.functionx.com/win32/Lesson01.htm
// http://www.codeproject.com/Articles/15829/Vista-Goodies-in-C-Monitoring-the-Computer-s-Power
// http://www.zachburlingame.com/2011/04/capturing-windows-power-events-in-a-console-application/
// https://msdn.microsoft.com/en-us/library/ms703398%28v=vs.85%29.aspx
// http://www.transmissionzero.co.uk/computing/win32-apps-with-mingw/
// http://www.rw-designer.com/DPI-aware
LRESULT CALLBACK WndProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

HWND BLDL_InitWindow(HINSTANCE hInstance)
{
	HWND hWnd;
	HANDLE notPowerSrc, notBatPer;
	WNDCLASSEXW WndClsEx;

	// Create Window
	ZeroMemory(&WndClsEx, sizeof(WNDCLASSEXW));
	WndClsEx.cbSize 		= sizeof(WNDCLASSEX);
	WndClsEx.style 			= 0x0;
	WndClsEx.lpfnWndProc	= WndProcedure;
	WndClsEx.cbClsExtra 	= 0;
	WndClsEx.cbWndExtra 	= 0;
	WndClsEx.hIcon      	= (HICON) LoadImageW(hInstance, MAKEINTRESOURCEW(IDI_MAINICON), IMAGE_ICON, GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
	WndClsEx.hCursor     	= (HCURSOR) LoadImageW(NULL, MAKEINTRESOURCEW(OCR_NORMAL), IMAGE_CURSOR, GetSystemMetrics(SM_CXCURSOR), GetSystemMetrics(SM_CYCURSOR), LR_SHARED);
	WndClsEx.hbrBackground 	= GetStockObject(WHITE_BRUSH); // 0x00
	WndClsEx.lpszMenuName  	= NULL;
	WndClsEx.lpszClassName 	= BL_ClassName;
	WndClsEx.hInstance 		= hInstance;
	WndClsEx.hIconSm      	= (HICON) LoadImageW(hInstance, MAKEINTRESOURCEW(IDI_MAINICON), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);

	if (WndClsEx.hIcon == NULL || WndClsEx.hIconSm == NULL)
		JV_ErrorHandle(JVERR_LoadIcon, TRUE);
	if (WndClsEx.hCursor == NULL)
		JV_ErrorHandle(JVERR_LoadCursor, TRUE);
	RegisterClassExW(&WndClsEx);

	hWnd = CreateWindowExW(WS_EX_NOACTIVATE | WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT | WS_EX_TOPMOST, // dwExStyle 0x080800A8
						BL_ClassName, 	// lpClassName
						BL_WindowName, 	// lpWindowName
						WS_POPUP,		 // dwStyle, 0x80000000
						0, 		// X
						0, 		// Y
						0, 		// nWidth
						0, 		// nHeight
						NULL,		// hWndParent
						NULL, 		// hMenu
						hInstance,	// hInstance
						NULL);		// lpParam

	if (!IsWindow(hWnd)) // Is Window Created?
		JV_ErrorHandle(JVERR_CreateWindowEx, TRUE); // If Not Created, terminate

	// This Function is supported since Vista
	notPowerSrc = RegisterPowerSettingNotification(hWnd, &GUID_ACDC_POWER_SOURCE, DEVICE_NOTIFY_WINDOW_HANDLE);
	notBatPer = RegisterPowerSettingNotification(hWnd, &GUID_BATTERY_PERCENTAGE_REMAINING, DEVICE_NOTIFY_WINDOW_HANDLE);
	if (notPowerSrc == NULL || notBatPer == NULL)
		JV_ErrorHandle(JVERR_RegisterPowerSettingNotification, TRUE);


	// Window 불투명도 설정
	// if (SetLayeredWindowAttributes(hWnd, 0, 200, LWA_ALPHA) == 0)
	if (SetLayeredWindowAttributes(hWnd, 0, option.transparency, LWA_ALPHA) == 0)
		JV_ErrorHandle(JVERR_SetLayeredWindowAttributes, TRUE);

	if (GetSystemPowerStatus(&stat) == 0)
		JV_ErrorHandle(JVERR_GetSystemPowerStatus, TRUE);
	if (stat.BatteryFlag & 0x80) // Battery가 장착되어 있는가
		JV_ErrorHandle(JVERR_BATTERY_NOT_EXIST, FALSE);

	ShowWindow(hWnd, SW_SHOWNOACTIVATE); // 4
	BLDL_SetWindowPos(hWnd, &stat);
	if (UpdateWindow(hWnd) == 0)
		JV_ErrorHandle(JVERR_UpdateWindow, FALSE);

	return hWnd;
}

void BLDL_SetWindowPos(HWND hWnd, SYSTEM_POWER_STATUS* stat)
{
	int scr_x = 0, scr_y = 0;
	int scr_win_x = 0, scr_win_y = 0, scr_batPer = 0;
	HWND hTray = NULL;
	RECT trayPos;

	hTray = FindWindowW(L"Shell_TrayWnd", NULL);
	if (hTray == NULL)
		JV_ErrorHandle(JVERR_FindWindow, TRUE);
	if (GetWindowRect(hTray, &trayPos) == 0)
		JV_ErrorHandle(JVERR_GetWindowRect, TRUE);

//	if (option.monitor == BL_MON_PRIMARY) // Draw to primary monitor
	{
		scr_x = GetSystemMetrics(SM_CXSCREEN);
		scr_y = GetSystemMetrics(SM_CYSCREEN);
	}
/*	else // It goes complex in context of multiple monitor...
	{
		// https://msdn.microsoft.com/en-us/library/windows/desktop/dd144968%28v=vs.85%29.aspx
		EnumDisplayMonitors(NULL, NULL, BLCB_MonEnumProc, 0);
	}
*/
	if (scr_x == 0 || scr_y == 0)
		JV_ErrorHandle(JVERR_GetSystemMetrics, FALSE);

	// Set Position of window
	switch (option.position)
	{
	case BL_POS_TOP:
		if (stat->ACLineStatus == 1 && !(stat->BatteryFlag & 0x08)) // Not Charging, because battery is full
			scr_batPer = scr_x;
		else
			scr_batPer = scr_x * stat->BatteryLifePercent / 100;
		if (option.taskbar != BL_TASKBAR_IGNORE // IGNORE -> behave regardless of taskbar
			&& trayPos.top == 0 && trayPos.left == 0 && trayPos.right == scr_x) // TaskBar is on TOP - conflict
		{
			if (option.taskbar == BL_TASKBAR_EVADE)
				scr_win_y = trayPos.bottom;
			else
				JV_ErrorHandle(JVERR_OPT_INVALID_TASKBAR, FALSE);
		}
		else // TaskBar is on BOTTOM | LEFT | RIGHT
			scr_win_y = 0;
		// Draw
		if (SetWindowPos(hWnd, HWND_TOPMOST, 0, scr_win_y, scr_batPer, option.height, 0) == 0)
			JV_ErrorHandle(JVERR_SetWindowPos, TRUE);
		break;
	case BL_POS_BOTTOM:
		if (stat->ACLineStatus == 1 && !(stat->BatteryFlag & 0x08)) // Not Charging, because battery is full
			scr_batPer = scr_x;
		else
			scr_batPer = scr_x * stat->BatteryLifePercent / 100;
		if (option.taskbar != BL_TASKBAR_IGNORE // IGNORE -> behave regardless of taskbar
			&& trayPos.left == 0 && trayPos.right == scr_x && trayPos.bottom == scr_y) // TaskBar is on BOTTOM - conflict
		{
			if (option.taskbar == BL_TASKBAR_EVADE)
				scr_win_y = trayPos.top - option.height;
			else
				JV_ErrorHandle(JVERR_OPT_INVALID_TASKBAR, FALSE);
		}
		else // TaskBar is on TOP | LEFT | RIGHT
			scr_win_y = scr_y - option.height;
		// Draw
		if (SetWindowPos(hWnd, HWND_TOPMOST, 0, scr_win_y, scr_batPer, option.height, 0) == 0)
			JV_ErrorHandle(JVERR_SetWindowPos, TRUE);
		break;
	case BL_POS_LEFT:
		if (stat->ACLineStatus == 1 && !(stat->BatteryFlag & 0x08)) // Not Charging, because battery is full
			scr_batPer = scr_y;
		else
			scr_batPer = scr_y * stat->BatteryLifePercent / 100;
		if (option.taskbar != BL_TASKBAR_IGNORE // IGNORE -> behave regardless of taskbar
			&& trayPos.top == 0 && trayPos.left == 0 && trayPos.bottom == scr_y) // TaskBar is on LEFT - conflict
		{
			if (option.taskbar == BL_TASKBAR_EVADE)
				scr_win_x = trayPos.right;
			else
				JV_ErrorHandle(JVERR_OPT_INVALID_TASKBAR, FALSE);
		}
		else // TaskBar is on TOP | BOTTOM | RIGHT
			scr_win_x = 0;
		// Draw
		if (SetWindowPos(hWnd, HWND_TOPMOST, scr_win_x, scr_y - scr_batPer, option.height, scr_batPer, 0) == 0)
			JV_ErrorHandle(JVERR_SetWindowPos, TRUE);
		break;
	case BL_POS_RIGHT:
		if (stat->ACLineStatus == 1 && !(stat->BatteryFlag & 0x08)) // Not Charging, because battery is full
			scr_batPer = scr_y;
		else
			scr_batPer = scr_y * stat->BatteryLifePercent / 100;
		if (option.taskbar != BL_TASKBAR_IGNORE // IGNORE -> behave regardless of taskbar
			&& trayPos.top == 0 && trayPos.right == scr_x && trayPos.bottom == scr_y) // TaskBar is on RIGHT - conflict
		{
			if (option.taskbar == BL_TASKBAR_EVADE)
				scr_win_x = trayPos.left - option.height;
			else
				JV_ErrorHandle(JVERR_OPT_INVALID_TASKBAR, FALSE);
		}
		else // TaskBar is on TOP | BOTTOM | LEFT
			scr_win_x = scr_x - option.height;
		// Draw
		if (SetWindowPos(hWnd, HWND_TOPMOST, scr_win_x, scr_y - scr_batPer, option.height, scr_batPer, 0) == 0)
			JV_ErrorHandle(JVERR_SetWindowPos, TRUE);
		break;
	default:
		JV_ErrorHandle(JVERR_OPT_INVALID_POSITION, FALSE);
		break;
	}
}

// Paint color to BatteryLine Window
void BLCB_WM_PAINT(HWND hWnd)
{
	int color_idx = -1;
	RECT linePos;
	PAINTSTRUCT paint;
	HDC hDC, memDC;
	HBITMAP memBM;
	HGDIOBJ original;

	// Get this process' window handle and create DCs
	if (!GetClientRect(hWnd, &linePos))
		JV_ErrorHandle(JVERR_GetClientRect, TRUE);
	hDC = BeginPaint(hWnd, &paint);
	memDC = CreateCompatibleDC(hDC);
	memBM = CreateCompatibleBitmap(hDC, linePos.right, linePos.bottom);
	original = SelectObject(memDC, memBM);

	// Get System Power Status
	if (!GetSystemPowerStatus(&stat))
		JV_ErrorHandle(JVERR_GetSystemPowerStatus, TRUE);

	// Set Line Color
	if ((stat.BatteryFlag & 0x08) && option.showcharge == TRUE) // Charging, and show charge color option set
		SetBkColor(memDC, option.chargecolor);
	else if (stat.ACLineStatus == 1) // Not Charging, because battery is full
		SetBkColor(memDC, option.fullcolor); // Even though BatteryLifePercent is not 100, consider it as 100
	else if (stat.ACLineStatus == 0) // Not Charging, running on battery
	{
		for (uint32_t i = 0; i < BL_COLOR_LEVEL; i++)
		{
			if (!(option.threshold[2*i] == 0 && option.threshold[2*i+1] == 0)  // 0,0 -> blank
				&& option.threshold[2*i] <= stat.BatteryLifePercent && stat.BatteryLifePercent <= option.threshold[2*i+1])// Ex 20 <= '45' <= 50
				color_idx = i;
		}

		if (color_idx != -1)
			SetBkColor(memDC, option.color[color_idx]);
		else
			SetBkColor(memDC, option.defaultcolor);
	}

	// Paint it!
	ExtTextOutW(memDC, 0, 0, ETO_OPAQUE, &linePos, NULL, 0, 0);
	BitBlt(hDC, 0, 0, linePos.right, linePos.bottom, memDC, 0, 0, SRCCOPY);
	SelectObject(memDC, original);

	// Delete DCs
	DeleteDC(memDC);
	DeleteObject(memBM);
	DeleteDC(hDC);

	// End of Paint
	EndPaint(hWnd, &paint);
}

// When Power Status is refreshed, this callback is called to update window's length.
void BLCB_WM_POWERBROADCAST(HWND hWnd)
{
	if (!GetSystemPowerStatus(&stat))
		JV_ErrorHandle(JVERR_GetSystemPowerStatus, TRUE);
	if (stat.BatteryFlag == 128) // No Battery in this System!
	{
		if (!IsWindowVisible(hWnd))
			ShowWindow(hWnd, SW_HIDE);
		// return 1;
	}
	else // Has Battery in this system
	{
		if (!IsWindowVisible(hWnd))
			ShowWindow(hWnd, SW_SHOWNOACTIVATE);
		BLDL_SetWindowPos(hWnd, &stat);
		InvalidateRect(hWnd, NULL, TRUE);
	}
}


void BLCB_WM_CLOSE(HWND hWnd, uint8_t postquit)
{
	BLDL_DelTrayIcon(hWnd, BL_SysTrayID_ON);
	BLDL_AddTrayIcon(hWnd, BL_SysTrayID_OFF, NIF_INFO, 0, L"BatteryLine OFF");
	BLDL_DelTrayIcon(hWnd, BL_SysTrayID_OFF);

	UnregisterPowerSettingNotification(not_bat_per);
	UnregisterPowerSettingNotification(not_power_src);

	if (postquit)
		PostQuitMessage(WM_QUIT); // 원래는 WM_QUIT가 들어 있었다?
}

/*
BOOL CALLBACK BLCB_MonEnumProc_Count(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{	//lprcMonitor holds the rectangle that describes the monitor position and resolution)
	g_nMon++;
	return TRUE;
}

BOOL CALLBACK BLCB_MonEnumProc_GetRes(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{	//lprcMonitor holds the rectangle that describes the monitor position and resolution)
	;
	return TRUE;
}
*/

// Right click BatteryLine icon in Notification area
BOOL BLDL_ShowPopupMenu( HWND hWnd, POINT *curpos, int wDefaultItem )
{

	//ADD MENUITEMS
	HMENU hPopMenu = CreatePopupMenu();
	InsertMenuW(hPopMenu, 0, MF_BYPOSITION | MF_STRING, ID_ABOUT, L"About");
	InsertMenuW(hPopMenu, 1, MF_BYPOSITION | MF_STRING, ID_SETTING, L"Setting");
	InsertMenuW(hPopMenu, 2, MF_BYPOSITION | MF_STRING, ID_POWERINFO, L"Power Info");
	InsertMenuW(hPopMenu, 3, MF_BYPOSITION | MF_STRING, ID_EXIT, L"Exit");

	SetMenuDefaultItem(hPopMenu, ID_ABOUT, FALSE);
	SetFocus(hWnd);
	SendMessage(hWnd, WM_INITMENUPOPUP, (WPARAM)hPopMenu, 0);

	// SHOW POPUPMENU
	// GET CURSOR POSITION TO CREATE POPUP THERE
	POINT pt;
	if (!curpos)
	{
		GetCursorPos(&pt);
		curpos =&pt;
	}

	WORD cmd = TrackPopupMenu(hPopMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD | TPM_NONOTIFY, curpos->x, curpos->y, 0,hWnd, NULL);
	SendMessage(hWnd, WM_COMMAND, cmd, 0);

	DestroyMenu(hPopMenu);

	return 0;
}

void BLDL_AddTrayIcon(HWND hWnd, UINT uID, UINT flag, UINT uCallbackMsg, LPCWSTR lpInfoStr)
{
	NOTIFYICONDATA nid;
	ZeroMemory(&nid, sizeof(NOTIFYICONDATA));

	// Notification Icon

	nid.cbSize 		= sizeof(NOTIFYICONDATA);
	nid.hWnd 		= hWnd;
	nid.uID 		= uID;
	nid.uFlags 		= NIF_ICON | flag;
	if (uCallbackMsg != 0) // Message 안 던지고 처리
		nid.uCallbackMessage = uCallbackMsg;
#ifdef _MSC_VER
	LoadIconMetric(g_hInst, MAKEINTRESOURCEW(IDI_MAINICON), LIM_SMALL, &(nid.hIcon)); // Load the icon for high DPI.
#else
	nid.hIcon 		= (HICON) LoadImageW(g_hInst, MAKEINTRESOURCEW(IDI_MAINICON), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
#endif
	StringCchCopyW(nid.szTip, ARRAYSIZE(nid.szTip), BL_SysTrayTip);
	StringCchCopyW(nid.szInfo, ARRAYSIZE(nid.szInfo), lpInfoStr);
	nid.uVersion 	= NOTIFYICON_VERSION_4;

	Shell_NotifyIcon(NIM_SETVERSION, &nid);
	Shell_NotifyIcon(NIM_ADD, &nid);
}

void BLDL_DelTrayIcon(HWND hWnd, UINT uID)
{
	NOTIFYICONDATA nid;

	nid.hWnd = hWnd;
	nid.uID = uID;

	Shell_NotifyIcon(NIM_DELETE, &nid);
}

// https://msdn.microsoft.com/en-us/library/windows/desktop/ee330740%28v=vs.85%29.aspx#install_icon
