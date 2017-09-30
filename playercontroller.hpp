#ifndef PLAYERCONTROLLER_HPP
#define PLAYERCONTROLLER_HPP

#include <QObject>
#include <QRect>
#include <QGraphicsScene>

#include "enums.hpp"
#include "playersprite.hpp"


class PlayerController : public QObject
{
    Q_OBJECT

public:
    PlayerController(const PlayerSprite *pl_1, const PlayerSprite *pl_2, const QGraphicsScene *scene);
public slots:
    void game();
    void keyEvent(QKeyEvent *event);
    void rectSceneChanged(const QRectF &rect);
    void endGame();

signals:
    void newPos_forPl_1(qreal x, qreal y);
    void newPos_forPl_2(qreal x, qreal y);

private:

    bool m_game = false;
    // FIXME: что то сделать с выравниванием структуры
    struct data {
        const PlayerSprite *m_pl = nullptr;
        void (PlayerController::* signal)(qreal, qreal) = nullptr;
        ushort m_dir = dir_cast(dir::No);
    } m_players[2];

    QRectF m_rectScene;
    const QGraphicsScene *m_scene;


    // test_sprite used in possibleMoveTo
    // for receiving objects in the test point
    class testSprite : public QGraphicsItem {
    public:
        QRectF boundingRect() const {
            return {0, 0, 1, 1};
        }
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
            Q_UNUSED(painter);
            Q_UNUSED(option);
            Q_UNUSED(widget);
        }
        int type() const {
            return static_cast<int>(typeItems::ignoreCollize);
        }
    } m_testing;
};

#endif // PLAYERCONTROLLER_HPP
