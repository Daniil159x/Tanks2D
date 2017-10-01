#include "mygraphicsscene.hpp"

#include <QKeyEvent>
#include <QDebug>

MyGraphicsScene::MyGraphicsScene(QObject *parent) : QGraphicsScene(parent)
{
}

void MyGraphicsScene::keyPressEvent(QKeyEvent *event)
{
    emit keyEvent(event);
    qDebug() << "emit press";

    QGraphicsScene::keyPressEvent(event);
}

void MyGraphicsScene::keyReleaseEvent(QKeyEvent *event)
{
    emit keyEvent(event);
    qDebug() << "emit release";

    QGraphicsScene::keyReleaseEvent(event);
}
