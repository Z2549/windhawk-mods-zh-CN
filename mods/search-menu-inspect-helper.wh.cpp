// ==WindhawkMod==
// @id              search-menu-inspect-helper
// @name            Search Menu Inspect Helper
// @name:zh-CN      搜索菜单检查助手
// @description     Helps inspect the search menu web view content via DevTools remote debugging, refer to the description for details
// @description:zh-CN 通过 DevTools 远程调试来检查搜索菜单的 WebView 内容，详情请参阅模组描述
// @version         1.0
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         SearchHost.exe
// @include         explorer.exe
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# 搜索菜单检查辅助

通过 DevTools 远程调试帮助检查搜索菜单的 web 视图内容。

此 mod 可防止搜索菜单在失去焦点时被关闭，从而允许在 DevTools 窗口位于前台时对
其进行检查。将焦点移到搜索栏并按 Escape 即可关闭搜索菜单。

## 操作步骤

按以下步骤配置远程调试：[Remote debugging desktop
WebView2 WinUI 2 (UWP)
apps](https://learn.microsoft.com/en-us/microsoft-edge/webview2/how-to/remote-debugging-desktop)。
跳过第 11 步（Configure your WebView2 WinUI 2 (UWP) app for remote
debugging），改为安装此 mod，然后终止 SearchHost.exe 进程。它会在启用远程调试的
情况下自动重新启动。
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

#include <atomic>
#include <string>
#include <string_view>

#include <shlwapi.h>

enum class Target {
    SearchHost,
    Explorer,
};

Target g_target;

std::atomic<bool> g_searchUiLoaded;

using UndockedSearchAppExperienceManager_OnSearchAppHideRequestedTimer_t =
    void(WINAPI*)(void* pThis);
UndockedSearchAppExperienceManager_OnSearchAppHideRequestedTimer_t
    UndockedSearchAppExperienceManager_OnSearchAppHideRequestedTimer_Original;
void WINAPI
UndockedSearchAppExperienceManager_OnSearchAppHideRequestedTimer_Hook(
    void* pThis) {
    Wh_Log(L">");

    return;
}

bool HookTwinuiPcshellSymbols() {
    HMODULE module = LoadLibraryEx(L"twinui.pcshell.dll", nullptr,
                                   LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"Loading twinui.pcshell.dll failed");
        return false;
    }

    // twinui.pcshell.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(private: void __cdecl UndockedSearchAppExperienceManager::OnSearchAppHideRequestedTimer(void))"},
            &UndockedSearchAppExperienceManager_OnSearchAppHideRequestedTimer_Original,
            UndockedSearchAppExperienceManager_OnSearchAppHideRequestedTimer_Hook,
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

using CreateProcessInternalW_t =
    BOOL(WINAPI*)(HANDLE hUserToken,
                  LPCWSTR lpApplicationName,
                  LPWSTR lpCommandLine,
                  LPSECURITY_ATTRIBUTES lpProcessAttributes,
                  LPSECURITY_ATTRIBUTES lpThreadAttributes,
                  BOOL bInheritHandles,
                  DWORD dwCreationFlags,
                  LPVOID lpEnvironment,
                  LPCWSTR lpCurrentDirectory,
                  LPSTARTUPINFOW lpStartupInfo,
                  LPPROCESS_INFORMATION lpProcessInformation,
                  PHANDLE hRestrictedUserToken);
CreateProcessInternalW_t CreateProcessInternalW_Original;
BOOL WINAPI
CreateProcessInternalW_Hook(HANDLE hUserToken,
                            LPCWSTR lpApplicationName,
                            LPWSTR lpCommandLine,
                            LPSECURITY_ATTRIBUTES lpProcessAttributes,
                            LPSECURITY_ATTRIBUTES lpThreadAttributes,
                            BOOL bInheritHandles,
                            DWORD dwCreationFlags,
                            LPVOID lpEnvironment,
                            LPCWSTR lpCurrentDirectory,
                            LPSTARTUPINFOW lpStartupInfo,
                            LPPROCESS_INFORMATION lpProcessInformation,
                            PHANDLE hRestrictedUserToken) {
    Wh_Log(L">");
    Wh_Log(L">> %s", lpApplicationName);
    Wh_Log(L">> %s", lpCommandLine);

    std::wstring newCmdLine;
    if (lpCommandLine &&
        StrStrI(lpCommandLine, L" --webview-exe-name=SearchHost.exe")) {
        newCmdLine = lpCommandLine;

        constexpr std::wstring_view prefix = L" --enable-features=";
        auto it = newCmdLine.find(prefix);
        if (it != newCmdLine.npos) {
            newCmdLine.insert(it + prefix.length(),
                              L"msEdgeDevToolsWdpRemoteDebugging,");
        } else {
            newCmdLine +=
                L" --enable-features=msEdgeDevToolsWdpRemoteDebugging";
        }

        Wh_Log(L">> New cmdline: %s", newCmdLine.c_str());

        lpCommandLine = newCmdLine.data();
    }

    return CreateProcessInternalW_Original(
        hUserToken, lpApplicationName, lpCommandLine, lpProcessAttributes,
        lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment,
        lpCurrentDirectory, lpStartupInfo, lpProcessInformation,
        hRestrictedUserToken);
}

using TaskbarSearchAppStateManager_OnHideViewRequested_t =
    void(WINAPI*)(void* pThis, void* param1, void* param2);
TaskbarSearchAppStateManager_OnHideViewRequested_t
    TaskbarSearchAppStateManager_OnHideViewRequested_Original;
void WINAPI
TaskbarSearchAppStateManager_OnHideViewRequested_Hook(void* pThis,
                                                      void* param1,
                                                      void* param2) {
    Wh_Log(L">");

    return;
}

bool HookSearchUiSymbols(HMODULE module) {
    // SearchUx.UI.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(private: void __cdecl Cortana::UI::TaskbarSearchAppStateManager::OnHideViewRequested(class Platform::Object ^,class WindowsUdk::UI::Shell::HideViewRequestedEventArgs ^))"},
            &TaskbarSearchAppStateManager_OnHideViewRequested_Original,
            TaskbarSearchAppStateManager_OnHideViewRequested_Hook,
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

HMODULE GetSearchUiModuleHandle() {
    return GetModuleHandle(L"SearchUx.UI.dll");
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (!module) {
        return module;
    }

    if (!g_searchUiLoaded && GetSearchUiModuleHandle() == module &&
        !g_searchUiLoaded.exchange(true)) {
        Wh_Log(L"Loaded %s", lpLibFileName);

        if (HookSearchUiSymbols(module)) {
            Wh_ApplyHookOperations();
        }
    }

    return module;
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    g_target = Target::SearchHost;

    WCHAR moduleFilePath[MAX_PATH];
    switch (
        GetModuleFileName(nullptr, moduleFilePath, ARRAYSIZE(moduleFilePath))) {
        case 0:
        case ARRAYSIZE(moduleFilePath):
            Wh_Log(L"GetModuleFileName failed");
            return FALSE;

        default:
            if (PCWSTR moduleFileName = wcsrchr(moduleFilePath, L'\\')) {
                moduleFileName++;
                if (_wcsicmp(moduleFileName, L"explorer.exe") == 0) {
                    g_target = Target::Explorer;
                }
            } else {
                Wh_Log(L"GetModuleFileName returned an unsupported path");
                return FALSE;
            }
            break;
    }

    if (g_target == Target::SearchHost) {
        if (HMODULE searchUiModule = GetSearchUiModuleHandle()) {
            g_searchUiLoaded = true;
            if (!HookSearchUiSymbols(searchUiModule)) {
                return FALSE;
            }
        } else {
            Wh_Log(L"Search UI module not loaded yet");

            HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
            auto pKernelBaseLoadLibraryExW =
                (decltype(&LoadLibraryExW))GetProcAddress(kernelBaseModule,
                                                          "LoadLibraryExW");
            WindhawkUtils::Wh_SetFunctionHookT(pKernelBaseLoadLibraryExW,
                                               LoadLibraryExW_Hook,
                                               &LoadLibraryExW_Original);
        }

        auto pCreateProcessInternalW = (CreateProcessInternalW_t)GetProcAddress(
            GetModuleHandle(L"kernelbase.dll"), "CreateProcessInternalW");

        WindhawkUtils::Wh_SetFunctionHookT(pCreateProcessInternalW,
                                           CreateProcessInternalW_Hook,
                                           &CreateProcessInternalW_Original);
    } else {
        if (!HookTwinuiPcshellSymbols()) {
            return FALSE;
        }
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    if (g_target == Target::SearchHost && !g_searchUiLoaded) {
        if (HMODULE searchUiModule = GetSearchUiModuleHandle()) {
            if (!g_searchUiLoaded.exchange(true)) {
                Wh_Log(L"Got Taskbar.View.dll");

                if (HookSearchUiSymbols(searchUiModule)) {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");
}
