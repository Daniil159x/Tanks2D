#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "mygraphicsview.hpp"
#include "bulletsprite.hpp"

#include <QThread>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
signals:
    void startGame();
    void endGame();

private:
    Ui::Widget *ui;
    QList<BulletSprite*> m_list;
    QThread th;
};

#endif // WIDGET_H
