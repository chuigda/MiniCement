#include <windows.h>
#include <stdio.h>

#include "CodeEdit.h"
#include "Codegen.h"
#include "Error.h"
#include "GlobalContext.h"
#include "X86.h"
#include "Util.h"

#include "resource.h"

static const LPCSTR WarnText = 
"注意：本程序是工程样品！\r\n"
"本程序的功能可能不稳定，请谨慎使用\r\n";

static const LPCSTR MainWindowClassName = "MainWindowClassName_79431242";

typedef struct st_main_window_init {
    GlobalContext globalContext;
    HFONT hFont;
} MainWindowInit;

typedef MainWindowInit* LPMWINIT;
typedef MainWindowInit const* LPCMWINIT;

typedef struct st_main_window_data {    
    /* handles to children widgets */
    HWND hWndEdit;
    HWND hWndStatus;

    /* context data */
    LPSTR lpszEditFileName;

    /* global context */
    GlobalContext globalContext;

    /* cached stuff */
    CHAR szFileNameBuffer[4096];
    OPENFILENAME openFileName;
} MainWindowData;

typedef MainWindowData* LPMWDATA;
typedef MainWindowData const* LPCMWDATA;

LRESULT CALLBACK MainWindowProc(HWND hWnd,
                                UINT msg,
                                WPARAM wParam,
                                LPARAM lParam);
LRESULT CALLBACK AboutDlgProc(HWND hwnd,
                              UINT msg,
                              WPARAM wParam,
                              LPARAM lParam);

int main(int argc, const char *argv[]) {
    INT n;
    BOOL bHeadless = FALSE;

    HWND hWnd;
    MSG msg;
    WNDCLASS wndClass;

    HFONT hFont;
    HICON hIcon;
    HMENU hMenu;
    MainWindowInit mainWindowInit;

    GlobalContext globalContext;

    for (n = 1; n < argc;  n++) {
        if (!strcmp(argv[n], "/Hl")) {
            bHeadless = TRUE;
        }
    }

    if (!bHeadless) {
        FreeConsole();
        MessageBox(NULL, WarnText, "警告", MB_ICONWARNING);
    } else {
        fprintf(stderr, "%s\r\n", WarnText);
    }

    globalContext.hInstance = GetModuleHandle(NULL);
    globalContext.hHeap = GetProcessHeap();
    if (!globalContext.hInstance) {
        MessageBox(NULL, "无法获取应用程序实例", "错误", MB_ICONSTOP);
        return -1;
    }

    hMenu = LoadMenu(globalContext.hInstance, MAKEINTRESOURCE(IDR_MAIN_MENU));
    if (!hMenu) {
        MessageBox(NULL, "无法初始化程序菜单", "错误", MB_ICONSTOP);
        return -1;
    }

    hFont = CreateFont(16, 8, 0, 0,
                       FW_REGULAR, FALSE, FALSE, FALSE,
                       ANSI_CHARSET,
                       OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, PROOF_QUALITY, DEFAULT_PITCH,
                       "SimSun");
    if (!hFont) {
        MessageBox(NULL, "无法初始化编辑器字体", "错误", MB_ICONSTOP);
        return -1;
    }

    hIcon = LoadIcon(globalContext.hInstance, MAKEINTRESOURCE(IDI_ICON_CEMENT));
    if (!hIcon) {
        MessageBox(NULL, "无法初始化程序图标资源", "错误", MB_ICONSTOP);
        return -1;
    }

    wndClass.style = 0;
    wndClass.lpfnWndProc = MainWindowProc;
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hInstance = globalContext.hInstance;
    wndClass.hIcon = hIcon;
    wndClass.hCursor = LoadCursor(globalContext.hInstance, IDC_ARROW);
    wndClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wndClass.lpszMenuName = MAKEINTRESOURCE(IDR_MAIN_MENU);
    wndClass.lpszClassName = MainWindowClassName;
    if (!RegisterClass(&wndClass)) {
        MessageBox(NULL, "注册窗口类失败", "错误", MB_ICONSTOP);
        return -1;
    }

    mainWindowInit.hFont = hFont;
    mainWindowInit.globalContext = globalContext;
    hWnd = CreateWindow(MainWindowClassName, "Mini Cement IDE",
                        WS_OVERLAPPEDWINDOW | WS_BORDER,
                        CW_USEDEFAULT, CW_USEDEFAULT, 640, 480,
                        NULL, hMenu, globalContext.hInstance, &mainWindowInit);
    if (!hWnd) {
        MessageBox(NULL, "无法初始化程序窗口", "错误", MB_ICONSTOP);
        return -1;
    }

    ShowWindow(hWnd, SW_SHOW);
    
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}

LRESULT CALLBACK MainWindowProc(HWND hWnd,
                                UINT msg,
                                WPARAM wParam,
                                LPARAM lParam) {
    LPMWDATA lpMainWindowData = (LPMWDATA)GetWindowLong(hWnd, GWL_USERDATA);

    switch (msg) {
    case WM_CREATE: {
        LPCREATESTRUCTA lpCreateStruct;
        LPCMWINIT lpMainWindowInit;
        RECT rect;

        lpCreateStruct = (LPCREATESTRUCT)lParam;
        lpMainWindowInit = (LPMWINIT)lpCreateStruct->lpCreateParams;

        lpMainWindowData = (LPMWDATA)HeapAlloc(lpMainWindowInit->globalContext.hHeap, HEAP_ZERO_MEMORY, sizeof(MainWindowData));
        if (!lpMainWindowData) {
            MessageBox(NULL, "无法初始化程序窗口", "错误", MB_ICONSTOP);
            PostQuitMessage(-1);
            break;
        }

        lpMainWindowData->globalContext = lpMainWindowInit->globalContext;

        lpMainWindowData->openFileName.lStructSize = sizeof(OPENFILENAME);
        lpMainWindowData->openFileName.hwndOwner = hWnd;
        lpMainWindowData->openFileName.lpstrFile = lpMainWindowData->szFileNameBuffer;
        lpMainWindowData->openFileName.nMaxFile = 4095;
        lpMainWindowData->openFileName.lpstrFilter = "*.CEM;*.CEMENT";

        GetClientRect(hWnd, &rect);
        lpMainWindowData->hWndEdit = CreateCodeEditControl(hWnd, 0, 0, rect.right, rect.bottom);
        if (!lpMainWindowData->hWndEdit) {
            MessageBox(NULL, "无法初始化程序窗口", "错误", MB_ICONSTOP);
            PostQuitMessage(-1);
            break;
        }

        SendMessage(lpMainWindowData->hWndEdit, WM_SETFONT, (WPARAM)lpMainWindowInit->hFont, (LPARAM)TRUE);
        SetWindowLong(hWnd, GWL_USERDATA, (LONG)lpMainWindowData);
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    case WM_SIZE: {
        MoveWindow(lpMainWindowData->hWndEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
        break;
    }
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_FILE_NEW: {
            BOOL bModified;
            INT iChoice;

            bModified = (BOOL)SendMessage(lpMainWindowData->hWndEdit, EM_GETMODIFY, 0, 0);
            if (bModified) {
                iChoice = MessageBox(hWnd, "有未保存的改动，确定继续吗", "警告", MB_OKCANCEL | MB_ICONWARNING);
            }

            if (!bModified || iChoice == IDOK) {
                if (lpMainWindowData->lpszEditFileName != NULL) {
                    HeapFree(lpMainWindowData->globalContext.hHeap, 0, lpMainWindowData->lpszEditFileName);
                    lpMainWindowData->lpszEditFileName = NULL;
                }
                SendMessage(lpMainWindowData->hWndEdit, WM_SETTEXT, (WPARAM)0, (LPARAM)"");
            }
            break;
        }
        case ID_FILE_OPEN: {
            BOOL bModified;
            INT iChoice;
            BOOL bSuccess;
            LPSTR lpszFileContent;

            lpMainWindowData->openFileName.Flags = OFN_FILEMUSTEXIST | OFN_ENABLESIZING;

            bModified = (BOOL)SendMessage(lpMainWindowData->hWndEdit, EM_GETMODIFY, 0, 0);
            if (bModified) {
                iChoice = MessageBox(hWnd, "有未保存的改动，确定继续吗", "警告", MB_OKCANCEL | MB_ICONWARNING);
            }

            if (!bModified || iChoice == IDOK) {
                bSuccess = GetOpenFileName(&lpMainWindowData->openFileName);
                if (!bSuccess) {
                    break;
                }

                bSuccess = ReadFileToString(lpMainWindowData->openFileName.lpstrFile, &lpszFileContent);
                if (!bSuccess) {
                    MessageBox(hWnd, "无法打开指定的文件", "错误", MB_ICONSTOP);
                    break;
                }

                if (lpMainWindowData->lpszEditFileName != NULL) {
                    HeapFree(lpMainWindowData->globalContext.hHeap, 0, lpMainWindowData->lpszEditFileName);
                }
                lpMainWindowData->lpszEditFileName = StringCopy(lpMainWindowData->openFileName.lpstrFile);
                SendMessage(lpMainWindowData->hWndEdit, WM_SETTEXT, (WPARAM)0, (LPARAM)lpszFileContent);
                HeapFree(lpMainWindowData->globalContext.hHeap, 0, lpszFileContent);
            }
            break;
        }
        case ID_FILE_SAVE: {
            BOOL bSuccess;
            INT iTextLength;
            LPSTR lpszBuffer;
            BOOL bCreateFile;

            lpMainWindowData->openFileName.Flags = OFN_CREATEPROMPT | OFN_ENABLESIZING;

            iTextLength = GetWindowTextLength(lpMainWindowData->hWndEdit);
            lpszBuffer = HeapAlloc(lpMainWindowData->globalContext.hHeap, HEAP_ZERO_MEMORY, iTextLength + 1);
            if (!lpszBuffer) {
                MessageBox(hWnd, "无法保存文件", "错误", MB_ICONSTOP);
                break;
            }
            GetWindowText(lpMainWindowData->hWndEdit, lpszBuffer, iTextLength + 1);

            if (lpMainWindowData->lpszEditFileName == NULL) {
                bCreateFile = TRUE;
                bSuccess = GetSaveFileName(&lpMainWindowData->openFileName);
                if (!bSuccess) {
                    break;
                }
                lpMainWindowData->lpszEditFileName = StringCopy(lpMainWindowData->openFileName.lpstrFile);
            } else {
                bCreateFile = FALSE;
            }

            bSuccess = WriteStringToFile(lpMainWindowData->lpszEditFileName, lpszBuffer);
            if (!bSuccess) {
                MessageBox(hWnd, "无法保存到指定的文件", "错误", MB_ICONSTOP);
                if (bCreateFile) {
                    HeapFree(lpMainWindowData->globalContext.hHeap, 0, lpMainWindowData->lpszEditFileName);
                    lpMainWindowData->lpszEditFileName = NULL;
                }
            } else {
                SendMessage(lpMainWindowData->hWndEdit, EM_SETMODIFY, (WPARAM)FALSE, (LPARAM)0);
            }
            HeapFree(lpMainWindowData->globalContext.hHeap, 0, lpszBuffer);
            break;
        }
        case ID_FILE_SAVE_AS: {
            BOOL bSuccess;
            INT iTextLength;
            LPSTR lpszFileName;
            LPSTR lpszBuffer;

            lpMainWindowData->openFileName.Flags = OFN_CREATEPROMPT | OFN_ENABLESIZING;
            iTextLength = GetWindowTextLength(lpMainWindowData->hWndEdit);
            lpszBuffer = HeapAlloc(lpMainWindowData->globalContext.hHeap, HEAP_ZERO_MEMORY, iTextLength + 1);
            if (!lpszBuffer) {
                MessageBox(hWnd, "无法保存文件", "错误", MB_ICONSTOP);
                break;
            }
            GetWindowText(lpMainWindowData->hWndEdit, lpszBuffer, iTextLength + 1);

            bSuccess = GetSaveFileName(&lpMainWindowData->openFileName);
            if (!bSuccess) {
                break;
            }

            lpszFileName = StringCopy(lpMainWindowData->openFileName.lpstrFile);
            bSuccess = WriteStringToFile(lpszFileName, lpszBuffer);
            if (!bSuccess) {
                MessageBox(hWnd, "无法保存到指定的文件", "错误", MB_ICONSTOP);
                HeapFree(lpMainWindowData->globalContext.hHeap, 0, lpszFileName);
            } else {
                if (lpMainWindowData->lpszEditFileName != NULL) {
                    HeapFree(lpMainWindowData->globalContext.hHeap, 0, lpMainWindowData->lpszEditFileName);
                }
                lpMainWindowData->lpszEditFileName = lpszFileName;
                SendMessage(lpMainWindowData->hWndEdit, EM_SETMODIFY, (WPARAM)FALSE, (LPARAM)0);
            }
            HeapFree(lpMainWindowData->globalContext.hHeap, 0, lpszBuffer);
            break;
        }
        case ID_FILE_EXIT:
            return SendMessage(hWnd, WM_CLOSE, 0, 0);
            break;
        case ID_PROG_INTERP:
            break;
        case ID_HELP_ABOUT:
            DialogBox(lpMainWindowData->globalContext.hInstance,
                      MAKEINTRESOURCE(IDD_ABOUT_DIALOG), hWnd, AboutDlgProc);
            break;
        }
        break;
    case WM_CLOSE: {
        BOOL bModified;
        INT iChoice;

        bModified = (BOOL)SendMessage(lpMainWindowData->hWndEdit, EM_GETMODIFY, 0, 0);
        if (bModified) {
            iChoice = MessageBox(hWnd, "有未保存的改动，确定继续吗", "警告", MB_OKCANCEL | MB_ICONWARNING);
        }

        if (!bModified || iChoice == IDOK) {
            DestroyWindow(hWnd);
        }
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    return TRUE;
}

LRESULT CALLBACK AboutDlgProc(HWND hWnd,
                              UINT msg,
                              WPARAM wParam,
                              LPARAM lParam) {
    switch (msg) {
    case WM_INITDIALOG:
        return TRUE;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDOK:
            EndDialog(hWnd, 0);
            break;
        }
        break;
    case WM_CLOSE:
        EndDialog(hWnd, 0);
        break;
    default:
        return FALSE;
    }
    return TRUE;
}
