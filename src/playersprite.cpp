#include "playersprite.hpp"

#include <QGraphicsScene>
#include <QPainter>
#include <QThread>
#include <QDebug>
#include <thread>
#include <chrono>


PlayerSprite::PlayerSprite(const MapField &map, QChar sprChar)
    : Sprite(map, sprChar, typeItems::player), m_isLive(true)
{
    static int n = 0;

    // TODO: костыль с присваением порядковых номеров игрокам
    m_number = (n++ % 2) + 1;
}

void PlayerSprite::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
#ifdef DEBUG_SPRITE
    {
        auto r = mapFromScene(this->x(), this->y());
        painter->fillRect(static_cast<int>(r.x()), static_cast<int>(r.y()),
                          this->width(), this->height(), Qt::green);
    }
#endif

    painter->drawImage(boundingRect(), m_img);

    Q_UNUSED(option);
    Q_UNUSED(widget);
}

QRectF PlayerSprite::boundingRect() const
{
    return m_img.rect();
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
            return {x + w / 2 - offset, y - h};
        case motion_vector::Right:
            return {x + w, y + h / 2 - offset};
        case motion_vector::Down:
            return {x + w / 2 - offset, y + h + 2};
        case motion_vector::Left:
            return {x - w, y + h / 2 - offset};
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

bool PlayerSprite::getIsLive() const
{
    return m_isLive;
}

void PlayerSprite::setIsLive(bool isLive)
{
    m_isLive = isLive;
}

QImage PlayerSprite::initImg()
{
    return m_map.getImage_forPlayer(m_sprChar);
}

