// lab5.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "lab5.h"
#include <stdio.h>
#include <Windows.h>
#include <string>
#include <vector>

#define IDC_EDIT_SUBKEY 1101
#define IDC_BUTTON_CREATE_KEY 1102
#define IDC_BUTTON_DELETE_KEY 1103
#define IDC_BUTTON_ADD_NEW_PARAMETER 1104

#define MAX_LOADSTRING 100

HWND hWnd;
HMENU hMenu;
HWND hEdit;
HWND hButton, hButtonDelete, hButtonAddNewParameter;
//HWND hValueNameEdit, hStringValueRadio, hDwordValueRadio;
//HWND hMainWindow;

HWND hComboBox;
HWND hValueDataEdit;
HWND hValueNameEdit;

HWND hMainWindow, hValueList;
HWND hSubKeyEdit;

struct RegistryValue
{
    std::wstring name;
    DWORD type;
    std::wstring data;
};

bool CreateRegistryKey(HKEY hRootKey, LPCWSTR subKey);
void PopulateValueList(HKEY hKey, LPCWSTR subKey);
void LoadRegistryInfoToListBox(HWND hListBox, LPCWSTR subKey);
bool DeleteRegistryKey(HKEY hKey, LPCWSTR subKey);
bool SetRegistryValue(HKEY hKey, LPCSTR subKey, LPCSTR valueName, DWORD valueType, const BYTE* valueData, DWORD dataSize);
bool CreateRegistryValue();

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_LAB5, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB5));

    MSG msg;

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}


ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LAB5));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_LAB5);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   const wchar_t CLASS_NAME[] = L"RegistryEditor";

   hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   hEdit = CreateWindowEx(
       WS_EX_CLIENTEDGE,
       L"EDIT",
       L"",
       WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
       10, 10, 300, 25,
       hWnd,
       (HMENU)IDC_EDIT_SUBKEY,
       hInstance,
       NULL
   );

   hButton = CreateWindowEx(
       0,
       L"BUTTON",
       L"Создать ключ",
       WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
       10, 45, 120, 30,
       hWnd,
       (HMENU)IDC_BUTTON_CREATE_KEY,
       hInstance,
       NULL
   );

   hButtonDelete = CreateWindowEx(
       0,
       L"BUTTON",
       L"Удалить ключ",
       WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
       140, 45, 120, 30, // Новые координаты (140, 45)
       hWnd,
       (HMENU)IDC_BUTTON_DELETE_KEY, // Новый идентификатор
       hInstance,
       NULL
   );

   hButtonAddNewParameter = CreateWindowEx(
       0,
       L"BUTTON",
       L"Создать параметр",
       WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
       270, 45, 120, 30, // Новые координаты (270, 45)
       hWnd,
       (HMENU)IDC_BUTTON_ADD_NEW_PARAMETER, // Новый идентификатор
       hInstance,
       NULL
   );

   hMenu = CreateMenu();
   HMENU hSubMenu = CreatePopupMenu();
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
        //hValueNameEdit = CreateWindowEx(0, L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
        //    50, 50, 200, 25, hWnd, NULL, NULL, NULL);

        //// Радиокнопка для строки (REG_SZ)
        //hStringValueRadio = CreateWindowEx(0, L"BUTTON", L"String Value", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
        //    50, 100, 150, 25, hWnd, NULL, NULL, NULL);
        //SendMessage(hStringValueRadio, BM_SETCHECK, BST_CHECKED, 0);

        //// Радиокнопка для DWORD (REG_DWORD)
        //hDwordValueRadio = CreateWindowEx(0, L"BUTTON", L"DWORD Value", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
        //    50, 125, 150, 25, hWnd, NULL, NULL, NULL);

        //// Кнопка "Изменить значение"
        //CreateWindowEx(0, L"BUTTON", L"Change Value", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        //    50, 175, 100, 25, hWnd, (HMENU)1, NULL, NULL);

        //            // Поле для ввода subKey
        //hSubKeyEdit = CreateWindowEx(0, L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
        //    50, 50, 200, 25, hWnd, NULL, NULL, NULL);

        //// Кнопка "Получить значения"
        //CreateWindowEx(0, L"BUTTON", L"Get Values", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        //    50, 100, 100, 25, hWnd, (HMENU)1, NULL, NULL);

        //// `listbox` для отображения значений
        //hValueList = CreateWindowEx(0, L"LISTBOX", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_DISABLENOSCROLL | LBS_HASSTRINGS | LBS_NOINTEGRALHEIGHT,
        //    50, 150, 300, 200, hWnd, NULL, NULL, NULL);

        hComboBox = CreateWindow(TEXT("ComboBox"), NULL, WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST,
            10, 100, 200, 200, hWnd, NULL, NULL, NULL);

        // Добавление элементов в выпадающий список с соответствующими типами данных
        SendMessage(hComboBox, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(TEXT("Строковый параметр")));
        SendMessage(hComboBox, CB_SETITEMDATA, 0, REG_SZ);
        SendMessage(hComboBox, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(TEXT("Параметр DWORD(32 бита)")));
        SendMessage(hComboBox, CB_SETITEMDATA, 1, REG_DWORD);
        SendMessage(hComboBox, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(TEXT("Параметр QWORD(64 бита)")));
        SendMessage(hComboBox, CB_SETITEMDATA, 2, REG_QWORD);

        // Создание поля ввода для значения данных
        hValueDataEdit = CreateWindow(TEXT("EDIT"), NULL, WS_VISIBLE | WS_CHILD | WS_BORDER,
            10, 200, 150, 20, hWnd, NULL, NULL, NULL);

        // Создание поля ввода для имени значения
        hValueNameEdit = CreateWindow(TEXT("EDIT"), NULL, WS_VISIBLE | WS_CHILD | WS_BORDER,
            10, 300, 150, 20, hWnd, NULL, NULL, NULL);

    }
    break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case IDC_BUTTON_CREATE_KEY:
        {
            int length = GetWindowTextLength(hEdit);
            LPWSTR subKey = new WCHAR[length + 1];
            GetWindowText(hEdit, subKey, length + 1);
            if (CreateRegistryKey(HKEY_CURRENT_USER, subKey))
            {
                MessageBox(hWnd, L"Ключ успешно создан в реестре.", L"Успех", MB_OK | MB_ICONINFORMATION);
            }
            else
            {
                MessageBox(hWnd, L"Ошибка при создании ключа в реестре.", L"Ошибка", MB_OK | MB_ICONERROR);
            }
            delete[] subKey; // Не забудьте освободить память
        }
        break;

        case IDC_BUTTON_DELETE_KEY:
        {
            int length = GetWindowTextLength(hEdit);
            LPWSTR subKey = new WCHAR[length + 1];
            GetWindowText(hEdit, subKey, length + 1);
            
            HKEY hKey;
            if (RegOpenKeyExW(HKEY_CURRENT_USER, NULL, 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS) {
                // Удалить подключ реестра
                if (DeleteRegistryKey(hKey, subKey)) {
                    MessageBox(hWnd, L"Ключ реестра успешно удален.", L"Успех", MB_OK | MB_ICONINFORMATION);
                }
                else {
                    MessageBox(hWnd, L"Не удалось удалить ключ реестра.", L"Ошибка", MB_OK | MB_ICONINFORMATION);
                }

                // Закрыть ключ реестра
                RegCloseKey(hKey);
            }
            else {
                MessageBox(hWnd, L"Не удалось открыть ключ реестра.", L"Ошибка", MB_OK | MB_ICONINFORMATION);
            }

            delete[] subKey; // Не забудьте освободить память
        }
        break;
        case IDC_BUTTON_ADD_NEW_PARAMETER:
        {
            //int length = GetWindowTextLength(hEdit);
            //LPWSTR subKey = new WCHAR[length + 1];
            //GetWindowText(hEdit, subKey, length + 1);



            //LPCSTR subKey = "Software\\MyApp";

            //LPCSTR valueName = "MyValue";
            //DWORD valueType = REG_SZ;
            //const BYTE* valueData = (const BYTE*)"Hello, World!";
            //DWORD dataSize = strlen((const char*)valueData) + 1;

            //if (SetRegistryValue(HKEY_CURRENT_USER, subKey, valueName, valueType, valueData, dataSize)) {
            //    MessageBox(hWnd, L"Successfully created registry key and set value.", L"Успех", MB_OK | MB_ICONINFORMATION);
            //}

            CreateRegistryValue();
        }
        break;
        }
        break;
        // Остальные случаи обработки команд
    }

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

bool CreateRegistryKey(HKEY hRootKey, LPCWSTR subKey)
{
    HKEY hKey;
    LONG result;

    result = RegCreateKeyEx(hRootKey, subKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL);
    if (result == ERROR_SUCCESS)
    {
        RegCloseKey(hKey);
        return true;
    }
    else
    {
        return false;
    }
}

bool DeleteRegistryKey(HKEY hKey, LPCWSTR subKey) {
    // Удалить ключ реестра
    if (RegDeleteKeyW(hKey, subKey) == ERROR_SUCCESS) {
        return true;
    }
    else {
        return false;
    }
}

bool SetRegistryValue(HKEY hKey, LPCSTR subKey, LPCSTR valueName, DWORD valueType, const BYTE* valueData, DWORD dataSize)
{
    HKEY hSubKey;
    LONG openResult = RegCreateKeyExA(hKey, subKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hSubKey, NULL);

    if (openResult != ERROR_SUCCESS) {
        MessageBox(hWnd, L"Failed to create registry key.", L"Успех", MB_OK | MB_ICONINFORMATION);
        return false;
    }

    LONG setValueResult = RegSetValueExA(hSubKey, valueName, 0, valueType, valueData, dataSize);

    if (setValueResult != ERROR_SUCCESS) {
        MessageBox(hWnd, L"Failed to set registry value", L"Успех", MB_OK | MB_ICONINFORMATION);
        RegCloseKey(hSubKey);
        return false;
    }

    RegCloseKey(hSubKey);
    return true;
}

bool CreateRegistryValue()
{
    // Получение выбранного типа данных из выпадающего списка
    int selectedIndex = SendMessage(hComboBox, CB_GETCURSEL, 0, 0);
    DWORD valueType = static_cast<DWORD>(SendMessage(hComboBox, CB_GETITEMDATA, selectedIndex, 0));

    // Получение введенного значения данных из поля ввода
    char valueData[256];
    GetWindowTextA(hValueDataEdit, valueData, sizeof(valueData));

    // Получение введенного имени значения из поля ввода
    char valueName[256];
    GetWindowTextA(hValueNameEdit, valueName, sizeof(valueName));

    // Создание нового значения в реестре
    HKEY hKey;
    LPCSTR subKey = "Software\\MyApp";
    LONG openResult = RegCreateKeyExA(HKEY_CURRENT_USER, subKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL);

    if (openResult != ERROR_SUCCESS) {
        MessageBox(NULL, L"Failed to create registry key", L"Error", MB_ICONERROR);
        return FALSE;
    }

    LONG setValueResult;
    if (valueType == 4) {

        DWORD result;
        char* endPtr; // Указатель, указывающий на первый символ, не преобразованный в число

        result = strtoul(valueData, &endPtr, 10);

        if (*endPtr == '\0') {
            // Преобразование прошло успешно, строка была полностью преобразована в число
            // result теперь содержит число в формате DWORD
        }
        else {
            // Преобразование не удалось, строка содержит нечисловые символы
            // Обработка ошибки здесь
        }
        /*setValueResult = RegSetValueExA(hKey, valueName, 0, valueType, reinterpret_cast<const BYTE*>(result), strlen(valueData) + 1);*/
        BYTE data[sizeof(DWORD)]; // Создаем буфер для хранения данных DWORD
        memcpy(data, &result, sizeof(DWORD)); // Копируем DWORD в буфер
        setValueResult = RegSetValueExA(hKey, valueName, 0, REG_DWORD, data, sizeof(DWORD));
    }
    else if (valueType == 1) {
        setValueResult = RegSetValueExA(hKey, valueName, 0, valueType, reinterpret_cast<const BYTE*>(valueData), strlen(valueData) + 1);
    }


    if (setValueResult != ERROR_SUCCESS) {
        MessageBox(NULL, L"Failed to set registry value", L"Error", MB_ICONERROR);
        RegCloseKey(hKey);
        return FALSE;
    }

    RegCloseKey(hKey);
    MessageBox(NULL, L"Registry value created successfully", L"Success", MB_ICONINFORMATION);

    return TRUE;
}

void DeleteSubkeys(HKEY hKey) {
    TCHAR achKey[255]; // Буфер для имени подключа
    DWORD cbName;      // Длина имени подключа
    LONG lResult;

    while (true) {
        cbName = 255;
        lResult = RegEnumKeyEx(hKey, 0, achKey, &cbName, NULL, NULL, NULL, NULL);
        if (lResult == ERROR_NO_MORE_ITEMS) {
            break;
        }
        if (lResult == ERROR_SUCCESS) {
            HKEY hSubKey;
            if (RegOpenKeyEx(hKey, achKey, 0, KEY_ALL_ACCESS, &hSubKey) == ERROR_SUCCESS) {
                // Рекурсивно удаляем подключи
                DeleteSubkeys(hSubKey);
                RegCloseKey(hSubKey);
            }
        }
    }
}

