from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.support import expected_conditions as EC
from selenium.webdriver.support.wait import WebDriverWait
from selenium.common.exceptions import TimeoutException, NoSuchElementException
import requests
from PIL import Image
import cv2 as cv
import pytesseract
import json
import time
import re

#selenium + chromedriver安装和配置：https://blog.csdn.net/wade1203/article/details/108786481
#Pytesseract安装参考：https://www.jianshu.com/p/2db541800418
#验证码识别参考：https://blog.csdn.net/wade1203/article/details/108786481

#识别验证码


def img_ocr(path):
    #path = 'temp.jpg'
    image = cv.imread(path)
    #cv.imshow('raw', image)
    # 边缘保留滤波  去噪
    blur = cv.pyrMeanShiftFiltering(image, sp=8, sr=60)
    #cv.imshow('dst', blur)
    # 灰度图像
    gray = cv.cvtColor(blur, cv.COLOR_BGR2GRAY)
    # 二值化
    ret, binary = cv.threshold(
        gray, 0, 255, cv.THRESH_BINARY_INV | cv.THRESH_OTSU)
    #print(f'二值化自适应阈值：{ret}')
    #cv.imshow('binary', binary)
    # 形态学操作  获取结构元素  开操作
    kernel = cv.getStructuringElement(cv.MORPH_RECT, (3, 2))
    bin1 = cv.morphologyEx(binary, cv.MORPH_OPEN, kernel)
    #cv.imshow('bin1', bin1)
    kernel = cv.getStructuringElement(cv.MORPH_OPEN, (2, 3))
    bin2 = cv.morphologyEx(bin1, cv.MORPH_OPEN, kernel)
    #cv.imshow('bin2', bin2)
    # 逻辑运算  让背景为白色  字体为黑  便于识别
    cv.bitwise_not(bin2, bin2)
    #cv.imshow('binary-image', bin2)
    # 识别
    test_message = Image.fromarray(bin2)
    text = pytesseract.image_to_string(
        test_message, lang='eng', config='--psm 6')
    # 去掉非字母
    new_crazy = filter(str.isalpha, text)
    text = ''.join(list(new_crazy))

    print('验证码:', text)
    #cv.waitKey(0)
    #cv.destroyAllWindows()
    return text

#检测请求结果，判断验证码是否异常


class result_check():
  def __init__(self):
      pass

  def __call__(self, driver):
      try:
          el = driver.find_element_by_xpath(
              ('//div[@class="end-warrantly-date"]/span[@class="in-span-value"]'))
          #print('ret', el.text)
          return el
      except:
          try:
              el = driver.find_element_by_xpath(
                  ('//li[@class="wic-identify"]/span[@style="display: inline-block;"]'))
              #print('warn', el.text)
              return el
          except:
              return False

      return False

#读取验证码列表文件


def read_snfile(path):
    ret = []
    with open(path, 'r', encoding='utf8') as f:
        for line in f.readlines():
            linestr = line.strip()
            ret.append(linestr)
    return ret

#写爬取结果


def write_datefile(path, text, clear):
    flag = 'w+' if clear else 'a+'
    with open(path, mode=flag, encoding='utf8') as f:
        f.write(text+'\n')


base_url = 'https://consumer.huawei.com/cn/support/warranty-query/'

#爬取


def test():
    browser = webdriver.Chrome(
        r'C:\Users\1992\AppData\Local\Google\Chrome\Application\chromedriver.exe')
    wait = WebDriverWait(browser, 20)  # 设置几秒超时时间
    wait_result = WebDriverWait(browser, 10)  # 设置几秒超时时间

    #sn_arr = [
    #    "2155030979TV11041409",
    #    "11111111"
    #]
    sn_arr = read_snfile('sn_list.txt')
    if len(sn_arr) < 1:
        print('sn list is empty')
        return
    print('sn count=', len(sn_arr))
    sn_index = 0
    flag = True
    #顺序遍历每个序列号
    while flag:
        sn = sn_arr[sn_index]
        try:
            browser.get(base_url)

            print('\n序列号:', sn)
            #序列号输入
            input_sn = wait.until(EC.presence_of_element_located(
                (By.XPATH, '//input[@class="s-input ip"]')))
            input_sn.clear()
            input_sn.send_keys(sn)
            #验证码图片
            img_code = wait.until(EC.presence_of_element_located(
                (By.XPATH, '//div[@class="fr wiw-img"]/img')))
            img_response = requests.get(img_code.get_attribute('src'))
            img = img_response.content
            with open('temp.jpg', 'wb') as f:
                f.write(img)
            code = img_ocr('temp.jpg')
            if len(code) != 4:
                raise Exception('code len error')
            #验证码输入
            input_code = wait.until(EC.presence_of_element_located(
                (By.XPATH, '//input[@class="s-input ip01 ga-page-view"]')))
            input_code.clear()
            input_code.send_keys(code)
            #查询按钮
            btn_query = wait.until(EC.presence_of_element_located(
                (By.XPATH, '//a[@class="s-btn"]')))
            btn_query.click()

            #获取查询结果
            #span_value = wait_result.until(EC.presence_of_element_located(
            #    (By.XPATH, '//div[@class="end-warrantly-date"]/span[@class="in-span-value"]'))
            span_value = wait_result.until(result_check())
            if span_value.text.find('验证码') != -1 or len(span_value.text) < 1:
                raise Exception('code text error')
            print('保修截至日期:', span_value.text)
            write_datefile(
                'date_list.txt', sn_arr[sn_index]+':'+span_value.text, bool(sn_index == 0))

        except TimeoutException:
            print('test timeout')
        except Exception as e:
            print('test exception:', e)
        else:
            sn_index += 1
            #当前进度
            print('test success', sn_index, '/', len(sn_arr))
            if sn_index >= len(sn_arr):
                #执行结束
                flag = False
                time.sleep(5)
        finally:
            print('test final')

    #close只会关闭当前页面，quit会退出驱动并且关闭所关联的所有窗口，最后执行完以后就关闭。
    browser.quit()
    print('test browser quit')


if __name__ == "__main__":
    test()
