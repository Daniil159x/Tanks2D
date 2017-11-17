#ifndef MEDIATOR_HPP
#define MEDIATOR_HPP

#include <QGraphicsScene>
#include <QGraphicsView>

#include <QObject>
#include <QList>
#include <QLabel>

#include <chrono>

#include "enums.hpp"
#include "mapfield.hpp"
#include "bulletsprite.hpp"
#include "playersprite.hpp"


#include "menu.hpp"

class Mediator : public QObject
{
    Q_OBJECT


public:
    using clock = Menu::clock;
    using duration = Menu::duration;
    using time_poing = Menu::time_point;

    explicit Mediator(const QString &fileName, const QString &title, QColor c);

    void exec(bool fullScrean = true);

    virtual ~Mediator();


    void setSpeedGen(const Menu::generator_pos_t &speedGen);

    void setSpeedBulletGen(const Menu::generator_pos_t &speedBulletGen);

    void setShotDelayGen(const Menu::generator_time_t &shotDelayGen);

signals:
    void keyEvent(QKeyEvent *ev);
    void endGame(int loser);
    void beginGame();
    void exit();
    void pauseGame();
    void continueGame();
protected:
    virtual bool eventFilter(QObject *obj, QEvent *event) override;
    virtual void timerEvent(QTimerEvent *event) override;

private:
    QGraphicsScene *m_scene;
    QGraphicsView  *m_view;
    QLabel         *m_labMsg;

    bool m_isGame;

//    qreal m_speedW, m_speedH;
//    qreal m_bulletSpeedW, m_bulletSpeedH;
//    duration m_shotDelay;

    Menu::generator_pos_t  m_speedGen;
    Menu::generator_pos_t  m_speedBulletGen;
    Menu::generator_time_t m_shotDelayGen;


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

    void updateSize();
    void showLabMsg(const QString &str);
    void hideLabMsg();


private slots:
    void slotEndGame();
    void deleteBullet(QObject *ptr);
};

#endif // MEDIATOR_HPP
