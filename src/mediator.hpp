#ifndef MEDIATOR_HPP
#define MEDIATOR_HPP

#include <QGraphicsScene>
#include <QGraphicsView>

#include <QObject>
#include <QThread>

#include "enums.hpp"
#include "mapfield.hpp"
#include "bulletsprite.hpp"
#include "playersprite.hpp"
#include "playercontroller.hpp"


class Mediator : public QObject
{
    Q_OBJECT

public:
    explicit Mediator(const QString &fileName);

    void exec();

    virtual ~Mediator();
signals:
    void keyEvent(QKeyEvent *ev);
    void endGame();
    void beginGame();
protected:
    virtual bool eventFilter(QObject *obj, QEvent *event) override;

    void updateGame();
private:
    QGraphicsScene *m_scene;
    QGraphicsView  *m_view;

    PlayerSprite     *m_players[2];
    PlayerController *m_controller;

    QList<BulletSprite*> m_listBullet;

    MapField m_map;

    QTimer  m_timer;
    QThread m_threadController;
};

#endif // MEDIATOR_HPP
