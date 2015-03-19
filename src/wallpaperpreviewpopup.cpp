#include "wallpaperpreviewpopup.h"
#include "loading.h"
#include "button.h"
#include "notification.h"
#include "thumbnailholder.h"
#include "maincontainer.h"
#include "button.h"

#include <QPainter>
#include <QDateTime>
#include <QEvent>
#include <QNetworkRequest>
#include <QDir>
#include <QParallelAnimationGroup>
#include <QThreadPool>

#ifdef Q_OS_SYMBIAN
#include <AknsWallpaperUtils.h>

static const QString kSavePath = "e:\\data\\images\\wallpapers\\";
#elif defined (Q_OS_LINUX)
static const QString kSavePath = "/home/user/MyDocs/wallpapers/";
#else
static const QString kSavePath = "c:\\wallpapers\\";
#endif
// --

FileSaveTask::FileSaveTask( const QString& aFilePath, const QImage& aImage, bool aSetAsWallpaper) 
    : iFilePath( aFilePath ), iImage (aImage), iSetAsWallpaper(aSetAsWallpaper)
 { }

void FileSaveTask::run()
{
    iImage.save(iFilePath,"png");
    emit fileSaved(iSetAsWallpaper, iFilePath);
}

bool FileSaveTask::setAsWallpaper() const
    {
    return iSetAsWallpaper;
    }

QString FileSaveTask::wallpaperPath() const
    {
    return iFilePath;
    }
// --

WallpaperPreviewPopup::WallpaperPreviewPopup(QGraphicsItem* parent, const QString& aUrl,QNetworkAccessManager* aNam)
        : QGraphicsObject(parent),
        iCloseButton( new Button(QPixmap(":/images/closebutton.png"),this)),
        iSaveButton( new Button(QPixmap(":/images/savebutton.png"), this)),
        iSetWallpaperButton ( new Button(QPixmap(":/images/wallpaperbutton.png"),this)),
        iImage ( NULL ), iImageTitle ( NULL ),
        iBackground ( new QGraphicsPixmapItem(this)),
        iNam ( aNam ),
        iImageUrl ( aUrl )
{
    setFlag(QGraphicsItem::ItemHasNoContents, true);
    updateUI();
    startDownload();
}

void WallpaperPreviewPopup::updateUI()
{
#if defined (Q_OS_LINUX)
    iBackground->setPixmap(QPixmap(":/images/popup_background_meego.png"));
#else
    iBackground->setPixmap(QPixmap(":/images/popup_background_symbian.png"));
#endif
    iBackground->setZValue(0);

    iSaveButton->setZValue(1);
    connect (iSaveButton, SIGNAL(clicked()), this, SLOT(saveButtonTapped()));
    iSaveButton->hide();

    iSetWallpaperButton->setZValue(1);
    connect (iSetWallpaperButton, SIGNAL(clicked()), this, SLOT(setWallpaperButtonTapped()));
    iSetWallpaperButton->hide();

    iCloseButton->setZValue(1);
    iCloseButton->setPos((boundingRect().width()-iCloseButton->boundingRect().width())/2, 10);
    connect (iCloseButton, SIGNAL(clicked()), this, SLOT(closeButtonTapped()));

}

void WallpaperPreviewPopup::startDownload()
{
    showLoading(true);

    QNetworkRequest request;
    request.setUrl(iImageUrl);

    /* Load from cache */
    request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
    request.setAttribute(QNetworkRequest::CacheSaveControlAttribute, true);
    request.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true);

    const QByteArray postBody;
    const QNetworkReply* reply = iNam->post(request, postBody);

    QObject::connect(reply, SIGNAL(finished()),  this, SLOT(networkReplyFinished()));
    QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
                             this, SLOT(networkReplyError(QNetworkReply::NetworkError)));

}

void WallpaperPreviewPopup::showLoading(bool aShow)
{

    if (aShow)
    {
        MainContainer::loading()->start();
        MainContainer::notification()->show("Downloading ..");
    }
    else
    {
        MainContainer::loading()->stop();
        MainContainer::notification()->hide();
    }
}

void WallpaperPreviewPopup::networkReplyError ( QNetworkReply::NetworkError )
{
    showLoading(false);
}

void WallpaperPreviewPopup::networkReplyFinished ()
{

    QNetworkReply* netReply = static_cast <QNetworkReply*> (sender());
    const QByteArray responseData = netReply->readAll();
    netReply->deleteLater();

    iImg = QImage::fromData(responseData);
    QSize pixmapSize = boundingRect().size().toSize();
    pixmapSize.setWidth( pixmapSize.width() * 0.9 );
    pixmapSize.setHeight( pixmapSize.height() * 0.5);

    const QPixmap pixmap = QPixmap::fromImage(iImg).scaled(pixmapSize,Qt::IgnoreAspectRatio);
    QParallelAnimationGroup* group = new QParallelAnimationGroup();

    iImage = new QGraphicsPixmapItem(this);
    iImage->setZValue(1);
    iImage->setPixmap(pixmap);
    iImage->setPos ( (iBackground->boundingRect().width() - pixmap.width() )/2 , 20 );

    iImageTitle = new QGraphicsTextItem(this);
    iImageTitle->setZValue(1);
    iImageTitle->setPlainText(QString("Size: " + QString::number( ( iImg.byteCount()/1024) ) + " KB"));
    iImageTitle->setPos((iBackground->boundingRect().width() - iImageTitle->boundingRect().width() )/2, iImage->pos().y() + iImage->pixmap().height() + 10 );
    group->addAnimation(fadeIn(iImageTitle));

    const int heightIncrement = iImageTitle->pos().y() + iImageTitle->boundingRect().height() + 10;
    const int totalWidth = iSaveButton->boundingRect().width() +
                     iSetWallpaperButton->boundingRect().width() + 10;

    iSetWallpaperButton->setPos((iBackground->pixmap().width() - totalWidth)/2, heightIncrement);
    iSetWallpaperButton->show();
    group->addAnimation(fadeIn(iSetWallpaperButton));

    iSaveButton->setPos( iSetWallpaperButton->pos().x() + iSetWallpaperButton->boundingRect().width() + 10, heightIncrement);
    iSaveButton->show();
    group->addAnimation(fadeIn(iSaveButton));

    QPropertyAnimation* anim = new QPropertyAnimation(iCloseButton, "pos");
    anim->setEasingCurve(QEasingCurve::OutExpo);
    anim->setDuration(1000);
    anim->setEndValue(QPointF(iCloseButton->pos().x(),
                              iSaveButton->pos().y() + iSaveButton->boundingRect().height() + 10));
    group->addAnimation(anim);

    group->start(QAbstractAnimation::DeleteWhenStopped);

    showLoading(false);
}

QPropertyAnimation* WallpaperPreviewPopup::fadeIn(QObject* aObject) const
{
    aObject->setProperty("opacity", 0);

    QPropertyAnimation* anim = new QPropertyAnimation(aObject, "opacity");
    anim->setEasingCurve(QEasingCurve::OutExpo);
    anim->setDuration(1000);
    anim->setEndValue(qreal(1.0));

    return anim;

}

 void WallpaperPreviewPopup::paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget * ) { }

 QRectF WallpaperPreviewPopup::boundingRect() const
 {
     return QRectF(0,0, iBackground->pixmap().width(), iBackground->pixmap().height());
 }

 void WallpaperPreviewPopup::saveCurrentWallpaper(bool aSetAsWallpaper)
     {
     MainContainer::notification()->show("Wait a few moments ...");
     MainContainer::loading()->start();

     QDir wallpaperDir (kSavePath);

     if (!wallpaperDir.exists())
         wallpaperDir.mkpath(kSavePath);

     QString fileName = kSavePath +
                        QString::number( QDateTime::currentDateTime().toTime_t() ) + ".png";

     FileSaveTask* task = new FileSaveTask(fileName, iImg, aSetAsWallpaper);
     connect(task, SIGNAL(fileSaved(bool, QString)), this, SLOT(fileSaved(bool, QString)));
     QThreadPool::globalInstance()->start(task); // QThreadPool will delete task
     }
 


 void WallpaperPreviewPopup::fileSaved(bool aSetAsWallpaper, QString aWallpaperPath)
    {
     Q_UNUSED(aWallpaperPath);

     MainContainer::loading()->stop();

    if (aSetAsWallpaper)
        {
        
#ifdef Q_OS_SYMBIAN
        TBuf<256> buff(aWallpaperPath.utf16());
        if (AknsWallpaperUtils::SetIdleWallpaper( 
                buff, NULL,NULL) == KErrNone)
            {
        MainContainer::notification()->show("Wallpaper set");
            }
        else
            {
        MainContainer::notification()->show("Unable to set wallpaper");
            }
#endif

        }
    else
        {
        MainContainer::notification()->show("Wallpaper saved");
        }
    }

 void WallpaperPreviewPopup::saveButtonTapped()
 {
     saveCurrentWallpaper(false);
 }
 
 void WallpaperPreviewPopup::setWallpaperButtonTapped()
 {
     saveCurrentWallpaper(true);
 }

 void  WallpaperPreviewPopup::closeButtonTapped()
 {
     MainContainer::loading()->stop();
     emit dismissed();
 }
