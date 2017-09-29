#include "mygraphicsview.hpp"

#include <QResizeEvent>
#include <QDebug>
#include <QGraphicsItem>

MyGraphicsView::MyGraphicsView(QWidget *parent) : QGraphicsView(parent),
    m_background("/home/daniil159x/Загрузки/kartinka-apelsiny-1885.jpg")
{
}

void MyGraphicsView::drawBackground(QPainter *painter, const QRectF &rect)
{
//    painter->drawImage(rect, m_background);
    QGraphicsView::drawBackground(painter, rect);
}

void MyGraphicsView::resizeEvent(QResizeEvent *event)
{

    fitInView( scene()->sceneRect() );

    QGraphicsView::resizeEvent(event);
}
