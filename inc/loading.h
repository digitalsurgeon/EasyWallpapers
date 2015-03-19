#ifndef LOADING_H
#define LOADING_H

#include <QObject>
#include <QGraphicsItem>
#include <QBasicTimer>
#include <QPainter>
#include <QSharedPointer>
#include <QList>
#include <QPixmap>

class Loading : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
public:
    Loading(QGraphicsItem *parent = 0);
    virtual ~Loading() {}

    void loadImages();

    QRectF boundingRect () const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

    void start() {  show(); m_ticker.start(50, this); }
    void stop() { m_ticker.stop(); hide(); }

protected:
    void timerEvent(QTimerEvent *event);

private:
    int iCurrentImageIndex;
    QRectF iBoundingRect;
    QBasicTimer m_ticker;

    typedef QSharedPointer<QPixmap> QPixmapPointer;
    QList<QPixmapPointer> iLoadingImages;

};

#endif // LOADING_H
