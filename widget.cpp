#include "widget.h"
#include "ui_widget.h"

#include <mygraphicsscene.hpp>
#include "mygraphicsview.hpp"
#include "player.hpp"
#include <QGraphicsScene>

#include <QGraphicsPolygonItem>

#include <QDebug>
#include <QObject>
#include <QResizeEvent>
#include <QSizePolicy>

#include <sprite.hpp>
#include <thread>

#include <QTimer>

#define M 10
#define N 15

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    auto *ptr_view = new MyGraphicsView(this);
    auto *ptr_scene = new MyGraphicsScene(this);

    ptr_scene->setSceneRect(0, 0, N*W, M*H);

    auto *ptr_player = new Player();
    auto *ptr_player2 = new Player();
    auto *ptr_playyerContrl = new PlayerController(ptr_player, ptr_player2, ptr_scene);

    ptr_playyerContrl->moveToThread(&th);

    connect(this, &Widget::startGame, ptr_playyerContrl, &PlayerController::game);
    connect(this, &Widget::endGame, [=](){
        ptr_playyerContrl->endGame();
    });
    connect(ptr_scene, &MyGraphicsScene::keyEvent, [=](auto event){
        ptr_playyerContrl->keyEvent(event);
    });
    connect(ptr_scene, &MyGraphicsScene::sceneRectChanged, ptr_playyerContrl, &PlayerController::rectSceneChanged);

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

    emit startGame();
}

Widget::~Widget()
{
    emit endGame();

    th.quit();
    th.wait();

    qDebug() << "thread playerConstrol stoped";

    delete ui;
}

