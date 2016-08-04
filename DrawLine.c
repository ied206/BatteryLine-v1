#include "Var.h"
#include "rc\resource.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <windows.h>
#include <shellapi.h>
#include <commctrl.h>
#ifdef _DEBUG
#undef __CRT__NO_INLINE
#endif
#include <strsafe.h>
#ifdef _DEBUG
#define __CRT__NO_INLINE
#endif

#include "DrawLine.h"
#include "BatStat.h"
#include "ErrorHandle.h"
#include "BasicIO.h"


LRESULT CALLBACK WndProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
extern BL_ARG g_arg;
extern HWND g_hWnd;

HWND BLDL_InitWindow(HINSTANCE hInstance)
{
	HWND hWnd;
	HANDLE notPowerSrc, notBatPer;
	WNDCLASSEXW WndClsEx;

	// Create Window
	ZeroMemory(&WndClsEx, sizeof(WNDCLASSEXW));
	WndClsEx.cbSize 		= sizeof(WNDCLASSEXW);
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

	// Set window Transparency
	if (SetLayeredWindowAttributes(hWnd, 0, option.transparency, LWA_ALPHA) == 0)
		JV_ErrorHandle(JVERR_SetLayeredWindowAttributes, TRUE);

	// Init first window
	if (GetSystemPowerStatus(&stat) == 0)
		JV_ErrorHandle(JVERR_GetSystemPowerStatus, TRUE);
	ShowWindow(hWnd, SW_SHOWNOACTIVATE);
	BLDL_SetWindowPos(hWnd, &stat);
	if (UpdateWindow(hWnd) == 0)
		JV_ErrorHandle(JVERR_UpdateWindow, FALSE);

	return hWnd;
}

void BLDL_SetWindowPos(HWND hWnd, SYSTEM_POWER_STATUS* stat)
{
	int32_t scr_x = 0, scr_y = 0; // full resolution of monitor
	int32_t base_x = 0, base_y = 0; // base virtual coord of monitor
	int32_t scr_win_x = 0, scr_win_y = 0;
	int32_t scr_batPer = 0;
	uint32_t taskbar = 0;
	RECT trayPos;

	if (option.monitor == BL_MON_PRIMARY) // Draw to primary monitor
	{
		// Get primary monitor's screen resolution
		scr_x = GetSystemMetrics(SM_CXSCREEN);
		scr_y = GetSystemMetrics(SM_CYSCREEN);
		// Primary monitor's virtual coordinate is always (0,0)
		base_x = 0;
		base_y = 0;

		HWND hTray = FindWindowW(L"Shell_TrayWnd", NULL);
		if (hTray == NULL)
			JV_ErrorHandle(JVERR_FindWindow, TRUE);
		if (GetWindowRect(hTray, &trayPos) == 0)
			JV_ErrorHandle(JVERR_GetWindowRect, TRUE);

		// Calculate where taskbar is, using rcWork and rcMonitor's relation.
		if (trayPos.top == 0 && trayPos.left == 0 && trayPos.right == scr_x)
			taskbar = BL_TASKBAR_TOP;
		else if (trayPos.left == 0 && trayPos.right == scr_x && trayPos.bottom == scr_y)
			taskbar = BL_TASKBAR_BOTTOM;
		else if (trayPos.top == 0 && trayPos.left == 0 && trayPos.bottom == scr_y)
			taskbar = BL_TASKBAR_LEFT;
		else if (trayPos.top == 0 && trayPos.right == scr_x && trayPos.bottom == scr_y)
			taskbar = BL_TASKBAR_RIGHT;

		if (scr_x == 0 || scr_y == 0)
			JV_ErrorHandle(JVERR_GetSystemMetrics, FALSE);
	}
	else // Draw to n'th monitor
	{
		g_nMon = 0;
		ZeroMemory(&g_monInfo, sizeof(MONITORINFO) * BL_MAX_MONITOR);
		// EnumDisplayMonitors(NULL, NULL, BLCB_MonEnumProc_GetRes, 0);
		EnumDisplayMonitors(NULL, NULL, BLCB_MonEnumProc_GetFullInfo, 0);

		// option.monitor is pointing non-exsit monitor
		if (!(option.monitor <= g_nMon))
		{
			option.monitor = BL_MON_PRIMARY;
			JV_WarnHandle(JVWARN_OPT_INVALID_MONITOR, FALSE);
		}

		// Get this monitor's screen resolution
		scr_x = g_monInfo[option.monitor-1].rcMonitor.right - g_monInfo[option.monitor-1].rcMonitor.left;
		scr_y = g_monInfo[option.monitor-1].rcMonitor.bottom - g_monInfo[option.monitor-1].rcMonitor.top;
		// Non-primary monitor's virtual coordinate can be negative value
		base_x = g_monInfo[option.monitor-1].rcMonitor.left;
		base_y = g_monInfo[option.monitor-1].rcMonitor.top;



		// Calculate where taskbar is, using rcWork and rcMonitor's relation.
		// trayPos is calculated as taskbar is in PRIMARY, since coord is calculated relative to base coord of monitor.
		if (g_monInfo[option.monitor-1].rcMonitor.top != g_monInfo[option.monitor-1].rcWork.top)
		{
			// TaskBar is on TOP
			taskbar = BL_TASKBAR_TOP;
			trayPos.top 	= 0;
			trayPos.bottom 	= g_monInfo[option.monitor-1].rcWork.top - g_monInfo[option.monitor-1].rcMonitor.top;
			trayPos.left 	= 0;
			trayPos.right 	= scr_x;
		}
		else if (g_monInfo[option.monitor-1].rcMonitor.bottom != g_monInfo[option.monitor-1].rcWork.bottom)
		{
			// TaskBar is on BOTTOM
			taskbar = BL_TASKBAR_BOTTOM;
			trayPos.top 	= g_monInfo[option.monitor-1].rcWork.bottom - g_monInfo[option.monitor-1].rcMonitor.top;
			trayPos.bottom 	= scr_y;
			trayPos.left 	= 0;
			trayPos.right 	= scr_x;
		}
		else if (g_monInfo[option.monitor-1].rcMonitor.left != g_monInfo[option.monitor-1].rcWork.left)
		{
			// TaskBar is on LEFT
			taskbar = BL_TASKBAR_LEFT;
			trayPos.top 	= 0;
			trayPos.bottom 	= scr_y;
			trayPos.left 	= 0;
			trayPos.right 	= g_monInfo[option.monitor-1].rcWork.left - g_monInfo[option.monitor-1].rcMonitor.left;
		}
		else if (g_monInfo[option.monitor-1].rcMonitor.right != g_monInfo[option.monitor-1].rcWork.right)
		{
			// TaskBar is on RIGHT
			taskbar = BL_TASKBAR_RIGHT;
			trayPos.top 	= 0;
			trayPos.bottom 	= scr_y;
			trayPos.left 	= g_monInfo[option.monitor-1].rcWork.right - g_monInfo[option.monitor-1].rcMonitor.left;
			trayPos.right 	= scr_x;
		}

#ifdef _DEBUG_MONITOR
		puts("[LineDisplay]");
		printf("Displaying on monitor %d\n", option.monitor - 1);
		printf("Screen Resolution      : (%d, %d)\n", scr_x, scr_y);
		printf("Base Coordinate        : (%d, %d)\n", base_x, base_y);
		printf("Taskbar (left,top)     : (%ld, %ld)\n", trayPos.left, trayPos.top);
		printf("Taskbar (right,bottom) : (%ld, %ld)\n", trayPos.right, trayPos.bottom);
		putchar('\n');
#endif // _DEBUG_MONITOR
	}

	// Set Position of window
	switch (option.position)
	{
	case BL_POS_TOP:
		// Calculate batter percent relative to screen resolution
		if (stat->ACLineStatus == 1 && !(stat->BatteryFlag & 0x08)) // Not Charging, because battery is full
			scr_batPer = scr_x;
		else
			scr_batPer = ((scr_x * stat->BatteryLifePercent) / 100);

		// Only for primary monitor. Evade taskbar logic.
		if (option.taskbar != BL_TASKBAR_IGNORE // IGNORE -> behave regardless of taskbar
				&& taskbar == BL_TASKBAR_TOP) // TaskBar is on TOP - conflict
		{
			if (option.taskbar == BL_TASKBAR_EVADE)
				scr_win_y = trayPos.bottom;
			else
				JV_ErrorHandle(JVERR_OPT_INVALID_TASKBAR, FALSE);
		}
		else // TaskBar is on BOTTOM | LEFT | RIGHT, or Monitor is not primary
			scr_win_y = 0;
		// Draw
		if (SetWindowPos(hWnd, HWND_TOPMOST, base_x + 0, base_y + scr_win_y, scr_batPer, option.height, 0) == 0)
			JV_ErrorHandle(JVERR_SetWindowPos, TRUE);
		break;
	case BL_POS_BOTTOM:
		// Calculate batter percent relative to screen resolution
		if (stat->ACLineStatus == 1 && !(stat->BatteryFlag & 0x08)) // Not Charging, because battery is full
			scr_batPer = scr_x;
		else
			scr_batPer = scr_x * stat->BatteryLifePercent / 100;

		// Only for primary monitor. Evade taskbar logic.
		if (option.taskbar != BL_TASKBAR_IGNORE // IGNORE -> behave regardless of taskbar
				&& taskbar == BL_TASKBAR_BOTTOM) // TaskBar is on BOTTOM - conflict
		{
			if (option.taskbar == BL_TASKBAR_EVADE)
				scr_win_y = trayPos.top - option.height;
			else
				JV_ErrorHandle(JVERR_OPT_INVALID_TASKBAR, FALSE);
		}
		else // TaskBar is on TOP | LEFT | RIGHT, or Monitor is not primary
			scr_win_y = scr_y - option.height;
		// Draw
		if (SetWindowPos(hWnd, HWND_TOPMOST, base_x + 0, base_y + scr_win_y, scr_batPer, option.height, 0) == 0)
			JV_ErrorHandle(JVERR_SetWindowPos, TRUE);
		break;
	case BL_POS_LEFT:
		// Calculate batter percent relative to screen resolution
		if (stat->ACLineStatus == 1 && !(stat->BatteryFlag & 0x08)) // Not Charging, because battery is full
			scr_batPer = scr_y;
		else
			scr_batPer = scr_y * stat->BatteryLifePercent / 100;

		// Only for primary monitor. Evade taskbar logic.
		if (option.taskbar != BL_TASKBAR_IGNORE // IGNORE -> behave regardless of taskbar
				&& taskbar == BL_TASKBAR_LEFT) // TaskBar is on LEFT - conflict
		{
			if (option.taskbar == BL_TASKBAR_EVADE)
				scr_win_x = trayPos.right;
			else
				JV_ErrorHandle(JVERR_OPT_INVALID_TASKBAR, FALSE);
		}
		else // TaskBar is on TOP | BOTTOM | RIGHT, or Monitor is not primary
			scr_win_x = 0;
		// Draw
		if (SetWindowPos(hWnd, HWND_TOPMOST, base_x + scr_win_x, base_y + scr_y - scr_batPer, option.height, scr_batPer, 0) == 0)
			JV_ErrorHandle(JVERR_SetWindowPos, TRUE);
		break;
	case BL_POS_RIGHT:
		// Calculate batter percent relative to screen resolution
		if (stat->ACLineStatus == 1 && !(stat->BatteryFlag & 0x08)) // Not Charging, because battery is full
			scr_batPer = scr_y;
		else
			scr_batPer = scr_y * stat->BatteryLifePercent / 100;

		// Only for primary monitor. Evade taskbar logic.
		if (option.taskbar != BL_TASKBAR_IGNORE // IGNORE -> behave regardless of taskbar
				&& taskbar == BL_TASKBAR_RIGHT) // TaskBar is on RIGHT - conflict
		{
			if (option.taskbar == BL_TASKBAR_EVADE)
				scr_win_x = trayPos.left - option.height;
			else
				JV_ErrorHandle(JVERR_OPT_INVALID_TASKBAR, FALSE);
		}
		else // TaskBar is on TOP | BOTTOM | LEFT, or Monitor is not primary
			scr_win_x = scr_x - option.height;
		// Draw
		if (SetWindowPos(hWnd, HWND_TOPMOST, base_x + scr_win_x, base_y + scr_y - scr_batPer, option.height, scr_batPer, 0) == 0)
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
					&& option.threshold[2*i] <= stat.BatteryLifePercent && stat.BatteryLifePercent <= option.threshold[2*i+1]) // Ex 20 <= '45' <= 50
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
void BLCB_SetWindowPos(HWND hWnd)
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

	UnregisterPowerSettingNotification(not_bat_per);
	UnregisterPowerSettingNotification(not_power_src);

	if (postquit)
		PostQuitMessage(WM_QUIT);
}

// It is assumed that g_monInfo, g_nMon is set to 0 before calling EnumDisplayMonitors
BOOL CALLBACK BLCB_MonEnumProc_Count(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
	// It is assumed that g_nMon is init to 0 before calling EnumDisplayMonitors
	g_nMon++;

	return TRUE;
}

// It is assumed that g_monInfo, g_nMon is set to 0 before calling EnumDisplayMonitors
// _GetRes is faster, but cannot know which monitor is primary monitor
// _GetFullInfo takes a little more time, but it can know which monitor is primary monitor and taskbar position
BOOL CALLBACK BLCB_MonEnumProc_GetRes(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
	// It is assumed that g_monInfo is init to 0 using ZeroMemory before calling EnumDisplayMonitors
	// ZeroMemory(&g_monInfo, sizeof(MONITORINFO) * BL_MAX_MONITOR);

	// lprcMonitor holds this monitor's virtual-screen coordinates
	g_monInfo[g_nMon].rcMonitor.top 	= lprcMonitor->top;
	g_monInfo[g_nMon].rcMonitor.bottom 	= lprcMonitor->bottom;
	g_monInfo[g_nMon].rcMonitor.left 	= lprcMonitor->left;
	g_monInfo[g_nMon].rcMonitor.right 	= lprcMonitor->right;

#ifdef _DEBUG_MONITOR
	printf("[Monitor %d]\n", g_nMon);
	printf("res_x        : %ld\n", g_monInfo[g_nMon].rcMonitor.right - g_monInfo[g_nMon].rcMonitor.left);
	printf("res_y        : %ld\n", g_monInfo[g_nMon].rcMonitor.bottom - g_monInfo[g_nMon].rcMonitor.top);
	printf("left-top     : (%ld, %ld)\n", g_monInfo[g_nMon].rcMonitor.left, g_monInfo[g_nMon].rcMonitor.top);
	printf("right-bottom : (%ld, %ld)\n", g_monInfo[g_nMon].rcMonitor.right, g_monInfo[g_nMon].rcMonitor.bottom);
	putchar('\n');
#endif

	// It is assumed that g_nMon is init to 0 before calling EnumDisplayMonitors
	g_nMon++;
	// _GetRes can not inspect which monitor is primary, so set it to -1
	g_nPriMon = -1;

	return TRUE;
}

// It is assumed that g_monInfo, g_nMon is set to 0 before calling EnumDisplayMonitors
// _GetRes is faster, but cannot know which monitor is primary monitor
// _GetFullInfo takes a little more time, but it can know which monitor is primary monitor and taskbar position
BOOL CALLBACK BLCB_MonEnumProc_GetFullInfo(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
	// It is assumed that g_monInfo is init to 0 using ZeroMemory before calling EnumDisplayMonitors
	// ZeroMemory(&g_monInfo, sizeof(MONITORINFO) * BL_MAX_MONITOR);

	// Get monitor info
	g_monInfo[g_nMon].cbSize = sizeof(MONITORINFO);
	GetMonitorInfoW(hMonitor, &g_monInfo[g_nMon]);

	if (g_monInfo[g_nMon].dwFlags == MONITORINFOF_PRIMARY)
		g_nPriMon = g_nMon;

#ifdef _DEBUG_MONITOR
	printf("[Monitor %d]\n", g_nMon);
	printf("res_x            : %ld\n", g_monInfo[g_nMon].rcMonitor.right - g_monInfo[g_nMon].rcMonitor.left);
	printf("res_y            : %ld\n", g_monInfo[g_nMon].rcMonitor.bottom - g_monInfo[g_nMon].rcMonitor.top);
	printf("left-top         : (%ld, %ld)\n", g_monInfo[g_nMon].rcMonitor.left, g_monInfo[g_nMon].rcMonitor.top);
	printf("right-bottom     : (%ld, %ld)\n", g_monInfo[g_nMon].rcMonitor.right, g_monInfo[g_nMon].rcMonitor.bottom);
	printf("left-top     (W) : (%ld, %ld)\n", g_monInfo[g_nMon].rcWork.left, g_monInfo[g_nMon].rcWork.top);
	printf("right-bottom (W) : (%ld, %ld)\n", g_monInfo[g_nMon].rcWork.right, g_monInfo[g_nMon].rcWork.bottom);
	printf("primary      : %d\n", (g_monInfo[g_nMon].dwFlags == MONITORINFOF_PRIMARY) ? TRUE : FALSE);
	putchar('\n');
#endif

	// It is assumed that g_nMon is init to 0 before calling EnumDisplayMonitors
	g_nMon++;

	return TRUE;
}

// Right click BatteryLine icon in Notification Area
BOOL BLDL_ShowPopupMenu(HWND hWnd, POINT *curpos, int wDefaultItem)
{
	// Add Menu Items
	HMENU hPopMenu = CreatePopupMenu();
	InsertMenuW(hPopMenu, 0, MF_BYPOSITION | MF_STRING, ID_ABOUT, L"About");
	InsertMenuW(hPopMenu, 1, MF_BYPOSITION | MF_STRING, ID_HELP, L"Help");
	InsertMenuW(hPopMenu, 10, MF_BYPOSITION | MF_SEPARATOR, 0, NULL);
	InsertMenuW(hPopMenu, 11, MF_BYPOSITION | MF_STRING, ID_HOMEPAGE, L"Homepage");
	InsertMenuW(hPopMenu, 12, MF_BYPOSITION | MF_STRING, ID_LICENSE, L"License");
	InsertMenuW(hPopMenu, 20, MF_BYPOSITION | MF_SEPARATOR, 0, NULL);
	InsertMenuW(hPopMenu, 21, MF_BYPOSITION | MF_STRING, ID_SETTING, L"Setting");
	InsertMenuW(hPopMenu, 22, MF_BYPOSITION | MF_STRING, ID_POWERINFO, L"Power Info");
	InsertMenuW(hPopMenu, 30, MF_BYPOSITION | MF_SEPARATOR, 0, NULL);
	InsertMenuW(hPopMenu, 31, MF_BYPOSITION | MF_STRING, ID_EXIT, L"Exit");

	SetMenuDefaultItem(hPopMenu, ID_ABOUT, FALSE);
	SetFocus(hWnd);
	SendMessage(hWnd, WM_INITMENUPOPUP, (WPARAM)hPopMenu, 0);

	// Show Popup Menu
	POINT pt;
	if (!curpos)
	{
		GetCursorPos(&pt);
		curpos = &pt;
	}

	WORD cmd = TrackPopupMenu(hPopMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD | TPM_NONOTIFY, curpos->x, curpos->y, 0,hWnd, NULL);
	SendMessage(hWnd, WM_COMMAND, cmd, 0);

	DestroyMenu(hPopMenu);

	return 0;
}

void BLDL_AddTrayIcon(HWND hWnd, UINT uID, UINT flag, UINT uCallbackMsg, LPCWSTR lpInfoStr)
{
	NOTIFYICONDATAW nid;
	ZeroMemory(&nid, sizeof(NOTIFYICONDATAW));

	// Notification Icon
	nid.cbSize 		= sizeof(NOTIFYICONDATAW);
	nid.hWnd 		= hWnd;
	nid.uID 		= uID;
	nid.uFlags 		= NIF_ICON | flag;
	nid.dwInfoFlags = NIIF_NOSOUND | NIIF_USER;

	// Don't throw an message
	if (uCallbackMsg)
		nid.uCallbackMessage = uCallbackMsg;

#ifdef _MSC_VER
	LoadIconMetric(g_hInst, MAKEINTRESOURCEW(IDI_MAINICON), LIM_SMALL, &(nid.hIcon)); // Load the icon for high DPI. However, MinGW-w64 cannot link this function...
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

void BLCB_OpenSettingIni(HWND hWnd)
{
	wchar_t pathBuf[MAX_PATH] = {0};
	BLBS_GetIniFullPath(pathBuf, sizeof(pathBuf));
	// Open BatteryLine.ini
	ShellExecuteW(hWnd, L"open", pathBuf, NULL, NULL, SW_SHOW);
}

void BLDL_OpenHomepage(HWND hWnd)
{
	// Open GitHub repository's LICENSE page
	ShellExecuteW(hWnd, L"open", BL_WebBinary, NULL, NULL, SW_SHOW);
}

void BLDL_OpenLicense(HWND hWnd)
{
	// Open GitHub repository's LICENSE page
	ShellExecuteW(hWnd, L"open", L"https://github.com/ied206/BatteryLine/blob/master/LICENSE", NULL, NULL, SW_SHOW);
}

void BLDL_PrintBanner(HWND hWnd)
{
	wchar_t msg[BL_MSGBOX_BUF_SIZE];

	StringCchPrintfW(msg, BL_MSGBOX_BUF_SIZE,
					 L"Joveler's BatteryLine v%d.%d (%dbit)\n"
					 L"Show battery status as line in screen.\n\n"
					 L"[Binary] %s\n"
					 L"[Source] %s\n\n"
					 L"Build %04d%02d%02d",
					 BL_MAJOR_VER, BL_MINOR_VER, WhatBitOS(),
					 BL_WebBinary, BL_WebSource,
					 CompileYear(), CompileMonth(), CompileDay());
	printf("%S\n\n", msg);
	MessageBoxW(hWnd, msg, L"BatteryLine", MB_ICONINFORMATION | MB_OK);
}

void BLDL_PrintHelp(HWND hWnd)
{
	wchar_t* msg = 	L"[BatteryLine Help Message]\n"
					L"Show battery status as line in screen.\n\n"
					L"[Command Line Option]\n"
					L"-q : Launch this program without notification.\n"
					L"-h : Print this help message and exit.\n\n"
					L"[Setting]\n"
					L"You can edit BatteryLine's setting in BatteryLine.ini.";
	printf("%S\n\n", msg);
	MessageBoxW(hWnd, msg, L"BatteryLine", MB_ICONINFORMATION | MB_OK);
}
