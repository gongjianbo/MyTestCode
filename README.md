# MyTestCode

![](https://img.shields.io/badge/language-Cpp/QML-orange) ![](https://img.shields.io/badge/license-Free-green)

![](https://img.shields.io/github/languages/code-size/gongjianbo/MyTestCode) ![](https://img.shields.io/github/stars/gongjianbo/MyTestCode) ![](https://img.shields.io/github/forks/gongjianbo/MyTestCode)

Some test code, as example code of my blog, mainly Qt related demo. 一些测试程序，主要作为我博客的示例代码，主要是Qt相关的Demo。

CSDN Blog（博客地址）：https://blog.csdn.net/gongjianbo1992

Github：https://github.com/gongjianbo/MyTestCode

Gitee（码云地址）: https://gitee.com/gongjianbo/MyTestCode

# 目录

- [说明](#说明)
- [Qt Demo](#Qt)
- [QML Demo](#QML)
- [Cpp Demo](#Cpp)
- [Python Demo](#Python)
- [其他](#其他)

# 说明

- 目前主要使用 Win10 / Win11 + MSVC 2019 / 2022 + Qt 5.15.2 进行开发。一些老的代码可能使用 Qt 5.12 或者更早的版本；对于 Qt 6.x 的代码后续会拆分开来，目前是命名中标记上 Qt6
- 部分链接不是本仓库的内容，只是为了补全完整性，所以放到了一起。也有部分代码没有整理到列表中
- 欢迎反馈问题

# Qt 

- QPainter 各种绘图操作：https://github.com/gongjianbo/EasyQPainter
- Qt 示例学习：Cube OpenGL ES 2.0 [[Code](./Qt/QtExampleCube/)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/122945293)]
- 绘制饼图 [[Code](./Qt/QtPainter/)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/100096216)]
- 绘制热力图 [[Code](./Qt/MyHeatMap/)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/104566768)]
- 绘制滚动文本 [[Code](./Qt/ScrollLabel/)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/106579690)]
- 绘制轮播图 [[Code](./Qt/TestQt_20211029_Swiper/)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/121072373)]
- 模仿 python turtle 海龟绘图 [[Code](https://github.com/gongjianbo/QtTurtle)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/112111264)]
- 自定义 QComboBox 下拉框，添加删除按钮 [[Code](./Qt/MyComboBox/)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/106558887)]
- 自定义 QTabWidget，实现拖入拖出 [[Code](./Qt/MyTabWidget/)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/108502299)]
- QTableView 拖拽交换行、列、单元格 [[Code](./Qt/QTableViewMoveAction/)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/106932706)]
- QHeaderView 表头排序 [[Code](./Qt/SortHeaderView/)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/108249292)]
- 路径渐变填充 [[Code](./Qt/PathGradient/)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/123767575)]
- QTreeView 展示和编辑 JSON [[Code](./Qt/QJsonAndTreeView/)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/103191023)]
- QTextEdit 几种高亮设置 [[Code](./Qt/TextEditHighlight/)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/122527076)]
- QDataWidgetMapper，数据 Model 与组件属性绑定 [[Code](./Qt/TestQt_20200615_DataMapper/)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/106772319)]
- UI 线程与子线程交互 [[Code](./Qt/UiAndSubThread/)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/105171077)]
- QCharts 练习 [[Code](./Qt/TestQt_20190907_QChartsSeries/)]
- QCharts 添加背景图片 [[Code](./Qt/TestQt_20190806_QChartsBackground/)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/98615059)]
- QFuture 练习 [[Code](./Qt/TestQt_20200622_QFuture/)] 
- QFuture 多线程更新进度条 [[Code](./Qt/TestQt_20200625_QFuture/)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/106957888)]
- Qt HTTP 客户端操作 [[Code](./Qt/Qt5HttpDemo/)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/97568863)]
- Qt HTTP 文件上传下载 [[Code](./Qt/TestQt_20210807_HttpFile/)] [[Blog](https://gongjianbo1992.blog.csdn.net/article/details/119490369)]
- Qt Remote Objects 远程调用 [[Code](./Qt/QtRemoteObjects/)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/120103289)]
- Qt LocalSocket 练习 [[Code](./Qt/TestLocalSocket/)] 
- Qt WebSocket 练习 [[Code](./Qt/QtWebSocketDemo/)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/107306121)]
- Qt WebChannel 学习 [[Code](https://github.com/gongjianbo/QtWebCannelAndMap.git)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/89707251)]
- 重定向 QDebug，封装日志类 [[Code](https://github.com/gongjianbo/SimpleQtLogger)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/108030391)]
- PIMPL 练习 [[Code](./Qt/QtPimpl/)]
- 隐式共享练习 [[Code](./Qt/TestQt_20211225_SharedData/)]
- 事件流程练习 [[Code](./Qt/TestQt_20220915_EventFlow/)]
- 自定义事件 [[Code](./Qt/TestQt_20230710_CustomEvent/)]
- Qt DPI 缩放测试 [[Code](./Qt/TestQt_20230529_DpiScaleTest/)]
- Qt Widgets 换肤 [[Code](./Qt/QtSwitchSkin/)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/106608353)]
- QtAV 库的使用 [[Code](./Qt/QtAV/)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/94631123)]
- QScintilla 文本编辑控件 [[Code](./Qt/QScintilla/)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/100828180)]
- libqrencode 生成二维码 [[Code](./Qt/QtQRencodeVS2019/)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/122422331)]
- QZXing 生成二维码 [[Code](./Qt/QtQZXingVS2019/)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/122463817)]
- USB 热插拔检测（Windows） [[Code](./Qt/DeviceHotplug_Win/)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/128702423)]
- USB 热插拔检测（MacOS） [[Code](./Qt/DeviceHotplug_Mac/)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/129780213)]
- axcontainer 写 doc 和 xls [[Code](./Qt/TestQt_20211012_DocXls/)]
- 自定义 ini 读写类 [[Code](./Qt/TestQt_20210425_ini/)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/116239087)]
- Assimp 加载 3D 模型 [[Code](./Qt/TestQt_20211018_Assimp/)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/120857151)]
- FFmpeg 获取音频信息 [[Code](./Qt/GetAudioInfo/)] [[Blog-1](https://blog.csdn.net/gongjianbo1992/article/details/109832829)] [[Blog-2](https://blog.csdn.net/gongjianbo1992/article/details/109975687)]
- giflib 库学习 [[Code](./Qt/GifLib%E6%B5%8B%E8%AF%95%E4%BB%A3%E7%A0%81/)] 
- giflib 读写 gif 文件 [[Code](https://github.com/gongjianbo/QtGifTool)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/115449858)]
- libsndfile 读写音频数据 [[Code](./Qt/libsndfile/)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/99678466)]
- silk 音频转 wav [[Code](./Qt/SilkToWav/)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/110351329)]
- Win32 多进程 [[Code](./Qt/MultiProcess_Win/)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/120030085)]
- Win32 Dump [[Code](./Qt/TestQt_20210211_Dump/)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/113791423)]

# QML 

- QML 组件样式自定义：https://github.com/gongjianbo/QmlComponentStyle
- Qt 示例学习：OpenGL Under QML [[Code](./Qml/QtExampleOpenGLUnderQML/)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/124769013)]
- QML 调用 Cpp [[Code](./Qml/QmlCallCpp2020/)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/87965925)]
- Cpp 调用 QML [[Code](./Qml/CppCallQml2020/)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/87965925)]
- QML 与 Cpp 之间的函数回调 [[Code](./Qml/TestQml_20220908_Callback/)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/126769501)]
- 动态创建 QML 对象 [[Code](./Qml/TestQml_20201118_createComponent/)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/109800614)]
- QML 中使用 QtWidgets 模块的文件对话框 [[Code](./Qml/FileDialogWrap/)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/128521716)]
- QML 快捷键 [[Code](./Qml/TestQml_20210517_ShortcutKey/)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/116954979)]
- eventFilter 过滤快捷键 [[Code](./Qml/KeysFilter/)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/116954979)]
- Qt Quick Scene Graph 学习1：画线 [[Code](./Qml/LearnQSG_20210614_Line/)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/117923933)]
- Qt Quick Scene Graph 学习2：纹理 [[Code](./Qml/LearnQSG_20210624_Texture/)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/118257844)]
- QT_OPENGL_BUGLIST OpenGL 黑名单 [[Code](./Qml/OpenGLBlackList/)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/125400723)]
- QML 中使用 AJAX [[Code](./Qml/QmlAjax/)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/108923476)]
- Animation 动画练习 [[Code](./Qml/QmlAnimation/)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/102135779)]
- QML 实现程序退出确认功能 [[Code](./Qml/QmlCloseEvent/)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/108270551)]
- QML 实现输入框 Completer 自动补全 [[Code](./Qml/QmlCompleter/)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/122569547)]
- QML 实现分页组件 [[Code](./Qml/QmlPagination/)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/111820528)]
- QML 实现一个 IP 地址输入框 [[Code](./Qml/TestQml_20210717_IpInput/)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/118860100)]
- QML 用 PathView 实现轮播图 [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/103517663)]
- QML 自定义 Loading 等待样式（一） [[Code](https://github.com/gongjianbo/QmlComponentStyle)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/112748866)]
- QML 自定义 Loading 等待样式（二） [[Code](https://github.com/gongjianbo/QmlComponentStyle)] [[Blog](https://gongjianbo1992.blog.csdn.net/article/details/122824084)]
- QML 实现异形按钮（不规则点击区域） [[Blog](https://gongjianbo1992.blog.csdn.net/article/details/123002716)]
- 简易的无边框 [[Code](./Qml/QmlFramelessWindow/)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/109708951)]
- 简易的登录界面 [[Code](./Qml/QmlLoginPage/)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/102753985)]
- QML 换肤设计 [[Code](./Qml/QmlSkin/)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/124080543)]
- QML WebEngine + ECharts 展示实时曲线 [[Code](./Qml/QmlWebEngineECharts/)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/108778548)]
- QML 做一个聊天框 [[Code](./Qml/TalkList/)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/115410423)]
- QML 波纹效果 [[Code](./Qml/TestQml_20191128_Wave/)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/117639209)]
- QML Canvas 绘制波浪进度球 [[Code](./Qml/TestQml_20210310_Wave/)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/114684010)]
- QML Canvas 绘制进度条（环形和波浪水球） [[Code](./Qml/TestQml_20220210_ProgressBar/)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/122870986)]
- QML Shapes 模块中渐变的使用简例 [[Code](./Qml/TestQml_20220412_ShapeGradient/)] [[Blog](https://gongjianbo1992.blog.csdn.net/article/details/124137788)]
- Qt Quick OpenGL 学习笔记：通过 FBO 帧缓冲绘制一个三角 [[Code](./Qml/TestQml_20200128_FBO/)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/104107692)]
- QML 粒子系统制作心形效果 [[Code](./Qml/TestQml_20200521_Love/)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/106256286)]
- Qt 多语言翻译（国际化）[[Code](./Qml/TestQml_20211215_Translator/)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/122014168)]
- PathView 实现环形菜单 [[Code](./Qml/TestQml_20220313_PathView/)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/123465756)]
- QML 实现图片拖入拖出、剪贴板复制粘贴 [[Code](./Qml/TestQml_20221125_CopyPaste/)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/128058766)]
- Control1 TreeView 选中效果 [[Code](./Qml/TestQml_20221120_TreeSelection/)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/127956454)]
- 练习，多个 View 共享数据 [[Code](./Qml/TestQml_20220709_CommonData/)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/125712442)]
- QML Image 通过 QQuickImageProvider 加载图片 [[Code](./Qml/TestQml_20221225_ImageProvider/)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/128439262)]
- FPS 统计 [[Code](./Qml/TestQml_20230211_QmlFps/)]
- 制作 QML 扩展插件 [[Code](https://github.com/gongjianbo/QmlExtensionPlugin)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/104079885)]
- 嵌套 ListView 实现 TreeView [[Code](https://github.com/gongjianbo/QmlTreeView)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/90575519)]
- 封装 QPainter 在 QML 中使用 [[Code](https://github.com/gongjianbo/QmlPainter)] 
- Qt6 QML TreeView 基本使用 [[Code](./Qml/TestQml_20220422_Qt6TreeView/)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/124373674)]
- Qt6 QML 日历样式自定义 [[Code](./Qml/TestQml_20220416_Qt6Calendar/)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/124212915)]

# Cpp 

- Winsock UDP 练习 [[Code](./Cpp/TestWinsock/)] 
- spdlog 学习 [[Code](./Cpp/spdlog/)] [[Blog-1](https://blog.csdn.net/gongjianbo1992/article/details/113279632)] [[Blog-2](https://blog.csdn.net/gongjianbo1992/article/details/112797764)]
- TinyXML2 学习 [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/107947647)]

# Python 

- ctypes 基本使用 [[Code](./Py/TestPy_20210617_ctypes/)] [[Blog](https://gongjianbo1992.blog.csdn.net/article/details/118019271)]
- selenium 练习：华为保修查询 [[Code](./Py/HuaweiQuery/)] [[Blog](https://gongjianbo1992.blog.csdn.net/article/details/119878883)]
- uiautomation UI 自动化测试 [[Code](./Py/UIAutoTest/)] [[Blog](https://gongjianbo1992.blog.csdn.net/article/details/121895644)]
- PyQt5 资源文件的使用 [[Code](./Py/TestPy_20210628_PyQtQrc/)] [[Blog](https://gongjianbo1992.blog.csdn.net/article/details/105361880)]
- PyQt5 属性动画 [[Code](./Py/PyQt%E5%B1%9E%E6%80%A7%E5%8A%A8%E7%94%BB/)] [[Blog](https://gongjianbo1992.blog.csdn.net/article/details/107052036)]

# 其他 

- Inno Setup 保留原安装路径 [[Code](./Other/InnoSetup%E4%BF%9D%E7%95%99%E5%8E%9F%E5%AE%89%E8%A3%85%E8%B7%AF%E5%BE%84.iss)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/120211915)]
- Inno Setup 语言选择 [[Code](./Other/InnoSetup%E8%AF%AD%E8%A8%80%E9%80%89%E6%8B%A9.iss)] [[Blog](https://blog.csdn.net/gongjianbo1992/article/details/122119957)]
