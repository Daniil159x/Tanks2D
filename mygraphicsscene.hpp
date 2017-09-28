#ifndef MYGRAPHICSSCENE_HPP
#define MYGRAPHICSSCENE_HPP

#include <QObject>
#include <QGraphicsScene>

class MyGraphicsScene : public QGraphicsScene
{
    Q_OBJECT

public:
    MyGraphicsScene(QObject *parent = Q_NULLPTR);

signals:
    void keyEvent(QKeyEvent *event);

protected:
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void keyReleaseEvent(QKeyEvent *event) override;
};

#endif // MYGRAPHICSSCENE_HPP
