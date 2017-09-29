#include "player.hpp"

#include <QGraphicsScene>
#include <QPainter>
#include <QThread>
#include <QDebug>
#include <thread>
#include <chrono>

#include <QCoreApplication>
#include <QtCore>

#define DEL_DELAY 180

#define W 20
#define H 20

#define SPEED_W ((0.3 * 120) / DEL_DELAY)
#define SPEED_H ((0.3 * 120) / DEL_DELAY)

Player::Player(const QVector<QRect> &coords_img, const QImage &img, QSize size)
    : Sprite(coords_img, img, size)
{

}

void Player::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
//    qDebug() << "painted";
    painter->fillRect(0, 0, W, H, Qt::green);

    Q_UNUSED(option);
    Q_UNUSED(widget);

}

QRectF Player::boundingRect() const
{
    return {0, 0, W, H};
}

void Player::editPos(qreal x, qreal y)
{
    qDebug() << "pos edit";
    this->setPos(x, y);
}

PlayerController::PlayerController(const Player *pl_1, const Player *pl_2, const QGraphicsScene *scene)
    : m_players{ {pl_1}, {pl_2} }, m_scene(scene)
{
    if(m_players[0].m_pl == nullptr) {
        throw std::invalid_argument("PlayerController(): player 1 is nullptr");
    }
    if(m_players[1].m_pl == nullptr) {
        throw std::invalid_argument("PlayerController(): player 2 is nullptr");
    }
    if(m_scene == nullptr) {
        throw std::invalid_argument("PlayerController(): scene is nullptr");
    }

    m_rectScene = m_scene->sceneRect();

    connect(this, &PlayerController::newPos_forPl_1, m_players[0].m_pl, &Player::editPos);
    connect(this, &PlayerController::newPos_forPl_2, m_players[1].m_pl, &Player::editPos);

}

void PlayerController::game()
{
    using namespace std::chrono_literals;

    m_game = true;
    while (m_game) {
        auto start = std::chrono::steady_clock::now();

        bool editPos_pl[2] = { 0 };
        // player1
        qreal x[2] = {m_players[0].m_pl->x(), m_players[1].m_pl->x()};
        qreal y[2] = {m_players[0].m_pl->y(), m_players[1].m_pl->y()};

        for(int i = 0; i < 2; ++i){
            if(m_players[i].m_dir & static_cast<ushort>(dir::Up)) {
                y[i] -= SPEED_H;
                if(y[i] < 0) {
                    y[i] = 0;
                }
                editPos_pl[i] = true;
            }
            if(m_players[i].m_dir & static_cast<ushort>(dir::Down)) {
                y[i] += SPEED_H;
                if(y[i] > m_rectScene.height() - H) {
                    y[i] = m_rectScene.height() - H + 3;
                }
                editPos_pl[i] = true;
            }
            if(m_players[i].m_dir & static_cast<ushort>(dir::Right)) {
                x[i] += SPEED_W;
                if(x[i] > m_rectScene.width() - W){
                    x[i] = m_rectScene.width() - W + 3;
                }
                editPos_pl[i] = true;
            }
            if(m_players[i].m_dir & static_cast<ushort>(dir::Left)) {
                x[i] -= SPEED_W;
                if(x[i] < 0){
                    x[i] = 0;
                }
                editPos_pl[i] = true;
            }
        }

        if(editPos_pl[0]){
            emit newPos_forPl_1(x[0], y[0]);
        }
        if(editPos_pl[1]){
            emit newPos_forPl_2(x[1], y[1]);
        }

        std::this_thread::sleep_for(1000ms/DEL_DELAY);

        auto end = std::chrono::steady_clock::now();
//        qDebug() << (end - start).count();
    }


    qDebug() << "game loop";
}

void PlayerController::keyEvent(QKeyEvent *event)
{
    qDebug() << "keyEvent";
    if(event->type() == QEvent::KeyPress) {

        // TODO: Сделать bind кнопок
        switch (event->key()) {
        // player1
            case Qt::Key_W:
                m_players[0].m_dir |= static_cast<ushort>(dir::Up);
                break;
            case Qt::Key_S:
                m_players[0].m_dir |= static_cast<ushort>(dir::Down);
                break;
            case Qt::Key_D:
                m_players[0].m_dir |= static_cast<ushort>(dir::Right);
                break;
            case Qt::Key_A:
                m_players[0].m_dir |= static_cast<ushort>(dir::Left);
                break;
        // player2
            case Qt::Key_8:
                if(event->modifiers()){
                    m_players[1].m_dir |= static_cast<ushort>(dir::Up);
                }
                break;
            case Qt::Key_5:
                if(event->modifiers()){
                    m_players[1].m_dir |= static_cast<ushort>(dir::Down);
                }
                break;
            case Qt::Key_6:
                if(event->modifiers()){
                    m_players[1].m_dir |= static_cast<ushort>(dir::Right);
                }
                break;
            case Qt::Key_4:
                if(event->modifiers()){
                    m_players[1].m_dir |= static_cast<ushort>(dir::Left);
                }
                break;

            default:
                qDebug() << "unreg key: " << event->key();
                break;
        }


    }
    else if(event->type() == QEvent::KeyRelease) {
        switch (event->key()) {
        // player1
            case Qt::Key_W:
                m_players[0].m_dir &= ~static_cast<ushort>(dir::Up);
                break;
            case Qt::Key_S:
                m_players[0].m_dir &= ~static_cast<ushort>(dir::Down);
                break;
            case Qt::Key_D:
                m_players[0].m_dir &= ~static_cast<ushort>(dir::Right);
                break;
            case Qt::Key_A:
                m_players[0].m_dir &= ~static_cast<ushort>(dir::Left);
                break;
        // player2
            case Qt::Key_8:
                if(event->modifiers()){
                    m_players[1].m_dir &= ~static_cast<ushort>(dir::Up);
                }
                break;
            case Qt::Key_5:
                if(event->modifiers()){
                    m_players[1].m_dir &= ~static_cast<ushort>(dir::Down);
                }
                break;
            case Qt::Key_6:
                if(event->modifiers()){
                    m_players[1].m_dir &= ~static_cast<ushort>(dir::Right);
                }
                break;
            case Qt::Key_4:
                if(event->modifiers()){
                    m_players[1].m_dir &= ~static_cast<ushort>(dir::Left);
                }
                break;
        default:
            qDebug() << "unreg key: " << event->key();
            break;
        }
    }
}

void PlayerController::rectSceneChanged(const QRectF &rect)
{
    qDebug() << "rectSceneChanged: " << m_rectScene << rect;
    m_rectScene = rect;
}

void PlayerController::endGame()
{
    m_game = false;
}

bool PlayerController::moveTo(qreal newX, qreal newY, const Player *player)
{
    // TODO: сделать метод длины и высоты
    if( 0 <= newX && newX <= (m_rectScene.width()  /*- player->width()*/) &&
        0 <= newY && newY <= (m_rectScene.height() /*- player->height()*/)  ) {
        auto oldX = player->x();
        auto oldY = player->y();

        m_scene->collidingItems(player);

        // TODO: доделать
    }
}
