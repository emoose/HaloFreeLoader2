// stdafx.cpp : source file that includes just the standard includes
// HaloFree.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file

void dbglog(const char* module, char* format, ...)
{
	va_list ap;
	va_start(ap, format);

	char buff[4096];
	memset(buff, 0, 4096);

	vsprintf_s(buff, 4096, format, ap);
	va_end(ap);

	std::ofstream outfile;
	outfile.open("halofree.log", std::ios_base::app);
	outfile << "[" << module << "] " << buff << "\n";
	outfile.close();
}

void dbglogf(const char* module, char* format, va_list ap)
{
	char buff[256];
	memset(buff, 0, 256);

	vsprintf_s(buff, 256, format, ap);

	std::ofstream outfile;
	outfile.open("halofree.log", std::ios_base::app);
	outfile << "[" << module << "] " << buff << "\n";
	outfile.close();
}