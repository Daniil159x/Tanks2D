#include "playercontroller.hpp"

#include <QDebug>
#include <thread>
#include "sprite.hpp"

using namespace std::chrono_literals;

// FIX: сделать нормальную скорость
#define DEL_DELAY 120
#define SPEED_W ((0.2 * 120) / DEL_DELAY)
#define SPEED_H ((0.3 * 120) / DEL_DELAY)

#define SHOOT_DELAY 50ms

PlayerController::PlayerController(const PlayerSprite *pl_1, const PlayerSprite *pl_2, const QList<BulletSprite*> &listBullet,
                                   const QGraphicsScene *scene)
    : m_players{ {pl_1, &PlayerController::newPos_forPl_1, &PlayerController::newDir_forPl_1, &PlayerController::damage_forPl_1},
                 {pl_2, &PlayerController::newPos_forPl_2, &PlayerController::newDir_forPl_2, &PlayerController::damage_forPl_2} },
      m_bulletList(listBullet),
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

    connect(this, m_players[0].signal_newPos, m_players[0].m_pl, &PlayerSprite::editPos);
    connect(this, m_players[1].signal_newPos, m_players[1].m_pl, &PlayerSprite::editPos);

    connect(this, m_players[0].signal_newDir, m_players[0].m_pl, &PlayerSprite::editDir);
    connect(this, m_players[1].signal_newDir, m_players[1].m_pl, &PlayerSprite::editDir);

    connect(this, m_players[0].signal_damage, m_players[0].m_pl, &PlayerSprite::deleteLater);
    connect(this, m_players[1].signal_damage, m_players[1].m_pl, &PlayerSprite::deleteLater);


}

void PlayerController::game()
{
    using namespace std::chrono_literals;

    // 2 - if has bullet
    // 1 - if has collision
    // 0 - if has not collision
    auto check_collision = [](const QList<QGraphicsItem*> &list, const QGraphicsItem *ignoreObj) {
        for(const auto& item : list) {
            if (item->type() != static_cast<int>(typeItems::ignoreCollize)
                    && item != ignoreObj) {
                return 1;
            }
            else if(item->type() == static_cast<int>(typeItems::bullet)) {
                return 2;
            }
        }
        return 0;
    };

    m_game = true;
    while (m_game) {
        bool ctrl = false;
        auto start = std::chrono::steady_clock::now();

        // players
        for(int i = 0; i < 2; ++i){
            // NOTE: возможен баг с тем, что игрок получил урон, а снаряд пролетел
            qreal Y =  m_players[i].m_pl->y()
                      -SPEED_H * bool(m_players[i].m_dir & dir_cast(dir::Up))
                      +SPEED_H * bool(m_players[i].m_dir & dir_cast(dir::Down));

            // check possible Y
            if(!qFuzzyCompare(Y, m_players[i].m_pl->y()) &&
                    0 <= Y && Y <= (m_rectScene.height() - m_players[i].m_pl->height())) {

                // check dir player
                dir to = (m_players[i].m_pl->y() - Y < 0)? dir::Down : dir::Up;
                if(m_players[i].m_pl->m_dir != to){
                    emit (this->*m_players[i].signal_newDir)(to);
                    continue;
                }

                // check collision
                auto list = m_scene->items({m_players[i].m_pl->x(), Y, static_cast<qreal>(m_players[i].m_pl->width()),
                                                                       static_cast<qreal>(m_players[i].m_pl->height())});
                auto res_check = check_collision(list, m_players[i].m_pl);

                // to move
                if(res_check == 0){
                    emit (this->*m_players[i].signal_newPos)(m_players[i].m_pl->x(), Y);
                    continue;
                }
                // move is impossible
                else if(res_check == 1) {
                    // nothing
                }
                // to move and damage
                else if(res_check == 2) {
                    emit (this->*m_players[i].signal_newPos)(m_players[i].m_pl->x(), Y);
                    emit (this->*m_players[i].signal_damage)();
                    qDebug() << "player" << i+1 << "killed";
                    continue;
                }
            }


            qreal X =  m_players[i].m_pl->x()
                      -SPEED_W * bool(m_players[i].m_dir & dir_cast(dir::Left))
                      +SPEED_W * bool(m_players[i].m_dir & dir_cast(dir::Right));

            // check possible X
            if(!qFuzzyCompare(X, m_players[i].m_pl->x()) &&
                    0 <= X && X <= (m_rectScene.width()  - m_players[i].m_pl->width())) {

                // check dir player
                dir to = (m_players[i].m_pl->x() - X < 0)? dir::Right : dir::Left;
                if(m_players[i].m_pl->m_dir != to){
                    emit (this->*m_players[i].signal_newDir)(to);
                    continue;
                }

                // check collision
                auto list = m_scene->items({X, m_players[i].m_pl->y(), static_cast<qreal>(m_players[i].m_pl->width()),
                                                                       static_cast<qreal>(m_players[i].m_pl->height())});
                auto res_check = check_collision(list, m_players[i].m_pl);

                // to move
                if(res_check == 0){
                    emit (this->*m_players[i].signal_newPos)(X, m_players[i].m_pl->y());
                    continue;
                }
                // move is impossible
                else if(res_check == 1) {
                    // nothing
                }
                // to move and damage
                else if(res_check == 2) {
                    emit (this->*m_players[i].signal_newPos)(X, m_players[i].m_pl->y());
                    emit (this->*m_players[i].signal_damage)();
                    qDebug() << "player" << i+1 << "killed";
                    continue;
                }
            }
        }


        // TODO: добавить спрайты выстрелов
        qDebug() << "numbers of bullet = " << m_bulletList.size();


        std::this_thread::sleep_for(1000ms/DEL_DELAY);

        auto end = std::chrono::steady_clock::now();
        if(ctrl){
            qDebug() << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()  << "n";
            qDebug() << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "mk";
            qDebug() << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "m";
            qDebug() << std::chrono::duration_cast<std::chrono::seconds>(end - start).count()      << "sec";
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
            case Qt::Key_Space:
                emitCreateBuller(m_players[0]);
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
            case Qt::Key_0:
                if(event->modifiers()){
                    emitCreateBuller(m_players[1]);
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

void PlayerController::emitCreateBuller(data &dt)
{
    auto time = clock::now();

    if((time - dt.m_timeLastShoot) > SHOOT_DELAY){
        dt.m_timeLastShoot = time;
        auto point = dt.m_pl->getMuzzle();

        emit createBuller(point.x(), point.y(), dt.m_pl->m_dir);
    }
}
