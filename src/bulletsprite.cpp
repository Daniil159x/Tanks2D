#include "bulletsprite.hpp"


#define DELAY_ANIMATION 100/*ms*/

BulletSprite::BulletSprite(dir m_dir, const QVector<QImage> &vec_imgs, QSize size)
    : Sprite (vec_imgs, size), m_dir(m_dir)
{
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
            if(m_currFrame >= m_imgs.size()){
                m_timer.stop();
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

    connect(&m_timer, &QTimer::timeout, this, &BulletSprite::nextFrame);
    m_timer.start(DELAY_ANIMATION);
}
