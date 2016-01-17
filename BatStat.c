#include "Var.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <windows.h>
#include <shlwapi.h>
#ifdef _DEBUG
#undef __CRT__NO_INLINE
#endif
#include <strsafe.h>
#ifdef _DEBUG
#define __CRT__NO_INLINE
#endif

#include "BatStat.h"
#include "DrawLine.h"
#include "ErrorHandle.h"
#include "BasicIO.h"

const uint16_t BOM = 0xFEFF;

// Default Setting File as unicode string
// Must have \r\n in the end, because parsing algorithm slice a line by \r\n
wchar_t* BL_DefaultSettingStr = L"# Joveler's BatteryLine v1.0\r\n\r\n"
								"[General]\r\n"
								"# showcharge   : Show BatteryLine when charging\r\n"
								"#                  {true, false}\r\n"
								"# monitor      : Which monitor to view BatteryLine?\r\n"
								"#                Note : Experimental option, May contatin bugs\r\n"
								"#                  {primary, 1, 2, ... , 32}\r\n"
								"#                  primary for using primary monitor\r\n"
								"#                  number for n'th monitor\r\n"
								"#                   (Number can point all monitors, primary and non-primary)\r\n"
								"#                   (Monitor number is relative, it is prone to change)\r\n"
								"#                   (Strongly recommend to use default \'primary\' option)\r\n"
								"# position     : BatteryLine's position\r\n"
								"#                  {top, bottom, left, right}\r\n"
								"# taskbar      : Move BatteryLine when meet with taskbar\r\n"
								"#                  {ignore, evade}\r\n"
								"# transparency : Transparency of BatteryLine\r\n"
								"#                  255 is opaque, 0 is totally transparent\r\n"
								"#                  {0 <= number <= 255}\r\n"
								"# height       : BatteryLine's height in pixel\r\n"
								"#                  {1 <= number <= 25z5}\r\n"
								"showcharge   = true\r\n"
								"monitor      = primary\r\n"
								"position     = top\r\n"
								"taskbar      = ignore\r\n"
								"transparency = 196\r\n"
								"height       = 5\r\n\r\n"
								"[Color]\r\n"
								"# defaultcolor : BatteryLine's default color\r\n"
								"# chargecolor  : BatteryLine's color when charging\r\n"
								"# fullcolor    : BatteryLine's color when battery is full\r\n"
								"# Format : {R, G, B}\r\n"
								"defaultcolor = 0, 255, 0\r\n"
								"chargecolor  = 0, 200, 255\r\n"
								"fullcolor    = 0, 162, 232\r\n"
								"# Support up to 16 thresholds\r\n"
								"# Format : {LowEdge, HighEdge, R, G, B}\r\n"
								"color = 0, 20, 237, 28, 36\r\n"
								"color = 20, 50, 255, 140, 15\r\n";

int BLBS_ReadSetting()
{
	// Init and declare variables
    HANDLE hFile;
	uint8_t make_ini = FALSE; // If this is true, use default option
	wchar_t* file_buf = NULL, *path_buf = NULL;
	uint32_t file_size = 0, path_size = 0;

	// Get current directory's string length
    path_size = GetCurrentDirectoryW(path_size, path_buf);
	if (path_size == 0)
		JV_ErrorHandle(JVERR_GetCurrentDirectory, TRUE);

	// Allocate file_buf to write absolute path
	path_size = path_size + wcslen(BL_SettingFile) + 8;
	path_buf = (PWSTR) malloc(sizeof(wchar_t) * path_size); // 8 for \\ and NULL
	if (0 == GetCurrentDirectoryW(path_size, path_buf)) // Error!
	{
		free(path_buf);
		JV_ErrorHandle(JVERR_GetCurrentDirectory, TRUE);
	}

	StringCchCatW(path_buf, path_size, L"\\");
	StringCchCatW(path_buf, path_size, BL_SettingFile);

	#ifdef _DEBUG_CONSOLE
	printf("[IniFile]\n%S\n\n", path_buf);
	#endif // _DEBUG_CONSOLE

	// File IO
	// https://msdn.microsoft.com/en-us/library/windows/desktop/bb540534%28v=vs.85%29.aspx
	// Open File
	hFile = CreateFileW(path_buf,				// lpFileName
					  GENERIC_READ | GENERIC_WRITE, // dwDesiredAccess
					  FILE_SHARE_READ, 				// dwShareMode
					  NULL, 						// lpSecurityAttributes
					  OPEN_EXISTING, 				// dwCreationDisposition
					  FILE_ATTRIBUTE_NORMAL,		// dwFlagsAndAttributes
					  NULL);						// hTemplateFile

	// Cannot open! Error Handling
	if (hFile == INVALID_HANDLE_VALUE)
	{
		// No ini file, use default opntion!
		if (GetLastError() == ERROR_FILE_NOT_FOUND)
			make_ini = TRUE;
		else
			JV_ErrorHandle(JVERR_CreateFile, TRUE);
	}

	if (make_ini)
	{ // no BatteryLine.ini, so generate BatteryLine.ini. Note : hFile is invalid at this point
		uint32_t written_byte = 0;

		// Write default setting to file
		hFile = CreateFileW(path_buf,				// lpFileName
							GENERIC_READ | GENERIC_WRITE, // dwDesiredAccess
							0, 						// dwShareMode
							NULL, 						// lpSecurityAttributes
							CREATE_ALWAYS, 				// dwCreationDisposition
							FILE_ATTRIBUTE_NORMAL,		// dwFlagsAndAttributes
							NULL);						// hTemplateFile
		// Is handle is opened without problem?
		if (hFile == INVALID_HANDLE_VALUE)
			JV_ErrorHandle(JVERR_CreateFile, TRUE);
		// Write BOM to File
		if (!WriteFile(hFile, &BOM, sizeof(uint16_t), (DWORD*)&written_byte, NULL))
			JV_ErrorHandle(JVERR_WriteFile, TRUE);
		// Is BOM written to file fully?
		if (written_byte != sizeof(uint16_t))
			JV_ErrorHandle(JVERR_FILEIO_WRITTEN_BYTES, FALSE);
		// Write default setting string to File
		if (!WriteFile(hFile, (void*) BL_DefaultSettingStr, wcslen(BL_DefaultSettingStr) * sizeof(wchar_t), (DWORD*)&written_byte, NULL))
			JV_ErrorHandle(JVERR_WriteFile, TRUE);
		// Is string writeen to file fully?
		if (written_byte != wcslen(BL_DefaultSettingStr) * sizeof(wchar_t))
			JV_ErrorHandle(JVERR_FILEIO_WRITTEN_BYTES, FALSE);
		// Close Handle
		CloseHandle(hFile);

        // point file_buf to BL_DefaultSettingStr, no dyn alloc
		file_buf = BL_DefaultSettingStr;
	}
	else
	{ // file is successfully open, read setting from file
		uint32_t read_byte = 0;
		size_t sztmp = 0;

		// Get file size, and dyn allocate file_buf
		file_size = GetFileSize(hFile, NULL);
		file_buf = (wchar_t*) malloc(file_size+16);
		memset((void*) file_buf, 0, file_size+16);

		// Read from file
		if (!ReadFile(hFile, 		// hFile
					  file_buf, 	// lpBuffer
					  file_size, 	// nNumberOfBytesToRead
					  (DWORD*) &read_byte, // lpNumberOfBytesRead
					  NULL)) 		// lpOverlapped
			JV_ErrorHandle(JVERR_ReadFile, TRUE);
		// Is all bytes successfully read?
		if (read_byte != file_size)
			JV_ErrorHandle(JVERR_FILEIO_READ_BYTES, FALSE);
		// Close Handle
		CloseHandle(hFile);

		// Add \r\n at EOF
		sztmp = wcslen(file_buf);
		file_buf[sztmp] = L'\r';
		file_buf[sztmp+1] = L'\n';
		file_buf[sztmp+2] = L'\0';
	}

	// Do not need absolute path of BatteryLine.ini
	free(path_buf);
	path_buf = NULL;

	// Parse ini File - Too complex...
	wchar_t* str_cursor = NULL, *str_next = NULL;
	wchar_t line_token[BS_TOKEN_BUF_SIZE]; // Unicode LineFeed : \r\n (00d0 00a0)
	wchar_t line_rawbuf[BS_LINE_BUF_SIZE], line_buf[BS_LINE_BUF_SIZE];
	size_t line_len = 0;
	wchar_t* equal_pos = NULL;
	wchar_t equal_left[BS_LINE_BUF_SIZE], equal_right[BS_LINE_BUF_SIZE];
	uint8_t state_section = BS_SECTION_OFF;
	uint8_t color_idx = 0;
	BL_OPTION valid;

	// 01 using \r\n as token, traverse all lines in BL_SettingFile
	line_token[0] = L'\r';
	line_token[1] = L'\n';
	line_token[2] = L'\0';
	// str_cursor will serve as cursor for each line
	str_cursor = file_buf;
	// 'valid' will be used to check wether these option is already set - FALSE for not set, TRUE for set
	ZeroMemory(&valid, sizeof(BL_OPTION));

	// Parsing loop. Escape when meet EOF
	while (1)
	{
		// 02 Copy one line to line_rawbuf.
		// 02 Note that line_rawbuf contains 'raw' strings, which has tab, space, \r, \n.
		str_next = StrStrW(str_cursor, line_token); // Start of next line
		if (str_next == NULL) // No more line, EOF
			break;
		line_len = str_next - str_cursor; // Get this line's length
		StringCchCopyNW(line_rawbuf, BS_LINE_BUF_SIZE, str_cursor+1, line_len-1); // Copy one line to line_buf, +1 for remove first \r\n, -1 for remove last \r
		str_cursor = str_next + 1; // Fot next iteration, move cursor to next line

		// For debugging, there is too many segfaults in parsing alg!
		#ifdef _DEBUG_PARSING
		printf("line_rawbuf : %S\n", line_rawbuf);
		printf("state       : %u\n", state_section);
		#endif

		// Finite State Machine Model
		// 03 line_rawbuf has [] -> start a section, represent as 'state'
		if (StrChrW(line_rawbuf, L'[') != NULL && StrChrW(line_rawbuf, L']') != NULL)
		{ // Contains [ ]
			switch (state_section)
			{
			case BS_SECTION_OFF:
				if (StrStrIW(line_rawbuf, L"[General]") != NULL)
					state_section = BS_SECTION_GENERAL;
				else if (StrStrIW(line_rawbuf, L"[Color]") != NULL)
					state_section = BS_SECTION_COLOR;
				break;
			case BS_SECTION_GENERAL:
				if (StrStrIW(line_rawbuf, L"[General]") != NULL)
					state_section = BS_SECTION_GENERAL;
				else if (StrStrIW(line_rawbuf, L"[Color]") != NULL)
					state_section = BS_SECTION_COLOR;
				else
					state_section = BS_SECTION_OFF;
				break;
			case BS_SECTION_COLOR:
				if (StrStrIW(line_rawbuf, L"[General]") != NULL)
					state_section = BS_SECTION_GENERAL;
				else if (StrStrIW(line_rawbuf, L"[Color]") != NULL)
					state_section = BS_SECTION_COLOR;
				else
					state_section = BS_SECTION_OFF;
				break;
			}
		}
		// 04 line_rawbuf does not have [] -> remove Tab, Space, \r, \n and put into line_buf
		else if (line_rawbuf[0] != L'#') // This line is not comment and do not contain []
		{
			int32_t dword = 0;
			uint8_t lowedge8 = 0, highedge8 = 0, r8 = 0, g8 = 0, b8 = 0;
			wchar_t quote_tmp[BS_TOKEN_BUF_SIZE] = {0};
			wchar_t* quote_pos = NULL, *quote_next = NULL;
			uint32_t x = 0;

			// 04 Remove Tab, Space, \r, \n from line_rawbuf and put into line_buf
			for (uint32_t i = 0; i < line_len; i++)
			{
				if (line_rawbuf[i] == L'#') // if we meet # in the middle, ignore remnant characters
                    break;

				if (line_rawbuf[i] != L'\t' && line_rawbuf[i] != L' ' && line_rawbuf[i] != L'\r' && line_rawbuf[i] != L'\n')
				{
					line_buf[x] = line_rawbuf[i];
					x++;
				}
			}
			line_buf[x] = L'\0';
			line_len = x;

			#ifdef _DEBUG_PARSING
			printf("line_buf    : %S\n", line_buf);
			#endif

			switch (state_section)
			{
			case BS_SECTION_OFF:
				break;
			case BS_SECTION_GENERAL:
				// 05 using = as basis, cut left sting and right string.
				// 06 left string : which option to set?
				// 06 right string : how to parse right string?
				equal_pos = StrChrW(line_buf, L'=');
				if (equal_pos == NULL) // no '='
				{
					state_section = BS_SECTION_OFF; // invalid option, think this as end of section.
					continue; // so ignore this line and go to next line
				}
				StringCchCopyNW(equal_left, BS_LINE_BUF_SIZE, line_buf, equal_pos-line_buf); // Copy left part
				StringCchCopyW(equal_right, BS_LINE_BUF_SIZE, line_buf+(equal_pos-line_buf+1)); // Copy right part

				if (StrCmpIW(equal_left, L"showcharge") == 0)
				{ // {true, false}
					if (StrCmpIW(equal_right, L"true") == 0)
						option.showcharge = TRUE;
					else if (StrCmpIW(equal_right, L"false") == 0)
						option.showcharge = FALSE;
					else
						JV_ErrorHandle(JVERR_OPT_INI_INVALID_SHOWCHARGE, FALSE);
					valid.showcharge = TRUE;
				}
				else if (StrCmpIW(equal_left, L"monitor") == 0)
				{ // {primary, 1, 2, ... , 32}
					if (StrCmpIW(equal_right, L"primary") == 0)
						option.monitor = BL_MON_PRIMARY;
					else
					{
						int32_t dword = _wtoi(equal_right);

						// Count Monitor's number and write to g_nMon
						g_nMon = 0;
						g_nPriMon = 0;
						ZeroMemory(&g_monInfo, sizeof(MONITORINFO) * BL_MAX_MONITOR);
						EnumDisplayMonitors(NULL, NULL, BLCB_MonEnumProc_GetFullInfo, 0);

						#ifdef _DEBUG_MONITOR
						printf("[Monitor]\nThis system has %d monitors.\n\n", g_nMon);
						#endif

						if (!(1 <= dword && dword <= BL_MAX_MONITOR))
							JV_ErrorHandle(JVERR_OPT_INI_INVALID_MONITOR, FALSE);
						if (!(dword <= g_nMon)) //
							JV_ErrorHandle(JVERR_OPT_INI_NOT_EXIST_MONITOR, FALSE);

						option.monitor = (uint8_t) dword;
					}
					valid.monitor = TRUE;
				}
				else if (StrCmpIW(equal_left, L"position") == 0)
				{ // {top, bottom, left, right}
					if (StrCmpIW(equal_right, L"top") == 0)
						option.position = BL_POS_TOP;
					else if (StrCmpIW(equal_right, L"bottom") == 0)
						option.position = BL_POS_BOTTOM;
					else if (StrCmpIW(equal_right, L"left") == 0)
						option.position = BL_POS_LEFT;
					else if (StrCmpIW(equal_right, L"right") == 0)
						option.position = BL_POS_RIGHT;
					else
						JV_ErrorHandle(JVERR_OPT_INI_INVALID_POSITION, FALSE);
					valid.position = TRUE;
				}
				else if (StrCmpIW(equal_left, L"taskbar") == 0)
				{ // {ignore, evade}
					if (StrCmpIW(equal_right, L"ignore") == 0)
						option.taskbar = BL_TASKBAR_IGNORE;
					else if (StrCmpIW(equal_right, L"evade") == 0)
						option.taskbar = BL_TASKBAR_EVADE;
					else
						JV_ErrorHandle(JVERR_OPT_INI_INVALID_TASKBAR, FALSE);
					valid.taskbar = TRUE;
				}
				else if (StrCmpIW(equal_left, L"transparency") == 0)
				{ // {0 <= number <= 255}
					int32_t dword = _wtoi(equal_right);
					if (!(0 <= dword && dword <= 255))
						JV_ErrorHandle(JVERR_OPT_INI_INVALID_TRANSPARENCY, FALSE);
					option.transparency = (uint8_t) dword;
					valid.transparency = TRUE;
				}
				else if (StrCmpIW(equal_left, L"height") == 0)
				{ // {1 <= number <= 255}
					int32_t dword = _wtoi(equal_right);
					if (!(1 <= dword && dword <= 255))
						JV_ErrorHandle(JVERR_OPT_INI_INVALID_HEIGHT, FALSE);
					option.height = (uint8_t) dword;
					valid.height = TRUE;
				}
				break;
			case BS_SECTION_COLOR:
				// 05 using = as basis, cut left sting and right string.
				// 06 left string : which option to set?
				// 06 right string : how to parse right string?
				equal_pos = StrChrW(line_buf, L'=');
				if (equal_pos == NULL) // no '='
				{
					state_section = BS_SECTION_OFF; // invalid option, think this as end of section.
					continue; // so ignore this line and go to next line
				}
				StringCchCopyNW(equal_left, BS_LINE_BUF_SIZE, line_buf, equal_pos-line_buf); // Copy left part
				StringCchCopyW(equal_right, BS_LINE_BUF_SIZE, line_buf+(equal_pos-line_buf+1)); // Copy right part

				if (!StrCmpIW(equal_left, L"defaultcolor"))
				{ // Format : {R, G, B}
					quote_pos = equal_right;
					for (uint32_t i = 0; i < 3; i++) // R, G, B
					{
						quote_next = StrChrW(quote_pos, L',');
						if (quote_next != NULL) // , still exists
						{
							StringCchCopyNW(quote_tmp, BS_TOKEN_BUF_SIZE, quote_pos, quote_next-quote_pos); // Copy a number
							quote_pos = quote_next+1; // +1 for ,
						}
						else // NULL, no ','!
							StringCchCopyW(quote_tmp, BS_TOKEN_BUF_SIZE, quote_pos); // Copy last number

						dword = _wtoi(quote_tmp);
						if (!(0 <= dword && dword <= 255))
							JV_ErrorHandle(JVERR_OPT_INI_INVALID_DEFAULTCOLOR, FALSE);
						switch (i)
						{
						case 0:		r8 = dword;		break;
						case 1:		g8 = dword;		break;
						case 2:		b8 = dword;		break;
						}
					}
					option.defaultcolor = RGB(r8, g8, b8);
					valid.defaultcolor = TRUE;
				}
				else if (!StrCmpIW(equal_left, L"chargecolor"))
				{ // Format : {R, G, B}
					quote_pos = equal_right;
					for (uint32_t i = 0; i < 3; i++) // R, G, B
					{
						quote_next = StrChrW(quote_pos, L',');
						if (quote_next != NULL) // , still exists
						{
							StringCchCopyNW(quote_tmp, BS_TOKEN_BUF_SIZE, quote_pos, quote_next-quote_pos); // Copy a number
							quote_pos = quote_next+1; // +1 for ,
						}
						else // NULL, no ','!
							StringCchCopyW(quote_tmp, BS_TOKEN_BUF_SIZE, quote_pos); // Copy last number
						dword = _wtoi(quote_tmp);
						if (!(0 <= dword && dword <= 255))
							JV_ErrorHandle(JVERR_OPT_INI_INVALID_CHARGECOLOR, FALSE);
						switch (i)
						{
						case 0:		r8 = dword;		break;
						case 1:		g8 = dword;		break;
						case 2:		b8 = dword;		break;
						}
					}
					option.chargecolor = RGB(r8, g8, b8);
					valid.chargecolor = TRUE;
				}
				else if (!StrCmpIW(equal_left, L"fullcolor"))
				{ // Format : {R, G, B}
					quote_pos = equal_right;
					for (uint32_t i = 0; i < 3; i++) // R, G, B
					{
						quote_next = StrChrW(quote_pos, L',');
						if (quote_next != NULL) // , still exists
						{
							StringCchCopyNW(quote_tmp, BS_TOKEN_BUF_SIZE, quote_pos, quote_next-quote_pos); // Copy a number
							quote_pos = quote_next+1; // +1 for ,
						}
						else // NULL, no ','!
							StringCchCopyW(quote_tmp, BS_TOKEN_BUF_SIZE, quote_pos); // Copy last number
						dword = _wtoi(quote_tmp);
						if (!(0 <= dword && dword <= 255))
							JV_ErrorHandle(JVERR_OPT_INI_INVALID_FULLCOLOR, FALSE);
						switch (i)
						{
						case 0:		r8 = dword;		break;
						case 1:		g8 = dword;		break;
						case 2:		b8 = dword;		break;
						}
					}
					option.fullcolor = RGB(r8, g8, b8);
					valid.fullcolor = TRUE;
				}
				else if (StrCmpIW(equal_left, L"color") == 0)
				{ // {LowEdge, HighEdge, R, G, B}, Support up to 16 thresholds
					quote_pos = equal_right;
					for (uint32_t i = 0; i < 5; i++) // R, G, B
					{
						quote_next = StrChrW(quote_pos, L',');
						if (quote_next != NULL) // , still exists
						{
							StringCchCopyNW(quote_tmp, BS_TOKEN_BUF_SIZE, quote_pos, quote_next-quote_pos); // Copy a number
							quote_pos = quote_next+1; // +1 for ,
						}
						else // NULL, no ','!
							StringCchCopyW(quote_tmp, BS_TOKEN_BUF_SIZE, quote_pos); // Copy last number
						dword = _wtoi(quote_tmp);
						if (!(0 <= dword && dword <= 255))
							JV_ErrorHandle(JVERR_OPT_INI_INVALID_COLOR, FALSE);
						switch (i)
						{
						case 0:		lowedge8 = dword;	break;
						case 1:		highedge8 = dword;	break;
						case 2:		r8 = dword;			break;
						case 3:		g8 = dword;			break;
						case 4:		b8 = dword;			break;
						}
					}

					if (BL_COLOR_LEVEL <= color_idx)
						JV_ErrorHandle(JVERR_OPT_INI_TOO_MUCH_COLOR, FALSE);

					option.threshold[color_idx*2+0]	= lowedge8;
					option.threshold[color_idx*2+1] = highedge8;
					option.color[color_idx]			= RGB(r8, g8, b8);
					color_idx++;
				}
				break;
			}
		}
	}

	for (uint32_t i = color_idx; i < BL_COLOR_LEVEL; i++)
	{
		option.color[i]			= 0;
		option.threshold[2*i] 	= 0;
		option.threshold[2*i+1] = 0;
	}

	// Free allocated byte in file_buf
	if (file_buf != BL_DefaultSettingStr)
	{
		free(file_buf);
		file_buf = NULL;
	}

	// Debug print
 	#ifdef _DEBUG_CONSOLE
	puts("[Setting]");
	printf("showcharge   = %u\n", option.showcharge);
	printf("monitor      = %u\n", option.monitor);
	printf("position     = %u\n", option.position);
	printf("taskbar      = %u\n", option.taskbar);
	printf("transparency = %u\n", option.transparency);
	printf("height       = %u\n", option.height);

	putchar('\n');
	puts("[Color]");
	printf("defaultcolor = %lu,%lu,%lu\n", BLU_RGB_R(option.defaultcolor), BLU_RGB_G(option.defaultcolor), BLU_RGB_B(option.defaultcolor));
	printf("chargecolor  = %lu,%lu,%lu\n", BLU_RGB_R(option.chargecolor), BLU_RGB_G(option.chargecolor), BLU_RGB_B(option.chargecolor));

	for (uint32_t i = 0; i < BL_COLOR_LEVEL; i++)
	{
		printf("color[%02d]     = %lu,%lu,%lu\n", i, BLU_RGB_R(option.color[i]), BLU_RGB_G(option.color[i]), BLU_RGB_B(option.color[i]));
		printf("threshold[%02d] = %u\n", 2*i, option.threshold[2*i]);
		printf("threshold[%02d] = %u\n", 2*i+1, option.threshold[2*i+1]);
	}
	putchar('\n');
	puts("[Event]"); // For WndProcedure Debug Message
 	#endif

	// Check necessary options are read successfully
    if (!(valid.showcharge && valid.monitor && valid.position && valid.taskbar && valid.transparency && valid.height // Section [General]
		&& valid.defaultcolor && valid.chargecolor && valid.fullcolor)) // Section [Color]
	{
        JV_ErrorHandle(JVERR_OPT_INI_MISSING_OPTIONS, FALSE);
	}

	return 0;
}

int BLBS_GetBatteryStat()
{
	wchar_t* msg_ac = NULL;
	wchar_t* msg_charge = NULL;
	wchar_t fullmsg[BS_STRING_BUF_SIZE];

	if (GetSystemPowerStatus(&stat) == 0)
		JV_ErrorHandle(JVERR_GetSystemPowerStatus, TRUE);

// DEBUG Information Start
	switch (stat.ACLineStatus)
	{
	case 0: // AC Off
		msg_ac = L"Battery";
		break;
	case 1: // AC OnS
		msg_ac = L"AC";
		break;
	case 255: // Unknown
		msg_ac = L"Unknown";
		break;
	}

	if (stat.BatteryFlag & 0x08)
		msg_charge = L"Charging";
	else if (stat.ACLineStatus == 1)
		msg_charge = L"Full";
	else if (stat.ACLineStatus == 0)
		msg_charge = L"Using Battery";

	StringCchPrintfW(fullmsg, BS_STRING_BUF_SIZE,
					L"Power Source : %ws\n"
					L"Battery Status : %ws\n"
					L"Battery Percent : %d%%\n",
					msg_ac, msg_charge, stat.BatteryLifePercent);
	MessageBoxW(NULL, fullmsg, L"Power Info", MB_ICONINFORMATION | MB_OK);
// DEBUG Information End

    return 0;
}
