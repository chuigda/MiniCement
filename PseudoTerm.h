#pragma once

#include <wtypes.h>

typedef void (*TERMUPDATEPROC)(HANDLE hReadPipe, LPVOID lpvUserData);

extern HWND __cdecl CreatePseudoTerm(TERMUPDATEPROC lpfnTermUpdate, LPVOID lpvUserData);