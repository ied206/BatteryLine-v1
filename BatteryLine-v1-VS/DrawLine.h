﻿#pragma once

#define ID_ABOUT		2000
#define ID_HELP			2001
#define ID_HOMEPAGE		2010
#define ID_LICENSE		2011
#define ID_SETTING		2020
#define ID_POWERINFO	2021
#define ID_EXIT			2030

HWND BLDL_InitWindow(HINSTANCE hInstance);
void BLDL_SetWindowPos(HWND hWnd, SYSTEM_POWER_STATUS* stat);
void BLCB_WM_PAINT(HWND hWnd);
void BLCB_SetWindowPos(HWND hWnd);
void BLDL_SetTimer(hWnd);
void BLCB_WM_CLOSE(HWND hWnd, uint8_t postquit);
BOOL BLDL_ShowPopupMenu(HWND hWnd, POINT *curpos, int wDefaultItem);

BOOL CALLBACK BLCB_MonEnumProc_Count(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData);
BOOL CALLBACK BLCB_MonEnumProc_GetRes(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData);
BOOL CALLBACK BLCB_MonEnumProc_GetFullInfo(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData);

void BLDL_AddTrayIcon(HWND hWnd, UINT uID, UINT flag, UINT uCallbackMsg, LPCWSTR lpInfoStr);
void BLDL_DelTrayIcon(HWND hWnd, UINT uID);

void BLCB_OpenSettingIni(HWND hWnd);
void BLDL_OpenHomepage(HWND hWnd);
void BLDL_OpenLicense(HWND hWnd);
void BLDL_PrintBanner(HWND hWnd);
void BLDL_PrintHelp(HWND hWnd);

