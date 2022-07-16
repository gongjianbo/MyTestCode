#using flask 2.0.1
import os,sys
from flask import Flask,request,jsonify,send_file,send_from_directory

app = Flask(__name__)
filename_temp = ''
BASE_PATH=os.path.join(os.path.dirname(os.path.abspath(__file__)),'upload')

#测试
@app.route('/',methods=['GET','POST'])
def hello():
    return '<p>Hello!</p>'

#上传
@app.route('/upload',methods=['POST'])
def upload_file():
    try:
        global filename_temp
        f = request.files['myfile']
        filename_temp = f.filename
        print('upload file:'+f.filename)
        f.save(os.path.join(BASE_PATH,f.filename))
        return jsonify({
            'filename':f.filename,
            'fileid':0 #假装对每个文件返回一个id，然后通过id再下载
        })
    except Exception as e:
        print('error:'+str(e))
        return jsonify({'error':0}),0

#下载
@app.route('/download/<fileid>',methods=['GET'])
def download_file(fileid):
    try:
        global filename_temp
        print('download file:'+filename_temp) #假装是通过id从数据库拿到的文件
        return send_from_directory(BASE_PATH,filename_temp,as_attachment=True)
    except Exception as e:
        print('error:'+str(e))
        return jsonify({'error':0}),0 

if __name__ == '__main__':
    print('server runing... ...')
    if not os.path.exists(BASE_PATH): 
        os.makedirs(BASE_PATH) 
    app.run(host='127.0.0.1',port=12345,debug=True)