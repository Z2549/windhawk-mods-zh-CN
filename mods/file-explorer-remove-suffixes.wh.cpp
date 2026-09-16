// ==WindhawkMod==
// @id              file-explorer-remove-suffixes
// @name            Remove Taskbar Window Suffixes
// @name:zh-CN      移除任务栏窗口标题后缀
// @description     Remove suffixes from taskbar window titles for File Explorer and other programs, or configure custom text replacement rules
// @description:zh-CN 移除文件资源管理器等程序任务栏标题中多余的后缀，也可自定义文本替换规则
// @version         1.1.1
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @compilerOptions -lole32 -loleaut32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# 移除任务栏窗口标题后缀

Windows 会在任务栏的窗口标题后附加多余的后缀，例如「 - File Explorer」或
「 - Notepad」。此 mod 会移除这些后缀，让任务栏保持整洁易读。

## 后缀移除模式

选择移除后缀的方式：

- **仅文件资源管理器**（默认）：从文件资源管理器窗口移除「 - File Explorer」
  后缀。
- **通用**：自动从任意窗口标题中移除「 - 」「 — 」（带空格的破折号）或「—」
  （单独破折号）之后的部分。示例：
    - 「Document - Notepad」变为「Document」
    - 「Downloads - File Explorer」变为「Downloads」
    - 「Windhawk — Firefox」变为「Windhawk」
- **关闭**：禁用自动后缀移除。

## 修改前后

![修改前](https://i.imgur.com/ErUN0YU.png) \
_修改前：每个文件夹都显示「 - File Explorer」后缀_

![修改后](https://i.imgur.com/tblTr3Q.png) \
_修改后：干净的文件夹名称，没有多余后缀_

## 自定义正则规则

此外，你可以为特定程序或所有程序定义基于正则的查找替换规则。每条规则需指定：

- **进程标识**：按进程名（如 `notepad.exe`）、完整路径（如
  `C:\Windows\notepad.exe`）或 UWP 应用的 App ID（如
  `Microsoft.WindowsCalculator_8wekyb3d8bbwe!App`）匹配。留空表示匹配所有进程。
- **查找模式**：在窗口标题中查找的正则表达式。支持标准正则语法，包括锚点、分组
  与量词。
- **替换为**：替换文本。可使用正则捕获组（$1、$2 等）引用匹配部分。留空表示删除
  匹配到的文本。

多条规则可以匹配同一窗口，所有匹配的模式会按定义顺序依次应用。

### 示例规则

**移除记事本的「 - Notepad」后缀：**
- 进程：`notepad.exe`
- 查找：` - Notepad$`
- 替换：（留空）

**重排 Chrome 标题各部分：**
- 进程：`chrome.exe`
- 查找：`^(.*) - Google Chrome$`
- 替换：`Chrome: $1`

**从任意窗口移除构建配置：**
- 进程：（留空，匹配所有）
- 查找：` \(Debug|Release\)$`
- 替换：（留空）

## 说明

- 更改只影响标题在任务栏上的显示方式，不影响实际的窗口标题。
- 通用模式会在自定义正则规则之前处理标题，因此可在移除后缀后再施加其他修改。
- 进程匹配不区分大小写，以获得更好的兼容性。
- 无效的正则表达式会被记录到日志，但不会影响其他规则生效。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- suffixRemovalMode: fileExplorerOnly
  $name: Suffix removal mode
  $name:zh-CN: 后缀移除模式
  $options:
  - off: Off
  - fileExplorerOnly: File Explorer only
  - universal: Universal
  $options:zh-CN:
  - off: 关闭
  - fileExplorerOnly: 仅文件资源管理器
  - universal: 通用
  $description: >-
    Controls how suffixes are removed from taskbar titles. "File Explorer only"
    only removes File Explorer suffixes. "Universal" removes the suffix from any
    title (e.g., "Document - Editor" becomes "Document").
  $description:zh-CN: 控制如何从任务栏标题中移除后缀。“仅文件资源管理器”只移除文件资源管理器的后缀。“通用”会从任意标题中移除后缀（例如“文档 - 编辑器”会变成“文档”）。
- suffixRules:
  - - processIdentifier: ""
      $name: Process (name, path, or App ID)
      $name:zh-CN: 进程（名称、路径或应用 ID）
      $description: >-
        Can be a process name (explorer.exe), full path
        (C:\Windows\explorer.exe), or App ID
        (Microsoft.WindowsCalculator_8wekyb3d8bbwe!App). Leave empty to match
        all processes.
      $description:zh-CN: 可以是进程名（explorer.exe）、完整路径（C:\Windows\explorer.exe）或应用 ID（Microsoft.WindowsCalculator_8wekyb3d8bbwe!App）。留空则匹配所有进程。
    - search: ""
      $name: Search pattern (regex)
      $name:zh-CN: 搜索模式（正则表达式）
      $description: >-
        Regular expression pattern to search for in window titles. Example: " -
        Notepad$" to match " - Notepad" at the end of the title.
      $description:zh-CN: 用于在窗口标题中搜索的正则表达式模式。例如：用“ - Notepad$”匹配标题末尾的“ - Notepad”。
    - replace: ""
      $name: Replace with
      $name:zh-CN: 替换为
      $description: >-
        Replacement text. Can use regex capture groups ($1, $2, etc.). Leave
        empty to remove the matched text.
      $description:zh-CN: 替换文本。可以使用正则捕获组（$1、$2 等）。留空则删除匹配到的文本。
  $name: Custom title modification rules
  $name:zh-CN: 自定义标题修改规则
  $description: >-
    Define regex-based search and replace rules for window titles. Each rule
    specifies a process and a regex pattern. Multiple rules can match the same
    window, and all matching patterns are applied in order.
  $description:zh-CN: 为窗口标题定义基于正则的搜索和替换规则。每条规则指定一个进程和一个正则表达式模式。多条规则可以匹配同一个窗口，所有匹配到的模式会按顺序依次应用。
*/
// ==/WindhawkModSettings==

#include <psapi.h>

#include <regex>
#include <string>
#include <vector>

#include <winrt/base.h>

enum class SuffixRemovalMode {
    Off,
    FileExplorerOnly,
    Universal,
};

struct SuffixRule {
    std::wstring processIdentifier;  // Stored in uppercase, empty = match all
    std::wregex search;
    std::wstring replace;
};

struct {
    SuffixRemovalMode suffixRemovalMode;
    std::vector<SuffixRule> suffixRules;
} g_settings;

HWND FindCurrentProcessTaskbarWnd() {
    HWND hTaskbarWnd = nullptr;

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) WINAPI -> BOOL {
            DWORD dwProcessId;
            WCHAR className[32];
            if (GetWindowThreadProcessId(hWnd, &dwProcessId) &&
                dwProcessId == GetCurrentProcessId() &&
                GetClassName(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"Shell_TrayWnd") == 0) {
                *reinterpret_cast<HWND*>(lParam) = hWnd;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&hTaskbarWnd));

    return hTaskbarWnd;
}

HWND GetTaskBandWnd() {
    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (hTaskbarWnd) {
        return (HWND)GetProp(hTaskbarWnd, L"TaskbandHWND");
    }

    return nullptr;
}

// https://gist.github.com/m417z/451dfc2dad88d7ba88ed1814779a26b4
std::wstring GetWindowAppId(HWND hWnd) {
    // {c8900b66-a973-584b-8cae-355b7f55341b}
    constexpr winrt::guid CLSID_StartMenuCacheAndAppResolver{
        0x660b90c8,
        0x73a9,
        0x4b58,
        {0x8c, 0xae, 0x35, 0x5b, 0x7f, 0x55, 0x34, 0x1b}};

    // {de25675a-72de-44b4-9373-05170450c140}
    constexpr winrt::guid IID_IAppResolver_8{
        0xde25675a,
        0x72de,
        0x44b4,
        {0x93, 0x73, 0x05, 0x17, 0x04, 0x50, 0xc1, 0x40}};

    struct IAppResolver_8 : public IUnknown {
       public:
        virtual HRESULT STDMETHODCALLTYPE GetAppIDForShortcut() = 0;
        virtual HRESULT STDMETHODCALLTYPE GetAppIDForShortcutObject() = 0;
        virtual HRESULT STDMETHODCALLTYPE
        GetAppIDForWindow(HWND hWnd,
                          WCHAR** pszAppId,
                          void* pUnknown1,
                          void* pUnknown2,
                          void* pUnknown3) = 0;
        virtual HRESULT STDMETHODCALLTYPE
        GetAppIDForProcess(DWORD dwProcessId,
                           WCHAR** pszAppId,
                           void* pUnknown1,
                           void* pUnknown2,
                           void* pUnknown3) = 0;
    };

    HRESULT hr;
    std::wstring result;

    winrt::com_ptr<IAppResolver_8> appResolver;
    hr = CoCreateInstance(CLSID_StartMenuCacheAndAppResolver, nullptr,
                          CLSCTX_INPROC_SERVER | CLSCTX_INPROC_HANDLER,
                          IID_IAppResolver_8, appResolver.put_void());
    if (SUCCEEDED(hr)) {
        WCHAR* pszAppId;
        hr = appResolver->GetAppIDForWindow(hWnd, &pszAppId, nullptr, nullptr,
                                            nullptr);
        if (SUCCEEDED(hr)) {
            result = pszAppId;
            CoTaskMemFree(pszAppId);
        }
    }

    return result;
}

std::vector<const SuffixRule*> GetRulesForWindow(HWND hWnd) {
    std::vector<const SuffixRule*> matchedRules;

    if (g_settings.suffixRules.empty()) {
        return matchedRules;
    }

    // Get process path and convert to uppercase
    WCHAR resolvedWindowProcessPath[MAX_PATH];
    WCHAR resolvedWindowProcessPathUpper[MAX_PATH];
    DWORD resolvedWindowProcessPathLen = 0;

    DWORD dwProcessId = 0;
    if (GetWindowThreadProcessId(hWnd, &dwProcessId)) {
        HANDLE hProcess =
            OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, dwProcessId);
        if (hProcess) {
            DWORD dwSize = ARRAYSIZE(resolvedWindowProcessPath);
            if (QueryFullProcessImageName(hProcess, 0,
                                          resolvedWindowProcessPath, &dwSize)) {
                resolvedWindowProcessPathLen = dwSize;
            }
            CloseHandle(hProcess);
        }
    }

    if (resolvedWindowProcessPathLen > 0) {
        LCMapStringEx(LOCALE_NAME_USER_DEFAULT, LCMAP_UPPERCASE,
                      resolvedWindowProcessPath,
                      resolvedWindowProcessPathLen + 1,
                      resolvedWindowProcessPathUpper,
                      resolvedWindowProcessPathLen + 1, nullptr, nullptr, 0);
    } else {
        resolvedWindowProcessPathUpper[0] = L'\0';
    }

    // Extract process name from path
    PCWSTR programFileNameUpper =
        wcsrchr(resolvedWindowProcessPathUpper, L'\\');
    if (programFileNameUpper) {
        programFileNameUpper++;
    }

    // Get App ID once (expensive operation)
    std::wstring appId;
    bool appIdFetched = false;

    // Check each rule and collect all matches
    for (const auto& rule : g_settings.suffixRules) {
        bool matches = false;

        // Empty process identifier matches all processes
        if (rule.processIdentifier.empty()) {
            matches = true;
        }
        // Check full path match
        else if (wcscmp(resolvedWindowProcessPathUpper,
                        rule.processIdentifier.c_str()) == 0) {
            matches = true;
        }
        // Check process name match
        else if (programFileNameUpper && *programFileNameUpper &&
                 wcscmp(programFileNameUpper, rule.processIdentifier.c_str()) ==
                     0) {
            matches = true;
        }
        // Check App ID match
        else {
            if (!appIdFetched) {
                appId = GetWindowAppId(hWnd);
                if (!appId.empty()) {
                    LCMapStringEx(
                        LOCALE_NAME_USER_DEFAULT, LCMAP_UPPERCASE, appId.data(),
                        static_cast<int>(appId.length()), appId.data(),
                        static_cast<int>(appId.length()), nullptr, nullptr, 0);
                }
                appIdFetched = true;
            }
            if (!appId.empty() &&
                wcscmp(appId.c_str(), rule.processIdentifier.c_str()) == 0) {
                matches = true;
            }
        }

        if (matches) {
            matchedRules.push_back(&rule);
        }
    }

    return matchedRules;
}

using FindResourceExW_t = decltype(&FindResourceExW);
FindResourceExW_t FindResourceExW_Original;
HRSRC WINAPI FindResourceExW_Hook(HMODULE hModule,
                                  LPCWSTR lpType,
                                  LPCWSTR lpName,
                                  WORD wLanguage) {
    if (g_settings.suffixRemovalMode == SuffixRemovalMode::FileExplorerOnly &&
        hModule && lpType == RT_STRING && lpName == MAKEINTRESOURCE(2195) &&
        hModule == GetModuleHandle(L"explorerframe.dll")) {
        Wh_Log(L">");
        SetLastError(ERROR_RESOURCE_NAME_NOT_FOUND);
        return nullptr;
    }

    return FindResourceExW_Original(hModule, lpType, lpName, wLanguage);
}

using InternalGetWindowText_t = int(WINAPI*)(HWND hWnd,
                                             LPWSTR pString,
                                             int cchMaxCount);
InternalGetWindowText_t InternalGetWindowText_Original;
int WINAPI InternalGetWindowText_Hook(HWND hWnd,
                                      LPWSTR pString,
                                      int cchMaxCount) {
    int result = InternalGetWindowText_Original(hWnd, pString, cchMaxCount);
    if (result == 0 || !pString || cchMaxCount == 0) {
        return result;
    }

    if (g_settings.suffixRemovalMode != SuffixRemovalMode::Universal &&
        g_settings.suffixRules.empty()) {
        return result;
    }

    void* retAddress = __builtin_return_address(0);

    HMODULE taskbarModule = GetModuleHandle(L"taskbar.dll");
    if (!taskbarModule) {
        return result;
    }

    HMODULE module;
    if (!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                               GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           (PCWSTR)retAddress, &module) ||
        module != taskbarModule) {
        return result;
    }

    Wh_Log(L"Original text: %s", pString);

    std::wstring text = pString;
    bool modified = false;

    // Apply universal mode: remove last " - <text>" or "—<text>" part
    if (g_settings.suffixRemovalMode == SuffixRemovalMode::Universal &&
        result > 0 && pString) {
        // Find the last occurrence of " - ", " — ", or "—"
        size_t lastSepPos = std::wstring::npos;

        size_t hyphenPos = text.rfind(L" - ");
        size_t emDashSpacePos = text.rfind(L" — ");
        size_t emDashPos = text.rfind(L"—");

        // Find the rightmost separator
        // Note: Check longer patterns first to avoid matching "—" that's part
        // of " — "
        if (hyphenPos != std::wstring::npos) {
            lastSepPos = hyphenPos;
        }
        if (emDashSpacePos != std::wstring::npos &&
            (lastSepPos == std::wstring::npos || emDashSpacePos > lastSepPos)) {
            lastSepPos = emDashSpacePos;
        }
        // Only use emDashPos if it's not part of " — " pattern
        if (emDashPos != std::wstring::npos &&
            (lastSepPos == std::wstring::npos || emDashPos > lastSepPos) &&
            !(emDashSpacePos != std::wstring::npos &&
              emDashPos == emDashSpacePos + 1)) {
            lastSepPos = emDashPos;
        }

        if (lastSepPos != std::wstring::npos) {
            text = text.substr(0, lastSepPos);
            modified = true;
            Wh_Log(L"Universal mode: removed suffix after last separator");
        }
    }

    // Get all matching rules for this window's process
    std::vector<const SuffixRule*> rules = GetRulesForWindow(hWnd);

    if (!rules.empty() && result > 0 && pString) {
        // Apply all matching rules in order
        for (const auto* rule : rules) {
            try {
                std::wstring newText =
                    std::regex_replace(text, rule->search, rule->replace);
                if (newText != text) {
                    text = newText;
                    modified = true;
                }
            } catch (const std::regex_error& ex) {
                Wh_Log(L"Regex replace error %08X: %S",
                       static_cast<DWORD>(ex.code()), ex.what());
            }
        }
    }

    // Update the window text if it changed
    if (modified) {
        if (text.length() < static_cast<size_t>(cchMaxCount)) {
            wcscpy_s(pString, cchMaxCount, text.c_str());
            result = static_cast<int>(text.length());
            Wh_Log(L"Modified text: %s", pString);
        } else {
            Wh_Log(L"Result too long (%zu chars), keeping original",
                   text.length());
        }
    }

    return result;
}

void ApplySettings() {
    HWND hTaskBandWnd = GetTaskBandWnd();
    if (!hTaskBandWnd) {
        return;
    }

    static const UINT WM_SHELLHOOK = RegisterWindowMessage(L"SHELLHOOK");

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) WINAPI -> BOOL {
            if (IsWindowVisible(hWnd)) {
                PostMessage(reinterpret_cast<HWND>(lParam), WM_SHELLHOOK,
                            HSHELL_REDRAW, reinterpret_cast<LPARAM>(hWnd));
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(hTaskBandWnd));
}

void LoadSettings() {
    Wh_Log(L"LoadSettings");

    // Load File Explorer suffix mode
    PCWSTR mode = Wh_GetStringSetting(L"suffixRemovalMode");
    g_settings.suffixRemovalMode = SuffixRemovalMode::FileExplorerOnly;
    if (wcscmp(mode, L"off") == 0) {
        g_settings.suffixRemovalMode = SuffixRemovalMode::Off;
    } else if (wcscmp(mode, L"universal") == 0) {
        g_settings.suffixRemovalMode = SuffixRemovalMode::Universal;
    }
    Wh_FreeStringSetting(mode);

    // Load custom suffix rules
    g_settings.suffixRules.clear();

    for (int i = 0;; i++) {
        PCWSTR processId =
            Wh_GetStringSetting(L"suffixRules[%d].processIdentifier", i);
        PCWSTR search = Wh_GetStringSetting(L"suffixRules[%d].search", i);
        PCWSTR replace = Wh_GetStringSetting(L"suffixRules[%d].replace", i);

        bool hasRule = *search;

        if (!hasRule) {
            Wh_FreeStringSetting(processId);
            Wh_FreeStringSetting(search);
            Wh_FreeStringSetting(replace);
            break;
        }

        try {
            SuffixRule rule;

            // Convert processIdentifier to uppercase (empty = match all)
            if (*processId) {
                rule.processIdentifier = processId;
                LCMapStringEx(LOCALE_NAME_USER_DEFAULT, LCMAP_UPPERCASE,
                              &rule.processIdentifier[0],
                              static_cast<int>(rule.processIdentifier.length()),
                              &rule.processIdentifier[0],
                              static_cast<int>(rule.processIdentifier.length()),
                              nullptr, nullptr, 0);
            }

            rule.search = std::wregex(search);
            rule.replace = replace;

            Wh_Log(L"Loaded rule for '%s': '%s' -> '%s'",
                   rule.processIdentifier.empty()
                       ? L"<all processes>"
                       : rule.processIdentifier.c_str(),
                   search, replace);

            g_settings.suffixRules.push_back(std::move(rule));
        } catch (const std::regex_error& ex) {
            Wh_Log(L"Invalid regex pattern '%s': %S (code %08X)", search,
                   ex.what(), static_cast<DWORD>(ex.code()));
        }

        Wh_FreeStringSetting(processId);
        Wh_FreeStringSetting(search);
        Wh_FreeStringSetting(replace);
    }
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
    HMODULE kernel32Module = GetModuleHandle(L"kernel32.dll");

    auto setKernelFunctionHook = [kernelBaseModule, kernel32Module](
                                     PCSTR targetName, void* hookFunction,
                                     void** originalFunction) {
        void* targetFunction =
            (void*)GetProcAddress(kernelBaseModule, targetName);
        if (!targetFunction) {
            targetFunction = (void*)GetProcAddress(kernel32Module, targetName);
            if (!targetFunction) {
                return FALSE;
            }
        }

        return Wh_SetFunctionHook(targetFunction, hookFunction,
                                  originalFunction);
    };

    setKernelFunctionHook("FindResourceExW", (void*)FindResourceExW_Hook,
                          (void**)&FindResourceExW_Original);

    HMODULE user32Module = GetModuleHandle(L"user32.dll");
    if (user32Module) {
        void* pInternalGetWindowText =
            (void*)GetProcAddress(user32Module, "InternalGetWindowText");
        if (pInternalGetWindowText) {
            Wh_SetFunctionHook(pInternalGetWindowText,
                               (void*)InternalGetWindowText_Hook,
                               (void**)&InternalGetWindowText_Original);
        }
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    ApplySettings();
}

void Wh_ModUninit() {
    Wh_Log(L">");

    ApplySettings();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();

    ApplySettings();
}
