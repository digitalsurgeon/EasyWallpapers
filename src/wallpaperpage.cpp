#include "wallpaperpage.h"
#include "photo.h"
#include "thumbnailholder.h"
#include "wallpaperpreviewpopup.h"
#include "maincontainer.h"
#include "notification.h"

#include <QNetworkAccessManager>
#include <QGraphicsGridLayout>
#include <QPropertyAnimation>
#include <QDebug>

static const qreal kListHeight = 800.0;
static const QPointF kListPos = QPointF (2, 142);
static const QSize kThumbSize = QSize(200,200);

WallpaperPage::WallpaperPage(QObject* aObjParent,QGraphicsItem *parent, QNetworkAccessManager* aNam) :
    QObject(aObjParent),
    QGraphicsRectItem(parent),
    iBackground ( new QGraphicsPixmapItem(this)),
    iNam (aNam),
    iLayoutWidget ( new QGraphicsWidget(this) ),
    iPreviewPopup ( NULL ),
    iThumbLoadedCount(0)
{
    setFlag(QGraphicsItem::ItemHasNoContents, true);

    iBackground->setZValue(0);

    updateUI();
    updateHoldersUI();
}

WallpaperPage::~WallpaperPage(){}

void WallpaperPage::activate(bool state)
{
    iLayoutWidget->setEnabled(state);
}


void WallpaperPage::updateUI()
{
#if defined(Q_OS_LINUX)
    const QPixmap backgroundPixmap (":/images/list_background_meego.png");
#else
    const QPixmap backgroundPixmap (":/images/list_background_symbian.png");
#endif
    iBackground->setPixmap(backgroundPixmap);
    iBackground->setPos(QPointF(0,39));

    setRect(0, 0, backgroundPixmap.width(), kListPos.y() + kListHeight);
}

void WallpaperPage::updateHoldersUI()
{
    QPointF tl( iBackground->pos() + QPointF(0,10));
    QPointF br( tl.x() + iBackground->pixmap().width(),
               tl.y() + iBackground->pixmap().height());

    iLayoutWidget->setGeometry( QRectF(tl,br) );
}

void WallpaperPage::wallpapersForPage(QList<Photo> aList)
 {
    if ( iWallpapersList.count() && aList[0] == iWallpapersList[0])
        return;

     iWallpapersList = aList;

     if ( iLayoutWidget->layout() == NULL )
     {
         QGraphicsGridLayout *layout = new QGraphicsGridLayout();
         layout->setSpacing(0);

         int col = 0;
         for ( int i = 0 ; i < iWallpapersList.count(); i ++)
         {
             ThumbnailHolder* tnHolder = new ThumbnailHolder();
             connect( tnHolder, SIGNAL(thmbnailTapped()), this, SLOT(thmbnailTapped()));
             layout->addItem(tnHolder, i / 2, col);
             col ++ ;

             if (col == 2) col = 0;
         }

         iLayoutWidget->setLayout(layout);

     }

     const QGraphicsGridLayout *layout = static_cast<QGraphicsGridLayout*>
                                   (iLayoutWidget->layout());

     const QByteArray postBody ;
     iNetworkReplyList.clear();
     iThumbLoadedCount = 0;

     emit busy(true);
     for ( int i = 0 ; i < iWallpapersList.count(); i ++)
     {
         ThumbnailHolder* tnHolder = static_cast <ThumbnailHolder*> (layout->itemAt(i));
         tnHolder->setOpacity(0.0);
         const Photo p = aList[i];

         QNetworkRequest request;
         request.setUrl(p.thumbNailUrl());
         tnHolder->setImageUrl(p.imageUrl());

         /* Load from cache */
         request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
         request.setAttribute(QNetworkRequest::CacheSaveControlAttribute, true);
		 request.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true);

         QNetworkReply* reply = iNam->post(request, postBody);
         iNetworkReplyList << reply;

         QObject::connect(reply, SIGNAL(finished()),  this, SLOT(networkReplyFinished()));
         QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
                                  this, SLOT(networkReplyError(QNetworkReply::NetworkError)));

     }
 }


 void WallpaperPage::networkReplyError ( QNetworkReply::NetworkError /* dont care what the code is*/ )
 {
     MainContainer::notification()->show("Network problems !");
 }

 void WallpaperPage::networkReplyFinished ()
 {

    QNetworkReply* netReply = static_cast <QNetworkReply*> (sender());
    const int index = iNetworkReplyList.indexOf(netReply);

    const QByteArray responseData = netReply->readAll();
    const QImage image = QImage::fromData(responseData);
    const QPixmap pixmap = QPixmap::fromImage(image).scaled(kThumbSize,Qt::IgnoreAspectRatio);

    QGraphicsGridLayout* layout =  static_cast<QGraphicsGridLayout*>
                                   (iLayoutWidget->layout());

    ThumbnailHolder* tnHolder =
    static_cast<ThumbnailHolder*> (layout->itemAt(index));

    tnHolder->setPixmap(pixmap);

    QPropertyAnimation* anim = new QPropertyAnimation(tnHolder, "opacity");
    anim->setEasingCurve(QEasingCurve::OutExpo);
    anim->setDuration(500);
    anim->setEndValue(qreal(1.0));
    anim->start(QAbstractAnimation::DeleteWhenStopped);


    iThumbLoadedCount++;
    if (iThumbLoadedCount == iWallpapersList.count())
    {
        emit busy(false);
    }

    netReply->deleteLater();
 }

 void WallpaperPage::thmbnailTapped()
 {
     if (iPreviewPopup)
         return ;

     const ThumbnailHolder* tnHolderSender =
     static_cast<const ThumbnailHolder*> (sender());

     animateFadeOut();
     showPopUp(tnHolderSender->imageUrl());
 }

 void WallpaperPage::showPopUp( const QString& aImageUrl )
 {
     iPreviewPopup = new WallpaperPreviewPopup(this, aImageUrl, iNam);
     iPreviewPopup->setZValue(10);

     iPreviewPopup->setPos(0,1000);

     connect(iPreviewPopup, SIGNAL(dismissed()), this, SLOT(previewPopupdismissed()));

     QPropertyAnimation* anim = new QPropertyAnimation(iPreviewPopup, "pos");
     anim->setEasingCurve(QEasingCurve::OutExpo);
     anim->setDuration(600);
     anim->setEndValue(QPointF(iBackground->pos().x()+5, iBackground->pos().y()));
     anim->start(QAbstractAnimation::DeleteWhenStopped);

     emit previewPopupOpen(true);
 }

 void WallpaperPage::previewPopupdismissed()
 {

     QPropertyAnimation* anim = new QPropertyAnimation(iPreviewPopup, "pos");
     anim->setEasingCurve(QEasingCurve::OutExpo);
     anim->setDuration(600);
     anim->setEndValue(QPointF(0,1000));
     anim->start(QAbstractAnimation::DeleteWhenStopped);

     connect(anim,SIGNAL(finished()), this, SLOT(popupAnimFinished()));

     animateFadeIn();

     emit previewPopupOpen(false);
 }

 void WallpaperPage::popupAnimFinished()
 {
     iPreviewPopup->deleteLater();
     iPreviewPopup = NULL;
 }

void WallpaperPage::animateFadeOut()
{
    QPropertyAnimation* anim = new QPropertyAnimation(iLayoutWidget, "opacity");
    anim->setEasingCurve(QEasingCurve::OutExpo);
    anim->setDuration(500);
    anim->setEndValue(qreal(0.1));
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void WallpaperPage::animateFadeIn()
{
    QPropertyAnimation* anim = new QPropertyAnimation(iLayoutWidget, "opacity");
    anim->setEasingCurve(QEasingCurve::OutExpo);
    anim->setDuration(500);
    anim->setEndValue(qreal(1.0));
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

bool WallpaperPage::previewPopupOpen() const
{
    return (iPreviewPopup != NULL);
}
