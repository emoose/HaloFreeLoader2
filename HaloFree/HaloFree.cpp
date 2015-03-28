// HaloFree.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "HaloFree.h"


// This is an example of an exported variable
HALOFREE_API int nHaloFree=0;

// This is an example of an exported function.
HALOFREE_API int fnHaloFree(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see HaloFree.h for the class definition
CHaloFree::CHaloFree()
{
	return;
}
