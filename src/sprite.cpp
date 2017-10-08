#include "sprite.hpp"

#include <QPainter>
#include <QDebug>

Sprite::Sprite(const MapField &map, QSize size, QChar spr, const typeItems type)
    : m_map(map), m_sprChar(spr), m_currFrame(0), m_size(size), m_type(type), m_currRotate(0)
{
    // TODO: добавить ассерты
}

void Sprite::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->drawImage(boundingRect(),
        m_map.getImage_forSprite(m_currFrame, m_sprChar, m_map.getImage_background()).transformed(m_matrix));

    Q_UNUSED(option);
    Q_UNUSED(widget);
}

QRectF Sprite::boundingRect() const
{
    return {0, 0, static_cast<qreal>(m_size.width()), static_cast<qreal>(m_size.height())};
}

int Sprite::type() const
{
    return static_cast<int>(m_type);
}

void Sprite::nextFrame()
{
    m_currFrame = (m_currFrame >= m_map.getSizeImage_forSprites(m_sprChar)) ? m_currFrame : m_currFrame + 1;

    if(m_currFrame == m_map.getSizeImage_forSprites(m_sprChar)) {
        m_type = typeItems::ignoreCollize;
    }
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
        m_matrix.reset();
        m_matrix.rotate(angleFromVector(v));
        m_currRotate = newRotate;
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
