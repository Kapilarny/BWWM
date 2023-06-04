#include "webview.h"

#include <unordered_map>
#include <windows.h>
#include <stdlib.h>
#include <string>
#include <tchar.h>
#include <wrl.h>
#include <wil/com.h>

using namespace Microsoft::WRL;

static std::unordered_map<HWND, WebViewWindow*> hwndToWebViewWindow;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    printf("Getting window with HWND of %i\n", hwnd);
    WebViewWindow* window = hwndToWebViewWindow[hwnd];

    if(window == nullptr) return DefWindowProc(hwnd, uMsg, wParam, lParam);

    switch (uMsg) {
        case WM_SIZE:
            if (window->webView != nullptr && window->webViewController != nullptr) {
                RECT bounds;
                GetClientRect(hwnd, &bounds);
                window->webViewController->put_Bounds(bounds);
            }
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

WebViewWindow::WebViewWindow(int width, int height) {
    this->width = width;
    this->height = height;
}

// THIS FUNCTION SHOULD BE RUN ON A SEPARATE THREAD
void WebViewWindow::Run() {
    // Register the window class.
    const wchar_t CLASS_NAME[]  = L"Sample Window Class";
    WNDCLASSEX wcex;
    HINSTANCE hInstance = GetModuleHandle(NULL);

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WindowProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = (LPCSTR) CLASS_NAME;
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

	if (!RegisterClassEx(&wcex))
	{
		MessageBox(NULL,
			_T("Call to RegisterClassEx failed!"),
			_T("Windows Desktop Guided Tour"),
			NULL);

		return;
	}


    // Create the window.
    hwnd = CreateWindow(
		(LPCSTR) CLASS_NAME,
		"WebView2 Sample",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		1200, 900,
		NULL,
		NULL,
		hInstance,
		NULL
	);

    if(hwnd == NULL) {
        MessageBoxW(NULL, L"Window creation failed", L"Error", MB_OK);
        return;
    }

    printf("Saving window with HWND of %i\n", hwnd);
    hwndToWebViewWindow[hwnd] = this;

    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);

    InitializeWebView();

    // Run the message loop.
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    hwndToWebViewWindow.erase(hwnd);
}

void WebViewWindow::InitializeWebView() {
    HRESULT hr = CreateCoreWebView2EnvironmentWithOptions(nullptr, nullptr, nullptr,
        Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
            [this](HRESULT result, ICoreWebView2Environment* environment) -> HRESULT {
                webViewEnvironment = environment;

                HRESULT hr = webViewEnvironment->CreateCoreWebView2Controller(hwnd, Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
                    [this](HRESULT result, ICoreWebView2Controller* controller) -> HRESULT {
                        if(webViewController == nullptr) {
                            webViewController = controller;
                            webViewController->get_CoreWebView2(&webView);
                        }

                        wil::com_ptr<ICoreWebView2Settings> settings;
                        webView->get_Settings(&settings);
                        settings->put_IsScriptEnabled(TRUE);
                        settings->put_AreDefaultScriptDialogsEnabled(TRUE);
                        settings->put_IsWebMessageEnabled(TRUE);
                        
                        // Resize WebView to fit the bounds of the parent window
                        RECT bounds;
						GetClientRect(hwnd, &bounds);
						webViewController->put_Bounds(bounds);

                        // <NavigationEvents>
						// Step 4 - Navigation events
						// register an ICoreWebView2NavigationStartingEventHandler to cancel any non-https navigation
						EventRegistrationToken token;
						webView->add_NavigationStarting(Callback<ICoreWebView2NavigationStartingEventHandler>(
							[](ICoreWebView2* webview, ICoreWebView2NavigationStartingEventArgs* args) -> HRESULT {
								wil::unique_cotaskmem_string uri;
								args->get_Uri(&uri);
								printf("Starting navigation to %ls\n", uri.get());
								return S_OK;
							}).Get(), &token);

                        webView->add_NavigationCompleted(Callback<ICoreWebView2NavigationCompletedEventHandler>(
                            [](ICoreWebView2* webview, ICoreWebView2NavigationCompletedEventArgs* args) -> HRESULT {
                                BOOL isSuccess = FALSE;
                                args->get_IsSuccess(&isSuccess);
                                printf("Finished navigation with %d\n", isSuccess);

                                return S_OK;  
                            }).Get(), &token);
						// </NavigationEvents>

                        webView->NavigateToString(L"<html><body><h1>Hello, world!</h1></body></html>");

                        // <Scripting>
						// Step 5 - Scripting
						// Schedule an async task to add initialization script that freezes the Object object
						// webView->AddScriptToExecuteOnDocumentCreated(L"Object.freeze(Object);", nullptr);
						// Schedule an async task to get the document URL
						// webView->ExecuteScript(L"window.document.URL;", Callback<ICoreWebView2ExecuteScriptCompletedHandler>(
						// 	[](HRESULT errorCode, LPCWSTR resultObjectAsJson) -> HRESULT {
						// 		LPCWSTR URL = resultObjectAsJson;
						// 		//doSomethingWithURL(URL);
						// 		return S_OK;
						// 	}).Get());
						// // </Scripting>

                        // <CommunicationHostWeb>
						// Step 6 - Communication between host and web content
						// Set an event handler for the host to return received message back to the web content
						// webView->add_WebMessageReceived(Callback<ICoreWebView2WebMessageReceivedEventHandler>(
							// [](ICoreWebView2* webview, ICoreWebView2WebMessageReceivedEventArgs* args) -> HRESULT {
							// 	wil::unique_cotaskmem_string message;
							// 	args->TryGetWebMessageAsString(&message);
							// 	// processMessage(&message);
							// 	webview->PostWebMessageAsString(message.get());
							// 	return S_OK;
							// }).Get(), &token);
                        
                        // Schedule an async task to add initialization script that
						// 1) Add an listener to print message from the host
						// 2) Post document URL to the host
						// webView->AddScriptToExecuteOnDocumentCreated(
						// 	L"window.chrome.webview.addEventListener(\'message\', event => alert(event.data));" \
						// 	L"window.chrome.webview.postMessage(window.document.URL);",
						// 	nullptr);
						// // </CommunicationHostWeb>

                        return S_OK;
                    }
                ).Get());

                
                return S_OK;
            }
        ).Get());

    if (FAILED(hr)) {
        MessageBoxW(nullptr, L"Failed to create webview environment", L"Error", MB_OK);
        return;
    }

}