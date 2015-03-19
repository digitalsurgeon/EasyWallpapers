#include "thumbnailholder.h"

#if defined(Q_OS_SYMBIAN)
#include <e32std.h>
#include <touchfeedback.h>
#endif


ThumbnailHolder::ThumbnailHolder() : iPixmap(NULL)
{
    setFlag(QGraphicsItem::ItemClipsToShape);
}

ThumbnailHolder::~ThumbnailHolder()
{
    delete iPixmap;
}

QRectF ThumbnailHolder::boundingRect() const
{
    if (!iPixmap)
        return QRectF(0,0,420,420);

    return QRectF(QPointF(0,0), iPixmap->size());
}

void ThumbnailHolder::paint(QPainter *painter, const QStyleOptionGraphicsItem *,
                            QWidget *)
{
    if(iPixmap)
    {
        const QRect br = boundingRect().toRect();
        painter->drawPixmap(br, *iPixmap);
    }

}

void ThumbnailHolder::mousePressEvent(QGraphicsSceneMouseEvent *)
{
#if defined(Q_OS_SYMBIAN)
            MTouchFeedback* feedBack = MTouchFeedback::Instance();
            if (feedBack)
            {
                feedBack->InstantFeedback(ETouchFeedbackBasic);
            }
#endif
    emit thmbnailTapped();
}

void ThumbnailHolder::setPixmap(const QPixmap &pixmap)
{
    delete iPixmap;
    prepareGeometryChange();
    iPixmap = new QPixmap(pixmap);
}


void ThumbnailHolder::setImageUrl ( const QString& aUrl)
{
    iImageUrl = aUrl;
}


QString ThumbnailHolder::imageUrl() const
{
    return iImageUrl;
}
