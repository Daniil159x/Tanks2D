#include "playersprite.hpp"

#include <QGraphicsScene>
#include <QPainter>
#include <QThread>
#include <QDebug>
#include <thread>
#include <chrono>


PlayerSprite::PlayerSprite(const MapField &map, QSize size, QChar sprChar)
    : Sprite(map, size, sprChar, typeItems::player)
{
    static int n = 0;

    m_number = ++n;
}

void PlayerSprite::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->drawImage(boundingRect(), m_map.getImage_forPlayer(m_sprChar).transformed(m_matrix));

    Q_UNUSED(option);
    Q_UNUSED(widget);
}

QRectF PlayerSprite::boundingRect() const
{
    return {0, 0, static_cast<qreal>(m_size.width()), static_cast<qreal>(m_size.height())};
}

int PlayerSprite::type() const
{
    return static_cast<int>(typeItems::player);
}

void PlayerSprite::nextFrame()
{
}

QPointF PlayerSprite::getMuzzle() const
{
    const auto offset = m_map.getSize_bullet().width() / 2;

    const auto x = this->x();
    const auto y = this->y();
    const auto w = this->boundingRect().width();
    const auto h = this->boundingRect().width();

    switch (getVector()) {
        case motion_vector::Up:
            return {x + w / 2 - offset, y};
        case motion_vector::Right:
            return {x + w, y + h / 2 - offset};
        case motion_vector::Down:
            return {x + w / 2 - offset, y + h};
        case motion_vector::Left:
            return {x, y + h / 2 - offset};
        default:
            return {x + w / 2 - offset, y};
    }
}

void PlayerSprite::moveOn(qreal x, qreal y)
{
    Sprite::moveOn(x, y);
//    this->setPos(this->mapToScene(x, y));
}

int PlayerSprite::getNumber() const
{
    return m_number;
}

