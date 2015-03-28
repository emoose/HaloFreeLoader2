// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the HALOFREE_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// HALOFREE_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef HALOFREE_EXPORTS
#define HALOFREE_API __declspec(dllexport)
#else
#define HALOFREE_API __declspec(dllimport)
#endif

// This class is exported from the HaloFree.dll
class HALOFREE_API CHaloFree {
public:
	CHaloFree(void);
	// TODO: add your methods here.
};

extern HALOFREE_API int nHaloFree;

HALOFREE_API int fnHaloFree(void);
