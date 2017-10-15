#ifndef SHELLSPRITE_HPP
#define SHELLSPRITE_HPP

#include <QObject>
#include <QTimer>

#include "sprite.hpp"
#include "enums.hpp"

#include <chrono>

class Mediator;


class BulletSprite : /*virtual*/ public QObject, public Sprite
{
    Q_OBJECT

    friend Mediator;

public:
    BulletSprite(const MapField &map, std::chrono::steady_clock::duration delay);

    enum class status {
        fly,
        destroy
    };

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    virtual int type() const override;
    virtual void nextFrame() override;

    virtual void moveOn(qreal x, qreal y) override;

    bool isFly() const {
        return (m_status == status::fly);
    }

protected slots:

    // class will destroy
    void collision();

protected:
    status m_status;

    std::chrono::steady_clock::duration m_delayAnimation;

    decltype (std::chrono::steady_clock::now()) m_time_lastFrame = {};

    virtual QImage initImg() override;
};

#endif // SHELLSPRITE_HPP
