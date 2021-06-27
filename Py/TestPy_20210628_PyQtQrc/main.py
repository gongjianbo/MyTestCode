import sys
# 引入资源描述文件编译出来的二进制文件
import src_rc
from PyQt5.QtCore import Qt
from PyQt5.QtGui import QPixmap
from PyQt5.QtWidgets import QApplication,QLabel
 
if __name__=="__main__":
    app=QApplication(sys.argv)
 
    label=QLabel()
    # 引用资源中的文件，以冒号开始
    # 这个多出来的new是我们在qrc中设置的prefix路径前缀
    # label.setPixmap(QPixmap(":/new/imgA.jpg"))
    label.setPixmap(QPixmap(":/new/folder/imgB.jpg"))
    label.setWindowTitle("龚建波1992")
    # 图片居中
    label.setAlignment(Qt.AlignCenter)
    label.resize(400,350)
    label.show()
 
    sys.exit(app.exec_())