#ifndef SPRITE_HPP
#define SPRITE_HPP

#include <QGraphicsItem>
#include <QBrush>

#define W 20
#define H 20

class Sprite : public QGraphicsItem
{
public:
    Sprite(int id);

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                       QWidget *widget = nullptr) override;

    virtual QRectF boundingRect() const override;

private:
    const QBrush m_brush;
};

#endif // SPRITE_HPP
