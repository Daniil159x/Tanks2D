#include "bulletsprite.hpp"

#include <QPainter>
#include <QDebug>

using namespace std::chrono_literals;


BulletSprite::BulletSprite(const MapField &map, std::chrono::steady_clock::duration delay)
    : Sprite (map, 'b', typeItems::bullet), m_status(status::fly), m_delayAnimation(delay)
{
}

void BulletSprite::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{    
    // TODO: может перенести этот if в nextFrame()?
    if(m_status == status::destroy) {
        auto now = std::chrono::steady_clock::now();
        if((now - m_time_lastFrame) > m_delayAnimation){
            m_time_lastFrame = now;
            nextFrame();
        }
    }

#ifdef DEBUG_SPRITE
    {
        auto r = mapFromScene(this->x(), this->y());
        painter->fillRect(static_cast<int>(r.x()), static_cast<int>(r.y()),
                          this->width(), this->height(), Qt::red);
    }
#endif

    painter->drawImage(boundingRect(), m_img);

    Q_UNUSED(option);
    Q_UNUSED(widget);
}

int BulletSprite::type() const
{
    return static_cast<int>(m_type);
}

void BulletSprite::nextFrame()
{
    switch (m_status) {
        case status::fly:
            break;

        case status::destroy:
            if(m_currIdxFrame >= m_map.getSize_effect()){
                this->deleteLater();
            }
            else {
                ++m_currIdxFrame;
                updateImg();
            }
            break;

        default:
            throw std::logic_error("BulletSprite::nextFrame(): unaccounted status");
            break;
    }
}

void BulletSprite::moveOn(qreal x, qreal y)
{
    if(m_status == status::fly){
        Sprite::moveOn(x, y);
    }
}

void BulletSprite::collision()
{
    m_status = status::destroy;
    m_type   = typeItems::ignoreCollision;


    // TODO: сделать выравнивание в nextFrame, а лучше в updateImg. Ибо могут прийти разный размеры эффектов
    const auto w = m_img.width();
    const auto h = m_img.height();
    switch (getVector()) {
        case motion_vector::Up:
            Sprite::moveOn(-w/2, 0);
            break;
        case motion_vector::Down:
            Sprite::moveOn(-w/2, 0);
            break;
        case motion_vector::Left:
            Sprite::moveOn(0, -h/2);
            break;
        case motion_vector::Right:
            Sprite::moveOn(0, -h/2);
            break;
        default:
            std::logic_error("BulletSprite::collision(): default branch");
    }
}


QImage BulletSprite::initImg()
{
    if(m_status == status::fly){
        return m_map.getImage_forBullet();
    }
    else if(m_status == status::destroy) {
        return m_map.getImage_forEffect(m_currIdxFrame, QImage());
    }
    else {
        throw std::logic_error("BulletSprite::initImg: else branch");
    }
}
