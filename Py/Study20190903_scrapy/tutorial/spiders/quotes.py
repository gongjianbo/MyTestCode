# -*- coding: utf-8 -*-
import scrapy

from tutorial.items import QuotesItem

class QuotesSpider(scrapy.Spider):
    #name是每个项目唯一的名字，用来区分不同的Spider
    name = 'quotes'
    #allowed_domains是允许爬取的域名，如果初始或后续的请求链接不是这个域名下的，请求链接会被过滤
    allowed_domains = ['quotes.toscrape.com']
    #start_urls包含了Spider启动时爬取的url列表，定义初始请求
    start_urls = ['http://quotes.toscrape.com/']

    '''
    默认情况下，parse被调用时start_urls里面的链接构成的请求完成下载执行后，
    返回的响应就会作为唯一的参数传递给这个函数。
    该方法负责解析返回的响应、提取数据或者进一步生成要处理的请求。
    '''
    def parse(self, response):
        print(response.url,'+++++++++++++++++++')
        quotes=response.css('.quote')
        for quote in quotes:
            item=QuotesItem()
            item['text']=quote.css('.text::text').extract_first()
            item['author']=quote.css('.author::text').extract_first()
            item['tags']=quote.css('.tags .tag::text').extract()
            yield item
        next=response.css('.pager .next a::attr("href")').extract_first()
        url=response.urljoin(next)
        print(url,'======================')
        yield scrapy.Request(url=url,callback=self.parse)
