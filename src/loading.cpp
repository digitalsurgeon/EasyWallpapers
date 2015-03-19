#include "loading.h"

static const int kProgressImageCount = 25;

void Loading::loadImages()
{
    if (iLoadingImages.count())
        return;

    for (int i = 1; i <= kProgressImageCount; i++)
    {
        const QString resourceString = ":/images/loading_" +  QString::number(i) + ".png";
        const QPixmapPointer img ( new QPixmap (resourceString) );
        iLoadingImages.append(img);
    }

    iBoundingRect = QRectF (0.0,0.0, iLoadingImages[0]->width(), iLoadingImages[0]->height()  );
}

Loading::Loading(QGraphicsItem *parent)
    : QGraphicsItem(parent)
    , iCurrentImageIndex(0)
{
    loadImages();
}

QRectF Loading::boundingRect () const
{
    return iBoundingRect;
}

void Loading::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->drawPixmap(0, 0, *iLoadingImages[iCurrentImageIndex]);
}

void Loading::timerEvent(QTimerEvent *)
{
    ++iCurrentImageIndex;
    iCurrentImageIndex %= kProgressImageCount;
    update();
}
