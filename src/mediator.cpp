#include "mediator.hpp"

#include "playersprite.hpp"

#include <QDebug>

using namespace std::chrono_literals;

#define DEL_DELAY 120
#define SPEED_W ((0.3 * 120) / DEL_DELAY)
#define SPEED_H ((0.3 * 120) / DEL_DELAY)

#define SPEED_W_BULLET SPEED_W*2
#define SPEED_H_BULLET SPEED_H*2

#define SHOT_DELAY 1s


Mediator::Mediator(const QString &fileName) : m_isGame(false), m_vectorPlayers{0, 0}, m_shot{false, false}, m_map(fileName)
{
    m_scene = new QGraphicsScene(0, 0,
                                 static_cast<qreal>(m_map.getSize_field().width()),
                                 static_cast<qreal>(m_map.getSize_field().height()), this);
    m_view  = new QGraphicsView;

    for(auto &ptr : m_map.getFiledSprites()) {
        m_scene->addItem(static_cast<QGraphicsItem*>(ptr.release()));
    }

    auto vec = m_map.getPlayerSprites();
    m_players[0] = vec[0].release();
    m_players[1] = vec[1].release();
    m_scene->addItem(m_players[0]);
    m_scene->addItem(m_players[1]);

    m_players[0]->setVector(motion_vector::Up);
    m_players[1]->setVector(motion_vector::Up);

    qDebug() << static_cast<QGraphicsItem*>(m_players[0]);
    qDebug() << static_cast<QGraphicsItem*>(m_players[1]);

    m_view->setScene(m_scene);
}

void Mediator::exec()
{
    m_view->installEventFilter(this);

    connect(this, &Mediator::endGame, this, &Mediator::slotEndGame);

//    this->m_view->show();
    this->m_view->showFullScreen();

    QTimer::singleShot(100, [=](){
        m_view->fitInView(m_scene->sceneRect(), Qt::KeepAspectRatio);
        m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
        m_view->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);

        m_isGame = true;
        m_idTimer =  this->startTimer(1000/60);
        emit beginGame();
    });
}

Mediator::~Mediator()
{

    delete m_view;
}

bool Mediator::eventFilter(QObject *obj, QEvent *event)
{
    Q_UNUSED(obj);
    if(event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease) {
        auto eventKey = static_cast<QKeyEvent*>(event);

        switch (eventKey->key()) {
        // player[0]
        case Qt::Key_W:
            m_vectorPlayers[0] ^= motion_vector_cast<int>(motion_vector::Up);
            break;
        case Qt::Key_S:
            m_vectorPlayers[0] ^= motion_vector_cast<int>(motion_vector::Down);
            break;
        case Qt::Key_D:
            m_vectorPlayers[0] ^= motion_vector_cast<int>(motion_vector::Right);
            break;
        case Qt::Key_A:
            m_vectorPlayers[0] ^= motion_vector_cast<int>(motion_vector::Left);
            break;
        // player[1]
        case Qt::Key_8:
            if(eventKey->modifiers()){
                m_vectorPlayers[1] ^= motion_vector_cast<int>(motion_vector::Up);
            }
            break;
        case Qt::Key_5:
            if(eventKey->modifiers()){
                m_vectorPlayers[1] ^= motion_vector_cast<int>(motion_vector::Down);
            }
            break;
        case Qt::Key_6:
            if(eventKey->modifiers()){
                m_vectorPlayers[1] ^= motion_vector_cast<int>(motion_vector::Right);
            }
            break;
        case Qt::Key_4:
            if(eventKey->modifiers()){
                m_vectorPlayers[1] ^= motion_vector_cast<int>(motion_vector::Left);
            }
            break;
        // bullet
        case Qt::Key_Space:
            m_shot[0] = (event->type() == QEvent::KeyPress);
            break;
        case Qt::Key_0:
            if(eventKey->modifiers()) {
                m_shot[1] = (event->type() == QEvent::KeyPress);
            }
            break;
        }

        return true;
    }
    else if(event->type() == QEvent::Resize){
        m_view->fitInView(m_scene->sceneRect(), Qt::KeepAspectRatio);
        return false;
    }
    return false;
}

motion_vector getMainVec(int vec){
    const auto up = motion_vector_cast<int>(motion_vector::Up);
    const auto down = motion_vector_cast<int>(motion_vector::Down);
    const auto left = motion_vector_cast<int>(motion_vector::Left);
    const auto right = motion_vector_cast<int>(motion_vector::Right);

    if(vec & up) {
        return motion_vector::Up;
    }
    else if(vec & down) {
        return motion_vector::Down;
    }
    else if(vec & left) {
        return motion_vector::Left;
    }
    else if(vec & right) {
        return motion_vector::Right;
    }
    else {
        return motion_vector::No;
    }

}

#define GAME_IS_CONTINUE { if(!m_isGame) return; }

void Mediator::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);

    for(int i = 0; i < 2; ++i) {
        GAME_IS_CONTINUE

        movePlayers(i);
    }

    for(int i = 0; i < 2; ++i) {
        GAME_IS_CONTINUE

        checkCreateBullet(i);
    }

    for(auto &bullet : m_listBullet) {
        GAME_IS_CONTINUE

        if(!bullet->isFly()){
            continue;
        }

        moveBullet(bullet);
        for(auto &el : m_scene->collidingItems(bullet)) {
            GAME_IS_CONTINUE
            if(el->type() == static_cast<int>(typeItems::ignoreCollize)) {
                // nothing
            }
            else {
                auto to = dynamic_cast<Sprite*>(el);
                if(to == nullptr) {
                    throw std::logic_error("Mediator::timerEvent: collision item is not inheritor 'Sprite'");
                }
                damage(bullet, to);
            }
        }
    }

    m_scene->update();
    m_view->update();
}



void Mediator::movePlayers(int i)
{

    motion_vector main_vec = getMainVec(m_vectorPlayers[i]);

    if(main_vec != motion_vector::No) {
        qreal Y = -SPEED_H * bool(main_vec == motion_vector::Up)
                  +SPEED_H * bool(main_vec == motion_vector::Down);
        qreal X = -SPEED_W * bool(main_vec == motion_vector::Left)
                  +SPEED_W * bool(main_vec == motion_vector::Right);

        if(qFuzzyIsNull(Y) && qFuzzyIsNull(X)) {
            return;
        }

        m_players[i]->setVector(main_vec);
        m_players[i]->savePos();

        m_players[i]->moveOn(X, Y);

        for(auto &el : m_scene->collidingItems(m_players[i])) {
            if(el->type() == static_cast<int>(typeItems::bullet)){
               damage(static_cast<BulletSprite*>(el), m_players[i]);
            }
            else if(el->type() == static_cast<int>(typeItems::ignoreCollize)){
                continue;
            }
            else {
                qDebug() << "collizion with:" << el->type();
                m_players[i]->restorePos();
                break;
            }
        }
    }
    return;
}

void Mediator::createBullet(qreal x, qreal y, motion_vector vec)
{
    auto bullet = new BulletSprite(m_map);
    bullet->setPos(x, y);
    bullet->setVector(vec);
    bullet->setZValue(10);

    connect(bullet, &BulletSprite::destroyed, this, &Mediator::deleteBullet);

    m_scene->addItem(bullet);
    m_listBullet.push_back(bullet);
}

void Mediator::checkCreateBullet(int i)
{
    auto now = clock::now();
    if(m_shot[i] && (now - m_lastShot[i]) > SHOT_DELAY) {
        m_lastShot[i] = now;
        const auto point = m_players[i]->getMuzzle();
        qDebug() << "bullet created on " << point;
        createBullet(point.x(), point.y(), m_players[i]->getVector());
    }
}

void Mediator::moveBullet(BulletSprite *ptr)
{
    const auto vec = ptr->getVector();

    qreal Y = -SPEED_H_BULLET * bool(vec == motion_vector::Up)
              +SPEED_H_BULLET * bool(vec == motion_vector::Down);
    qreal X = -SPEED_W_BULLET * bool(vec == motion_vector::Left)
              +SPEED_W_BULLET * bool(vec == motion_vector::Right);

    ptr->moveOn(X, Y);
}

void Mediator::damage(BulletSprite *out, Sprite *to)
{
    switch (to->type()) {
        case static_cast<int>(typeItems::block):
            out->collision();
            break;
        case static_cast<int>(typeItems::breakable):
            out->collision();
            to->nextFrame();
            break;
        case static_cast<int>(typeItems::bullet):
            out->collision();
            static_cast<BulletSprite*>(to)->collision();
            break;
        case static_cast<int>(typeItems::player):
            out->collision();
            hitPlayer(static_cast<PlayerSprite*>(to));
            break;
        default:
            throw std::logic_error("Mediator::damage: defoult branch");
    }
}

void Mediator::hitPlayer(PlayerSprite *ptr)
{
    emit endGame(ptr->getNumber());
}

void Mediator::slotEndGame()
{
    m_isGame = false;
    this->killTimer(m_idTimer);
}

void Mediator::deleteBullet(QObject *ptr)
{
    // WARNING: возможен баг, но адреса совпадают
    auto bullet = static_cast<BulletSprite*>(ptr);

    if(!m_listBullet.removeOne(bullet)) {
        throw std::logic_error("Mediator::deleteBullet: m_listBullet is not contains ptr");
    }
}
