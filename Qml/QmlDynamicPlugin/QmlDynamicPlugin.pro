TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS += App ItemPlugin WindowPlugin
App.depends += ItemPlugin WindowPlugin

