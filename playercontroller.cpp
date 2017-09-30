#include "playercontroller.hpp"

#include <QDebug>
#include <thread>
#include "sprite.hpp"


// FIX: сделать нормальную скорость
#define DEL_DELAY 180
#define SPEED_W ((0.1 * 120) / DEL_DELAY)
#define SPEED_H ((0.3 * 120) / DEL_DELAY)

PlayerController::PlayerController(const PlayerSprite *pl_1, const PlayerSprite *pl_2, const QGraphicsScene *scene)
    : m_players{ {pl_1, &PlayerController::newPos_forPl_1},
                 {pl_2, &PlayerController::newPos_forPl_2} },
      m_scene(scene)
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

    connect(this, m_players[0].signal, m_players[0].m_pl, &PlayerSprite::editPos);
    connect(this, m_players[1].signal, m_players[1].m_pl, &PlayerSprite::editPos);

}

void PlayerController::game()
{
    using namespace std::chrono_literals;

    // true - if has collision
    // false - if has not collision
    auto check_collision = [](const QList<QGraphicsItem*> &list, const QGraphicsItem *ignoreObj) {
        for(const auto& item : list) {
            if (item->type() != static_cast<int>(typeItems::ignoreCollize)
                    && item != ignoreObj) {
                return true;
            }
        }
        return false;
    };

    m_game = true;
    while (m_game) {
        bool ctrl = false;
        auto start = std::chrono::steady_clock::now();

        for(int i = 0; i < 2; ++i){
            // BUG: при долгом соприкосновении крештся
            // FIX: при недоступности перемещении по одной координате, но возможности по другой - не перемещает
            qreal Y =  m_players[i].m_pl->y()
                      -SPEED_H * (m_players[i].m_dir & dir_cast(dir::Up))
                      +SPEED_H * (m_players[i].m_dir & dir_cast(dir::Down));

            qreal X =  m_players[i].m_pl->x()
                      -SPEED_W * (m_players[i].m_dir & dir_cast(dir::Left))
                      +SPEED_W * (m_players[i].m_dir & dir_cast(dir::Right));

            if(qFuzzyCompare(Y, m_players[i].m_pl->y()) && qFuzzyCompare(X, m_players[i].m_pl->x())){
                continue;
            }


            if(0 <= X && X <= (m_rectScene.width()  - m_players[i].m_pl->width()) &&
               0 <= Y && Y <= (m_rectScene.height() - m_players[i].m_pl->height())) {

                auto list = m_scene->items({X, Y, static_cast<qreal>(m_players[i].m_pl->width()),
                                                  static_cast<qreal>(m_players[i].m_pl->height())});
                if(!check_collision(list, m_players[i].m_pl)) {
                    emit (this->*m_players[i].signal)(X, Y);
                    qDebug() << "emit";
                }
            }
        }


        // TODO: добавить спрайты выстрелов

        std::this_thread::sleep_for(1000ms/DEL_DELAY);

        auto end = std::chrono::steady_clock::now();
        if(ctrl){
            qDebug() << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() << "n";
            qDebug() << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "mk";
            qDebug() << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "m";
            qDebug() << std::chrono::duration_cast<std::chrono::seconds>(end - start).count() << "sec";
            qDebug() << "----------------------";
        }
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
                m_players[0].m_dir |= dir_cast(dir::Up);
                break;
            case Qt::Key_S:
                m_players[0].m_dir |= dir_cast(dir::Down);
                break;
            case Qt::Key_D:
                m_players[0].m_dir |= dir_cast(dir::Right);
                break;
            case Qt::Key_A:
                m_players[0].m_dir |= dir_cast(dir::Left);
                break;
        // player2
            case Qt::Key_8:
                if(event->modifiers()){
                    m_players[1].m_dir |= dir_cast(dir::Up);
                }
                break;
            case Qt::Key_5:
                if(event->modifiers()){
                    m_players[1].m_dir |= dir_cast(dir::Down);
                }
                break;
            case Qt::Key_6:
                if(event->modifiers()){
                    m_players[1].m_dir |= dir_cast(dir::Right);
                }
                break;
            case Qt::Key_4:
                if(event->modifiers()){
                    m_players[1].m_dir |= dir_cast(dir::Left);
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
