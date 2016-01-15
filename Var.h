#ifndef VAR_H_INCLUDED
#define VAR_H_INCLUDED

// #include "Var.h" must be included at very top of .c file

#define UNICODE
#define _UNICODE
#define OEMRESOURCE
#define NTDDI_VERSION NTDDI_VISTA
#define _WIN32_WINNT _WIN32_WINNT_VISTA

#include <windows.h>
#include <stdint.h>

#ifdef _DEBUG
	#define _DEBUG_CONSOLE
#endif

#define TRUE	1
#define FALSE	0

// BL for BatteryLine
#define BL_WindowName 	L"Joveler\'s BatteryLine"
#define BL_ClassName	L"Joveler_BatteryLine"
#define BL_SysTrayTip   L"BatteryLine"
#define BL_SettingFile 	L"BatteryLine.ini"
#define BL_SysTrayID_ON		1
#define BL_SysTrayID_OFF	2

#define BL_MAJOR_VER		1
#define BL_MINOR_VER		0
#define BL_MSGBOX_BUF_SIZE	2048

#define WM_APP_SYSTRAY_POPUP (WM_APP + 0x0001)

#define BL_COLOR_LEVEL		16
#define BL_MON_PRIMARY 		0
#define BL_POS_TOP 			1
#define BL_POS_BOTTOM 		2
#define BL_POS_LEFT 		3
#define BL_POS_RIGHT 		4
#define BL_TASKBAR_IGNORE	0
#define BL_TASKBAR_EVADE	1

// BS for Battery Status
#define BS_TOKEN_BUF_SIZE	16
#define BS_LINE_BUF_SIZE	512
#define BS_STRING_BUF_SIZE	2048

#define BS_SECTION_OFF		0
#define BS_SECTION_SETTING	1
#define BS_SECTION_COLOR	2

#define BS_OPT_SHOWCHARGE	1
#define BS_OPT_MONITOR		2
#define BS_OPT_POSITION 	3
#define BS_OPT_TASKBAR		4
#define BS_OPT_TRANSPARENCY	5
#define BS_OPT_HEIGHT		6

#define BS_OPT_DEFAULTCOLOR	100
#define BS_OPT_CHARGECOLOR	101
#define BS_OPT_COLOR		102

// BLU for BatteryLine Utlities
#define BLU_RGB_R(X)	((X) % 0x100)
#define BLU_RGB_G(X)	(((X) % 0x10000) / 0x100)
#define BLU_RGB_B(X)	(((X) % 0x1000000) / 0x10000)


struct bl_option
{
	uint8_t showcharge;	// 충전할 때 계속 표시할까
	uint8_t monitor;		// 어떤 모니터에 표시할까
	uint8_t position;		// 어디에 표시할까
	uint8_t taskbar;		// 작업표시줄이랑 겹치면 회피할까?
	uint8_t transparency;	// 투명도
	uint8_t height;		// 배터리 표시줄 두께, In Pixel
	COLORREF defaultcolor;	// 배터리 표시줄 기본 색
	COLORREF chargecolor;	// 배터리 표시줄 충전시 색
	COLORREF fullcolor;	// 배터리 표시줄 충전시 색
	COLORREF color[BL_COLOR_LEVEL];			// 배터리 표시줄 색
	uint8_t threshold[BL_COLOR_LEVEL*2];	// 배터리 색 경계
};
typedef struct bl_option BL_OPTION;

struct bl_monres
{
	int scr_x;
	int scr_y;
};
typedef struct bl_monres BL_MONRES;

extern HANDLE not_power_src, not_bat_per;
extern SYSTEM_POWER_STATUS stat;
extern BL_OPTION option;
extern HINSTANCE g_hInst;
extern int g_nMon; // Number of monitors installed on this system
extern BL_MONRES* monres;
// http://www.transmissionzero.co.uk/computing/win32-apps-with-mingw/

#endif // VAR_H_INCLUDED
