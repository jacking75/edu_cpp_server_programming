; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{698C5042-5FBA-4FF0-A287-2FEDDE04290B}
AppName=http client
AppVerName=http client
AppPublisher=zsxxsz
AppPublisherURL=http://www.sourceforge.net/projects/acl/
AppSupportURL=http://www.sourceforge.net/projects/acl/
AppUpdatesURL=http://www.sourceforge.net/projects/acl/
DefaultDirName={pf}\Acl Project\http client
DefaultGroupName=Acl Project
OutputDir=.\
OutputBaseFilename=http client
SetupIconFile=..\res\http_client.ico
Compression=lzma
SolidCompression=yes

[Languages]
Name: "chinese"; MessagesFile: "compiler:Languages\ChineseSimplified.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "..\ReleaseDll\http_client.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\ReleaseDll\lib_protocol.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\ReleaseDll\lib_acl.dll"; DestDir: "{app}"; Flags: ignoreversion
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{group}\http client"; Filename: "{app}\http_client.exe"
Name: "{group}\{cm:ProgramOnTheWeb,http client}"; Filename: "http://www.sourceforge.net/projects/acl/"
Name: "{group}\{cm:UninstallProgram,http client}"; Filename: "{uninstallexe}"
Name: "{commondesktop}\http client"; Filename: "{app}\http_client.exe"; Tasks: desktopicon
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\http client"; Filename: "{app}\http_client.exe"; Tasks: quicklaunchicon

[Run]
Filename: "{app}\http_client.exe"; Description: "{cm:LaunchProgram,http client}"; Flags: nowait postinstall skipifsilent

