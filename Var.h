#ifndef VAR_H_INCLUDED
#define VAR_H_INCLUDED

// #include "Var.h" must be included at very top of .c file

// These directives are for Windows SDK headers
#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif
#define OEMRESOURCE
#define NTDDI_VERSION NTDDI_VISTA
#define _WIN32_WINNT _WIN32_WINNT_VISTA

#include <windows.h>
#include <stdint.h>

// MACRO for DEBUG
#ifdef _DEBUG
	#define _DEBUG_CONSOLE
	//#define _DEBUG_PARSING
	//#define _DEBUG_MONITOR
#endif

#define TRUE	1
#define FALSE	0

// BL for BatteryLine
#define BL_WindowName 		L"Joveler\'s BatteryLine"
#define BL_ClassName		L"Joveler_BatteryLine"
#define BL_SysTrayTip   	L"BatteryLine"
#define BL_SettingFile 		L"BatteryLine.ini"
#define BL_WebBinary		L"https://joveler.kr/project/batteryline.html"
#define BL_WebSource		L"https://github.com/ied206/BatteryLine"
#define BL_SysTrayID_ON		1
#define BL_SysTrayID_OFF	2

#define BL_MAJOR_VER		1
#define BL_MINOR_VER		1
#define BL_MSGBOX_BUF_SIZE	2048

#define WM_APP_SYSTRAY_POPUP (WM_APP + 0x0001)

// BatteryLine Option constants
#define BL_MAX_MONITOR		32
#define BL_COLOR_LEVEL		16
#define BL_MON_PRIMARY 		0
#define BL_POS_TOP 			1
#define BL_POS_BOTTOM 		2
#define BL_POS_LEFT 		3
#define BL_POS_RIGHT 		4
#define BL_TASKBAR_IGNORE	0
#define BL_TASKBAR_EVADE	1

#define BL_TASKBAR_TOP		1
#define BL_TASKBAR_BOTTOM	2
#define BL_TASKBAR_LEFT		3
#define BL_TASKBAR_RIGHT	4

// BS for Battery Status
#define BS_TOKEN_BUF_SIZE	16
#define BS_LINE_BUF_SIZE	512
#define BS_STRING_BUF_SIZE	2048

// For parsing, [Section]
#define BS_SECTION_OFF		0
#define BS_SECTION_GENERAL	1
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

// BLA for BattreyLine Arguments
#define BLA_QUIET_OFF	0
#define BLA_QUIET_ON	1
#define BLA_HELP_OFF	0
#define BLA_HELP_ON		1

struct bl_arg
{
    int quiet; // Do notification?
    int help; // help message
};
typedef struct bl_arg BL_ARG;


struct bl_option
{
	uint8_t showcharge;		// Show battery line when charging?
	uint8_t monitor;		// Which monitor to show battery line?
	uint8_t position;		// Where to show battery line? (TOP | BOTTOM | LEFT | RIGHT)
	uint8_t taskbar;		// Evade or not if battery line is overlapped with taskbar
	uint8_t transparency;	// Transparency of battery line
	uint8_t height;			// Battery line's height (in pixel)
	COLORREF defaultcolor;	// Battery line's default color
	COLORREF chargecolor;	// Battery line's color when charging
	COLORREF fullcolor;		// Battery line's color when charging is done
	COLORREF color[BL_COLOR_LEVEL];			// User defined battery line's color
	uint8_t threshold[BL_COLOR_LEVEL*2];	// User defined edges to pick up user defined color
};
typedef struct bl_option BL_OPTION;

struct bl_moninfo
{
	// resoultion
	uint32_t res_x;
	uint32_t res_y;
	// virt coord
	int32_t left;
	int32_t top;
	int32_t right;
	int32_t bottom;
	// Is this monitor primary?
	// device name
	// wchar_t name[CCHDEVICENAME];
};
typedef struct bl_moninfo BL_MONINFO;

extern HANDLE not_power_src, not_bat_per;
extern SYSTEM_POWER_STATUS stat;
extern BL_OPTION option;
extern HINSTANCE g_hInst;
extern int g_nMon; // Number of monitors installed on this system
extern int g_nPriMon; // Which monitor is primary monitor?
extern MONITORINFO g_monInfo[BL_MAX_MONITOR];
extern BL_MONINFO g_monRes[BL_MAX_MONITOR];
// http://www.transmissionzero.co.uk/computing/win32-apps-with-mingw/

#endif // VAR_H_INCLUDED
