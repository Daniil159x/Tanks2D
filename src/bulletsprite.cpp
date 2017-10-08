#include "bulletsprite.hpp"

#include <QPainter>
#include <QDebug>

#include <chrono>
using namespace std::chrono_literals;

#define DELAY_ANIMATION 100ms

BulletSprite::BulletSprite(const MapField &map, QSize size, dir dir)
    : Sprite (map, size, 'b', typeItems::bullet), m_status(status::fly), m_dir(dir)
{
}

void BulletSprite::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{    

    static decltype (std::chrono::steady_clock::now()) time = {};


    if(m_status == status::fly){
        painter->drawImage(boundingRect(), m_map.getImage_forBullet(m_dir));
    }
    else if(m_status == status::destroy) {
        auto now = std::chrono::steady_clock::now();
        if((now - time) > DELAY_ANIMATION){
            time = now;
            nextFrame();
        }

        painter->drawImage(boundingRect(), m_map.getImage_forEffect(m_currFrame, QImage()));
    }

    Q_UNUSED(option);
    Q_UNUSED(widget);
}

int BulletSprite::type() const
{
    return static_cast<int>(typeItems::bullet);
}

void BulletSprite::nextFrame()
{
    switch (m_status) {
        case status::fly:
            break;

        case status::destroy:
            if(m_currFrame >= m_map.getSize_effect()){
                this->deleteLater();
            }
            else {
                ++m_currFrame;
            }
            break;

        default:
            throw std::logic_error("BulletSprite::nextFrame(): unaccounted status");
            break;
    }
}

void BulletSprite::editPos(qreal x, qreal y)
{
    if(m_status == status::fly){
        this->setPos(x, y);
    }
}

void BulletSprite::collision()
{
    m_status = status::destroy;
    m_type   = typeItems::ignoreCollize;
}
