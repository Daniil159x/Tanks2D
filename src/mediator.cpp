#include "mediator.hpp"

#include "playersprite.hpp"
#include "playercontroller.hpp"

#include <QDebug>

Mediator::Mediator(const QString &fileName) : m_map(fileName)
{
    qDebug() << "allocate...";
    m_scene = new QGraphicsScene(0, 0,
                                 static_cast<qreal>(m_map.getFieldSize().width() + 3),
                                 static_cast<qreal>(m_map.getFieldSize().height() + 3), this);
    m_view  = new QGraphicsView;

    qDebug() << "init scene...";
    m_scene->setSceneRect({});
    for(auto &ptr : m_map.getFiledSprites()) {
        m_scene->addItem(ptr.release());
    }

    qDebug() << "init players...";
    auto tuple = m_map.getPlayers();
    m_players[0] = std::get<0>(tuple).release();
    m_players[1] = std::get<1>(tuple).release();
    m_scene->addItem(m_players[0]);
    m_scene->addItem(m_players[1]);

    qDebug() << "allocate playerController...";
    m_controller = new PlayerController(m_players[0], m_players[1], m_listBullet, m_scene);

    qDebug() << "init view ...";
    m_view->setScene(m_scene);
    m_view->fitInView(m_scene->sceneRect());
}

void Mediator::exec()
{
    // TODO: добавть коннекты, ивентфильтры...

    // keyboard
    m_view->installEventFilter(this);
    // FIX: костыль с проброской ивентов, исправить
//    QObject::connect(this, &Mediator::keyEvent, m_controller, &PlayerController::keyEvent);

    // edit pos
    QObject::connect(m_controller, &PlayerController::newPos, this, [](qreal x, qreal y, QGraphicsItem *ptr){
        ptr->setPos(x, y);
    });

    // edit dir
    QObject::connect(m_controller, &PlayerController::newDir_Player, this, [](dir d, PlayerSprite *ptr){
        ptr->editDir(d);
    });

    // damage
    QObject::connect(m_controller, &PlayerController::damage, this, [](BulletSprite *ptr_out, Sprite *ptr_in){
        ptr_out->collision();
        ptr_in->nextFrame();
    });

    // create bullet
    QObject::connect(m_controller, &PlayerController::createBuller, this, [&](qreal x, qreal y, dir d){
        qDebug() << "creator bullet";
        auto bullet = new BulletSprite(d, m_map.getBulletImages(), {static_cast<int>(x), static_cast<int>(y)});
        m_listBullet << bullet;
        QObject::connect(bullet, &BulletSprite::destroyed, this, [&](QObject *ptr){
            m_listBullet.removeOne(static_cast<BulletSprite*>(ptr));
        });

        this->m_scene->addItem(bullet);
    });

    // end game
    QObject::connect(this, &Mediator::endGame, m_controller, &PlayerController::endGame);

    // controller to thread
    QObject::connect(this, &Mediator::beginGame, m_controller, &PlayerController::game);
    m_controller->moveToThread(&m_threadController);
    m_threadController.start();
    qDebug() << "emit beginGame...";

    emit beginGame();

    QObject::connect(&m_timer, &QTimer::timeout, this, &Mediator::updateGame);
    m_timer.start(1000/60);

    m_view->show();
}

Mediator::~Mediator()
{
    qDebug() << "emit endGame...";
    emit endGame();
    // FIX: костыль, исправить
    m_controller->endGame();

    m_timer.stop();
    qDebug() << "timer stopped";
    m_threadController.quit();
    m_threadController.wait();
    qDebug() << "thread stopped";

    delete m_view;
    delete m_controller;
}

bool Mediator::eventFilter(QObject *obj, QEvent *event)
{
    Q_UNUSED(obj);
    if(event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease) {
        qDebug() << event;
//        emit keyEvent(static_cast<QKeyEvent*>(event));
        m_controller->keyEvent(*static_cast<QKeyEvent*>(event));
        return true;
    }
    // TODO: изменить
    return false;
}

void Mediator::updateGame()
{
    m_scene->update();
//    m_view->repaint();
}
