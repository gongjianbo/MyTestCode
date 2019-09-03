# -*- coding: utf-8 -*-

# Define your item pipelines here
#
# Don't forget to add your pipeline to the ITEM_PIPELINES setting
# See: https://docs.scrapy.org/en/latest/topics/item-pipeline.html

from scrapy.exceptions import DropItem

class TextPipeline(object):
    def __init__(self):
        self.limit=50
    
    def process_item(self,item,spider):
        if item['text']:
            if len(item['text']) > self.limit:
                item['text']=item['text'][0:self.limit].rstrip()+'...'
            print('text:',item['text'])
            return item
        else:
            return DropItem('Missing Text')


class TutorialPipeline(object):
    def process_item(self, item, spider):
        return item
