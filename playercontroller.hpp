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
    PlayerController(const PlayerSprite *pl_1, const PlayerSprite *pl_2, const QList<BulletSprite*> &listBullet,
                     const QGraphicsScene *scene);
public slots:
    void game();
    void keyEvent(QKeyEvent *event);
    void rectSceneChanged(const QRectF &rect);
    void endGame();

signals:
    void newPos_forPl_1(qreal x, qreal y);
    void newPos_forPl_2(qreal x, qreal y);

    void newDir_forPl_1(dir d);
    void newDir_forPl_2(dir d);

    void damage_forPl_1();
    void damage_forPl_2();

    void createBuller(qreal x, qreal y, dir d);

private:

    bool m_game = false;
    // FIXME: что то сделать с выравниванием структуры
    struct data {
        const PlayerSprite *m_pl = nullptr;
        void (PlayerController::*signal_newPos)(qreal, qreal) = nullptr;
        void (PlayerController::*signal_newDir)(dir d) = nullptr;
        void (PlayerController::*signal_damage)() = nullptr;
        ushort m_dir = dir_cast(dir::No);
        time_point m_timeLastShoot = time_point();
    } m_players[2];

    QRectF m_rectScene;
    const QList<BulletSprite*> &m_bulletList;
    const QGraphicsScene       *m_scene;


    void emitCreateBuller(data &dt);
};

#endif // PLAYERCONTROLLER_HPP
