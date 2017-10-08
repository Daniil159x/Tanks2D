#include "mediator.hpp"

#include "playersprite.hpp"
#include "playercontroller.hpp"

#include <QDebug>

Mediator::Mediator(const QString &fileName) : m_map(fileName)
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

    m_controller = new PlayerController(m_players[0], m_players[1], m_listBullet, m_scene);

    m_view->setScene(m_scene);

    QTimer::singleShot(100, [&](){
        m_view->fitInView(m_scene->sceneRect(), Qt::KeepAspectRatio);
        m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
        m_view->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
    });

}

void Mediator::exec()
{

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
    QObject::connect(m_controller, &PlayerController::damage, this, [&](BulletSprite *ptr_out, Sprite *ptr_in){
        ptr_out->collision();
        // TODO: сделать обработку игроков
        if(ptr_in->type() != static_cast<int>(typeItems::block)){
            ptr_in->nextFrame();
        }
        this->m_scene->update();
    });

    // create bullet
    QObject::connect(m_controller, &PlayerController::createBuller, this, [&](qreal x, qreal y, dir d){
        auto bullet = new BulletSprite(this->m_map, m_map.getSize_sprites(), d);
        bullet->setPos(x, y);
        bullet->setZValue(10);
        m_listBullet << bullet;
        QObject::connect(bullet, &BulletSprite::destroyed, this, [&](QObject *ptr){
            m_listBullet.removeOne(static_cast<BulletSprite*>(ptr));
        });

        this->m_scene->addItem(bullet);
        this->m_scene->update();
        this->m_view->update();
//        this->m_view->repaint();
    });

    // end game
    QObject::connect(this, &Mediator::endGame, m_controller, &PlayerController::endGame);

    // controller to thread
    QObject::connect(this, &Mediator::beginGame, m_controller, &PlayerController::game);
    m_controller->moveToThread(&m_threadController);
    m_threadController.start();

    emit beginGame();

//    m_view->showFullScreen();
    m_view->show();
}

Mediator::~Mediator()
{
    emit endGame();
    // FIX: костыль, исправить
    m_controller->endGame();

    m_threadController.quit();
    m_threadController.wait();

    delete m_view;
    delete m_controller;
}

bool Mediator::eventFilter(QObject *obj, QEvent *event)
{
    Q_UNUSED(obj);
    if(event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease) {
//        qDebug() << event;
//        emit keyEvent(static_cast<QKeyEvent*>(event));
        m_controller->keyEvent(*static_cast<QKeyEvent*>(event));
        return true;
    }
    return false;
}
