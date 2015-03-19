#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <QGraphicsObject>

class Notification : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit Notification(QGraphicsItem *parent = 0);

    void show(const QString& aText, bool aAutoHide = true);

signals:
    void dismissed();

private slots:
    void fadeOutComplete();
    void fadeInComplete();

    void timerComplete();

private:
    void fade(bool aIn);

     void mousePressEvent(QGraphicsSceneMouseEvent *event);

    QRectF boundingRect() const ;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0) ;

    QGraphicsTextItem* iText;
    QGraphicsPixmapItem* iBackground;

    bool iAutoHide;

};

#endif // NOTIFICATION_H
