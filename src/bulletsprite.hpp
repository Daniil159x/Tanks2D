#ifndef SHELLSPRITE_HPP
#define SHELLSPRITE_HPP

#include <QObject>
#include <QTimer>

#include "sprite.hpp"
#include "enums.hpp"

class PlayerController;
class Mediator;


/* coords_img:
 * coords_img[0] = fly
 * coords_img[1] = destroy, frame 1
 * coords_img[2] = destroy, frame 2
 * ...
 * coords_img[n] = destroy, frame n
 *
 */

class BulletSprite : /*virtual*/ public QObject, public Sprite
{
    Q_OBJECT

    friend PlayerController;
    friend Mediator;

public:
    BulletSprite(const MapField &map, QSize size);

    enum class status {
        fly,
        destroy
    };

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    virtual int type() const override;
    virtual void nextFrame() override;

    virtual void moveOn(qreal x, qreal y) override;

protected slots:

    // class will destroy
    void collision();

protected:
    status m_status;
};

#endif // SHELLSPRITE_HPP
