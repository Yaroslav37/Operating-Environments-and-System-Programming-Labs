#include "framework.h"
#include "lab3.h"
#include <windows.h>
#include <CommCtrl.h>
#include <psapi.h>
#include <cstdio>


#define SIZE 1024
#define ID_UPDATE_BUTTON 1
#define ID_TERMINATE_BUTTON 2
DWORD processes[SIZE];

HWND listBoxControl = NULL;
HWND terminateButton = NULL;
HWND updateButton = NULL;

#define MAX_LOADSTRING 100

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

#define IDM_CODE_SAMPLES 2001 

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

void updateProcessList();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_LAB3, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB3));

    MSG msg;

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LAB3));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_LAB3);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance;

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    SetWindowText(hWnd, L"Process Viewer");

    HMENU hMenu = CreateMenu();
    SetMenu(hWnd, hMenu);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        ZeroMemory(processes, sizeof(processes));

        RECT clientRect;
        GetClientRect(hWnd, &clientRect);

        listBoxControl = CreateWindowW(
            L"LISTBOX",
            L"",
            WS_CHILD | WS_VISIBLE | LBS_STANDARD,
            0, 0, clientRect.right - clientRect.left, clientRect.bottom - clientRect.top - 50,
            hWnd,
            NULL,
            hInst,
            NULL);

        terminateButton = CreateWindowW(
            L"BUTTON",
            L"Terminate Process",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            0, clientRect.bottom - clientRect.top - 50, 150, 30,
            hWnd,
            (HMENU)ID_TERMINATE_BUTTON,
            hInst,
            NULL);

        updateButton = CreateWindowW(
            L"BUTTON",
            L"Update",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            160,
            clientRect.bottom - clientRect.top - 50,
            150,
            30,
            hWnd,
            (HMENU)ID_UPDATE_BUTTON,
            hInst,
            NULL);

        updateProcessList();
    }
    break;
    case WM_SIZE:
    {
        RECT clientRect;
        GetClientRect(hWnd, &clientRect);

        SetWindowPos(listBoxControl, 0, 0, 0, clientRect.right - clientRect.left, clientRect.bottom - clientRect.top - 50, SWP_NOMOVE);
        SetWindowPos(terminateButton, 0, 0, clientRect.bottom - clientRect.top - 50, 150, 30, SWP_NOSIZE);
        SetWindowPos(updateButton, 0, 160, clientRect.bottom - clientRect.top - 50, 150, 30, SWP_NOSIZE);
    }
    break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case ID_UPDATE_BUTTON:
            updateProcessList();
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        case ID_TERMINATE_BUTTON:
        {
            int index = SendMessage(listBoxControl, LB_GETCURSEL, 0, 0);

            TCHAR buffer[256];
            
            SendMessage(listBoxControl, LB_GETTEXT, (WPARAM)index, (LPARAM)buffer);

            DWORD processId;
            
            swscanf_s(buffer, L"%*[^:]: %u", &processId);

            if (index != LB_ERR)
            {
                HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
                if (hProcess != NULL)
                {
                    BOOL result = TerminateProcess(hProcess, 1);
                    CloseHandle(hProcess);
                    if (!result)
                    {
                        TCHAR buffer[256];
                        SendMessage(listBoxControl, LB_GETTEXT, index, (LPARAM)buffer);

                        TCHAR msg[512];
                        _stprintf_s(msg, _countof(msg), TEXT("Не удалось завершить процесс: %s"), buffer);
                        MessageBox(NULL, msg, TEXT("Ошибка"), MB_OK | MB_ICONERROR);
                    }
                    else
                    {
                        TCHAR buffer[256];
                        SendMessage(listBoxControl, LB_GETTEXT, index, (LPARAM)buffer);
                        updateProcessList();
                    }
                }
                else
                {
                    TCHAR buffer[256];
                    SendMessage(listBoxControl, LB_GETTEXT, index, (LPARAM)buffer);

                    TCHAR msg[512];
                    _stprintf_s(msg, _countof(msg), TEXT("Не удалось получить доступ к процессу: %s"), buffer);
                    MessageBox(NULL, msg, TEXT("Ошибка"), MB_OK | MB_ICONERROR);
                }
            }
        }
        break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void updateProcessList()
{
    DWORD cbNeeded;
    DWORD cProcesses;
    unsigned int i;

    ZeroMemory(processes, sizeof(processes));

    if (!EnumProcesses(processes, sizeof(processes), &cbNeeded))
    {
        // Обработка ошибок
        return;
    }

    cProcesses = cbNeeded / sizeof(DWORD);

    SendMessage(listBoxControl, LB_RESETCONTENT, 0, 0);

    for (i = 0; i < cProcesses; i++)
    {
        if (processes[i] != 0)
        {
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processes[i]);

            if (NULL != hProcess)
            {
                char szProcessName[MAX_PATH] = "<unknown>";

                if (GetModuleBaseNameA(hProcess, NULL, szProcessName, sizeof(szProcessName) / sizeof(char)))
                {
                    wchar_t buffer[256];

                    PROCESS_MEMORY_COUNTERS_EX pmc;
                    if (GetProcessMemoryInfo(hProcess, (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc)))
                    {
                        unsigned long long memoryMB = pmc.PrivateUsage / 1024 / 1024;

                        swprintf_s(buffer, 256, L"%S (PID: %u, Memory: %llu MB)", szProcessName, processes[i], memoryMB);
                    }
                    else
                    {
                        swprintf_s(buffer, 256, L"%S (PID: %u)", szProcessName, processes[i]);
                    }

                    SendMessageW(listBoxControl, LB_ADDSTRING, 0, (LPARAM)buffer);
                }

                CloseHandle(hProcess);
            }
        }
    }
}