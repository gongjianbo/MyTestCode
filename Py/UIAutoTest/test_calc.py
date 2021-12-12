import subprocess
import uiautomation as uia

# 设置全局搜索超时 15s
uia.uiautomation.SetGlobalSearchTimeout(15)

# 测试Win10计算器
def test_calc():
    # 启动计算器进程
    subprocess.Popen(args='calc.exe')
    # 首先从桌面的第一层子控件中找到程序的窗口，再从这个窗口查找子控件
    window = uia.WindowControl(
        searchDepth=1, ClassName='ApplicationFrameWindow', Name='计算器')
    # 窗口置顶
    window.SetTopmost(True)
    # 根据文档描述，直接从顶层找空间的效率较低，因为每次找都需要去遍历
    # 所以这里可以根据inspect查看到的对象树来逐层找
    # Name/ClassName/AutomationId都是UIAutomation属性，可以通过工具查看到，类似于网页的选择器
    window_main = window.Control(
        searchDepth=1, ClassName='Windows.UI.Core.CoreWindow')
    button_group = window_main.Control(
        searchDepth=1, ClassName='LandmarkTarget')
    number_group = button_group.Control(searchDepth=1, Name='数字键盘')
    calc_group = button_group.Control(searchDepth=1, Name='标准运算符')

    # 模拟按键
    window.SendKeys('1')
    #number_group.ButtonControl(Name='一').Click()
    calc_group.ButtonControl(Name='加').Click()
    number_group.ButtonControl(Name='四').Click()
    calc_group.ButtonControl(Name='等于').Click()
    result = button_group.Control(AutomationId='NormalOutput').Name
    print("测试结果：", result, ' bytes:', bytes(result, encoding='utf8').hex())
    # 获取到的文本前后有不可见字符，这里先替换掉
    result = result.replace(b'\xe2\x80\xad'.decode('utf8'), '').replace(
        b'\xe2\x80\xac'.decode('utf8'), '')
    print("测试结束，预期=5，实际={0}，{1}".format(
        result, "测试通过" if result == "5" else "测试失败"))
    # 通过复制粘贴获取，这样得到的文本没有其他字符
    window.SendKeys('{Ctrl}c', waitTime=1)
    result = uia.GetClipboardText()
    print("测试结果：", result, ' bytes:', bytes(result, encoding='utf8').hex())
    # 截图并保存到指定路径
    window.CaptureToImage('C:/Users/1992/Pictures/1.png')
    # 关闭程序
    #window.ButtonControl(Name='关闭').Click()
    window.GetWindowPattern().Close()

if __name__ == '__main__':
    test_calc()

