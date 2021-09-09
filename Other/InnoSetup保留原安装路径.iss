;有关创建 Inno Setup 脚本文件的详细资料请查阅帮助文档！
//分号注释会在预处理器输出中，斜杠注释不会
//先定义一些常量
//程序名
#define MyAppName "程序名"
//程序图标路径，相对于iss脚本的相对路径
#define MyAppIcon "logo.ico"
//程序版本号
#define MyAppVersion "1.0.0"
// 发行者
#define MyAppPublisher "公司"
//网址
#define MyAppURL "https://www.gongsi.com/"
//可执行程序名
#define MyAppExeName "app.exe"
//待打包的文件路径，相对于iss脚本的相对路径
#define MyAppExePath "client\bin"
//程序默认安装目录
#define MyAppInstallPath "D:\folder\sub"
//程序uuid，点工具生成唯一的GUID
#define MyUid "0000-0000-0000-0000-0000"

[Setup]
//[Setup]-该区段包含用于安装程序和卸载程序的全局设置
;注: AppId的值为单独标识该应用程序。
; 不要为其他安装程序使用相同的AppId值。
; (若要生成新的 GUID，可在菜单中点击 "工具|生成 GUID"。)
//每个程序单独一个id，这是会作为注册表key的，别混用
AppId={{{#MyUid}}
//程序安装后的名字
AppName={#MyAppName}
//程序版本号
AppVersion={#MyAppVersion}
//AppVerName={#MyAppName} {#MyAppVersion}
//默认安装目录
DefaultDirName={#MyAppInstallPath}
//使用已安装版本的目录安装，为yes则默认选择已有的目录且不能选择
UsePreviousAppDir=no
//不显示选择开始菜单文件夹 向导页面
DisableProgramGroupPage=yes
//以管理权限运行安装
PrivilegesRequired=admin
//安装包生成后所在文件夹和文件名
OutputDir=install\{#MyAppVersion}
OutputBaseFilename={#MyAppName} V{#MyAppVersion}
//选择压缩方法，lzma为7z
Compression=lzma
//启用固态压缩，详见文档
SolidCompression=yes
//安装和卸载程序现代外观
//WizardStyle=modern
//指定安装和卸载程序图标
SetupIconFile={#MyAppIcon}
//控制面板卸载图标
UninstallDisplayIcon={#MyAppIcon}
//开始菜单分组，see[Icons]区段
//DefaultGroupName={#MyAppPublisher}\by
//控制面板-添加/删除页面中的程序相关信息
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}

[Languages]
//[Languages]-定义安装程序中可使用的语言
Name: "chinesesimp"; MessagesFile: "compiler:Default.isl"

[Tasks]
//[Tasks]-定义安装程序在执行安装期间所有由用户定制的任务。这些任务以选项框和单选项形式在附加任务向导页面中出现。
//创建桌面图标
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: checkablealone

[Files]
//[Files]-这是定义安装程序安装文件到用户系统中的可选文件区段
Source: "{#MyAppExePath}\{#MyAppExeName}"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#MyAppExePath}\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
//注意: 不要在任何共享系统文件上使用“Flags: ignoreversion”

[Icons]
//[Icons]-定义所有创建在开始菜单和/或其它位置(比如桌面)的快捷方式
//see[Setup]区段DefaultGroupName设置
//使用菜单分组
//Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; IconFilename:"{app}\{#MyAppIcon}"
//Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"; IconFilename:"{app}\{#MyAppIcon}"
//Name: "{userdesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon; IconFilename:"{app}\{#MyAppIcon}"
//没有菜单分组
Name: "{autoprograms}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; IconFilename:"{app}\{#MyAppIcon}"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon; IconFilename:"{app}\{#MyAppIcon}"

[Run]
// [Run]- 用来指定在程序完成安装后，在安装程序显示最终对话框前要执行的一些程序
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent

[Code]
//[Code]-指定 Pascal 脚本的可选区段
//路径参照两个博客：
//https://blog.csdn.net/c1007857613/article/details/81475167
//https://www.cnblogs.com/csuftzzk/p/innosetup_read_registry.html
//全局变量保存安装路径
var 
  globalInstallPath: String;

//初始化时把路径设置到编辑框
procedure InitializeWizard;
begin
  WizardForm.DirEdit.Text := globalInstallPath;
end;

//检测程序是否运行
function IsAppRunning(const FileName: string): Boolean;
var
  FWMIService: Variant;
  FSWbemLocator: Variant;
  FWbemObjectSet: Variant;
begin
  Result := false;
  FSWbemLocator := CreateOleObject('WBEMScripting.SWBEMLocator');
  FWMIService := FSWbemLocator.ConnectServer('', 'root\CIMV2', '', '');
  FWbemObjectSet := FWMIService.ExecQuery(Format('SELECT Name FROM Win32_Process Where Name="%s"',[FileName]));
  Result := (FWbemObjectSet.Count > 0);
  FWbemObjectSet := Unassigned;
  FWMIService := Unassigned;
  FSWbemLocator := Unassigned;
end;

//获取历史安装路径，Inno Setup保存的一些信息可自己在注册表中查看
//64位会映射到：计算机\HKEY_LOCAL_MACHINE\SOFTWARE\WOW6432Node\Microsoft\Windows\CurrentVersion\Uninstall
function GetInstallString(): String;
var
  InstallPath: String;
begin
  InstallPath := '{#MyAppInstallPath}';
  if RegValueExists(HKLM, 'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{{#MyUid}}_is1', 'Inno Setup: App Path') then
    begin
      RegQueryStringValue(HKLM, 'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{{#MyUid}}_is1', 'Inno Setup: App Path', InstallPath)
    end;
  result := InstallPath;
end;

//准备安装
function InitializeSetup(): Boolean;  
var  
  ResultStr: String;  
  ResultCode: Integer;  
begin     
  globalInstallPath := GetInstallString();
  result := IsAppRunning('{#MyAppExeName}');
  if result then
    begin
      MsgBox('检测到{#MyAppName}正在运行，请先关闭程序后重试! ', mbError, MB_OK); 
      result:=false;
    end
  else if RegQueryStringValue(HKLM, 'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{{#MyUid}}_is1', 'UninstallString', ResultStr) then
    begin  
    if  MsgBox('是否卸载已安装的{#MyAppName}，并保留历史数据？', mbConfirmation, MB_YESNO) = IDYES then
      begin  
        ResultStr := RemoveQuotes(ResultStr);  
        Exec(ResultStr, '/silent', '', SW_HIDE, ewWaitUntilTerminated, ResultCode); 
      end;
    result:=true;
    end
  else
    begin
        result:=true; 
    end;
end;

//准备卸载
function InitializeUninstall(): Boolean;
begin
  result := IsAppRunning('{#MyAppExeName}');
  if result then
    begin
      MsgBox('检测到{#MyAppName}正在运行，请先关闭程序后重试! ', mbError, MB_OK); 
      result:=false;
    end
  else
    begin
      result:=true;
    end
end;