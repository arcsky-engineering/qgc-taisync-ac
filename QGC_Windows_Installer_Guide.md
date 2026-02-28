# QGroundControl Custom Build: Windows Installer Creation Guide

A complete guide to packaging your custom QGC build into a distributable Windows installer with custom icons.

---

## Prerequisites

- Qt Creator with a compatible Qt version installed (5.15.x or 6.x depending on your QGC version)
- QGroundControl source code (cloned with `--recursive` for submodules)
- GStreamer 1.0 MSVC runtime installed (download from gstreamer.freedesktop.org)
- OpenSSL binaries for Windows (typically bundled with Qt or GStreamer)
- Inno Setup 6.x (download from jrsoftware.org)
- Your custom icon file in .ico format (256x256 recommended with multiple sizes embedded)

> **Note:** Ensure your icon file contains multiple sizes (16x16, 32x32, 48x48, 256x256) for best appearance across Windows Explorer, taskbar, and desktop shortcuts.

---

## 1. Building in Release Mode

Before deployment, ensure you have a Release build of your custom QGC. Debug builds include extra debugging symbols and dependencies that are not suitable for distribution.

1. Open your QGroundControl project in Qt Creator (CMakeLists.txt or .pro file)
2. In the bottom-left corner, click the kit selector
3. Change the build type from **Debug** to **Release**
4. Build the project (`Ctrl+B` or Build menu → Build Project)
5. Locate the output executable in your build directory (e.g., `build/release/` or `build-Release/`)

> **Tip:** The build directory location depends on your Qt Creator settings. Check Projects → Build Settings to see the exact output path.

---

## 2. Setting Up the Custom Application Icon

To have your custom icon embedded in the .exe file itself (visible in Explorer and taskbar), you need to configure the build system before compiling.

### For qmake (.pro file):

```qmake
# Add this line to your .pro file
RC_ICONS = resources/your_icon.ico
```

### For CMake (CMakeLists.txt):

First, create a Windows resource file (e.g., `resources/app.rc`):

```rc
IDI_ICON1 ICON "your_icon.ico"
```

Then add to your CMakeLists.txt:

```cmake
# Add the resource file to your executable
set(APP_ICON_RESOURCE "${CMAKE_SOURCE_DIR}/resources/app.rc")

add_executable(YourAppName
    ${SOURCES}
    ${APP_ICON_RESOURCE}
)
```

> **Important:** After adding the icon configuration, you must rebuild the project for the icon to be embedded in the executable.

---

## 3. Deploying with windeployqt

The `windeployqt` tool copies all required Qt DLLs, plugins, and QML modules to your deployment folder. Without this step, the .exe will fail to run on machines without Qt installed.

### Step-by-step process:

**1. Create a clean deployment folder:**

```cmd
mkdir C:\QGC_Deploy
```

**2. Copy your built executable:**

```cmd
copy "C:\path\to\build\release\YourCustomQGC.exe" C:\QGC_Deploy\
```

**3. Open a Qt Command Prompt** (not regular Command Prompt):

Find "Qt 5.15.x (MSVC 2019 64-bit)" or similar in your Start Menu. This ensures Qt's bin directory is in your PATH.

**4. Run windeployqt:**

```cmd
cd C:\QGC_Deploy
windeployqt.exe --release --qmldir C:\path\to\qgroundcontrol\src YourCustomQGC.exe
```

> **Important:** The `--qmldir` flag is critical for QGC because it uses QML extensively. Point it to your QGC source directory so windeployqt can scan for required QML modules.

### Common windeployqt options:

| Option | Description |
|--------|-------------|
| `--release` | Deploy release versions of Qt libraries |
| `--qmldir <path>` | Scan QML files to determine required modules |
| `--no-translations` | Skip Qt translation files (reduces size) |
| `--no-opengl-sw` | Skip software OpenGL renderer |
| `--verbose 2` | Show detailed output for debugging |

---

## 4. Adding GStreamer Dependencies

QGroundControl uses GStreamer for video streaming. The `windeployqt` tool does not handle GStreamer dependencies, so you must copy them manually.

### Locate your GStreamer installation:

```
# Typical installation paths:
C:\gstreamer\1.0\msvc_x86_64\    (64-bit MSVC)
C:\gstreamer\1.0\mingw_x86_64\   (64-bit MinGW)
```

### Option A: Copy all GStreamer DLLs (simple but larger):

```cmd
# Copy all DLLs from GStreamer bin folder
copy "C:\gstreamer\1.0\msvc_x86_64\bin\*.dll" C:\QGC_Deploy\

# Copy GStreamer plugins
mkdir C:\QGC_Deploy\lib\gstreamer-1.0
copy "C:\gstreamer\1.0\msvc_x86_64\lib\gstreamer-1.0\*.dll" C:\QGC_Deploy\lib\gstreamer-1.0\
```

### Option B: Copy only required DLLs (smaller package):

Core GStreamer libraries typically needed:

- `gstreamer-1.0-0.dll`
- `gstbase-1.0-0.dll`
- `gstapp-1.0-0.dll`
- `gstvideo-1.0-0.dll`
- `gstpbutils-1.0-0.dll`
- `gstaudio-1.0-0.dll`
- `gstrtp-1.0-0.dll`
- `gstrtsp-1.0-0.dll`
- `gstsdp-1.0-0.dll`
- `gstgl-1.0-0.dll`
- `gstcodecparsers-1.0-0.dll`

Plus supporting libraries:

- `glib-2.0-0.dll`
- `gobject-2.0-0.dll`
- `gmodule-2.0-0.dll`
- `intl-8.dll`
- `ffi-7.dll`
- `z-1.dll` (or `zlib1.dll`)
- `orc-0.4-0.dll`

> **Tip:** Start with Option A (all DLLs). Once everything works, you can trim unnecessary files to reduce installer size. Use a tool like Dependencies (github.com/lucasg/Dependencies) to analyze which DLLs are actually loaded.

---

## 5. Adding OpenSSL Dependencies

QGC requires OpenSSL for HTTPS connections (MAVLink over secure connections, map tile downloads, etc.).

### Required files (OpenSSL 1.1.x):

```
libssl-1_1-x64.dll
libcrypto-1_1-x64.dll
```

### Or for OpenSSL 3.x:

```
libssl-3-x64.dll
libcrypto-3-x64.dll
```

### Common locations to find these files:

- GStreamer installation: `C:\gstreamer\1.0\msvc_x86_64\bin\`
- Qt installation: `C:\Qt\5.15.x\msvc2019_64\bin\`
- Standalone OpenSSL: `C:\Program Files\OpenSSL-Win64\bin\`

---

## 6. Testing the Deployment

Before creating the installer, verify that your deployment folder works correctly.

### Testing checklist:

- [ ] Test on a "clean" machine (or VM) without Qt or development tools installed
- [ ] Verify the application launches without DLL errors
- [ ] Test video streaming functionality (requires GStreamer)
- [ ] Test map tile loading (requires OpenSSL for HTTPS)
- [ ] Verify custom icon appears in taskbar and window title
- [ ] Check all QML-based UI elements load correctly

> **Debugging tip:** If you get a DLL not found error, the error message will tell you exactly which DLL is missing. Find and copy that DLL to your deploy folder.

### Using Dependencies tool:

Download Dependencies from github.com/lucasg/Dependencies. Open your .exe with it to see a complete tree of all required DLLs and identify any that are missing.

---

## 7. Creating the Inno Setup Installer Script

Inno Setup uses `.iss` script files to define how the installer is built. Create a new file called `installer.iss` in your project directory.

### Complete Inno Setup Script:

```iss
; installer.iss - Inno Setup Script for Custom QGC Build
; =========================================================

[Setup]
; Application Information
AppName=Your Ground Station Name
AppVersion=1.0.0
AppVerName=Your Ground Station 1.0.0
AppPublisher=Your Company Name
AppPublisherURL=https://yourwebsite.com
AppSupportURL=https://yourwebsite.com/support
AppUpdatesURL=https://yourwebsite.com/updates

; Installation Settings
DefaultDirName={autopf}\Your Ground Station
DefaultGroupName=Your Ground Station
AllowNoIcons=yes
LicenseFile=C:\path\to\license.txt
InfoBeforeFile=C:\path\to\readme.txt

; Output Settings
OutputDir=C:\Installers
OutputBaseFilename=YourGroundStation_Setup_1.0.0
SetupIconFile=C:\path\to\your_icon.ico
UninstallDisplayIcon={app}\YourCustomQGC.exe

; Compression
Compression=lzma2/ultra64
SolidCompression=yes

; Windows Version Requirements
MinVersion=10.0

; Privileges (remove for per-user install)
PrivilegesRequired=admin

; Visual Settings
WizardStyle=modern

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
; Copy all files from deploy folder
Source: "C:\QGC_Deploy\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
; Include your icon file for shortcuts
Source: "C:\path\to\your_icon.ico"; DestDir: "{app}"; Flags: ignoreversion

[Icons]
; Start Menu shortcut
Name: "{group}\Your Ground Station"; Filename: "{app}\YourCustomQGC.exe"; IconFilename: "{app}\your_icon.ico"
; Desktop shortcut (if selected)
Name: "{commondesktop}\Your Ground Station"; Filename: "{app}\YourCustomQGC.exe"; IconFilename: "{app}\your_icon.ico"; Tasks: desktopicon
; Uninstaller in Start Menu
Name: "{group}\Uninstall Your Ground Station"; Filename: "{uninstallexe}"

[Run]
; Option to launch after install
Filename: "{app}\YourCustomQGC.exe"; Description: "{cm:LaunchProgram,Your Ground Station}"; Flags: nowait postinstall skipifsilent

[UninstallDelete]
; Clean up any generated files on uninstall
Type: filesandordirs; Name: "{app}\logs"
Type: filesandordirs; Name: "{app}\cache"
```

### Key sections explained:

| Section | Purpose |
|---------|---------|
| `[Setup]` | Core installer configuration: app name, version, paths, compression |
| `[Files]` | Specifies which files to include and where to install them |
| `[Icons]` | Defines Start Menu and Desktop shortcuts with custom icons |
| `[Tasks]` | Optional checkboxes shown during install (desktop icon, etc.) |
| `[Run]` | Commands to run after installation (launch app option) |

> **Note:** Remove `LicenseFile` and `InfoBeforeFile` lines if you don't have these files. Adjust `MinVersion` based on your Windows compatibility requirements.

---

## 8. Building the Installer

### Using Inno Setup GUI:

1. Open Inno Setup Compiler (installed with Inno Setup)
2. Open your `installer.iss` file (File → Open)
3. Review the script for any path errors (shown in red)
4. Click Build → Compile (or press `F9`)
5. Wait for compilation to complete
6. Find your installer in the `OutputDir` specified in the script

### Command-line compilation:

```cmd
# Compile from command line (useful for CI/CD)
"C:\Program Files (x86)\Inno Setup 6\ISCC.exe" installer.iss
```

### Verifying the installer:

- [ ] Run the installer on a clean test machine or VM
- [ ] Verify all files are installed to the correct location
- [ ] Check that shortcuts are created with the correct icon
- [ ] Launch the application from the shortcut
- [ ] Test the uninstaller removes all files cleanly

---

## 9. Troubleshooting Common Issues

### Application fails to start with DLL error

**Symptom:** Error dialog saying "XXXXX.dll was not found"

**Solution:** Copy the missing DLL from Qt, GStreamer, or Visual C++ Redistributable installation to your deploy folder. Use the Dependencies tool to find all missing DLLs at once.

---

### QML errors or blank UI

**Symptom:** Application starts but UI is blank or shows QML errors in console

**Solution:** Re-run windeployqt with the `--qmldir` flag pointing to your QGC source. Ensure all required QML modules are in the `qml/` subfolder of your deploy directory.

---

### Video streaming not working

**Symptom:** Application runs but video feeds show nothing or error

**Solution:** Verify GStreamer DLLs are present. Check that gstreamer plugins are in `lib/gstreamer-1.0/` subfolder. You may need to set `GST_PLUGIN_PATH` environment variable.

---

### Map tiles not loading

**Symptom:** Map area is blank or shows "offline" message

**Solution:** This usually indicates missing OpenSSL. Verify `libssl` and `libcrypto` DLLs are in your deploy folder. Check that the version matches what Qt was compiled against.

---

### Icon not showing correctly

**Symptom:** Generic icon appears instead of your custom icon

**Solution:** For the .exe icon, you must rebuild after adding `RC_ICONS` to your .pro or adding the .rc file to CMake. For shortcuts, ensure `IconFilename` paths in the .iss script are correct and the .ico file is included in `[Files]`.

---

### VCRUNTIME140.dll not found

**Symptom:** Error about missing Visual C++ runtime DLLs

**Solution:** Either bundle the VC++ Redistributable installer with your package, or add the following to your .iss file to download and install it automatically:

```iss
; Add to [Files] section - bundle the redistributable
Source: "vc_redist.x64.exe"; DestDir: "{tmp}"; Flags: deleteafterinstall

; Add to [Run] section - install before your app
Filename: "{tmp}\vc_redist.x64.exe"; Parameters: "/quiet /norestart"; StatusMsg: "Installing Visual C++ Runtime..."; Flags: waituntilterminated
```

---

## Quick Reference: Complete Deployment Workflow

1. Configure custom icon in `.pro` or `CMakeLists.txt`
2. Build project in Release mode
3. Create clean deployment folder
4. Copy .exe to deployment folder
5. Run `windeployqt` with `--qmldir` flag
6. Copy GStreamer DLLs and plugins
7. Verify OpenSSL DLLs are present
8. Test on clean machine
9. Create and customize `installer.iss`
10. Compile installer with Inno Setup
11. Test installer on clean machine
12. Distribute to customers

---

## Arcsky Control: Rebuilding the Installer

After making code changes and rebuilding in Qt Creator, use the automated script to recreate the installer.

### Prerequisites (already set up)

| Component | Path |
|-----------|------|
| Qt 6.8.3 MSVC | `C:\Qt\6.8.3\msvc2022_64\` |
| GStreamer MSVC | `C:\gstreamer\1.0\msvc_x86_64\` |
| Inno Setup 6 | `C:\Program Files (x86)\Inno Setup 6\` |
| Custom icon | `resources\icons\qgroundcontrol.ico` |
| ISS script | `arcskycontrol.iss` |
| Build script | `build_installer.ps1` |

### Steps

**1. Build in Qt Creator**

- Open the project in Qt Creator
- Select the kit: **Desktop Qt 6.8.3 MSVC2022 64-bit**
- Set build type to **Release** (bottom-left selector)
- Build (`Ctrl+B`)
- Verify the exe appears at: `build\Desktop_Qt_6_8_3_MSVC2022_64bit-Release\Release\ArcskyControl.exe`

**2. Run the installer build script**

Open PowerShell in the repo root (`C:\q\qgroundcontrol`) and run:

```powershell
.\build_installer.ps1
```

This does everything automatically:
- Cleans and recreates `C:\QGC_Deploy\`
- Copies the Release exe
- Runs `windeployqt6` with QML scanning
- Copies all GStreamer DLLs and plugins
- Copies the custom icon
- Compiles the installer via Inno Setup

Output: `C:\q\qgroundcontrol\ArcskyControl_Setup.exe`

**Quick rebuild (exe-only change, no Qt/GStreamer updates):**

```powershell
.\build_installer.ps1 -SkipDeploy
```

This reuses the existing `C:\QGC_Deploy` folder, just swaps the exe and recompiles the installer.

### Updating the custom icon

1. Replace `resources\icons\qgroundcontrol.ico` with your new icon (256x256 with embedded sizes recommended)
2. Also replace `deploy\windows\WindowsQGC.ico` (same file — this is what gets embedded in the exe)
3. Also replace `custom-example\res\icons\custom_qgroundcontrol.ico` and `custom-example\deploy\windows\WindowsQGC.ico` if applicable
4. **Rebuild in Qt Creator** (the icon is baked into the exe at compile time via the RC file)
5. Run `.\build_installer.ps1`

### Updating the version number

Edit `arcskycontrol.iss` and change `AppVersion=1.0` to the new version.

### Troubleshooting

- **"Release exe not found"** — Build the project in Release mode in Qt Creator first
- **"windeployqt6.exe not found"** — Update `$QtBinDir` in `build_installer.ps1` if Qt path changed
- **Generic icon on shortcuts** — Make sure `resources\icons\qgroundcontrol.ico` is your custom icon, then rebuild
- **Generic icon on the exe itself** — The RC file (`deploy\windows\WindowsQGC.ico`) must be your custom icon, and you must rebuild in Qt Creator
