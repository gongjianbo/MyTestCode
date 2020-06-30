import urllib.request
import threading
import datetime
import random
#import lxml.etree
from pyquery import PyQuery as pq

#爬代理https://blog.csdn.net/follow_sunshine/article/details/82501550
#代理没啥效果，主要是时间间隔
'''
proxy_list = [
    {'http': 'http://1.197.204.138:9999'},
    {'http': 'http://42.238.91.46:9999'},
    {'http': 'http://1.197.203.40:9999'},
    {'http': 'http://222.223.182.66:8000'},
    {'http': 'http://183.185.2.23:9797'},
    {'http': 'http://222.223.182.66:8000'},
    {'http': 'http://113.121.23.159:9999'},
    {'http': 'http://182.34.36.170:9999'},
    {'http': 'http://115.53.18.236:9999'},
    {'http': 'http://120.83.107.148:9999'}
]
'''

#谷歌火狐edge
user_agent_list = [
    'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/67.0.3396.99 Safari/537.36',
    'Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:61.0) Gecko/20100101 Firefox/61.0',
    'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.36 Edge/16.16299'
]


url_list=[]

def get_url_func(pagemax,baseurl):
    for index in range(1,pagemax+1):
        response=pq(url=baseurl+'/article/list/'+str(index)+'?')
        items_tag=response('.article-item-box.csdn-tracking-statistics h4 a').items()
        for item in items_tag:
            url=item.attr.href
            #print(url)
            if baseurl in url:
                title=item.text()
                #print(item.text())
                url_list.append((url,title))


#循环访问制定页面
def timer_func():
    global work_count
    work_count += 1
    print('\n', work_count, ' begin-------------------------------------------')
    print(datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S'))

    header = {
        'User-Agent': random.choice(user_agent_list)
    }
    #测试发现代理没啥效果
    #proxy_item = proxy_list[work_count % 10]
    #httpproxy_handler = urllib.request.ProxyHandler(proxy_item)
    #opener = urllib.request.build_opener(httpproxy_handler)
    #print(proxy_item)

    try:
        for url,title in url_list:
            request=urllib.request.Request(url, headers=header)
            response = urllib.request.urlopen(request,timeout=5) #5s超时
            # response = opener.open(request,timeout=5)
            print(response.status,title)
    except:
        print(work_count, ' error...')
    else:
        print(work_count, ' success...')

    global timer
    #timer = threading.Timer(random.randint(60, 70), timer_func)
    #实验发现一分钟左右再访问才会增加一次访问量
    timer = threading.Timer(60, timer_func)
    timer.start()


work_count = 0
#timer = threading.Timer(1, timer_func)
#timer.start()

page_max = 2
url_base = 'https://blog.csdn.net/gongjianbo1992'
#page_max = 4
#url_base = 'https://blog.csdn.net/wangwenjing90'
get_url_func(page_max,url_base)
timer_func()
