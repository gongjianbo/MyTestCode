from flask import Flask, jsonify, request

app = Flask(__name__)

#测试 get
@app.route('/text', methods=['GET'])
def testGetText():
    return 'Get '+request.args.get('arg')

#测试 post
@app.route('/text', methods=['POST'])
def testPostText():
    return 'Post '+request.form['arg']

#测试传 json
@app.route('/json', methods=['POST'])
def testModel():
    #return jsonify(['red', 'green', 'blue'])
    #request.json 只能够接受方法为POST、Body为raw，header 内容为 application/json类型的数据
    return jsonify(request.json)

if __name__ == '__main__':
    app.run(host='127.0.0.1', port=54321, debug=True)
