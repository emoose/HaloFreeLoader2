// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "proxy.h"
#include <AntTweakBar.h>
#include <d3d9.h>

// global variables
#pragma data_seg (".gamedll_shared")
HINSTANCE           gl_hOriginalDll;
HINSTANCE           gl_hThisInstance;
HANDLE				g_mainModule;
HANDLE				g_curProc;
#pragma data_seg ()

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
	)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH: InitInstance(hModule);	break;
	case DLL_PROCESS_DETACH: ExitInstance(); break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}

void WriteJump(LPVOID writeDest, LPCVOID jumpDest)
{
	BYTE jmp[] = { 0xE9, 0x00, 0x00, 0x00, 0x00 };
	DWORD_PTR diff = ((DWORD_PTR)writeDest + 5) - (DWORD_PTR)jumpDest;
	DWORD_PTR toWrite = 0xFFFFFFFF - diff;
	if ((DWORD_PTR)jumpDest > ((DWORD_PTR)writeDest + 5))
	{
		diff = (DWORD_PTR)jumpDest - ((DWORD_PTR)writeDest + 5);
		toWrite = diff;
	}

	WriteProcessMemory(g_curProc, writeDest, jmp, 5, NULL);
	WriteProcessMemory(g_curProc, (LPVOID)((DWORD_PTR)writeDest + 1), &toWrite, 4, NULL);
}

void WriteCall(LPVOID writeDest, LPCVOID jumpDest)
{
	BYTE jmp[] = { 0xE8, 0x00, 0x00, 0x00, 0x00 };
	DWORD_PTR diff = ((DWORD_PTR)writeDest + 5) - (DWORD_PTR)jumpDest;
	DWORD_PTR toWrite = 0xFFFFFFFF - diff;
	if ((DWORD_PTR)jumpDest > ((DWORD_PTR)writeDest + 5))
	{
		diff = (DWORD_PTR)jumpDest - ((DWORD_PTR)writeDest + 5);
		toWrite = diff;
	}

	WriteProcessMemory(g_curProc, writeDest, jmp, 5, NULL);
	WriteProcessMemory(g_curProc, (LPVOID)((DWORD_PTR)writeDest + 1), &toWrite, 4, NULL);
}

char* dbgLogHook(char* dstBuf, size_t size, char* format, ...)
{
	va_list args;
	va_start(args, format);
	vsnprintf(dstBuf, size, format, args);
	va_end(args);
	dstBuf[size - 1] = 0;

	//dbglog("Game", "arg1: %p %p %p", dstBuf, size, format);
	dbglog("Game", "%s", dstBuf);
	//vsprintf_s(dstBuf, size, format, ap);
	//_vsnprintf_s(dstBuf, size, size - 1, format, ap);
	//dstBuf[size - 1] = 0;
	return dstBuf;
}

int dbgLogHook2(char* format, ...)
{
	char dstBuf[4096];
	memset(dstBuf, 0, 4096);

	va_list args;
	va_start(args, format);
	vsnprintf(dstBuf, 4095, format, args);
	va_end(args);


	dbglog("Net", "%s", dstBuf);
	return 1;
}

HHOOK g_keyboardHook;
BOOL g_keysPressed[0xFF];


BOOL uiInited = false;
BOOL uiShown = false;

void SetMap(const void *value, void *clientData)
{

	//WriteProcessMemory(g_curProc, (LPVOID)((DWORD_PTR)g_mainModule + 0x1056D0), buf2, 1, NULL);
}

void GetMap(void *value, void *clientData)
{
	*(bool*)value = false;
}

HRESULT D3D9EndSceneHook(IDirect3DDevice9* dev)
{
	if (uiShown)
		TwDraw();
	return dev->EndScene();
}

bool b1 = false;
bool b2 = false;
bool b3 = false;
bool b4 = false;
bool b5 = false;
bool b6 = false;
	
void InitUI()
{
	if (!uiInited)
	{
		IDirect3DDevice9** devicePtr = (IDirect3DDevice9**)((DWORD_PTR)g_mainModule + 0x4CDADDC);

		TwInit(TW_DIRECT3D9, *devicePtr);
		TwWindowSize(1280, 800);
		uiInited = true;

		TwBar *myBar = TwNewBar("HaloFreeLoader2");
		//bool myVar;

		TwAddSeparator(myBar, "Load map:", "");
		TwAddVarRW(myBar, "guardian", TW_TYPE_BOOLCPP, &b1, "");
		TwAddVarRW(myBar, "riverworld", TW_TYPE_BOOLCPP, &b2, "");
		TwAddVarRW(myBar, "s3d_avalanche", TW_TYPE_BOOLCPP, &b3, "");
		TwAddVarRW(myBar, "s3d_edge", TW_TYPE_BOOLCPP, &b4, "");
		TwAddVarRW(myBar, "s3d_reactor", TW_TYPE_BOOLCPP, &b5, "");
		TwAddVarRW(myBar, "s3d_turf", TW_TYPE_BOOLCPP, &b6, "");
		//TwAddVarCB seems to crash :(
	}
}

void ToggleUI()
{
	uiShown = !uiShown;
}

LRESULT __stdcall KeyboardHook(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode >= 0 && nCode != HC_NOREMOVE && wParam != VK_MENU && wParam != VK_TAB)
	{
		g_keysPressed[wParam] = (GetAsyncKeyState((int)wParam) & (1 << 16));
		g_keysPressed[VK_LCONTROL] = (GetAsyncKeyState(VK_LCONTROL) & (1 << 16)) || (GetAsyncKeyState(VK_CONTROL) & (1 << 16));
		g_keysPressed[VK_LSHIFT] = (GetAsyncKeyState(VK_LSHIFT) & (1 << 16)) || (GetAsyncKeyState(VK_SHIFT) & (1 << 16));
		g_keysPressed[VK_CONTROL] = g_keysPressed[VK_LCONTROL];
		g_keysPressed[VK_SHIFT] = g_keysPressed[VK_LSHIFT];

		if (wParam == VK_LCONTROL || wParam == VK_LSHIFT || wParam == VK_CONTROL || wParam == VK_SHIFT)
			return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);

		BOOL ctrlState = g_keysPressed[VK_LCONTROL] || g_keysPressed[VK_CONTROL];
		BOOL shiftState = g_keysPressed[VK_LSHIFT] || g_keysPressed[VK_SHIFT];

		if (shiftState)
		{
			BOOL pageDown = g_keysPressed[VK_NEXT];
			BOOL pageUp = g_keysPressed[VK_PRIOR];
			BOOL home = g_keysPressed[VK_HOME];
			BOOL end = g_keysPressed[VK_END];
			BOOL ins = g_keysPressed[VK_INSERT];
			BOOL del = g_keysPressed[VK_DELETE];
			BOOL h = g_keysPressed['H'];
			BOOL c = g_keysPressed['C'];
			BOOL v = g_keysPressed['V'];
			BOOL z = g_keysPressed['Z'];
			BOOL x = g_keysPressed['X'];
			BOOL upArrow = g_keysPressed[VK_UP];
			BOOL downArrow = g_keysPressed[VK_DOWN];
			BOOL leftArrow = g_keysPressed[VK_LEFT];
			BOOL rightArrow = g_keysPressed[VK_RIGHT];
			if (z)
				InitUI();

			if (x)
				ToggleUI();
		}
	}

	// call the next hook in the hook chain. This is nessecary or your hook chain will break and the hook stops
	return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
}

void InitInstance(HANDLE hModule)
{
	//if (!patcher.IsInited)
	{
		AllocConsole();
		if (!(GetFileAttributes(L"halofree.log") == INVALID_FILE_ATTRIBUTES && GetLastError() == ERROR_FILE_NOT_FOUND))
			_unlink("halofree.log");

		dbglog("Wrapper", "InitInstance(): HaloFree initializing...");

		// Initialisation
		gl_hOriginalDll = NULL;
		gl_hThisInstance = NULL;

		// Storing Instance handle into global var
		gl_hThisInstance = (HINSTANCE)hModule;

		g_curProc = GetCurrentProcess();

		LoadOriginalDll();

		g_mainModule = GetModuleHandleA("eldorado.exe"); // always 0x400000 with x86 exes?
		BYTE nopBuf[] = { 0x90, 0x90, 0x90, 0x90, 0x90 };

		// disable game shutdown after timer runs out
		// note: the game constantly runs this code to try and close down the game after it's patched, making the game use a lot of CPU
		// im too lazy to fix that
		dbglog("Wrapper", "Patching out StopGame call when no login given..."); // --account & --code or whatever are still needed though, gonna have to look into this more
		{
			WriteProcessMemory(g_curProc, (LPVOID)((DWORD_PTR)g_mainModule + 0x4373AD), nopBuf, 5, NULL);
		}

		dbglog("Wrapper", "Patching StopGame function to prevent game from closing...");
		{
			BYTE buf2[] = { 0xC3 };
			WriteProcessMemory(g_curProc, (LPVOID)((DWORD_PTR)g_mainModule + 0x1056D0), buf2, 1, NULL);
		}

		dbglog("Wrapper", "Patching language to english...");
		{
			BYTE lang[] = { 0x0 };
			WriteProcessMemory(g_curProc, (LPVOID)((DWORD_PTR)g_mainModule + 0x2333FD), lang, 1, NULL);
		}

		dbglog("Wrapper", "Patching debug logging...");
		{
			// patch out transport_secure_address_resolve spam
			WriteProcessMemory(g_curProc, (LPVOID)((DWORD_PTR)g_mainModule + 0x306E8), nopBuf, 5, NULL);

			WriteJump((LPVOID)((DWORD_PTR)g_mainModule + 0x1910), &dbgLogHook);
			WriteJump((LPVOID)((DWORD_PTR)g_mainModule + 0x9858D0), &dbgLogHook2);
		}

		dbglog("Wrapper", "Patching MP mode to work offline...");
		{
			WriteProcessMemory(g_curProc, (LPVOID)((DWORD_PTR)g_mainModule + 0x2D26DF), nopBuf, 5, NULL);
		}

		BYTE botClient[] = { 0xb0, 0x01, 0xc3 };
		//WriteProcessMemory(g_curProc, (LPVOID)((DWORD_PTR)g_mainModule + 0x2E5F0), botClient, 3, NULL); //IsAppABotClient -> true
		//WriteProcessMemory(g_curProc, (LPVOID)((DWORD_PTR)g_mainModule + 0x2E600), botClient, 3, NULL); //IsAppADedicatedServer -> true
		//BYTE notClient[] = { 0x32, 0xC0, 0xC3 };
		//WriteProcessMemory(g_curProc, (LPVOID)((DWORD_PTR)g_mainModule + 0x2E610), notClient, 3, NULL); //IsAppAGame_2 -> false (crashes)

		dbglog("Wrapper", "Patching Session GUID...");
		{
			// have to unprotect the memory before we can patch it...

			MEMORY_BASIC_INFORMATION thunkMemInfo;
			DWORD junk;

			LPVOID addr = (LPVOID)((DWORD_PTR)g_mainModule + 0x120CCB8);

			VirtualQuery(addr, &thunkMemInfo, sizeof(MEMORY_BASIC_INFORMATION));
			VirtualProtect(thunkMemInfo.BaseAddress, thunkMemInfo.RegionSize, PAGE_EXECUTE_READWRITE, &thunkMemInfo.Protect);

			//BYTE buf3[] = { 0x0 };
			BYTE buf3[] = {
				0, //0x57, 0x68, 0x79, 0x53, 0x6F, 0x4D, 0x61, 0x64, 0x2C, 0x2F, 0x76, 0x2F, 0x3F, 0x0  // WhySoMad,/v/?
			};

			WriteProcessMemory(g_curProc, addr, buf3, 1, NULL);
			VirtualProtect(thunkMemInfo.BaseAddress, thunkMemInfo.RegionSize, thunkMemInfo.Protect, &junk);
		}

		BYTE oneNop[] = { 0x90 };
		WriteCall((LPVOID)((DWORD_PTR)g_mainModule + 0x621796), D3D9EndSceneHook);
		WriteProcessMemory(g_curProc, (LPVOID)((DWORD_PTR)g_mainModule + 0x621796 + 5), oneNop, 1, NULL);

		if (!(g_keyboardHook = SetWindowsHookEx(WH_KEYBOARD, KeyboardHook, GetModuleHandle(NULL), GetCurrentThreadId())))
			dbglog("Patcher", "Init(): Failed to install keyboard hook!");
	}
}

void LoadOriginalDll(void)
{
	dbglog("Wrapper", "LoadOriginalDll(): Loading original iphlpapi.dll...");

	// get path to iphplapi.dll
	char buffer[MAX_PATH];
	::GetSystemDirectoryA(buffer, MAX_PATH);
	strcat_s(buffer, MAX_PATH, "\\iphlpapi.dll");

	if (!gl_hOriginalDll) gl_hOriginalDll = ::LoadLibraryA(buffer);

	// Debug
	if (!gl_hOriginalDll)
	{
		dbglog("Wrapper", "LoadOriginalDll(): Original dll failed to load!");
		::ExitProcess(0); // exit the hard way
	}
}

void ExitInstance()
{
	dbglog("Wrapper", "ExitInstance(): Unloading HaloFree...");

	// Release the system's dll
	if (gl_hOriginalDll)
	{
		::FreeLibrary(gl_hOriginalDll);
		gl_hOriginalDll = NULL;
	}
}

void* GetOrigProcAddr(char* funcName)
{
	if (!gl_hOriginalDll) LoadOriginalDll();
	//if (settings.logApiCalls)
		dbglog("api_call", funcName);
#pragma warning(suppress: 6387)
	void* origFunc = GetProcAddress(gl_hOriginalDll, funcName);
	if (!origFunc)
	{
		dbglog("Wrapper", "GetOrigProcAddr(): %s not found!!", funcName);
		::ExitProcess(0);
	}
	return origFunc;
}

DWORD GetAdaptersInfo(PIP_ADAPTER_INFO pAdapterInfo, PULONG pOutBufLen)
{
	typedef DWORD(WINAPI* funcType)(PIP_ADAPTER_INFO pAdapterInfo, PULONG pOutBufLen);
	funcType origFunc = (funcType)GetOrigProcAddr("GetAdaptersInfo");

	return origFunc(pAdapterInfo, pOutBufLen);
}



/*++
Routine Description:
Replace the function pointer in a module's IAT.

Parameters:
Module              - Module to use IAT from.
ImportedModuleName  - Name of imported DLL from which
function is imported.
ImportedProcName    - Name of imported function.
AlternateProc       - Function to be written to IAT.
OldProc             - Original function.

Return Value:
S_OK on success.
(any HRESULT) on failure.
--*/
HRESULT PatchIat(
	__in HMODULE Module,
	__in PSTR ImportedModuleName,
	__in PSTR ImportedProcName,
	__in PVOID AlternateProc,
	__out_opt PVOID *OldProc
	)
{
	PIMAGE_DOS_HEADER DosHeader = (PIMAGE_DOS_HEADER)Module;
	PIMAGE_NT_HEADERS NtHeader;
	PIMAGE_IMPORT_DESCRIPTOR ImportDescriptor;
	UINT Index;

	_ASSERTE(Module);
	_ASSERTE(ImportedModuleName);
	_ASSERTE(ImportedProcName);
	_ASSERTE(AlternateProc);

	NtHeader = (PIMAGE_NT_HEADERS)
		PtrFromRva(DosHeader, DosHeader->e_lfanew);
	if (IMAGE_NT_SIGNATURE != NtHeader->Signature)
	{
		return HRESULT_FROM_WIN32(ERROR_BAD_EXE_FORMAT);
	}

	ImportDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)
		PtrFromRva(DosHeader,
		NtHeader->OptionalHeader.DataDirectory
		[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);

	//
	// Iterate over import descriptors/DLLs.
	//
	for (Index = 0;
		ImportDescriptor[Index].Characteristics != 0;
		Index++)
	{
		PSTR dllName = (PSTR)
			PtrFromRva(DosHeader, ImportDescriptor[Index].Name);

		if (0 == _strcmpi(dllName, ImportedModuleName))
		{
			//
			// This the DLL we are after.
			//
			PIMAGE_THUNK_DATA Thunk;
			PIMAGE_THUNK_DATA OrigThunk;

			if (!ImportDescriptor[Index].FirstThunk ||
				!ImportDescriptor[Index].OriginalFirstThunk)
			{
				return E_INVALIDARG;
			}

			Thunk = (PIMAGE_THUNK_DATA)
				PtrFromRva(DosHeader,
				ImportDescriptor[Index].FirstThunk);
			OrigThunk = (PIMAGE_THUNK_DATA)
				PtrFromRva(DosHeader,
				ImportDescriptor[Index].OriginalFirstThunk);

			for (; OrigThunk->u1.Function != NULL;
				OrigThunk++, Thunk++)
			{
				if (OrigThunk->u1.Ordinal & IMAGE_ORDINAL_FLAG)
				{
					//
					// Ordinal import - we can handle named imports
					// ony, so skip it.
					//
					continue;
				}

				PIMAGE_IMPORT_BY_NAME import = (PIMAGE_IMPORT_BY_NAME)
					PtrFromRva(DosHeader, OrigThunk->u1.AddressOfData);

				if (0 == strcmp(ImportedProcName,
					(char*)import->Name))
				{
					//
					// Proc found, patch it.
					//
					DWORD junk;
					MEMORY_BASIC_INFORMATION thunkMemInfo;

					//
					// Make page writable.
					//
					VirtualQuery(
						Thunk,
						&thunkMemInfo,
						sizeof(MEMORY_BASIC_INFORMATION));
					if (!VirtualProtect(
						thunkMemInfo.BaseAddress,
						thunkMemInfo.RegionSize,
						PAGE_EXECUTE_READWRITE,
						&thunkMemInfo.Protect))
					{
						return HRESULT_FROM_WIN32(GetLastError());
					}

					//
					// Replace function pointers (non-atomically).
					//
					if (OldProc)
					{
						*OldProc = (PVOID)(DWORD_PTR)
							Thunk->u1.Function;
					}
#ifdef _WIN64
					Thunk->u1.Function = (ULONGLONG)(DWORD_PTR)
						AlternateProc;
#else
					Thunk->u1.Function = (DWORD)(DWORD_PTR)
						AlternateProc;
#endif
					//
					// Restore page protection.
					//
					if (!VirtualProtect(
						thunkMemInfo.BaseAddress,
						thunkMemInfo.RegionSize,
						thunkMemInfo.Protect,
						&junk))
					{
						return HRESULT_FROM_WIN32(GetLastError());
					}

					return S_OK;
				}
			}

			//
			// Import not found.
			//
			return HRESULT_FROM_WIN32(ERROR_PROC_NOT_FOUND);
		}
	}

	//
	// DLL not found.
	//
	return HRESULT_FROM_WIN32(ERROR_MOD_NOT_FOUND);
}
