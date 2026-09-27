# Windhawk Mods 简体中文汉化整合

Windhawk 汉化 mod 的个人整合仓库。这里的每个 `.wh.cpp` 都是**完全汉化版**：
元数据（`@name:zh-CN` / `@description:zh-CN`）、设置项（`$name:zh-CN` / `$description:zh-CN` /
`$options:zh-CN`）以及 `==WindhawkModReadme==` 说明正文全部为简体中文。

共 **93** 个 mod：76 个来自 [m417z/my-windhawk-mods](https://github.com/m417z/my-windhawk-mods)，
17 个来自其他作者。

> **关于上游提交**：本仓库的「完全汉化版」（含 Readme 中文正文）**仅在本仓库使用**。
> Readme 无法通过官方本地化机制做多语言，因此不会把 Readme 中文版提交到他人仓库；
> 需要向上游贡献时，只提交元数据与设置项的汉化。

## 使用方式

1. 打开 Windhawk → 「设置」→ 打开 mod 源码目录（默认 `%ProgramData%\Windhawk\AppData\ModsSource`）。
2. 用本仓库 `mods/` 下同名文件覆盖对应的 `.wh.cpp`（覆盖前建议先备份）。
   文件名与 Windhawk 的 mod id 一致，例如 `windows-11-taskbar-styler.wh.cpp`。
3. 回到 Windhawk 重新编译该 mod（「高级」→ 重新编译，或重启 Windhawk）。

也可以直接修改 mod 源码：把 `mods/<id>.wh.cpp` 的内容整体替换进去即可。

## 说明

- **换行符**：与各上游仓库保持一致，未做统一改写（m417z 系 mod 多为 CRLF，其余作者多为 LF）。
- **版本号**：`@version` 一律沿用上游版本号，不做本地自增，便于与商店版本直接对照。
- **未翻译的内容**：专有名词（主题名、字体名、`JavaScript`/`CSS`/`Electron main.js` 等选项）
  保留原文，便于与上游文档和主题页面一一对应。
- **`AltDrag`、`NoFlashWindow`** 两个 mod 的名称本身即专有名词，故未添加 `@name:zh-CN`。

## Mod 清单

### 来自 m417z（76 个）

| # | Mod | 中文名称 | 作者 |
|---|---|---|---|
| 1 | `alt-drag` | AltDrag | [m417z](https://github.com/m417z) |
| 2 | `chrome-wheel-scroll-tabs` | 用滚轮切换 Chrome/Edge 标签页 | [m417z](https://github.com/m417z) |
| 3 | `common-controls-hook` | Common Controls 钩子 | [m417z](https://github.com/m417z) |
| 4 | `custom-corner-radius` | 自定义窗口圆角半径 | [m417z](https://github.com/m417z) |
| 5 | `desktop-icons-view` | 桌面图标视图 | [m417z](https://github.com/m417z) |
| 6 | `desktop-live-overlay` | 桌面实时叠加层 | [m417z](https://github.com/m417z) |
| 7 | `disable-rounded-corners` | 关闭 Windows 11 窗口圆角 | [m417z](https://github.com/m417z) |
| 8 | `explorer-context-menu-classic` | Windows 11 经典右键菜单 | [m417z](https://github.com/m417z) |
| 9 | `explorer-details-better-file-sizes` | 资源管理器更佳的文件大小显示 | [m417z](https://github.com/m417z) |
| 10 | `explorer-folder-hover-menu` | 文件夹悬停菜单 | [m417z](https://github.com/m417z) |
| 11 | `explorer-frame-classic` | 经典资源管理器导航栏 | [m417z](https://github.com/m417z) |
| 12 | `explorer-name-windows` | 命名资源管理器窗口 | [m417z](https://github.com/m417z) |
| 13 | `extension-change-no-warning` | 关闭修改扩展名警告 | [m417z](https://github.com/m417z) |
| 14 | `file-explorer-remove-suffixes` | 移除任务栏窗口标题后缀 | [m417z](https://github.com/m417z) |
| 15 | `flight-simulator-focus-helper` | 飞行模拟器窗口焦点助手 | [m417z](https://github.com/m417z) |
| 16 | `icon-resource-redirect` | 系统资源重定向 | [m417z](https://github.com/m417z) |
| 17 | `keyboard-shortcut-actions` | 键盘快捷键动作 | [m417z](https://github.com/m417z) |
| 18 | `more-space-in-language-indicator` | 语言指示器更多空间 | [m417z](https://github.com/m417z) |
| 19 | `no-flash-window` | NoFlashWindow | [m417z](https://github.com/m417z) |
| 20 | `notepad-dark-mode` | 记事本深色模式 | [m417z](https://github.com/m417z) |
| 21 | `notifications-placement` | 自定义 Windows 通知位置 | [m417z](https://github.com/m417z) |
| 22 | `pinned-items-double-click` | 双击打开固定项 | [m417z](https://github.com/m417z) |
| 23 | `search-menu-inspect-helper` | 搜索菜单检查助手 | [m417z](https://github.com/m417z) |
| 24 | `shell-flyout-positions` | 系统浮出菜单位置 | [m417z](https://github.com/m417z) |
| 25 | `slick-window-arrangement` | 顺滑窗口排列 | [m417z](https://github.com/m417z) |
| 26 | `start-menu-all-apps` | 开始菜单默认显示所有应用 | [m417z](https://github.com/m417z) |
| 27 | `start-menu-open-location` | 开始菜单打开位置 | [m417z](https://github.com/m417z) |
| 28 | `start-menu-size` | 开始菜单尺寸 | [m417z](https://github.com/m417z) |
| 29 | `taskbar-auto-hide-custom-activation-area` | 自定义任务栏自动隐藏触发区 | [m417z](https://github.com/m417z) |
| 30 | `taskbar-auto-hide-keyboard-only` | 任务栏自动隐藏微调 | [m417z](https://github.com/m417z) |
| 31 | `taskbar-auto-hide-per-monitor` | 每台显示器独立自动隐藏 | [m417z](https://github.com/m417z) |
| 32 | `taskbar-auto-hide-speed` | 任务栏自动隐藏速度 | [m417z](https://github.com/m417z) |
| 33 | `taskbar-auto-hide-when-maximized` | 最大化时自动隐藏任务栏 | [m417z](https://github.com/m417z) |
| 34 | `taskbar-background-helper` | 任务栏背景助手 | [m417z](https://github.com/m417z) |
| 35 | `taskbar-button-click` | 任务栏中键关闭程序 | [m417z](https://github.com/m417z) |
| 36 | `taskbar-button-scroll` | 滚轮最小化/还原任务栏窗口 | [m417z](https://github.com/m417z) |
| 37 | `taskbar-classic-menu` | 任务栏经典右键菜单 | [m417z](https://github.com/m417z) |
| 38 | `taskbar-clock-customization` | 任务栏时钟自定义 | [m417z](https://github.com/m417z) |
| 39 | `taskbar-grouping` | 关闭任务栏窗口合并 | [m417z](https://github.com/m417z) |
| 40 | `taskbar-hung-rearrangement-fix` | 任务栏无响应窗口乱序修复 | [m417z](https://github.com/m417z) |
| 41 | `taskbar-icon-size` | 任务栏高度与图标大小 | [m417z](https://github.com/m417z) |
| 42 | `taskbar-jump-list-on-cursor-pos` | 跳转列表跟随鼠标位置 | [m417z](https://github.com/m417z) |
| 43 | `taskbar-labels` | Windows 11 任务栏标签 | [m417z](https://github.com/m417z) |
| 44 | `taskbar-left-click-cycle` | 点击循环切换任务栏窗口 | [m417z](https://github.com/m417z) |
| 45 | `taskbar-multirow` | Windows 11 多行任务栏 | [m417z](https://github.com/m417z) |
| 46 | `taskbar-no-minimize` | 任务栏：点击不最小化 | [m417z](https://github.com/m417z) |
| 47 | `taskbar-notification-icon-spacing` | 任务栏托盘图标间距与网格 | [m417z](https://github.com/m417z) |
| 48 | `taskbar-notification-icons-show-all` | 始终显示全部托盘图标 | [m417z](https://github.com/m417z) |
| 49 | `taskbar-on-top` | Windows 11 顶部任务栏 | [m417z](https://github.com/m417z) |
| 50 | `taskbar-primary-on-secondary-monitor` | 主任务栏移至副显示器 | [m417z](https://github.com/m417z) |
| 51 | `taskbar-reorder-right-drag` | 任务栏右键拖动排序 | [m417z](https://github.com/m417z) |
| 52 | `taskbar-scroll-actions` | 任务栏滚动动作 | [m417z](https://github.com/m417z) |
| 53 | `taskbar-show-desktop-button-aero-peek` | “显示桌面”按钮悬停 Aero Peek | [m417z](https://github.com/m417z) |
| 54 | `taskbar-start-button-colorizer` | 开始按钮着色 | [m417z](https://github.com/m417z) |
| 55 | `taskbar-start-button-position` | 开始按钮固定靠左 | [m417z](https://github.com/m417z) |
| 56 | `taskbar-thumbnail-reorder` | 任务栏缩略图排序 | [m417z](https://github.com/m417z) |
| 57 | `taskbar-thumbnail-size` | 任务栏缩略图大小 | [m417z](https://github.com/m417z) |
| 58 | `taskbar-thumbnails` | 禁用任务栏缩略图 | [m417z](https://github.com/m417z) |
| 59 | `taskbar-tray-show-on-hover` | 托盘区自动隐藏（悬停显示） | [m417z](https://github.com/m417z) |
| 60 | `taskbar-tray-system-icon-tweaks` | 托盘系统图标调整 | [m417z](https://github.com/m417z) |
| 61 | `taskbar-vertical` | Windows 11 垂直任务栏 | [m417z](https://github.com/m417z) |
| 62 | `taskbar-volume-control-per-app` | 任务栏滚轮调节单应用音量 | [m417z](https://github.com/m417z) |
| 63 | `taskbar-volume-control` | 任务栏滚轮调节音量 | [m417z](https://github.com/m417z) |
| 64 | `taskbar-wheel-cycle` | 滚轮循环切换任务栏按钮 | [m417z](https://github.com/m417z) |
| 65 | `text-replace` | 任意文本替换 | [m417z](https://github.com/m417z) |
| 66 | `timer-resolution-control` | 计时器分辨率控制 | [m417z](https://github.com/m417z) |
| 67 | `virtual-desktop-taskbar-order` | 虚拟桌面保持任务栏顺序 | [m417z](https://github.com/m417z) |
| 68 | `visual-studio-anti-rich-header` | Visual Studio 禁用 Rich 头 | [m417z](https://github.com/m417z) |
| 69 | `vmware-disable-upgrade-dialog` | 关闭 VMware 升级提示 | [m417z](https://github.com/m417z) |
| 70 | `volume-control-open-location` | 音量控件打开位置 | [m417z](https://github.com/m417z) |
| 71 | `vscode-tweaker` | VSCode 调整器 | [m417z](https://github.com/m417z) |
| 72 | `windows-11-file-explorer-styler` | Windows 11 文件资源管理器样式器 | [m417z](https://github.com/m417z) |
| 73 | `windows-11-notification-center-styler` | Windows 11 通知中心样式器 | [m417z](https://github.com/m417z) |
| 74 | `windows-11-settings-styler` | Windows 11 设置样式器 | [m417z](https://github.com/m417z) |
| 75 | `windows-11-start-menu-styler` | Windows 11 开始菜单样式器 | [m417z](https://github.com/m417z) |
| 76 | `windows-11-taskbar-styler` | Windows 11 任务栏样式器 | [m417z](https://github.com/m417z) |

### 来自其他作者（17 个）

| # | Mod | 中文名称 | 作者 |
|---|---|---|---|
| 1 | `antigravity-portable` | Antigravity IDE 便携版 | [easyatm](https://github.com/easyatm) |
| 2 | `caps-ime-switcher` | Caps 输入法切换器 | [ZeonXr](https://github.com/ZeonXr) |
| 3 | `chinese-ime-fixed-mode` | 中文输入法固定模式 | [barry](https://github.com/barrypp) |
| 4 | `close-explorer-on-esc` | 按 Esc 关闭资源管理器 | [lieyanbang](https://github.com/lieyanbang) |
| 5 | `explorer-ctrln-newfile` | 用 ctrl+n 创建新文件 | [lieyanbang](https://github.com/lieyanbang) |
| 6 | `file-operation-styler` | 文件操作窗口美化 | [digART](https://github.com/digart11) |
| 7 | `ime-mode-lock` | 输入法原生模式锁定 | [ZeonXr](https://github.com/ZeonXr) |
| 8 | `modernize-folder-picker-dialog` | 现代化文件夹选择对话框 | [aubymori](https://github.com/aubymori) |
| 9 | `mouse-trail` | 鼠标拖尾 | [MCheng404](https://github.com/MCheng404) |
| 10 | `office-fix-account-disp-name` | Office 修复右上角账户名显示 | [Joe Ye](https://github.com/JoeYe-233) |
| 11 | `taskbar-autohide-better` | 更好的任务栏自动隐藏 | [Cirn09](https://github.com/Cirn09) |
| 12 | `taskbar-brightness-and-opacity-tuner` | 任务栏亮度和透明度调节器 | [lzxujun](https://github.com/lzxujun) |
| 13 | `taskbar-icon-group-centering` | 任务栏图标组居中 | [Suioio](https://github.com/Suioio) |
| 14 | `translucent-windows` | 半透明窗口效果 | [Undisputed00x](https://github.com/Undisputed00x) |
| 15 | `visio-pan-zoom` | Visio 中键平移与智能缩放 | [Joe Ye](https://github.com/JoeYe-233) |
| 16 | `win-d-per-monitor` | Win+D 仅作用于当前显示器（显示桌面） | [easyatm](https://github.com/easyatm) |
| 17 | `win11-power-buttons` | Windows 11 开始菜单一键电源按钮 | [Hakuuyosei](https://github.com/ahzvenol) |

## 致谢

所有 mod 的著作权归各自作者所有，本仓库仅提供简体中文翻译。
翻译内容按原项目的许可协议分发；如原项目采用 GPL 等协议，请一并遵守。

如果翻译有误或上游已更新，欢迎提 Issue。
