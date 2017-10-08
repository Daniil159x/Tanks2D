#include "sprite.hpp"

#include <QPainter>
#include <QDebug>

Sprite::Sprite(const MapField &map, QSize size, QChar spr, typeItems type)
    : m_map(map), m_sprChar(spr), m_currFrame(0), m_size(size), m_type(type)
{
    // TODO: добавить ассерты
}

void Sprite::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->drawImage(boundingRect(),
                m_map.getImage_forSprite(m_currFrame, m_sprChar, m_map.getImage_background()));

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
    // TODO: придумать, что будет, если закончатся все жизни у блока

    // добавить, если не предел
    m_currFrame = (m_currFrame >= m_map.getSize_forSprites(m_sprChar)) ? m_currFrame : m_currFrame + 1;
    // посмотреть придел ли, выставить влаги
    if(m_currFrame == m_map.getSize_forSprites(m_sprChar)) {
        m_type = typeItems::ignoreCollize;
    }
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
