//定义一些常量
//程序版本号
#define MyAppVersion "1.1.0.0001"
//可执行程序名
#define MyAppExeName "demo.exe"
//程序图标
#define MyAppIcon "logo.ico"
//程序uuid
#define MyAppUid "FCA18CA6-1C3D-4C6E-A8AF-823A34619B21"

[Setup]
//[Setup]-该区段包含用于安装程序和卸载程序的全局设置
//每个程序单独一个id，这是会作为注册表key的，别混用
AppId={{{#MyAppUid}}
//程序安装后的名字
AppName={cm:CmAppName}
//程序版本号
AppVersion={#MyAppVersion}
//指定安装程序版本信息的文件描述值
VersionInfoDescription={cm:CmAppName} Setup
//指定安装程序版本信息产品名称值
VersionInfoProductName={cm:CmAppName} 
//不显示选择开始菜单文件夹 向导页面
DisableProgramGroupPage=yes
//默认安装目录
DefaultDirName=D:\Demo
//安装包生成后所在文件夹和文件名
OutputDir=install
OutputBaseFilename=我的安装程序
//指定安装和卸载程序图标
SetupIconFile={#MyAppIcon}
//控制面板卸载图标
UninstallDisplayIcon={#MyAppIcon}
//语言选择框
ShowLanguageDialog=yes
//根据系统语言初始化显示的语言，默认uilanguage 
LanguageDetectionMethod=uilanguage
//默认Innosetup 会根据机器语言能否支持显示语言包
//新版本已废弃
//ShowUndisplayableLanguages=yes

[Languages]
//[Languages]-定义安装程序中可使用的语言
//破解版的Default一般是中文，如果是用的原版默认不带中文，需要单独下载
Name: "zh"; MessagesFile: "compiler:Default.isl"
Name: "en"; MessagesFile: "compiler:English.isl"

[CustomMessages]
//[CustomMessages]- 用于定义 {cm:...} 常量的自定义消息值
//也可以写到isl文件的CustomMessages字段
zh.CmAppName=我的例程
zh.CmMessage=测试消息框文本及参数，%1 ！

en.CmAppName=My Demo
en.CmMessage=Test message box text and parameters, %1 !

[Tasks]
//[Tasks]-定义安装程序在执行安装期间所有由用户定制的任务。这些任务以选项框和单选项形式在附加任务向导页面中出现。
//创建桌面图标
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: checkablealone

[Files]
//[Files]-这是定义安装程序安装文件到用户系统中的可选文件区段
Source: "bin\{#MyAppExeName}"; DestDir: "{app}";  Flags: ignoreversion
Source: "bin\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
//注意: 不要在任何共享系统文件上使用“Flags: ignoreversion”

[Icons]
//[Icons]-定义所有创建在开始菜单和/或其它位置(比如桌面)的快捷方式
//没有菜单分组
Name: "{autoprograms}\{cm:CmAppName}"; Filename: "{app}\{#MyAppExeName}"; IconFilename:"{app}\{#MyAppIcon}"
Name: "{autodesktop}\{cm:CmAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon; IconFilename:"{app}\{#MyAppIcon}"

[Code]
//[Code]-指定 Pascal 脚本的可选区段
//准备安装，测试MsgBox提示框内容翻译
function InitializeSetup(): Boolean;  
begin     
  MsgBox(ExpandConstant('{cm:CmMessage,{cm:CmAppName}}'), mbError, MB_OK); 
  result:=true;
end;

//在实际安装开始之前用 CurStep=ssInstall 调用，
//或在实际安装完成之后用 CurStep=ssPostInstall 调用，
//或在安装程序终止之前和安装完成之后用 CurStep=ssDone 调用。
procedure CurStepChanged(CurStep: TSetupStep);
var
languageName: string;
begin
  //软件安装后执行
  if CurStep=ssPostInstall  then    
  begin
    //获取用户选择的语言，对应Languages字段的Name
    languageName := ActiveLanguage();  
    //if 'zh' = languageName then
    //begin
    //  SetIniString('Setting','Language','zh',ExpandConstant('{app}/config.ini'));
    //end
    //else if 'en' = languageName then
    //begin
    //  SetIniString('Setting','Language','en',ExpandConstant('{app}/config.ini'));
    //end;
    //可以分别处理，或者直接写入配置文件（生成的配置文件，默认的卸载过程是不会删除的）
    //写ini函数原型：参数分别对应字段、key、Value、配置文件
    //function SetIniString(const Section, Key, Value, Filename: String): Boolean;
    SetIniString('Setting','Language',languageName,ExpandConstant('{app}/config.ini'));
  end;
end;

