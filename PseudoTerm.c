#include "PseudoTerm.h"
#include <windows.h>

typedef struct st_term_window_data {
    HWND hOutput;
    HWND hInput;
    HANDLE hPipeInput;
} TermWindowData;

typedef TermWindowData* LPTWINDATA;
typedef TermWindowData const* LPCTWINDATA;

typedef struct st_worker_thread_data {
    HANDLE hPipeOutput;
    TERMUPDATEPROC lpfnTermUpdate;
    LPVOID lpvUserData;
} WorkerThreadData;

typedef WorkerThreadData* LPWTHDATA;
typedef WorkerThreadData const* LPCWTHDATA;

HWND CreatePseudoTerm(TERMUPDATEPROC lpfnTermUpdate, LPVOID lpvUserData) {
    return NULL; // TODO
}
