#include "widget.h"
#include <QApplication>

#include "enums.hpp"

int main(int argc, char *argv[])
{
    qRegisterMetaType<dir>("dir");

    using namespace std::chrono_literals;
    QApplication a(argc, argv);

    Widget w;
//    w.showFullScreen();
    w.show();

    return a.exec();
}
