#ifndef SPRITE_HPP
#define SPRITE_HPP

#include <QGraphicsItem>
#include <QRect>
#include "enums.hpp"

class Sprite : public QGraphicsItem
{
    using baseClass = QGraphicsItem;

public:
    // NOTE: можно использовать при отрисовки не массив изображений, а передавать координаты фрейма в картинке
    Sprite(const QVector<QImage> &vec_imgs, QSize size, typeItems type = typeItems::ignoreCollize);

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                       QWidget *widget = nullptr) override;

    virtual QRectF boundingRect() const override;

    virtual int type() const override;

    virtual void nextFrame();

    QSize getSize() const {
        return m_size;
    }
    int width() const {
        return m_size.width();
    }
    int height() const {
        return m_size.height();
    }

    virtual bool collidesWithItem(const QGraphicsItem *other, Qt::ItemSelectionMode mode = Qt::IntersectsItemShape) const override;

protected:
    QVector<QImage> m_imgs;
    int             m_currFrame;
    QSize           m_size;
    typeItems       m_type;
};

#endif // SPRITE_HPP
