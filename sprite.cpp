#include "sprite.hpp"

#include <QPainter>

auto getColor(int id){
    switch (id) {
        case 0: return Qt::white;
        case 1: return Qt::black;
        case 2: return Qt::red;
        case 3: return Qt::yellow;
        case 4: return Qt::green;
        case 5: return Qt::gray;
        case 6: return Qt::blue;
        default: return Qt::lightGray;
    }
}


Sprite::Sprite(int id) : m_brush(getColor(id))
{
}

void Sprite::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->fillRect(0, 0, W, H, m_brush);
    Q_UNUSED(option);
    Q_UNUSED(widget);
}

QRectF Sprite::boundingRect() const
{
    return {0, 0, W, H};
}
