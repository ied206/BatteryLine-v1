#ifndef DRAWLINE_H_INCLUDED
#define DRAWLINE_H_INCLUDED

#define ID_ABOUT		2000
#define ID_HELP			2001
#define ID_LICENSE		2002
#define ID_SETTING		2003
#define ID_POWERINFO	2004
#define ID_EXIT			2005

HWND BLDL_InitWindow(HINSTANCE hInstance);
void BLDL_SetWindowPos(HWND hWnd, SYSTEM_POWER_STATUS* stat);
void BLCB_WM_PAINT(HWND hWnd);
void BLCB_SetWindowPos(HWND hWnd);
void BLCB_WM_CLOSE(HWND hWnd, uint8_t postquit);
BOOL BLDL_ShowPopupMenu(HWND hWnd, POINT *curpos, int wDefaultItem);

BOOL CALLBACK BLCB_MonEnumProc_Count(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData);
BOOL CALLBACK BLCB_MonEnumProc_GetRes(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData);
BOOL CALLBACK BLCB_MonEnumProc_GetFullInfo(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData);

void BLDL_AddTrayIcon(HWND hWnd, UINT uID, UINT flag, UINT uCallbackMsg, LPCWSTR lpInfoStr);
void BLDL_DelTrayIcon(HWND hWnd, UINT uID);

void BLCB_OpenSettingIni(HWND hWnd);
void BLDL_OpenLicense(HWND hWnd);
void BLDL_PrintBanner(HWND hWnd);
void BLDL_PrintHelp(HWND hWnd);

#endif // DRAWLINE_H_INCLUDED
