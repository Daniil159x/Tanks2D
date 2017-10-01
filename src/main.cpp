#include "widget.h"
#include <QApplication>

#include "enums.hpp"
#include "mapfield.hpp"

void game();

int main(int argc, char *argv[])
{
    qRegisterMetaType<dir>("dir");

    using namespace std::chrono_literals;
    QApplication a(argc, argv);

    MapField mp("/home/daniil159x/Qt_project/Tanks2D/json_test.json");
    auto vec = mp.getFiledSprites();

    return 0;
}


void game() {
    QGraphicsView  view;
    QGraphicsScene scene;

    // TODO: сделать нормальное задание пространства
    scene.setSceneRect(0, 0, 200, 200);



}
