#include <windows.h>

/* Virtual framebuffer (Game Boy resolution). */
#define VIRTUAL_FRAME_WIDTH 160
#define VIRTUAL_FRAME_HEIGHT 144

static HBITMAP g_hFramebuffer = NULL;

static int InitFramebuffer(void) {
    BITMAPINFO bi = {0};
    void *bits = NULL;
    HDC hdcScreen;
    DWORD *p;
    int x;
    int y;

    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = VIRTUAL_FRAME_WIDTH;
    bi.bmiHeader.biHeight = -VIRTUAL_FRAME_HEIGHT; /* top-down */
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    hdcScreen = GetDC(NULL);
    g_hFramebuffer = CreateDIBSection(hdcScreen, &bi, DIB_RGB_COLORS, &bits, NULL, 0);
    ReleaseDC(NULL, hdcScreen);

    if (!g_hFramebuffer || !bits) {
        return 0;
    }

    p = (DWORD *)bits;
    for (y = 0; y < VIRTUAL_FRAME_HEIGHT; y++) {
        for (x = 0; x < VIRTUAL_FRAME_WIDTH; x++) {
            p[y * VIRTUAL_FRAME_WIDTH + x] = (((x + y) & 1) ? 0x00999999UL : 0x00444444UL);
        }
    }

    return 1;
}

LRESULT CALLBACK window_procedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE:
            if (!InitFramebuffer()) {
                return -1;
            }
            return 0;

        case WM_ERASEBKGND:
            return 1;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT rc;
            int cw;
            int ch;
            HDC hdcMem;
            HBITMAP oldBmp;

            GetClientRect(hwnd, &rc);
            cw = rc.right - rc.left;
            ch = rc.bottom - rc.top;

            hdcMem = CreateCompatibleDC(hdc);
            oldBmp = (HBITMAP)SelectObject(hdcMem, g_hFramebuffer);

            SetStretchBltMode(hdc, COLORONCOLOR);
            StretchBlt(hdc, 0, 0, cw, ch, hdcMem, 0, 0, VIRTUAL_FRAME_WIDTH, VIRTUAL_FRAME_HEIGHT, SRCCOPY);

            SelectObject(hdcMem, oldBmp);
            DeleteDC(hdcMem);

            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_DESTROY:
            if (g_hFramebuffer) {
                DeleteObject(g_hFramebuffer);
                g_hFramebuffer = NULL;
            }
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE instance_handle, HINSTANCE previous_instance_handle, LPSTR lp_cmd_line, int n_cmd_show) {
    // Discarding unused arguments:
    (void) previous_instance_handle;
    (void) lp_cmd_line;

    const char CLASS_NAME[] = "MyWindowClass";
    RECT client_area;
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
    client_area.left = 0;
    client_area.top = 0;
    client_area.right = VIRTUAL_FRAME_WIDTH * window_scale;
    client_area.bottom = VIRTUAL_FRAME_HEIGHT * window_scale;

    AdjustWindowRect(&client_area, window_style, FALSE);
    window_width = client_area.right - client_area.left;
    window_height = client_area.bottom - client_area.top;

    if (SystemParametersInfo(SPI_GETWORKAREA, 0, &work_area, 0)) {
        int workW = work_area.right - work_area.left;
        int workH = work_area.bottom - work_area.top;
        window_start_x = work_area.left + ((workW - window_width) / 2);
        window_start_y = work_area.top + ((workH - window_height) / 2);
    }

    HWND hwnd = CreateWindowEx(
        0, CLASS_NAME, "Test App",
        window_style,
        window_start_x, window_start_y,
        window_width, window_height,
        NULL, NULL, instance_handle, NULL
    );

    if (!hwnd) {
        return 0;
    }

    ShowWindow(hwnd, n_cmd_show);

    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
