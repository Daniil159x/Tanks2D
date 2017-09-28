#ifndef MYGRAPHICSVIEW_HPP
#define MYGRAPHICSVIEW_HPP

#include <QGraphicsView>
#include <QObject>

class MyGraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    MyGraphicsView(QWidget *parent = nullptr);


signals:
    void resizeEv(QResizeEvent *ev);

protected:
    virtual void drawBackground(QPainter *painter, const QRectF &rect) override;

    virtual void resizeEvent(QResizeEvent *event) override;

private:
    QImage m_background;
};

#endif // MYGRAPHICSVIEW_HPP
