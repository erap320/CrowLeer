#pragma once
#include <windows.h>

void Color(int c)
{	
	HANDLE  hConsole;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, c);
}

void Color(char c)
{
	int code;
	switch (c)
	{
	case 'r':
		code = 0x0C;
		break;
	case 'w':
	default:
		code = 0x7;
		break;
	}
	Color(code);
}