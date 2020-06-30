import sys
from PyQt5.QtWidgets import *
from PyQt5.QtGui import *
from PyQt5.QtCore import *
#from PyQt5.QtCore import QObject,pyqtProperty

#使用一个背景颜色动画来展示Qt属性
class MyLabel(QLabel):
    def __init__(self):
        super().__init__()
        self.color=QColor(255,255,255)

        #颜色动画-开始
        self.anim_color=QPropertyAnimation(self)
        self.anim_color.setTargetObject(self)
        self.anim_color.setPropertyName(b'mycolor') #bytearray类型
        self.anim_color.setStartValue(QColor(255,255,255))
        self.anim_color.setEndValue(QColor(0,255,0))
        self.anim_color.setDuration(2000) #2s=2000ms  
        #尺寸+位置动画-开始  geometry为QWidget属性
        self.anim_rect=QPropertyAnimation(self,b'geometry',parent=self)
        #根据屏幕大小计算中间位置
        the_screen_center=self.screen().geometry().center()
        the_min_rect=QRect(0,0,200,200)
        the_min_rect.moveCenter(the_screen_center)
        the_max_rect=QRect(0,0,400,400)
        the_max_rect.moveCenter(the_screen_center)
        self.anim_rect.setStartValue(the_min_rect)
        self.anim_rect.setEndValue(the_max_rect)
        self.anim_rect.setDuration(2000) #2s=2000ms  

        #并行动画组
        self.anim_parallel=QParallelAnimationGroup(self)
        self.anim_parallel.addAnimation(self.anim_color)
        self.anim_parallel.addAnimation(self.anim_rect)

        #颜色动画-恢复
        self.anim_color2=QPropertyAnimation(self)
        self.anim_color2.setTargetObject(self)
        self.anim_color2.setPropertyName(b'mycolor') #bytearray类型
        self.anim_color2.setStartValue(QColor(0,255,0))
        self.anim_color2.setEndValue(QColor(255,255,255))
        self.anim_color2.setDuration(2000) #2s=2000ms  
        #尺寸+位置动画-恢复  
        self.anim_rect2=QPropertyAnimation(self,b'geometry',parent=self)
        self.anim_rect2.setStartValue(the_max_rect)
        self.anim_rect2.setEndValue(the_min_rect)
        self.anim_rect2.setDuration(2000) #2s=2000ms  

        #并行动画组
        self.anim_sequential=QSequentialAnimationGroup(self)
        self.anim_sequential.addAnimation(self.anim_rect2)
        self.anim_sequential.addAnimation(self.anim_color2)

        #串联两个动画组-循环播放
        self.anim_group=QSequentialAnimationGroup(self)
        self.anim_group.addAnimation(self.anim_parallel)
        self.anim_group.addAnimation(self.anim_sequential)
        self.anim_group.setLoopCount(-1) #循环播放
        self.anim_group.start()

    def paintEvent(self,event):
        super().paintEvent(event)
        painter=QPainter(self)
        painter.fillRect(self.rect(),self.color)

    #函数名是任意的，只是作为回调
    def get_mycolor(self):
        return self.color

    def set_mycolor(self,value):
        self.color=value
        self.update()

    #添加属性，注意放的位置
    mycolor=pyqtProperty(QColor,fget=get_mycolor,fset=set_mycolor)

if __name__ == "__main__":
    app=QApplication(sys.argv)

    w=MyLabel()
    w.setWindowTitle("龚建波 1992")
    w.show()
   
    sys.exit(app.exec_())