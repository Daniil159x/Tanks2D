#include "playercontroller.hpp"
#include "sprite.hpp"

#include <QDebug>
#include <thread>


using namespace std::chrono_literals;

// FIX: сделать нормальную скорость
#define DEL_DELAY 120
#define SPEED_W ((0.3 * 120) / DEL_DELAY)
#define SPEED_H ((0.3 * 120) / DEL_DELAY)

#define SPEED_W_BULLET SPEED_W*2
#define SPEED_H_BULLET SPEED_H*2

#define SHOOT_DELAY 50ms

PlayerController::PlayerController(PlayerSprite *pl_1, PlayerSprite *pl_2, const QList<BulletSprite*> &listBullet,
                                   const QGraphicsScene *scene)
    : m_players{ {pl_1},
                 {pl_2} },
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
}

void PlayerController::game()
{
    using namespace std::chrono_literals;

    m_game = true;
    while (m_game) {
//        qDebug() << "loop";
        /// players
        for(auto &dataPl : m_players){
            // NOTE: возможен баг с тем, что игрок получил урон, а снаряд пролетел
            qreal Y =  dataPl.m_pl->y()
                      -SPEED_H * bool(dataPl.m_dir & dir_cast(dir::Up))
                      +SPEED_H * bool(dataPl.m_dir & dir_cast(dir::Down));

            // check possible Y
            if(!qFuzzyCompare(Y, dataPl.m_pl->y()) &&
                    0 <= Y && Y <= (m_rectScene.height() - dataPl.m_pl->height())) {

                // check dir player
                dir to = (dataPl.m_pl->y() - Y < 0)? dir::Down : dir::Up;
                if(dataPl.m_pl->m_dir != to){
                    emit newDir_Player(to, dataPl.m_pl);
                    continue;
                }

                // check collision
//                qDebug() << "check collision Y calling...";
                auto list = m_scene->items({dataPl.m_pl->x(), Y, static_cast<qreal>(dataPl.m_pl->width()),
                                                                 static_cast<qreal>(dataPl.m_pl->height())});
//                qDebug() << "check collision Y called";
                bool collision = false;
                for(auto item : list) {
                    if(item == dataPl.m_pl) {
                        continue;
                    }
                    else if(item->type() == static_cast<int>(typeItems::ignoreCollize)) {
                        continue;
                    }
                    else if(item->type() == static_cast<int>(typeItems::bullet)) {
                        emit newPos(dataPl.m_pl->x(), Y, dataPl.m_pl);
                        emit damage(static_cast<BulletSprite*>(item), dataPl.m_pl);
                        collision = true;
                        break;
                    }
                    else {
                        collision = true;
                        break;
                    }
                }

                if(!collision) {
                    emit newPos(dataPl.m_pl->x(), Y, dataPl.m_pl);
                }
            }


            qreal X =  dataPl.m_pl->x()
                      -SPEED_W * bool(dataPl.m_dir & dir_cast(dir::Left))
                      +SPEED_W * bool(dataPl.m_dir & dir_cast(dir::Right));

            // check possible X
            if(!qFuzzyCompare(X, dataPl.m_pl->x()) &&
                    0 <= X && X <= (m_rectScene.width() - dataPl.m_pl->width())) {

                // check dir player
                dir to = (dataPl.m_pl->x() - X < 0)? dir::Right : dir::Left;
                if(dataPl.m_pl->m_dir != to){
                    emit newDir_Player(to, dataPl.m_pl);
                    continue;
                }

                // check collision
//                qDebug() << "check collision X calling...";
                auto list = m_scene->items({X, dataPl.m_pl->y(), static_cast<qreal>(dataPl.m_pl->width()),
                                                                 static_cast<qreal>(dataPl.m_pl->height())});
//                qDebug() << "check collision X called";
                bool collision = false;
                for(auto item : list) {
                    if(item == dataPl.m_pl || item->type() == static_cast<int>(typeItems::ignoreCollize)) {
                        continue;
                    }
                    else if(item->type() == static_cast<int>(typeItems::bullet)) {
                        emit newPos(X, dataPl.m_pl->y(), dataPl.m_pl);
                        emit damage(static_cast<BulletSprite*>(item), dataPl.m_pl);
                        collision = true;
                        break;
                    }
                    else {
                        collision = true;
                        break;
                    }
                }

                if(!collision) {
                    emit newPos(X, dataPl.m_pl->y(), dataPl.m_pl);
                }
            }
        }
//        qDebug() << "player worked. sleep 300us";
        // TODO: придумать константу
        std::this_thread::sleep_for(300us);



        static auto collision_bullet = [&](BulletSprite *bullet, qreal x, qreal y){
            emit newPos(x, y, bullet);
            if(bullet->isVisible()){
                for(auto item : m_scene->collidingItems(bullet)) {
                    if(item == bullet || item->type() == static_cast<int>(typeItems::ignoreCollize)) {
                        continue;
                    }
                    else {
                        // FIX: проверить есть ли тут баг
                        qDebug() << dynamic_cast<Sprite*>(item);
                        emit damage(bullet, dynamic_cast<Sprite*>(item));
                    }
                }
            }
        };

//        qDebug() << "before for: bullet";
        for(const auto &bullet : m_bulletList) {
            if(bullet->m_status == BulletSprite::status::fly) {
                switch (bullet->m_dir) {
                    case dir::Up:
                        collision_bullet(bullet, bullet->x(), bullet->y() - SPEED_H_BULLET);
                        break;
                    case dir::Down:
                        collision_bullet(bullet, bullet->x(), bullet->y() + SPEED_H_BULLET);
                        break;
                    case dir::Right:
                        collision_bullet(bullet, bullet->x() + SPEED_W_BULLET, bullet->y());
                        break;
                    case dir::Left:
                        collision_bullet(bullet, bullet->x() - SPEED_W_BULLET, bullet->y());
                        break;
                    default:
                        throw std::logic_error("PlayerController::game() - bullet collision: unaccounted direction");
                        break;
                }
            }
        }

        std::this_thread::sleep_for(1000ms/DEL_DELAY);
    }
}

// FIX: копия - это костыль
void PlayerController::keyEvent(QKeyEvent event)
{
    qDebug() << "keyEvent";
    if(event.type() == QEvent::KeyPress) {

        // TODO: Сделать bind кнопок
        switch (event.key()) {
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
                if(event.modifiers()){
                    m_players[1].m_dir |= dir_cast(dir::Up);
                }
                break;
            case Qt::Key_5:
                if(event.modifiers()){
                    m_players[1].m_dir |= dir_cast(dir::Down);
                }
                break;
            case Qt::Key_6:
                if(event.modifiers()){
                    m_players[1].m_dir |= dir_cast(dir::Right);
                }
                break;
            case Qt::Key_4:
                if(event.modifiers()){
                    m_players[1].m_dir |= dir_cast(dir::Left);
                }
                break;
            case Qt::Key_0:
                if(event.modifiers()){
                    emitCreateBuller(m_players[1]);
                }
                break;
            default:
                qDebug() << "unreg key: " << event.key();
                break;
        }


    }
    else if(event.type() == QEvent::KeyRelease) {
        switch (event.key()) {
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
                if(event.modifiers()){
                    m_players[1].m_dir &= ~static_cast<ushort>(dir::Up);
                }
                break;
            case Qt::Key_5:
                if(event.modifiers()){
                    m_players[1].m_dir &= ~static_cast<ushort>(dir::Down);
                }
                break;
            case Qt::Key_6:
                if(event.modifiers()){
                    m_players[1].m_dir &= ~static_cast<ushort>(dir::Right);
                }
                break;
            case Qt::Key_4:
                if(event.modifiers()){
                    m_players[1].m_dir &= ~static_cast<ushort>(dir::Left);
                }
                break;
            case Qt::Key_Space:
                break;
            case Qt::Key_0:
                break;
        default:
            qDebug() << "unreg key: " << event.key();
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
