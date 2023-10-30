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

#define MAX_LOADSTRING 100

HWND hWnd;
HMENU hMenu;
HWND hEdit;
HWND hButton, hButtonDelete;
HWND hValueNameEdit, hStringValueRadio, hDwordValueRadio;
//HWND hMainWindow;

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

//std::vector<RegistryValue> GetRegistryValues(HKEY hKey, LPCWSTR subKey)
//{
//    std::vector<RegistryValue> values;
//
//    HKEY hSubKey = NULL;
//    if (RegOpenKeyEx(hKey, subKey, 0, KEY_READ, &hSubKey) == ERROR_SUCCESS)
//    {
//        DWORD valueCount = 0;
//        DWORD maxValueNameLength = 0;
//        if (RegQueryInfoKey(hSubKey, NULL, NULL, NULL, NULL, NULL, NULL, &valueCount, &maxValueNameLength, NULL, NULL, NULL) == ERROR_SUCCESS)
//        {
//            for (DWORD i = 0; i < valueCount; i++)
//            {
//                DWORD valueNameLength = maxValueNameLength + 1;
//                WCHAR* valueNameBuffer = new WCHAR[valueNameLength];
//
//                DWORD valueType = 0;
//                DWORD valueDataSize = 0;
//
//                if (RegEnumValue(hSubKey, i, valueNameBuffer, &valueNameLength, NULL, &valueType, NULL, &valueDataSize) == ERROR_SUCCESS)
//                {
//                    if (valueType == REG_SZ || valueType == REG_EXPAND_SZ || valueType == REG_MULTI_SZ)
//                    {
//                        /*WCHAR* valueDataBuffer = new WCHAR[valueDataSize / sizeof(WCHAR)];*/
//                        BYTE valueDataBuffer[1024];
//
//                        if (RegEnumValue(hSubKey, i, valueNameBuffer, &valueNameLength, NULL, &valueType, reinterpret_cast<LPBYTE>(valueDataBuffer), &valueDataSize) == ERROR_SUCCESS)
//                        {
//                            RegistryValue value;
//                            value.name = valueNameBuffer;
//                            value.type = valueType;
//                            //value.data = valueDataBuffer;
//                            values.push_back(value);
//                        }
//
//                        delete[] valueDataBuffer;
//                    }
//                    else if (valueType == REG_DWORD || valueType == REG_QWORD)
//                    {
//                        DWORD valueData = 0;
//                        DWORD dataSize = sizeof(valueData); // Используйте правильный размер
//
//                        if (RegEnumValue(hSubKey, i, valueNameBuffer, &valueNameLength, NULL, &valueType, reinterpret_cast<LPBYTE>(&valueData), &dataSize) == ERROR_SUCCESS)
//                        {
//                            RegistryValue value;
//                            value.name = valueNameBuffer;
//                            value.type = valueType;
//                            value.data = std::to_wstring(valueData);
//                            values.push_back(value);
//                        }
//                    }
//                }
//
//
//                delete[] valueNameBuffer;
//            }
//        }
//
//        RegCloseKey(hSubKey);
//    }
//
//    return values;
//}

//std::vector<RegistryValue> GetRegistryValues(HKEY hKey, LPCWSTR subKey)
//{
//    std::vector<RegistryValue> values;
//
//    HKEY hSubKey = NULL;
//    if (RegOpenKeyEx(hKey, subKey, 0, KEY_READ, &hSubKey) == ERROR_SUCCESS)
//    {
//        DWORD valueCount = 0;
//        if (RegQueryInfoKey(hSubKey, NULL, NULL, NULL, NULL, NULL, NULL, &valueCount, NULL, NULL, NULL, NULL) == ERROR_SUCCESS)
//        {
//            for (DWORD i = 0; i < valueCount; i++)
//            {
//                DWORD valueNameLength = 16383; // Максимальная длина имени значения
//                std::vector<WCHAR> valueNameBuffer(valueNameLength);
//
//                DWORD valueType = 0;
//                DWORD valueDataSize = 0;
//
//                if (RegEnumValue(hSubKey, i, valueNameBuffer.data(), &valueNameLength, NULL, &valueType, NULL, &valueDataSize) == ERROR_SUCCESS)
//                {
//                    if (valueType == REG_SZ || valueType == REG_EXPAND_SZ || valueType == REG_MULTI_SZ)
//                    {
//                        std::vector<WCHAR> valueDataBuffer(valueDataSize / sizeof(WCHAR));
//
//                        if (RegEnumValue(hSubKey, i, valueNameBuffer.data(), &valueNameLength, NULL, &valueType, reinterpret_cast<LPBYTE>(valueDataBuffer.data()), &valueDataSize) == ERROR_SUCCESS)
//                        {
//                            RegistryValue value;
//                            value.name = valueNameBuffer.data();
//                            value.type = valueType;
//                            value.data = valueDataBuffer.data();
//                            values.push_back(value);
//                        }
//                    }
//                    else if (valueType == REG_DWORD || valueType == REG_QWORD)
//                    {
//                        DWORD valueData = 0;
//                        DWORD dataSize = sizeof(valueData);
//
//                        if (RegEnumValue(hSubKey, i, valueNameBuffer.data(), &valueNameLength, NULL, &valueType, reinterpret_cast<LPBYTE>(&valueData), &dataSize) == ERROR_SUCCESS)
//                        {
//                            RegistryValue value;
//                            value.name = valueNameBuffer.data();
//                            value.type = valueType;
//                            value.data = std::to_wstring(valueData);
//                            values.push_back(value);
//                        }
//                    }
//                }
//            }
//        }
//
//        RegCloseKey(hSubKey);
//    }
//
//    return values;
//}

//std::vector<RegistryValue> GetRegistryValues(HKEY hKey, LPCWSTR subKey)
//{
//    std::vector<RegistryValue> values;
//
//    HKEY hSubKey = NULL;
//    if (RegOpenKeyEx(hKey, subKey, 0, KEY_READ, &hSubKey) == ERROR_SUCCESS)
//    {
//        DWORD valueCount = 0;
//        if (RegQueryInfoKey(hSubKey, NULL, NULL, NULL, NULL, NULL, NULL, &valueCount, NULL, NULL, NULL, NULL) == ERROR_SUCCESS)
//        {
//            for (DWORD i = 0; i < valueCount; i++)
//            {
//                DWORD valueNameLength = 16383; // Максимальная длина имени значения
//                std::vector<WCHAR> valueNameBuffer(valueNameLength);
//
//                DWORD valueType = 0;
//                DWORD valueDataSize = 0;
//
//                if (RegEnumValue(hSubKey, i, valueNameBuffer.data(), &valueNameLength, NULL, &valueType, NULL, &valueDataSize) == ERROR_SUCCESS)
//                {
//                    if (valueType == REG_SZ || valueType == REG_EXPAND_SZ || valueType == REG_MULTI_SZ)
//                    {
//                        std::vector<WCHAR> valueDataBuffer(valueDataSize / sizeof(WCHAR));
//
//                        if (RegEnumValue(hSubKey, i, valueNameBuffer.data(), &valueNameLength, NULL, &valueType, reinterpret_cast<LPBYTE>(valueDataBuffer.data()), &valueDataSize) == ERROR_SUCCESS)
//                        {
//                            RegistryValue value;
//                            value.name = valueNameBuffer.data();
//                            value.type = valueType;
//                            value.data = valueDataBuffer.data();
//                            values.push_back(value);
//                        }
//                    }
//                    else if (valueType == REG_DWORD || valueType == REG_QWORD)
//                    {
//                        DWORD valueData = 0;
//                        DWORD dataSize = sizeof(valueData);
//
//                        if (RegEnumValue(hSubKey, i, valueNameBuffer.data(), &valueNameLength, NULL, &valueType, reinterpret_cast<LPBYTE>(&valueData), &dataSize) == ERROR_SUCCESS)
//                        {
//                            RegistryValue value;
//                            value.name = valueNameBuffer.data();
//                            value.type = valueType;
//                            value.data = std::to_wstring(valueData);
//                            values.push_back(value);
//                        }
//                    }
//                }
//            }
//        }
//
//        RegCloseKey(hSubKey);
//    }
//
//    return values;
//}

//void LoadRegistryInfoToListBox(HWND hListBox, LPCWSTR subKey)
//{
//    HKEY hRootKey = HKEY_CURRENT_USER;
//    HKEY hKey;
//    if (RegOpenKeyEx(hRootKey, subKey, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
//    {
//        // Переменные для хранения информации о реестровых элементах
//        wchar_t valueName[256];
//        DWORD valueNameSize = 256;
//        DWORD valueType;
//        BYTE valueData[1024];
//        DWORD valueDataSize = 1024;
//
//        // Переменные для хранения индекса и текстового представления
//        int index = 0;
//        wchar_t itemText[512];
//
//        // Загрузка информации о реестровых элементах и добавление их в listbox
//        while (RegEnumValue(hKey, index, valueName, &valueNameSize, NULL, &valueType, valueData, &valueDataSize) == ERROR_SUCCESS)
//        {
//            // Создание текстового представления информации о реестровом элементе
//            swprintf_s(itemText, L"%s (Type: %d)", valueName, valueType);
//
//            // Добавление элемента в listbox
//            SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)itemText);
//
//            // Следующий элемент
//            index++;
//            valueNameSize = 256;
//            valueDataSize = 1024;
//        }
//
//        RegCloseKey(hKey);
//    }
//}
//
//void PopulateValueList(HKEY hKey, LPCWSTR subKey)
//{
//    std::vector<RegistryValue> values = GetRegistryValues(hKey, subKey);
//
//    // Очистка `listbox`
//    SendMessage(hValueList, LB_RESETCONTENT, 0, 0);
//
//    // Добавление значений в список
//    for (const auto& value : values)
//    {
//        std::wstring itemText = value.name + L" | " + std::to_wstring(value.type) + L" | " + value.data;
//        SendMessage(hValueList, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(itemText.c_str()));
//    }
//}
