#include "win32_window.h"

int WINAPI WinMain(HINSTANCE instance_handle, HINSTANCE previous_instance_handle, LPSTR lp_cmd_line, int n_cmd_show) {
    // Discard unused arguments:
    (void) previous_instance_handle;
    (void) lp_cmd_line;

    const char CLASS_NAME[] = "MyWindowClass";
    RECT client_rectangle;
    RECT work_area;
    int window_scale;
    int window_width;
    int window_height;
    int window_start_x = CW_USEDEFAULT;
    int window_start_y = CW_USEDEFAULT;
    DWORD window_style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

    WNDCLASS window_class = {0};
    window_class.lpfnWndProc = window_procedure;
    window_class.hInstance = instance_handle;
    window_class.lpszClassName = CLASS_NAME;
    window_class.hbrBackground = NULL;

    RegisterClass(&window_class);

    window_scale = 6;
    client_rectangle.left = 0;
    client_rectangle.top = 0;
    client_rectangle.right = VIRTUAL_FRAME_WIDTH * window_scale;
    client_rectangle.bottom = VIRTUAL_FRAME_HEIGHT * window_scale;

    RECT window_rectangle = client_rectangle;
    AdjustWindowRect(&window_rectangle, window_style, FALSE);
    window_width = window_rectangle.right - window_rectangle.left;
    window_height = window_rectangle.bottom - window_rectangle.top;

    if (SystemParametersInfo(SPI_GETWORKAREA, 0, &work_area, 0)) {
        int workW = work_area.right - work_area.left;
        int workH = work_area.bottom - work_area.top;
        window_start_x = work_area.left + ((workW - window_width) / 2);
        window_start_y = work_area.top + ((workH - window_height) / 2);
    }

    HWND window_handle = CreateWindowEx(
        0, CLASS_NAME, "Test App",
        window_style,
        window_start_x, window_start_y,
        window_width, window_height,
        NULL, NULL, instance_handle, NULL
    );

    if (!window_handle) {
        return 0;
    }

    ShowWindow(window_handle, n_cmd_show);

    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
