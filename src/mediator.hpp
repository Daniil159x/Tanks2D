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
    using duration = clock::duration;

public:
    explicit Mediator(const QString &fileName);

    void exec(bool fullScrean = true);

    virtual ~Mediator();


    // TODO: заменить на генераторы
    qreal getSpeedW() const;
    void setSpeedW(const qreal &getSpeedW);

    qreal getSpeedH() const;
    void setSpeedH(const qreal &speedH);

    qreal getBulletSpeedW() const;
    void setBulletSpeedW(const qreal &bulletSpeedW);

    qreal getBulletSpeedH() const;
    void setBulletSpeedH(const qreal &bulletSpeedH);

    duration getShotDelay() const;
    void setShotDelay(const duration &shotDelay);

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

    qreal m_speedW, m_speedH;
    qreal m_bulletSpeedW, m_bulletSpeedH;
    duration m_shotDelay;

    PlayerSprite *m_players[2];
    int           m_vectorPlayers[2];
    bool          m_shot[2];
    time_poing    m_lastShot[2];

    int m_idTimer;


    QList<BulletSprite*> m_listBullet;

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
