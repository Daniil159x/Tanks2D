#ifndef SPRITE_HPP
#define SPRITE_HPP

#include <QGraphicsItem>
#include <QRect>

class Sprite : public QGraphicsItem
{
    using baseClass = QGraphicsItem;

public:
    Sprite(const QVector<QRect> &coords_img, const QImage &img, QSize size);

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                       QWidget *widget = nullptr) override;

    virtual QRectF boundingRect() const override;

    virtual int type() const override;

    virtual void nextFrame();

    QSize getSize() const;

    virtual bool collidesWithItem(const QGraphicsItem *other, Qt::ItemSelectionMode mode = Qt::IntersectsItemShape) const override;

protected:
    QVector<QImage> m_imgs;
    int             m_currFrame;
    QSize           m_size;
};

#endif // SPRITE_HPP
