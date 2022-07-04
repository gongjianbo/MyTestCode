TEMPLATE = subdirs

SUBDIRS += \
    Common \
    SubProcess \
    MainProcess

SubProcess.depends += Common
MainProcess.depends += SubProcess Common

