#include <QApplication>

#include "enums.hpp"
#include "mediator.hpp"

int main(int argc, char *argv[])
{
    qRegisterMetaType<motion_vector>("dir");

    QApplication a(argc, argv);

    Mediator game("/home/daniil159x/Qt_project/Tanks2D/maps/test/json_test.json");
    game.exec();

    return a.exec();
}
