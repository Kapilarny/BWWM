#include "webview.h"

WebViewWindow::WebViewWindow(int width, int height) {
    this->width = width;
    this->height = height;
}

void WebViewWindow::Run() {
    WebViewHandler* handler = new WebViewHandler(hwnd, webView);
    HRESULT hr = CreateCoreWebView2EnvironmentWithOptions(nullptr, nullptr, nullptr, handler);

    if (FAILED(hr)) {
        MessageBoxW(nullptr, L"Failed to create webview environment", L"Error", MB_OK);
        return;
    }

    webViewEnvironment = handler->GetWebViewEnvironment();
}

HRESULT STDMETHODCALLTYPE WebViewHandler::Invoke(HRESULT result, ICoreWebView2Environment* environment) {
    HRESULT hr = environment->CreateCoreWebView2Controller(hwnd, new WebViewControllerHandler(webView));

    return S_OK;
}

HRESULT STDMETHODCALLTYPE WebViewControllerHandler::Invoke(HRESULT result, ICoreWebView2Controller* controller) {
    controller->get_CoreWebView2(&webView);

    // TODO: Add event handlers
    
    webView->Navigate(L"https://www.google.com");
    
    return S_OK;
}

// BS from here on out

HRESULT STDMETHODCALLTYPE WebViewHandler::QueryInterface(REFIID riid, void** ppvObject) {
    if (riid == __uuidof(IUnknown) || riid == __uuidof(ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler))
    {
        *ppvObject = static_cast<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler*>(this);
        AddRef();
        return S_OK;
    }
    *ppvObject = nullptr;
    return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE WebViewHandler::AddRef(void) {
    return InterlockedIncrement(&refCount);
}

ULONG STDMETHODCALLTYPE WebViewHandler::Release(void) {
    ULONG newCount = InterlockedDecrement(&refCount);
    if (newCount == 0)
    {
        delete this;
    }
    return newCount;
}

HRESULT STDMETHODCALLTYPE WebViewControllerHandler::QueryInterface(REFIID riid, void** ppvObject) {
    if (riid == __uuidof(IUnknown) || riid == __uuidof(ICoreWebView2CreateCoreWebView2ControllerCompletedHandler))
    {
        *ppvObject = static_cast<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler*>(this);
        AddRef();
        return S_OK;
    }
    *ppvObject = nullptr;
    return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE WebViewControllerHandler::AddRef(void) {
    return InterlockedIncrement(&refCount);
}

ULONG STDMETHODCALLTYPE WebViewControllerHandler::Release(void) {
    ULONG newCount = InterlockedDecrement(&refCount);
    if (newCount == 0)
    {
        delete this;
    }
    return newCount;
}