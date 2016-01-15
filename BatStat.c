#include "Var.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <windows.h>
#include <shlwapi.h>
#undef __CRT__NO_INLINE
#include <strsafe.h>
#define __CRT__NO_INLINE

#include "BatStat.h"
#include "ErrorHandle.h"
#include "BasicIO.h"

const uint16_t BOM = 0xFEFF;

// Default Setting File in string
// Must have \r\n in the end, because parsing algorithm slice a line by \r\n
wchar_t* BL_DefaultSettingStr = L"# Joveler's BatteryLine\r\n\r\n"
								"[Setting]\r\n"
								"# showcharge   : Show BatteryLine when charging\r\n"
								"#                  {true, false}\r\n"
								"# monitor      : Which monitor to view BatteryLine?\r\n"
								"#                - Note : This option is not implemented in v1.0.\r\n"
								"#                  {primary, 1, 2, ... , 255}\r\n"
								"#                  primary for using primary monitor\r\n"
								"#                  number for n'th monitor\r\n"
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
								"fullcolor    = 0, 255, 255\r\n"
								"# Support up to 16 thresholds\r\n"
								"# Format : {LowEdge, HighEdge, R, G, B}\r\n"
								"color = 0, 20, 237, 28, 36\r\n"
								"color = 20, 50, 255, 201, 14\r\n";

int BLBS_InitBLOption(BL_OPTION* bl_option)
{
    // bl_option.chargecolor =
    return 0;
}

int BLBS_ReadSetting()
{
	// Init and declare variables
    HANDLE hFile;
	uint8_t make_ini = FALSE; // If this is true, use default option
	wchar_t* file_buf = NULL;
	uint32_t file_size = 0;

	/*
	// Get current directory's string length
    file_size = GetCurrentDirectoryW(file_size, file_buf);
	if (file_size == 0)
		JV_ErrorHandle(JVERR_GetCurrentDirectory, TRUE);

	// Allocate file_buf to write absolute path
	file_buf = (PWSTR) malloc(sizeof(wchar_t) * file_size);
	if (0 == GetCurrentDirectoryW(file_size, file_buf)) // Error!
	{
		free(file_buf);
		JV_ErrorHandle(JVERR_GetCurrentDirectory, TRUE);
	}

	// But I am using BL_SettingFile constant to open ini! Comment this.
	*/

	// File IO
	// https://msdn.microsoft.com/en-us/library/windows/desktop/bb540534%28v=vs.85%29.aspx
	// Open File
	hFile = CreateFileW(BL_SettingFile,				// lpFileName
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
		hFile = CreateFileW(BL_SettingFile,				// lpFileName
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

        // file_buf will point to BL_DefaultSettingStr
		file_buf = BL_DefaultSettingStr;
	}
	else
	{ // file is successfully open, read setting from file
		uint32_t read_byte = 0;
		size_t sztmp = 0;

		// Get file size, and allocate file_buf
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
		// Is all byte successfully read?
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

	// Parse ini File
	wchar_t line_token[BS_TOKEN_BUF_SIZE]; // Unicode LineFeed : \r\n (00d0 00a0)
	wchar_t line_rawbuf[BS_LINE_BUF_SIZE], line_buf[BS_LINE_BUF_SIZE];
	wchar_t* str_cursor = NULL, *str_next = NULL;
	size_t line_len = 0;
	uint8_t state_section = BS_SECTION_OFF;
	wchar_t* equal_pos = NULL;
	wchar_t equal_left[BS_LINE_BUF_SIZE], equal_right[BS_LINE_BUF_SIZE];
	uint8_t color_idx = 0;

	// 01 using \r\n as token, traverse all lines in BL_SettingFile

	// 03 []이 있나? -> section 시작
	// 04 []이 없다 -> 빈 칸 제거 (Tab, Space 제거)
	// 05 = 기준 left string -> 어떤 option에 넣을지, 어떻게 right string을 처리할지 결정
	// 06 = 기준 right string -> 단일 값 파싱 / , 기준 여러값 파싱
	// 07 파싱한 값을 option 구조체에 넣는다
	// StringCchCopyW(line_token, BS_TOKEN_BUF_SIZE, L"\r\n"); // token to specify end of line
	line_token[0] = L'\r';
	line_token[1] = L'\n';
	line_token[2] = L'\0';
	str_cursor = file_buf;

	while (1)
	{
		// 02 한줄을 복사해낸다
		str_next = StrStrW(str_cursor, line_token); // Start of next line
		if (str_next == NULL) // No more line, EOF
			break;
		line_len = str_next - str_cursor; // Get this line's length
		StringCchCopyNW(line_rawbuf, BS_LINE_BUF_SIZE, str_cursor+1, line_len-1); // Copy one line to line_buf, +1 for remove first \r\n, -1 for remove last \r
		str_cursor = str_next + 1; // Fot next iteration, move cursor to next line

		/*
		#ifdef _DEBUG_CONSOLE
		wprintf(L"line_rawbuf : %ws\n", line_rawbuf);
		wprintf(L"state       : %u\n", state_section);
		#endif
		*/

		// State Machine
		if (StrChrW(line_rawbuf, L'[') != NULL && StrChrW(line_rawbuf, L']') != NULL)
		{ // Contains [ ]
			switch (state_section)
			{
			case BS_SECTION_OFF:
				if (StrStrIW(line_rawbuf, L"[Setting]") != NULL)
					state_section = BS_SECTION_SETTING;
				else if (StrStrIW(line_rawbuf, L"[Color]") != NULL)
					state_section = BS_SECTION_COLOR;
				break;
			case BS_SECTION_SETTING:
				if (StrStrIW(line_rawbuf, L"[Setting]") != NULL)
					state_section = BS_SECTION_SETTING;
				else if (StrStrIW(line_rawbuf, L"[Color]") != NULL)
					state_section = BS_SECTION_COLOR;
				else
					state_section = BS_SECTION_OFF;
				break;
			case BS_SECTION_COLOR:
				if (StrStrIW(line_rawbuf, L"[Setting]") != NULL)
					state_section = BS_SECTION_SETTING;
				else if (StrStrIW(line_rawbuf, L"[Color]") != NULL)
					state_section = BS_SECTION_COLOR;
				else
					state_section = BS_SECTION_OFF;
				break;
			}
		}
		else if (line_rawbuf[0] != L'#') // This line is not comment
		{ // Do not contain []
			int32_t dword = 0;
			uint8_t lowedge8 = 0, highedge8 = 0, r8 = 0, g8 = 0, b8 = 0;
			wchar_t quote_tmp[BS_TOKEN_BUF_SIZE] = {0};
			wchar_t* quote_pos = NULL, *quote_next = NULL;
			uint32_t x = 0;

			// Remove Tab, Space, \r, \n from line_rawbuf and put into line_buf
			for (uint32_t i = 0; i < line_len; i++)
			{
				if (line_rawbuf[i] != L'\t' && line_rawbuf[i] != L' ' && line_rawbuf[i] != L'\r' && line_rawbuf[i] != L'\n')
				{
					line_buf[x] = line_rawbuf[i];
					x++;
				}
			}
			line_buf[x] = L'\0';
			line_len = x;

			switch (state_section)
			{
			case BS_SECTION_OFF:
				break;
			case BS_SECTION_SETTING:
				equal_pos = StrChrW(line_buf, L'=');
				if (equal_pos == NULL)
				{
					state_section = BS_SECTION_OFF; // = 이 없을 때는 패스
					continue;
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
				}
				else if (StrCmpIW(equal_left, L"monitor") == 0)
				{ // {primary, 1, 2, ... , 255}
					if (StrCmpIW(equal_right, L"primary") == 0)
						option.monitor = BL_MON_PRIMARY;
					else
					{
						int32_t dword = _wtoi(equal_right);
						if (!(1 <= dword && dword <= 255))
							JV_ErrorHandle(JVERR_OPT_INI_INVALID_MONITOR, FALSE);
						if (!(dword < g_nMon))
							JV_ErrorHandle(JVERR_OPT_INI_NOT_EXIST_MONITOR, FALSE);
						option.monitor = (uint8_t) dword;
					}
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
				}
				else if (StrCmpIW(equal_left, L"taskbar") == 0)
				{ // {ignore, evade}
					if (StrCmpIW(equal_right, L"ignore") == 0)
						option.taskbar = BL_TASKBAR_IGNORE;
					else if (StrCmpIW(equal_right, L"evade") == 0)
						option.taskbar = BL_TASKBAR_EVADE;
					else
						JV_ErrorHandle(JVERR_OPT_INI_INVALID_TASKBAR, FALSE);
				}
				else if (StrCmpIW(equal_left, L"transparency") == 0)
				{ // {0 <= number <= 255}
					int32_t dword = _wtoi(equal_right);
					if (!(0 <= dword && dword <= 255))
						JV_ErrorHandle(JVERR_OPT_INI_INVALID_TRANSPARENCY, FALSE);
					option.transparency = (uint8_t) dword;
				}
				else if (StrCmpIW(equal_left, L"height") == 0)
				{ // {1 <= number <= 255}
					int32_t dword = _wtoi(equal_right);
					if (!(1 <= dword && dword <= 255))
						JV_ErrorHandle(JVERR_OPT_INI_INVALID_HEIGHT, FALSE);
					option.height = (uint8_t) dword;
				}
				break;
			case BS_SECTION_COLOR:
				// Then, this is description of one section (Section이 on이여야 한다)
				equal_pos = StrChrW(line_buf, L'=');
				if (equal_pos == NULL)
				{
					state_section = BS_SECTION_OFF; // = 이 없을 때는 패스
					continue;
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
	puts("[Event]"); // For WndProcedure
 	#endif

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
		msg_ac = L"Battery Mode";
		break;
	case 1: // AC OnS
		msg_ac = L"AC Mode";
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
					L"ACLineStatus : %ws\n"
					L"BatteryFlag : %ws\n"
					L"BatteryLifePercent : %d%%\n",
					msg_ac, msg_charge, stat.BatteryLifePercent);
	MessageBoxW(NULL, fullmsg, L"Power Info", MB_ICONINFORMATION | MB_OK);
// DEBUG Information End

    return 0;
}
