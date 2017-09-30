#include "sprite.hpp"

#include <QPainter>

Sprite::Sprite(const QVector<QRect> &coords_img, const QImage &img, QSize size)
    : m_currFrame(0), m_size(size)
{
    // TODO: добавить ассерты
    m_imgs.reserve(coords_img.size());
    for(const auto& coord : coords_img) {
        m_imgs.push_back( img.copy(coord) );
    }
}

void Sprite::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->drawImage(QRect{0, 0, m_size.width(), m_size.height()}, m_imgs[m_currFrame]);

    Q_UNUSED(option);
    Q_UNUSED(widget);
}

QRectF Sprite::boundingRect() const
{
    return {0, 0, static_cast<qreal>(m_size.width()), static_cast<qreal>(m_size.height())};
}

int Sprite::type() const
{
    return static_cast<int>(QGraphicsItem::UserType);
}

void Sprite::nextFrame()
{
    m_currFrame = (m_currFrame + 1) % m_imgs.size();
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
