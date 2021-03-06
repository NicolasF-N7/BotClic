; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define MyAppName "BotClic"
#define MyAppVersion "1.0.0"
#define MyAppPublisher "Nico Compagny"
#define MyAppURL "http://wwwComingSoon.com/"
#define MyAppExeName "BotClic.exe"

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{34F953A0-634E-429B-957A-FBA2BF6109A6}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={pf32}\{#MyAppName}
DefaultGroupName={#MyAppName}
AllowNoIcons=yes
LicenseFile=C:\Program Files\Programmes\WinAPI\BotClic\License.txt
OutputDir=C:\Program Files\Programmes\Setup
OutputBaseFilename=BotClic Setup
SetupIconFile=C:\Program Files\Programmes\WinAPI\BotClic\SourisIcoSetupFile.ico
UninstallDisplayIcon=C:\Program Files\Programmes\WinAPI\BotClic Save\SourisIcoSetupFile.ico
Compression=lzma
SolidCompression=yes
ChangesAssociations=yes

[Languages]
Name: "french"; MessagesFile: "compiler:Languages\French.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked; OnlyBelowVersion: 0,6.1

[Dirs]
Name: "{app}\Sequences"; Permissions: users-modify
Name: "{app}\src"; Permissions: users-modify; 

[Files]
Source: "C:\Program Files\Programmes\WinAPI\BotClic\BotClic.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Program Files\Programmes\WinAPI\BotClic\src\Config.BCCfg"; DestDir: "{app}\src"; Flags: ignoreversion
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{commondesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: quicklaunchicon

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent

[UninstallDelete]
Type: filesandordirs; Name: "{app}\Sequences"
Type: filesandordirs; Name: "{app}\src"
Type: filesandordirs; Name: "{app}"

[Registry]
Root: HKCR; Subkey: .BCSeq; ValueType: string; ValueName: ""; ValueData: "BotClicSequence"; Flags: uninsdeletekey 
Root: HKCR; Subkey: BotClicSequence; ValueType: string; ValueName: ""; ValueData: "S?quence BotClic"; Flags: uninsdeletekey
Root: HKCR; Subkey: BotClicSequence\DefaultIcon; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},-3"; Flags: uninsdeletekey
Root: HKCR; Subkey: BotClicSequence\shell; Flags: uninsdeletevalue
Root: HKCR; Subkey: BotClicSequence\shell\open; Flags: uninsdeletevalue
Root: HKCR; Subkey: BotClicSequence\shell\open\command; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1""";  Flags: uninsdeletekey