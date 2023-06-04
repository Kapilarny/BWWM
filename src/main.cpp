#include <stdio.h>

#include "widget\webview.h"

// Make a WinMain function
int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow
) {
    printf("Hello, world!\n");

	WebViewWindow* window = new WebViewWindow(800, 600);
	window->Run();

    return 0;   
}