#ifndef COLOR_HPP
#define COLOR_HPP

#ifdef WINDOWS
#include <windows.h>
#endif
#ifdef LINUX
#include <iostream>
using std::cout;
#endif

void Color(int c)
{	
#ifdef WINDOWS
	HANDLE  hConsole;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, c);
#endif
#ifdef LINUX
	cout << "\033[" << c << "m";
#endif
}

void Color(char c)
{
#ifdef WINDOWS
	int code;
	switch (c)
	{
	case 'r':
		code = 0x0C;
		break;
	case 'y':
		code = 0x0E;
		break;
	case 'g':
		code = 0x0A;
		break;
	case 'w':
	default:
		code = 0x7;
		break;
	}
	Color(code);
#endif
#ifdef LINUX
	int code;
	switch (c)
	{
	case 'r':
		code = 31;
		break;
	case 'y':
		code = 33;
		break;
	case 'g':
		code = 32;
		break;
	case 'w':
	default:
		code = 0;
		break;
	}
	Color(code);
#endif
}

#endif