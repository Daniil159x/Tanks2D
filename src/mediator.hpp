#ifndef MEDIATOR_HPP
#define MEDIATOR_HPP

#include <QGraphicsScene>
#include <QGraphicsView>

#include <QObject>
#include <QList>

#include <chrono>

#include "enums.hpp"
#include "mapfield.hpp"
#include "bulletsprite.hpp"
#include "playersprite.hpp"


class Mediator : public QObject
{
    Q_OBJECT

    using clock = std::chrono::steady_clock;
    using time_poing = clock::time_point;

public:
    explicit Mediator(const QString &fileName);

    void exec();

    virtual ~Mediator();
signals:
    void keyEvent(QKeyEvent *ev);
    void endGame(int player);
    void beginGame();
protected:
    virtual bool eventFilter(QObject *obj, QEvent *event) override;
    virtual void timerEvent(QTimerEvent *event) override;

private:
    QGraphicsScene *m_scene;
    QGraphicsView  *m_view;

    bool m_isGame;

    PlayerSprite *m_players[2];
    int           m_vectorPlayers[2];
    bool          m_shot[2];
    time_poing    m_lastShot[2];

    int m_idTimer;


    QList<BulletSprite*> m_listBullet;


//    QList<BulletSprite*> m_listBullet;

    MapField m_map;

    void movePlayers(int i);
    void createBullet(qreal x, qreal y, motion_vector vec);
    void checkCreateBullet(int i);
    void moveBullet(BulletSprite *ptr);
    void damage(BulletSprite *out, Sprite *to);
    void hitPlayer(PlayerSprite *ptr);

private slots:
    void slotEndGame();
    void deleteBullet(QObject *ptr);
};

#endif // MEDIATOR_HPP
