#include "notification.h"

#include <QPropertyAnimation>
#include <QTimer>
#include <QFont>

Notification::Notification(QGraphicsItem *parent) :
    QGraphicsObject(parent),
    iText (new QGraphicsTextItem(this)),
    iBackground ( new QGraphicsPixmapItem(this)),
    iAutoHide ( false )
{
    setFlag(QGraphicsItem::ItemHasNoContents, true);

    setOpacity(0.0);
    hide();

    iBackground->setPixmap(QPixmap(":/images/notification_background.png"));
    iBackground->setZValue(0);
    iText->setZValue(1);
}

void Notification::show(const QString& aText, bool aAutoHide )
{
    iAutoHide = aAutoHide;
    setPos( (parentItem()->boundingRect().width() - boundingRect().width() )/2 ,
                    (parentItem()->boundingRect().height() - boundingRect().height() )/2);

    QFont font = iText->font();

#ifndef Q_OS_SYMBIAN
    font.setPointSize(12);
    font.setBold(true);
#endif

    iText->setFont(font);
    iText->setDefaultTextColor(QColor(Qt::black));

    iText->setPlainText(aText);
    iText->setPos( (iBackground->boundingRect().width() - iText->boundingRect().width() )/2 ,
                    (iBackground->boundingRect().height() - iText->boundingRect().height() )/2);

    QGraphicsObject::show();
    fade(true);
}

QRectF Notification::boundingRect() const
{
    return iBackground->boundingRect();
}

void Notification::paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget * )
{}

 void Notification::mousePressEvent(QGraphicsSceneMouseEvent *)
 {
     timerComplete();
 }

 void Notification::fade(bool aIn)
 {

     QPropertyAnimation* anim = new QPropertyAnimation(this, "opacity");
     anim->setEasingCurve(QEasingCurve::OutExpo);
     anim->setDuration(1000);

     if (aIn)
     {
        connect(anim,SIGNAL(finished()), this, SLOT(fadeInComplete()));
        anim->setEndValue(qreal(0.8));
    }
     else
     {
         connect(anim,SIGNAL(finished()), this, SLOT(fadeOutComplete()));
         anim->setEndValue(qreal(0.0));
     }

     anim->start(QAbstractAnimation::DeleteWhenStopped);

 }

 void Notification::fadeInComplete()
 {
     if (iAutoHide)
        QTimer::singleShot(1000*2, this, SLOT(timerComplete()));
 }

 void Notification::timerComplete()
 {
    fade(false);
 }

 void Notification::fadeOutComplete()
 {
    emit dismissed();

    setOpacity(0.0);
    hide();
 }
