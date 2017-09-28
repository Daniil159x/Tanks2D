#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <QGraphicsObject>
#include <QKeyEvent>
#include <QKeySequence>
#include <chrono>

class Player : public QGraphicsObject
{
    Q_OBJECT

public:
    Player();

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                       QWidget *widget = nullptr) override;

    virtual QRectF boundingRect() const override;

public slots:
    void editPos(qreal x, qreal y);
};


class PlayerController : public QObject
{
    Q_OBJECT

public:
    PlayerController(const Player *pl_1, const Player *pl_2, const QGraphicsScene *scene);
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
    enum class dir {
        No    = 0,
        Up    = 1 << 0,
        Down  = 1 << 1,
        Left  = 1 << 2,
        Right = 1 << 3
    };
    // FIXME: что то сделать с выравниванием структуры
    struct data {
        const Player *m_pl = nullptr;
        ushort m_dir = static_cast<ushort>(dir::No);
    } m_players[2];

    QRectF m_rectScene;
    const QGraphicsScene *m_scene;
};

#endif // PLAYER_HPP
