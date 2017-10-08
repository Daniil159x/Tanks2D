#include "playersprite.hpp"

#include <QGraphicsScene>
#include <QPainter>
#include <QThread>
#include <QDebug>
#include <thread>
#include <chrono>


PlayerSprite::PlayerSprite(const MapField &map, QSize size, QChar sprChar)
    : Sprite(map, size, sprChar, typeItems::player), m_dir(dir::Up)
{
    nextFrame();
}

void PlayerSprite::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->drawImage(boundingRect(), m_map.getImage_forPlayer(m_dir, m_sprChar));

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
    switch (m_dir) {
        case dir::Up:
            return {this->x(), this->y() - this->height()};
            break;
        case dir::Down:
            return {this->x(), this->y() + this->height()};
            break;
        case dir::Left:
            return {this->x() - this->width(), this->y()};
            break;
        case dir::Right:
            return {this->x() + this->width(), this->y()};
            break;
        default:
            throw std::logic_error("PlayerSprite::nextFrame(): unaccounted direction");
            break;
    }
}

void PlayerSprite::editDir(dir newDir)
{
    m_dir = newDir;
}

