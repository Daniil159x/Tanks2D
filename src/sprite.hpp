#ifndef SPRITE_HPP
#define SPRITE_HPP

#include <QGraphicsItem>
#include <QRect>
#include "enums.hpp"
#include "mapfield.hpp"

class Sprite : public QGraphicsItem
{
    using baseClass = QGraphicsItem;

public:

    Sprite(const MapField &map, QChar spr, const typeItems type = typeItems::ignoreCollize);

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                       QWidget *widget = nullptr) override;

    virtual QRectF boundingRect() const override;

    virtual int type() const override;

    virtual void nextFrame();

    QSize getSize() const {
        return m_img.size();
    }
    int width() const {
        return getSize().width();
    }
    int height() const {
        return getSize().height();
    }

    virtual void setVector(motion_vector v);
    motion_vector getVector() const;

    virtual void moveOn(qreal x, qreal y);

    virtual bool collidesWithItem(const QGraphicsItem *other, Qt::ItemSelectionMode mode = Qt::IntersectsItemShape) const override;

    void savePos();
    void restorePos();

protected:
    const MapField &m_map;
    QChar           m_sprChar;
    int             m_currIdxFrame;
//    QSize           m_size;
    typeItems       m_type;

    qreal           m_currRotate;

    QPointF m_save_pos;
    QImage  m_img;

    // m_img = getImage_for<class>(...);
    virtual QImage initImg();

    void updateImg();

};

#endif // SPRITE_HPP
