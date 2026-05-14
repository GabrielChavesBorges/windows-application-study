#include "win32_window.h"

static int SetFramebuffer(int type);
static HBITMAP g_hFramebuffer = NULL;
static int screen_type = 0;

LRESULT CALLBACK window_procedure(HWND window_handle, UINT message, WPARAM w_parameter, LPARAM l_parameter) {
    switch (message) {
        case WM_CREATE:
            if (!SetFramebuffer(screen_type)) {
                return -1;
            }
            SetTimer(window_handle, 1, 1000, NULL);
            return 0;

        case WM_ERASEBKGND:
            return 1;

        case WM_TIMER:
            screen_type = !screen_type;
            SetFramebuffer(screen_type);
            InvalidateRect(window_handle, NULL, FALSE);
            return 0;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(window_handle, &ps);
            RECT rc;
            int cw;
            int ch;
            HDC hdcMem;
            HBITMAP oldBmp;

            GetClientRect(window_handle, &rc);
            cw = rc.right - rc.left;
            ch = rc.bottom - rc.top;

            hdcMem = CreateCompatibleDC(hdc);
            oldBmp = (HBITMAP)SelectObject(hdcMem, g_hFramebuffer);

            SetStretchBltMode(hdc, COLORONCOLOR);
            StretchBlt(hdc, 0, 0, cw, ch, hdcMem, 0, 0, VIRTUAL_FRAME_WIDTH, VIRTUAL_FRAME_HEIGHT, SRCCOPY);

            SelectObject(hdcMem, oldBmp);
            DeleteDC(hdcMem);

            EndPaint(window_handle, &ps);
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
    return DefWindowProc(window_handle, message, w_parameter, l_parameter);
}

static int SetFramebuffer(int type) {
    if (type < 0 || type > 1) {
        return 0;
    }

    BITMAPINFO bitmap_info = {0};
    void* bits = NULL;
    HDC screen_context;
    DWORD* cursor;
    int x;
    int y;

    bitmap_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitmap_info.bmiHeader.biWidth = VIRTUAL_FRAME_WIDTH;
    bitmap_info.bmiHeader.biHeight = -VIRTUAL_FRAME_HEIGHT;
    bitmap_info.bmiHeader.biPlanes = 1;
    bitmap_info.bmiHeader.biBitCount = 32;
    bitmap_info.bmiHeader.biCompression = BI_RGB;

    screen_context = GetDC(NULL);
    g_hFramebuffer = CreateDIBSection(screen_context, &bitmap_info, DIB_RGB_COLORS, &bits, NULL, 0);
    ReleaseDC(NULL, screen_context);

    if (!g_hFramebuffer || !bits) {
        return 0;
    }

    cursor = (DWORD*) bits;
    for (y = 0; y < VIRTUAL_FRAME_HEIGHT; y++) {
        for (x = 0; x < VIRTUAL_FRAME_WIDTH; x++) {
            cursor[y * VIRTUAL_FRAME_WIDTH + x] = (((x + y + type) & 1) ? 0x00999999UL : 0x00444444UL);
        }
    }

    return 1;
}