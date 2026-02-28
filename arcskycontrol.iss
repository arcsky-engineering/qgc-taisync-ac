[Setup]
AppName=Arcsky Control
AppVersion=1.0
AppPublisher=Arcsky
DefaultDirName={autopf}\Arcsky Control
DefaultGroupName=Arcsky
OutputBaseFilename=ArcskyControl_Setup
Compression=lzma2
SolidCompression=yes
OutputDir=.
SetupIconFile=C:\q\qgroundcontrol\resources\icons\qgroundcontrol.ico
UninstallDisplayIcon={app}\ArcskyControl.exe
WizardStyle=modern
MinVersion=10.0

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"

[Files]
Source: "C:\QGC_Deploy\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "C:\q\qgroundcontrol\resources\icons\qgroundcontrol.ico"; DestDir: "{app}"; DestName: "arcskycontrol.ico"; Flags: ignoreversion

[Icons]
Name: "{group}\Arcsky Control"; Filename: "{app}\ArcskyControl.exe"; IconFilename: "{app}\arcskycontrol.ico"
Name: "{commondesktop}\Arcsky Control"; Filename: "{app}\ArcskyControl.exe"; IconFilename: "{app}\arcskycontrol.ico"; Tasks: desktopicon
Name: "{group}\Uninstall Arcsky Control"; Filename: "{uninstallexe}"

[Run]
Filename: "{app}\ArcskyControl.exe"; Description: "{cm:LaunchProgram,Arcsky Control}"; Flags: nowait postinstall skipifsilent