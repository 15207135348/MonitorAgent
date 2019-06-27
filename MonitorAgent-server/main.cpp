#include <QCoreApplication>

#include <mainconsole.h>
#include <iostream>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Mainconsole console(argc, argv);
    return a.exec();
}
