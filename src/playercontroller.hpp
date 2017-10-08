#ifndef PLAYERCONTROLLER_HPP
#define PLAYERCONTROLLER_HPP

#include <QObject>
#include <QRect>
#include <QGraphicsScene>
#include <QGraphicsItemGroup>

#include "enums.hpp"
#include "playersprite.hpp"
#include "bulletsprite.hpp"


class PlayerController : public QObject
{
    Q_OBJECT

    using clock = std::chrono::steady_clock;
    using time_point = clock::time_point;

public:
    PlayerController(PlayerSprite *pl_1, PlayerSprite *pl_2, const QList<BulletSprite*> &listBullet,
                     const QGraphicsScene *scene);
public slots:
    void game();
    void keyEvent(QKeyEvent event);
    void rectSceneChanged(const QRectF &rect);
    void endGame();

signals:
    void newPos(qreal x, qreal y, QGraphicsItem *ptr);
    void newDir_Player(dir d, PlayerSprite *ptr);
    void damage(BulletSprite *ptr_out, Sprite *ptr_in);
    void createBuller(qreal x, qreal y, dir d);

private:

    bool m_game = false;
    // FIXME: что то сделать с выравниванием структуры
    struct data {
        PlayerSprite *m_pl = nullptr;
        time_point m_timeLastShoot = time_point();
        ushort m_keyboardDir = dir_cast(dir::No);
    } m_players[2];

    QRectF m_rectScene;
    const QList<BulletSprite*> &m_bulletList;
    const QGraphicsScene       *m_scene;


    void emitCreateBuller(data &dt);

    // QObject interface
protected:
    virtual void timerEvent(QTimerEvent *event) override;

    void movePlayer(data &dataPl);
};

#endif // PLAYERCONTROLLER_HPP
