#include <windows.h>
#include <stdio.h>

#include "Codegen.h"
#include "Error.h"
#include "X86.h"
#include "Util.h"

#include "resource.h"

static const LPCSTR WarnText = 
"注意：本程序是工程样品！\r\n"
"本程序的功能可能不稳定，请谨慎使用\r\n";

typedef struct st_main_window_data {
    LPSTR lpszEditFileName;
} MainWindowData;

typedef MainWindowData* LPMWDATA;
typedef MainWindowData const* LPCMWDATA;

static WNDPROC EditControlWndProc = NULL;
static HINSTANCE GlobalInstance;

static CHAR FileNameBuffer[4096];
static OPENFILENAME OpenFileName;
static MENUITEMINFO FileNameDispSetter;

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
    HFONT hFont;
    HICON hIcon;
    HMENU hMenu;
    LPMWDATA lpMainWindowData;

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

    GlobalInstance = GetModuleHandle(NULL);
    if (!GlobalInstance) {
        MessageBox(NULL, "无法获取应用程序实例", "错误", MB_ICONSTOP);
        return -1;
    }

    hMenu = LoadMenu(GlobalInstance, MAKEINTRESOURCE(IDR_MAIN_MENU));
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

    hIcon = LoadIcon(GlobalInstance, MAKEINTRESOURCE(IDI_ICON_CEMENT));
    if (!hIcon) {
        MessageBox(NULL, "无法初始化程序图标资源", "错误", MB_ICONSTOP);
        return -1;
    }

    lpMainWindowData = (LPMWDATA)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(MainWindowData));

    hWnd = CreateWindow("edit", "Mini Cement IDE",
                        ES_AUTOVSCROLL | ES_MULTILINE | WS_OVERLAPPEDWINDOW | WS_BORDER,
                        CW_USEDEFAULT, CW_USEDEFAULT, 640, 480,
                        NULL, hMenu, GlobalInstance, NULL);
    if (!hWnd) {
        MessageBox(NULL, "无法初始化程序窗口", "错误", MB_ICONSTOP);
        return -1;
    }

    OpenFileName.lStructSize = sizeof(OPENFILENAME);
    OpenFileName.hwndOwner = hWnd;
    OpenFileName.lpstrFile = FileNameBuffer;
    OpenFileName.nMaxFile = 4095;
    OpenFileName.lpstrFilter = "*.CEM;*.CEMENT";

    FileNameDispSetter.cbSize = sizeof(MENUITEMINFO);
    FileNameDispSetter.fMask = MIIM_TYPE;
    FileNameDispSetter.fType = MFT_STRING;

    EditControlWndProc = (WNDPROC)GetWindowLong(hWnd, GWL_WNDPROC);
    SetWindowLong(hWnd, GWL_WNDPROC, (LONG)MainWindowProc);
    SetWindowLong(hWnd, GWL_USERDATA, (LONG)lpMainWindowData);
    SetWindowText(hWnd, "");
    SendMessage(hWnd, WM_SETFONT, (WPARAM)hFont, (LPARAM)TRUE);
    SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
    SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);

    ShowWindow(hWnd, SW_SHOW);
    
    while (GetMessage(&msg, hWnd, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}

LRESULT CALLBACK MainWindowProc(HWND hWnd,
                                UINT msg,
                                WPARAM wParam,
                                LPARAM lParam) {
    HMENU hMenu = (HMENU)GetMenu(hWnd);
    LPMWDATA lpMainWindowData = (LPMWDATA)GetWindowLong(hWnd, GWL_USERDATA);

    switch (msg) {
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_FILE_NEW: {
            BOOL bModified;
            INT iChoice;

            bModified = (BOOL)SendMessage(hWnd, EM_GETMODIFY, 0, 0);
            if (bModified) {
                iChoice = MessageBox(hWnd, "有未保存的改动，确定继续吗", "警告", MB_OKCANCEL | MB_ICONWARNING);
            }

            if (!bModified || iChoice == IDOK) {
                if (lpMainWindowData->lpszEditFileName != NULL) {
                    HeapFree(GetProcessHeap(), 0, lpMainWindowData->lpszEditFileName);
                    lpMainWindowData->lpszEditFileName = NULL;
                }
                SendMessage(hWnd, WM_SETTEXT, (WPARAM)0, (LPARAM)"");
                FileNameDispSetter.dwTypeData = (DWORD)"(当前未打开文件)";
                SetMenuItemInfo(hMenu, ID_FILE_NAME_DISP, FALSE, &FileNameDispSetter);
            }
            break;
        }
        case ID_FILE_OPEN: {
            BOOL bModified;
            INT iChoice;
            BOOL bSuccess;
            LPSTR lpszFileContent;

            OpenFileName.Flags = OFN_FILEMUSTEXIST | OFN_ENABLESIZING;

            bModified = (BOOL)SendMessage(hWnd, EM_GETMODIFY, 0, 0);
            if (bModified) {
                iChoice = MessageBox(hWnd, "有未保存的改动，确定继续吗", "警告", MB_OKCANCEL | MB_ICONWARNING);
            }

            if (!bModified || iChoice == IDOK) {
                bSuccess = GetOpenFileName(&OpenFileName);
                if (!bSuccess) {
                    break;
                }

                bSuccess = ReadFileToString(OpenFileName.lpstrFile, &lpszFileContent);
                if (!bSuccess) {
                    MessageBox(hWnd, "无法打开指定的文件", "错误", MB_ICONSTOP);
                    break;
                }

                if (lpMainWindowData->lpszEditFileName != NULL) {
                    HeapFree(GetProcessHeap(), 0, lpMainWindowData->lpszEditFileName);
                }
                lpMainWindowData->lpszEditFileName = StringCopy(OpenFileName.lpstrFile);
                FileNameDispSetter.dwTypeData = (DWORD)OpenFileName.lpstrFile;
                SetMenuItemInfo(hMenu, ID_FILE_NAME_DISP, FALSE, &FileNameDispSetter);
                SendMessage(hWnd, WM_SETTEXT, (WPARAM)0, (LPARAM)lpszFileContent);
                HeapFree(GetProcessHeap(), 0, lpszFileContent);
            }
            break;
        }
        case ID_FILE_SAVE: {
            BOOL bSuccess;
            INT iTextLength;
            LPSTR lpszBuffer;
            BOOL bCreateFile;

            OpenFileName.Flags = OFN_CREATEPROMPT | OFN_ENABLESIZING;

            iTextLength = GetWindowTextLength(hWnd);
            lpszBuffer = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, iTextLength + 1);
            if (!lpszBuffer) {
                MessageBox(hWnd, "无法保存文件", "错误", MB_ICONSTOP);
                break;
            }
            GetWindowText(hWnd, lpszBuffer, iTextLength);

            if (lpMainWindowData->lpszEditFileName == NULL) {
                bCreateFile = TRUE;
                bSuccess = GetSaveFileName(&OpenFileName);
                if (!bSuccess) {
                    break;
                }
                lpMainWindowData->lpszEditFileName = StringCopy(OpenFileName.lpstrFile);
            }

            bSuccess = WriteStringToFile(lpMainWindowData->lpszEditFileName, lpszBuffer);
            if (!bSuccess) {
                MessageBox(hWnd, "无法保存到指定的文件", "错误", MB_ICONSTOP);
                if (bCreateFile) {
                    HeapFree(GetProcessHeap(), 0, lpMainWindowData->lpszEditFileName);
                    lpMainWindowData->lpszEditFileName = NULL;
                }
            } else {
                FileNameDispSetter.dwTypeData = (DWORD)lpMainWindowData->lpszEditFileName;
                SetMenuItemInfo(hMenu, ID_FILE_NAME_DISP, FALSE, &FileNameDispSetter);
                SendMessage(hWnd, EM_SETMODIFY, (WPARAM)FALSE, (LPARAM)0);
            }
            HeapFree(GetProcessHeap(), 0, lpszBuffer);
            break;
        }
        case ID_FILE_SAVE_AS: {
            BOOL bSuccess;
            INT iTextLength;
            LPSTR lpszFileName;
            LPSTR lpszBuffer;

            OpenFileName.Flags = OFN_CREATEPROMPT | OFN_ENABLESIZING;
            iTextLength = GetWindowTextLength(hWnd);
            lpszBuffer = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, iTextLength + 1);
            if (!lpszBuffer) {
                MessageBox(hWnd, "无法保存文件", "错误", MB_ICONSTOP);
                break;
            }
            GetWindowText(hWnd, lpszBuffer, iTextLength);

            bSuccess = GetSaveFileName(&OpenFileName);
            if (!bSuccess) {
                break;
            }

            lpszFileName = StringCopy(OpenFileName.lpstrFile);
            bSuccess = WriteStringToFile(lpMainWindowData->lpszEditFileName, lpszBuffer);
            if (!bSuccess) {
                MessageBox(hWnd, "无法保存到指定的文件", "错误", MB_ICONSTOP);
                HeapFree(GetProcessHeap(), 0, lpszFileName);
            } else {
                if (lpMainWindowData->lpszEditFileName != NULL) {
                    HeapFree(GetProcessHeap(), 0, lpMainWindowData->lpszEditFileName);
                }
                FileNameDispSetter.dwTypeData = (DWORD)lpszFileName;
                SetMenuItemInfo(hMenu, ID_FILE_NAME_DISP, FALSE, &FileNameDispSetter);
                lpMainWindowData->lpszEditFileName = lpszFileName;
                SendMessage(hWnd, EM_SETMODIFY, (WPARAM)FALSE, (LPARAM)0);
            }
            HeapFree(GetProcessHeap(), 0, lpszBuffer);
            break;
        }
        case ID_FILE_EXIT:
            return SendMessage(hWnd, WM_CLOSE, 0, 0);
            break;
        case ID_PROG_INTERP:
            break;
        case ID_HELP_ABOUT:
            DialogBox(GlobalInstance, MAKEINTRESOURCE(IDD_ABOUT_DIALOG), hWnd, AboutDlgProc);
            break;
        }
        break;
    case WM_CLOSE: {
        BOOL bModified;
        INT iChoice;

        bModified = (BOOL)SendMessage(hWnd, EM_GETMODIFY, 0, 0);
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
    case WM_CHAR:
        switch (wParam) {
        case '\t': {
            INT i;
            for (i = 0; i < 2; i++) {
                SendMessage(hWnd, WM_CHAR, ' ', 0);
            }
            return TRUE;
        }
        }
        /* fallthrough */
    default:
        return EditControlWndProc(hWnd, msg, wParam, lParam);
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
