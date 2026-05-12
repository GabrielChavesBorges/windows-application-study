#include "win32_window.h"

static int InitFramebuffer(void);
static HBITMAP g_hFramebuffer = NULL;

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