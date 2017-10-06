#include "sprite.hpp"

#include <QPainter>
#include <QDebug>

Sprite::Sprite(const QVector<QImage> &vec_imgs, QSize size, typeItems type)
    : m_imgs(vec_imgs), m_currFrame(0), m_size(size), m_type(type)
{
    // TODO: добавить ассерты
}

void Sprite::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
//    qDebug() << m_imgs.size() << m_currFrame;
    painter->drawImage(boundingRect(), m_imgs[m_currFrame]);

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
    m_currFrame = (m_currFrame >= m_imgs.size()) ? m_currFrame : m_currFrame + 1;
    // посмотреть придел ли, выставить влаги
    if(m_currFrame == m_imgs.size()) {
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
