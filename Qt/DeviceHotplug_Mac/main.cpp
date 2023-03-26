#include <QCoreApplication>

#include <stdlib.h>
#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/usb/IOUSBLib.h>

// IOServiceAddMatchingNotification 回调函数
// refCon 是注册时传递的参数，比如对象指针等
void deviceAdded(void *refCon, io_iterator_t iterator) {
    io_service_t usb_device;
    while (usb_device = IOIteratorNext(iterator)) {
        // 设备名称
        CFStringRef device_name = static_cast<CFStringRef>(IORegistryEntryCreateCFProperty(usb_device, CFSTR(kUSBProductString), kCFAllocatorDefault, 0));
        if (!device_name) {
            device_name = static_cast<CFStringRef>(IORegistryEntryCreateCFProperty(usb_device, CFSTR(kUSBVendorString), kCFAllocatorDefault, 0));
        }
        if (!device_name) {
            device_name = CFStringCreateWithCString(kCFAllocatorDefault, "<Unknown>", kCFStringEncodingUTF8);
        }
        ssize_t len = CFStringGetMaximumSizeForEncoding(CFStringGetLength(device_name), kCFStringEncodingUTF8);
        char *buf = new char[len];
        CFStringGetCString(device_name, buf, len, kCFStringEncodingUTF8);
        fprintf(stderr, "Device added: %s.\n", buf);
        delete buf;
        CFRelease(device_name);

        // 获取 vip 和 pid
        CFTypeRef cf_vendor, cf_product;
        cf_vendor = (CFTypeRef)IORegistryEntrySearchCFProperty(usb_device, kIOServicePlane, CFSTR("idVendor"), kCFAllocatorDefault,
                                                               kIORegistryIterateRecursively | kIORegistryIterateParents);

        cf_product = (CFTypeRef)IORegistryEntrySearchCFProperty(usb_device, kIOServicePlane, CFSTR("idProduct"), kCFAllocatorDefault,
                                                                kIORegistryIterateRecursively | kIORegistryIterateParents);
        SInt32 vendor_id = 0, product_id = 0;
        if (cf_vendor && cf_product && CFNumberGetValue((CFNumberRef)cf_vendor, kCFNumberSInt32Type, &vendor_id) &&
                CFNumberGetValue((CFNumberRef)cf_product, kCFNumberSInt32Type, &product_id)) {
            fprintf(stderr, "Device vid 0x%04x, pid 0x%04x.\n", vendor_id, product_id);
        }
        if (cf_vendor)
            CFRelease(cf_vendor);
        if (cf_product)
            CFRelease(cf_product);

        IOObjectRelease(usb_device);
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    // 创建 I/O Kit 通知端口
    IONotificationPortRef notify_prot = IONotificationPortCreate(kIOMasterPortDefault);

    // 创建匹配字典，可以指定要监听的设备的厂商 ID 和产品 ID
    CFMutableDictionaryRef match_dict = IOServiceMatching(kIOUSBDeviceClassName);
    // SInt32 vendor_id = 0x2717;
    // SInt32 product_id = 0xFF08;
    // CFDictionarySetValue(match_dict, CFSTR(kUSBVendorID), CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &vendor_id));
    // CFDictionarySetValue(match_dict, CFSTR(kUSBProductID), CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &product_id));

    // 创建异步通知端口
    io_iterator_t add_iterator;
    kern_return_t result;
    // kIOMatchedNotification 添加，kIOTerminatedNotification 移除
    result = IOServiceAddMatchingNotification(notify_prot, kIOMatchedNotification, match_dict, deviceAdded, nullptr, &add_iterator);
    if (result != KERN_SUCCESS) {
        fprintf(stderr, "IOServiceAddMatchingNotification error: %d. \n", result);
        return -1;
    }
    // 初始化的时候要调用一次，不然没回调，也可以在这时枚举当前已有设备
    // 可以使用空的处理来跳过
    deviceAdded(nullptr, add_iterator);

    // 将通知端口连接到一个运行循环结构中
    // 运行循环属于 Core Foundation 变成模型，实现了消息循环，当消息到达通知端口时，用户提供的回调函数会被调用
    CFRunLoopSourceRef run_loop = IONotificationPortGetRunLoopSource(notify_prot);
    CFRunLoopAddSource(CFRunLoopGetCurrent(), run_loop, kCFRunLoopDefaultMode);

    // 进入主运行循环，等待设备插入或拔出事件
    // 如果是放到 Qt 事件循环里就不需要这个
    // CFRunLoopRun();

    int ret = app.exec();

    // 释放
    IONotificationPortDestroy(notify_prot);

    return ret;
}
