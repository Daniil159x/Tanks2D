#include "sprite.hpp"

#include <QPainter>
#include <QDebug>

Sprite::Sprite(const MapField &map, QChar spr, const typeItems type)
    : m_map(map), m_sprChar(spr), m_currIdxFrame(0),m_type(type),
        m_currRotate(0), m_img(initImg())
{
    // TODO: добавить ассерты
}

void Sprite::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->drawImage(boundingRect(), m_img);

    Q_UNUSED(option);
    Q_UNUSED(widget);
}

QRectF Sprite::boundingRect() const
{
    return m_img.rect();
}

int Sprite::type() const
{
    return static_cast<int>(m_type);
}

void Sprite::nextFrame()
{
    m_currIdxFrame = (m_currIdxFrame >= m_map.getSizeImage_forSprites(m_sprChar)) ? m_currIdxFrame : m_currIdxFrame + 1;

    if(m_currIdxFrame == m_map.getSizeImage_forSprites(m_sprChar)) {
        m_type = typeItems::ignoreCollize;
    }
    updateImg();
}

qreal angleFromVector(motion_vector v){
    switch (v) {
        case motion_vector::Down:
            return 180;
        case motion_vector::Left:
            return 270;
        case motion_vector::Right:
            return 90;
        case motion_vector::Up:
            return 0;
        default:
            return 0;
    }
}

void Sprite::setVector(motion_vector v)
{
    auto newRotate = angleFromVector(v);
    if(!qFuzzyCompare(m_currRotate, newRotate)){
        m_currRotate = newRotate;

        updateImg();
    }
}

motion_vector Sprite::getVector() const
{
    // up
    if(qFuzzyCompare(m_currRotate, 0)) {
        return motion_vector::Up;
    }
    // right
    else if(qFuzzyCompare(m_currRotate, 90)) {
        return motion_vector::Right;
    }
    // down
    else if(qFuzzyCompare(m_currRotate, 180)) {
        return motion_vector::Down;
    }
    // left
    else if(qFuzzyCompare(m_currRotate, 270)) {
        return motion_vector::Left;
    }
    // no = up
    else {
        return motion_vector::No;
    }
}

// NOTE: moveOn == moveBy?
void Sprite::moveOn(qreal x, qreal y)
{
    this->setPos(mapToScene(x, y));
}

bool Sprite::collidesWithItem(const QGraphicsItem *other, Qt::ItemSelectionMode mode) const
{
    if(this->type() == static_cast<int>(typeItems::ignoreCollize)) {
        return false;
    }
    else {
        return baseClass::collidesWithItem(other, mode);
    }
}

void Sprite::savePos()
{
    m_save_pos = this->pos();
}

void Sprite::restorePos()
{
    this->setPos(m_save_pos.x(), m_save_pos.y());
}

// NOTE: может возвращать const QImage& ?
QImage Sprite::initImg()
{
    return m_map.getImage_forSprite(m_currIdxFrame, m_sprChar, m_map.getImage_background());
}

// NOTE: костыль ли?
void Sprite::updateImg()
{
    QMatrix mx;
    mx.rotate(m_currRotate);

    m_img = initImg().transformed(mx);
}
