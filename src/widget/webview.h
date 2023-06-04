#pragma once

#include <Windows.h>
#include <WebView2.h>

class WebViewWindow {
public:
    WebViewWindow(int width, int height);

    void Run();
private:
    // Properties
    int width, height;

    // Window API stuff
    HWND hwnd;
    ICoreWebView2* webView;
    ICoreWebView2Environment* webViewEnvironment;
};

class WebViewHandler : public ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler {
public:
    WebViewHandler(HWND hwnd, ICoreWebView2* webView) : hwnd(hwnd), webView(webView) {}

    ICoreWebView2Environment* GetWebViewEnvironment() { return webViewEnvironment; }

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) override;
    ULONG STDMETHODCALLTYPE AddRef(void) override;
    ULONG STDMETHODCALLTYPE Release(void) override;
    HRESULT STDMETHODCALLTYPE Invoke(HRESULT result, ICoreWebView2Environment* environment) override;
private:
    HWND hwnd;
    WebViewWindow* window;
    ICoreWebView2Environment* webViewEnvironment;
    ICoreWebView2* webView;
    ULONG refCount = 1;
};

class WebViewControllerHandler : public ICoreWebView2CreateCoreWebView2ControllerCompletedHandler {
public:
    WebViewControllerHandler(ICoreWebView2* webView) : webView(webView) {}

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) override;
    ULONG STDMETHODCALLTYPE AddRef(void) override;
    ULONG STDMETHODCALLTYPE Release(void) override;
    HRESULT STDMETHODCALLTYPE Invoke(HRESULT result, ICoreWebView2Controller* controller) override;
private:
    ULONG refCount = 1;
    ICoreWebView2* webView;
};