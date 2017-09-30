#include "playersprite.hpp"

#include <QGraphicsScene>
#include <QPainter>
#include <QThread>
#include <QDebug>
#include <thread>
#include <chrono>


PlayerSprite::PlayerSprite(const QVector<QRect> &coords_img, const QImage &img, QSize size)
    : Sprite(coords_img, img, size), m_dir(dir::Up)
{
    nextFrame();
}

void PlayerSprite::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    nextFrame();

    painter->drawImage(boundingRect(), m_imgs[m_currFrame]);

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
    // FIX: убрать после загрузки спрайтов
    m_currFrame = 1;
    return;

    switch (m_dir) {
        case dir::No:
            m_currFrame = 0;
            break;
        case dir::Up:
            m_currFrame = 1;
            break;
        case dir::Down:
            m_currFrame = 2;
            break;
        case dir::Left:
            m_currFrame = 3;
            break;
        case dir::Right:
            m_currFrame = 4;
            break;
        default:
            throw std::logic_error("PlayerSprite::nextFrame(): unaccounted direction");
            break;
    }
}

QPointF PlayerSprite::getMuzzle() const
{
    switch (m_dir) {
        case dir::Up:
            return {this->x() + this->width()/2, this->y()};
            break;
        case dir::Down:
            return {this->x() + this->width()/2, this->y() + this->height()};
            break;
        case dir::Left:
            return {this->x(), this->y() + this->height()/2};
            break;
        case dir::Right:
            return {this->x() + this->width(), this->y() + this->height()/2};
            break;
        default:
            throw std::logic_error("PlayerSprite::nextFrame(): unaccounted direction");
            break;
    }
}

void PlayerSprite::editPos(qreal x, qreal y)
{
    qDebug() << "pos edit";
    this->setPos(x, y);
}

void PlayerSprite::editDir(dir newDir)
{
    m_dir = newDir;
    nextFrame();
}
