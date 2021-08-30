TEMPLATE = subdirs

SUBDIRS += \
    Client \
    Common \
    Server

Server.depends += Common
Client.depends += Server Common

