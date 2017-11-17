#include <QApplication>

#include "enums.hpp"
#include "mediator.hpp"
#include "menu.hpp"

#include <QMessageBox>

#include <QDir>
#include <QDebug>
#include <QDirIterator>

#define PATH_MAPS "../../maps/"

#include <QStyleFactory>

int main(int argc, char *argv[])
{
    qRegisterMetaType<motion_vector>("dir");

    QApplication a(argc, argv);

    QFile styleFile(":/style/style.css");
    styleFile.open(QFile::ReadOnly);
    a.setStyleSheet(styleFile.readAll());
    styleFile.close();

    Menu m;
    QObject::connect(&m, &Menu::start, [&m](const QString &path, const QString &name){
        m.hide();

        // FIXME: убрать утечку памяти
        auto ptr = new Mediator(path, name, m.getSetting().colorField);

        ptr->setSpeedGen(m.getSpeedGen());
        ptr->setSpeedBulletGen(m.getBulletSpeedGen());
        ptr->setShotDelayGen(m.getShotDelayGen());

        ptr->exec(m.getSetting().fullScrean);

        QObject::connect(ptr, &Mediator::exit, [ptr, &m](){
            m.show();
            delete ptr;
        });
    });

    m.show();





    return a.exec();
}
