#pragma once
#include "stdafx.h"
//#include "patcher.h"

// regular functions
void InitInstance(HANDLE hModule);
void ExitInstance(void);
void LoadOriginalDll(void);

void InitializeGameScriptDLL();
void ShutdownGameScriptDLL();