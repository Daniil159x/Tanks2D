#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <QGraphicsObject>
#include <QKeyEvent>
#include <QKeySequence>
#include <chrono>
#include <QVector>

#include "sprite.hpp"

class Mediator;
class PlayerController;

/*  coords_img:
 *
 *  coords_img[0] - destroyed
 *  coords_img[1] - up
 *  coords_img[2] - down
 *  coords_img[3] - left
 *  coords_img[4] - right
 */

class PlayerSprite : /*virtual*/ public QObject, public Sprite
{
    Q_OBJECT

    friend Mediator;
    friend PlayerController;

public:

    PlayerSprite(const QVector<QImage> &vec_imgs, QSize size);

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                       QWidget *widget = nullptr) override;

    virtual QRectF boundingRect() const override;

    virtual int type() const override;

    // sets 'm_currFrame' in accordance with 'dir'
    virtual void nextFrame() override;

    QPointF getMuzzle() const;

protected slots:
//    void editPos(qreal x, qreal y);
    void editDir(dir newDir);

protected:
    dir m_dir;
};


#endif // PLAYER_HPP
