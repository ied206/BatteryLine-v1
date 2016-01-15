#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BasicIO.h"
#include <windows.h>

int scanfile (const char name[]) // 파일의 존재 여부 검사
{
	FILE *fp;
	int tmp = 1;

    fp = fopen(name, "r");
	if (fp == NULL)
		tmp = 0;
	else
		fclose(fp);
	return tmp;
}

int WhatBitOS (int print) // 몇비트 컴파일인가 알아내기
{
    if (print)
    {
        if (sizeof(int*) == 4)
        {
            printf("This is 32bit Program!\n");
            return 32;
        }
        else if (sizeof(int*) == 8)
        {
            printf("This is 64bit Program!\n");
            return 64;
        }
        else
        {
			printf("This is %ubit Program!\n", (int) sizeof(int*) * 8);
            return (sizeof(int*) * 8);
        }
    }
    else
    {
        if (sizeof(int*) == 4)
            return 32;
        else if (sizeof(int*) == 8)
            return 64;
        else
            return (sizeof(int*) * 8);
    }
}

int CompileYear () // 컴파일한 년도
{
	const char macro[16] = __DATE__;
	char stmp[8] = {0}; // 전체 0으로 초기화

	stmp[0] = macro[7];
	stmp[1] = macro[8];
	stmp[2] = macro[9];
	stmp[3] = macro[10];
	stmp[4] = '\0';

	return atoi(stmp);
}

int CompileMonth ()
{ // 컴파일한 월 표시
	const char macro[16] = __DATE__;
	const char smonth[12][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
	int i = 0;

// 월 감지
	for (i = 0; i < 12; i++)
	{
		if (strstr(macro, smonth[i]) != NULL)
			return i + 1;
	}

// -1이 반환되는 경우는 월 인식 불가
	return -1;
}

int CompileDate ()
{ // 컴파일한 일 표시
	const char macro[16] = __DATE__;
	char stmp[4] = {0}; // 전체 0으로 초기화

// 일 감지
	stmp[0] = macro[4];
	stmp[1] = macro[5];
	stmp[2] = '\0';
	return atoi(stmp);
}

