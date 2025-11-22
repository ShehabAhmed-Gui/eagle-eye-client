#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>

#include <iostream>
#include <string>

const std::wstring WINDOW_TITLE = L"TestChamber";

LRESULT CALLBACK window_message_process(HWND window,
                                        UINT msg,
                                        WPARAM wparam,
                                        LPARAM lparam
                                        );

int WINAPI wWinMain(HINSTANCE hInstance,
                    HINSTANCE hPrevInstance,
                    PWSTR pCmdLine, int nCmdShow)
{
    AllocConsole();
    freopen("CONIN$", "r", stdin);
    freopen("CONOUT$", "w", stderr);
    freopen("CONOUT$", "w", stdout);

    const std::wstring WINDOW_CLASS_NAME = L"TestChamberWC";

    WNDCLASS window_class = {};
    
    window_class.lpfnWndProc = window_message_process;
    window_class.hInstance = hInstance;
    window_class.lpszClassName = WINDOW_CLASS_NAME.data();
    RegisterClass(&window_class);

    HWND window = CreateWindowEx(
        0,
        WINDOW_CLASS_NAME.data(),
        WINDOW_TITLE.data(),
        WS_OVERLAPPEDWINDOW,

        CW_USEDEFAULT, CW_USEDEFAULT,
        CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (window == nullptr)
    {
        printf("ERROR: window creation failed\n");
        return 1;
    }

    ShowWindow(window, nCmdShow);

    //Loop
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}


LRESULT CALLBACK window_message_process(HWND window,
                                        UINT msg,
                                        WPARAM wparam,
                                        LPARAM lparam
                                        )
{
    switch (msg)
    {
        case WM_DESTROY:
        case WM_QUIT:
        {
            PostQuitMessage(0);
            return 0;
        } break;

        case WM_PAINT:
        {
            PAINTSTRUCT paint;
            HDC dc = BeginPaint(window, &paint);

            FillRect(dc, &paint.rcPaint,
                     (HBRUSH)(COLOR_WINDOW+1));

            EndPaint(window, &paint);

            return 0;
        } break;
    }

    return DefWindowProc(window, msg, wparam, lparam);
}