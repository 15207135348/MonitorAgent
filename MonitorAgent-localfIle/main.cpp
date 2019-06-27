#include <QCoreApplication>
#include <iostream>
#include <thread/gatherthread.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    GatherThread gather;
    gather.start();
    return a.exec();
}
