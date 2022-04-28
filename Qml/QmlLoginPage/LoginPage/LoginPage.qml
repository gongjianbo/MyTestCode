import QtQuick 2.12
import QtQuick.Controls 2.12
import QtGraphicalEffects 1.0
//自己注册的模块，见main文件
import Tools 1.0
import DBOperate 1.0

//登录页

//加载完毕之后，如果有账号记录就把最后一次登录账号设置给login_accout
//如果密码保存选项为true，则把密码设置给login_password
//把账号列表设置给账号框使之可选，选择某个账号后把账号密码设置给输入框
Rectangle {
    id: login_page

    color: Qt.rgba(0.9,0.9,0.9)

    //主体结构为Column下包含一个标题和一个表单
    Column{
        anchors.centerIn: parent
        spacing: 40
        //标题
        Text {
            id: login_title
            text: "龚建波 1992"
            anchors{
                horizontalCenter: parent.horizontalCenter
            }

            font{
                bold: true
                pixelSize: 30
                family: "SimHei"
            }
            renderType: Text.QtRendering
        }
        //表单
        Item{
            width: 420
            height: 260
            //DropShadow放在Row里有问题，就拿Item包裹一下
            Rectangle{
                id: login_form
                anchors.fill: parent
                color: "white"
                radius: 6

                Column{
                    id: login_form_row
                    spacing: 24
                    anchors.horizontalCenter: parent.horizontalCenter
                    topPadding: 40

                    LoginField{
                        id: login_account
                        width: 300; height: 36
                        placeholderText: "请输入账号"
                        onFocusChanged: {
                            if(focus){
                                selectAll();
                            }
                        }
                        Image{
                            source: login_userspop.visible
                                    ?"qrc:/up.png"
                                    :"qrc:/down.png"
                            anchors{
                                right: parent.right
                                rightMargin: 4
                                verticalCenter: parent.verticalCenter
                                verticalCenterOffset: 3 //下的图标是顶部对齐的
                            }
                            MouseArea{
                                anchors.fill: parent
                                onClicked: login_userspop.open();
                            }
                        }
                        LoginCompleter{
                            id: login_userspop
                            width: parent.width
                            y: parent.height+2
                            model: login_db.opened?login_db.accounts:[]
                            onSelectText: {
                                fillUserInfo(login_db.searchUserInfo(text));
                            }
                        }
                    }
                    LoginField{
                        id: login_password
                        width: 300; height: 36
                        placeholderText: "请输入密码"
                        property bool passwordVisible: false
                        echoMode: passwordVisible?TextInput.Normal:TextInput.Password
                        Image{
                            source: login_password.passwordVisible
                                    ?"qrc:/visible.png"
                                    :"qrc:/invisible.png"
                            anchors{
                                right: parent.right
                                rightMargin: 4
                                verticalCenter: parent.verticalCenter
                            }
                            MouseArea{
                                anchors.fill: parent
                                onClicked: login_password.passwordVisible=!login_password.passwordVisible;
                            }
                        }
                        Text{
                            id: login_capslock
                            visible: login_keyboard.capslock
                            text:"（大写）"
                            color: "red"
                            anchors{
                                right: login_password.left
                                verticalCenter:login_password.verticalCenter
                                margins: 5
                            }
                        }
                        onFocusChanged: {
                            login_keyboard.checkCapslock();
                            if(focus){
                                selectAll();
                            }
                        }
                        Component.onCompleted: {
                            login_keyboard.attachWatch(login_page);
                        }
                    }
                    Item {
                        id: name
                        width: 300; height: 1
                        LoginCheckBox{
                            id: login_save
                            text: "记住密码"
                            note: "下次免输入"
                            anchors{
                                left: parent.left
                                verticalCenter: parent.verticalCenter
                            }
                        }
                        LoginLabel{
                            id: login_forget
                            text: "找回密码"
                            anchors{
                                right: parent.right
                                verticalCenter: parent.verticalCenter
                            }
                            onClicked: {
                                //跳转到密码找回页面
                            }
                        }
                    }
                    LoginButton{
                        id: login_submit
                        width: 300; height: 40
                        text: "登录"
                        onClicked: {
                            //只有点登录时是save password状态才保存密码
                            if(login_save.checked){
                                //to do save password
                                console.log('save password');
                                login_db.saveUserInfo(login_account.text,login_save.checked,login_password.text);
                            }else{
                                //to do erase password
                                console.log('erase password');
                                login_db.saveUserInfo(login_account.text,false,'none');
                            }
                        }
                    }
                }
            }
            //阴影效果
            DropShadow {
                id : login_shadow
                anchors.fill: login_form
                horizontalOffset: -6
                verticalOffset: 6
                radius: 12
                samples: 18
                color: "gray"
                source: login_form
            }
        }
    }

    Text {
        id: login_version
        text: "当前版本：V0.0.0"
        anchors{
            right: parent.right
            bottom: parent.bottom
            margins: 50
        }
        font{
            pixelSize: 14
            family: "SimHei"
        }
    }

    KeyboardUtil{
        //此处用于检测大小写
        id: login_keyboard
    }

    LoginDBOperate{
        //此处用于账号密码保存
        id: login_db
        opened: login_page.visible
    }

    Component.onCompleted: {
        fillUserInfo(login_db.lastUserInfo());
    }

    //此函数用于填充用户信息
    function fillUserInfo(info){
        login_account.text=info.account;
        if(info.savepassword){
            login_save.checked=true;
            login_password.text=info.password;
        }else{
            login_save.checked=false;
            login_password.text="";
        }
    }
}
