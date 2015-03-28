// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <fstream>
#include <IPTypes.h>

#define PtrFromRva( base, rva ) ( ( ( PBYTE ) base ) + rva )

// TODO: reference additional headers your program requires here


void dbglog(const char* module, char* format, ...);
void dbglogf(const char* module, char* format, va_list ap);