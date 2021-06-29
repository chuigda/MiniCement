#include <windows.h>
#include <stdio.h>

#include "Codegen.h"
#include "Error.h"
#include "X86.h"

#include "resource.h"

static const LPCSTR WarnText = 
"ע�⣺�������ǹ�����Ʒ��\r\n"
"������Ĺ��ܿ��ܲ��ȶ��������ʹ��\r\n";

static WNDPROC EditControlWndProc = NULL;
static HINSTANCE GlobalInstance;

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

    for (n = 1; n < argc;  n++) {
        if (!strcmp(argv[n], "/Hl")) {
            bHeadless = TRUE;
        }
    }

    if (!bHeadless) {
        FreeConsole();
        MessageBox(NULL, WarnText, "����", MB_ICONWARNING);
    } else {
        fprintf(stderr, "%s\r\n", WarnText);
    }

    GlobalInstance = GetModuleHandle(NULL);
    if (!GlobalInstance) {
        MessageBox(NULL, "�޷���ȡӦ�ó���ʵ��", "����", MB_ICONSTOP);
        return -1;
    }

    hMenu = LoadMenu(GlobalInstance, MAKEINTRESOURCE(IDR_MAIN_MENU));
    if (!hMenu) {
        MessageBox(NULL, "�޷���ʼ������˵�", "����", MB_ICONSTOP);
        return -1;
    }

    hFont = CreateFont(18, 10, 0, 0,
                       FW_REGULAR, FALSE, FALSE, FALSE,
                       ANSI_CHARSET,
                       OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, PROOF_QUALITY, DEFAULT_PITCH,
                       "Courier New");
    if (!hFont) {
        MessageBox(NULL, "�޷���ʼ���༭������", "����", MB_ICONSTOP);
        return -1;
    }

    hIcon = LoadIcon(GlobalInstance, MAKEINTRESOURCE(IDI_ICON_CEMENT));
    if (!hIcon) {
        MessageBox(NULL, "�޷���ʼ������ͼ����Դ", "����", MB_ICONSTOP);
        return -1;
    }

    hWnd = CreateWindow("edit", "Mini Cement IDE",
                        ES_MULTILINE | WS_OVERLAPPEDWINDOW | WS_BORDER,
                        CW_USEDEFAULT, CW_USEDEFAULT, 640, 480,
                        NULL, hMenu, GlobalInstance, NULL);
    if (!hWnd) {
        MessageBox(NULL, "�޷���ʼ�����򴰿�", "����", MB_ICONSTOP);
        return -1;
    }

    EditControlWndProc = (WNDPROC)GetWindowLong(hWnd, GWL_WNDPROC);
    SetWindowLong(hWnd, GWL_WNDPROC, (LONG)MainWindowProc);
    SetWindowLong(hWnd, GWL_USERDATA, (LONG)NULL);
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
    switch (msg) {
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
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
    case WM_CLOSE:
        DestroyWindow(hWnd);
        break;
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
