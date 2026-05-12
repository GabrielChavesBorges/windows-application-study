#ifndef WIN32_WINDOW_H
#define WIN32_WINDOW_H

#include <windows.h>

/* Virtual framebuffer (Game Boy resolution). */
#define VIRTUAL_FRAME_WIDTH 160
#define VIRTUAL_FRAME_HEIGHT 144

LRESULT CALLBACK window_procedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);


#endif // WIN32_WINDOW_H