#include "widget.h"
#include "ui_widget.h"

#include "mygraphicsview.hpp"
#include "playersprite.hpp"
#include "playercontroller.hpp"

#include <QGraphicsScene>

#include <QGraphicsPolygonItem>

#include <QDebug>
#include <QObject>
#include <QResizeEvent>
#include <QSizePolicy>

#include <thread>

#include <QTimer>

#define COL 4
#define ROW 2

#define W 149
#define H 187

#define W_SPRITE W
#define H_SPRITE ( (static_cast<qreal>(H)/W) * W_SPRITE )

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->resize(COL*W_SPRITE, static_cast<int>(ROW*H_SPRITE));
/*

    qDebug() << W_SPRITE << H_SPRITE;

    auto *ptr_view = new MyGraphicsView(this);

    auto *ptr_scene = new MyGraphicsScene(this);
    ptr_scene->setSceneRect(0, 0, COL*W_SPRITE, ROW*H_SPRITE+100);


    QImage img("/home/daniil159x/Qt_project/Tanks2D/test_sprite.jpg");

//    Sprite *array_sprite[8] = { nullptr };

//    for(int i = 0; i < ROW; ++i){
//        for(int j = 0; j < COL; ++j){
//            array_sprite[i * 4 + j] = new Sprite( {{j * W, i * H, W, H}},
//                                                  img,
//                                                  {static_cast<int>(W_SPRITE), static_cast<int>(H_SPRITE)} );
//            array_sprite[i * 4 + j]->setPos(W_SPRITE*j, H_SPRITE*i);

//            ptr_scene->addItem(array_sprite[i * 4 + j]);
//        }
//    }



    auto *ptr_player = new PlayerSprite({
                                            { 0, 0, 0, 0},
                                            { 0, 0, W, H}
                                        }, img, {static_cast<int>(W_SPRITE), static_cast<int>(H_SPRITE)});

    auto *ptr_player2 = new PlayerSprite({
                                             { 0, 0, 0, 0},
                                             { W, H, W, H}
                                         }, img, {static_cast<int>(W_SPRITE), static_cast<int>(H_SPRITE)});
    ptr_player2->setPos(W_SPRITE*2, H_SPRITE*0.5);

    auto *ptr_playyerContrl = new PlayerController(ptr_player, ptr_player2, m_list, ptr_scene);

    ptr_playyerContrl->moveToThread(&th);

    connect(this, &Widget::startGame, ptr_playyerContrl, &PlayerController::game);
    connect(this, &Widget::endGame, [=](){
        ptr_playyerContrl->endGame();
    });
    connect(ptr_scene, &MyGraphicsScene::keyEvent, [=](auto event){
        ptr_playyerContrl->keyEvent(event);
    });
    connect(ptr_scene, &MyGraphicsScene::sceneRectChanged, ptr_playyerContrl, &PlayerController::rectSceneChanged);

    connect(ptr_playyerContrl, &PlayerController::createBuller, this, [=](qreal x, qreal y, dir d){
        auto *bullet = new BulletSprite(d, {
                                            {0, 0, 10, 10},
                                            {10, 10, 10, 10}
                                          }, img, {10, 10});
        switch (d) {
            case dir::Up:
                y -= 11;
                break;
            case dir::Down:
                y += 1;
                break;
            case dir::Left:
                x -= 11;
                break;
            case dir::Right:
                x += 1;
                break;
        }


        bullet->setPos(x, y);
        connect(bullet, &BulletSprite::destroyed, [&](auto ptr){
            m_list.removeOne(static_cast<BulletSprite*>(ptr));
        });
        m_list += bullet;

        ptr_scene->addItem(bullet);
    });

    ptr_scene->addItem(ptr_player);
    ptr_scene->addItem(ptr_player2);

    ptr_view->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    ptr_view->setScene(ptr_scene);

    ui->verticalLayout->addWidget(ptr_view);

    auto *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, ptr_scene, [=](){
//        std::hash<std::thread::id> hasher;
//        qDebug() << "update" << hasher(std::this_thread::get_id());
        ptr_scene->update();
        this->repaint();

    });
    timer->start(1000/20);

    th.start();

    emit startGame(); */
}

Widget::~Widget()
{
    emit endGame();

    th.quit();
    th.wait();

    qDebug() << "thread playerConstrol stoped";

    delete ui;
}

