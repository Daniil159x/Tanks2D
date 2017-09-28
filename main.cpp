#include "widget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    using namespace std::chrono_literals;
    QApplication a(argc, argv);

    Widget w;
//    w.showFullScreen();
    w.show();

    return a.exec();
}
