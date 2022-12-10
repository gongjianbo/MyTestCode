TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS += App DynamicPlugin
App.depends += DynamicPlugin
