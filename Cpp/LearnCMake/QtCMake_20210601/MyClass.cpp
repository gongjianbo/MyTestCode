#include "MyClass.h"

MyClass::MyClass(QObject *parent) : QObject(parent)
{

}

QString MyClass::hello()
{
    return "Hello CMake";
}
