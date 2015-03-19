#ifndef WALLPAPERPAGE_H
#define WALLPAPERPAGE_H

#include <QObject>
#include <QGraphicsRectItem>
#include <QGraphicsPixmapItem>
#include <QNetworkReply>
#include "photo.h"

class QNetworkAccessManager;
class WallpaperPreviewPopup;

class WallpaperPage : public QObject, public QGraphicsRectItem
{
    Q_OBJECT

     Q_PROPERTY(QPointF position READ position WRITE setPosition);

public:
    explicit WallpaperPage(QObject* aObjParent, QGraphicsItem *parent, QNetworkAccessManager* aNam);
    virtual ~WallpaperPage();

    void wallpapersForPage(QList<Photo>);

    bool previewPopupOpen() const;

    QPointF position() const { return pos(); }
    void setPosition(QPointF pos) { setPos(pos); }

    void activate(bool);

signals:
    void busy(bool);
    void previewPopupOpen(bool);

private slots:

    void networkReplyError ( QNetworkReply::NetworkError /*aCode*/ );
    void networkReplyFinished ();

    void thmbnailTapped();

    void previewPopupdismissed();
    void popupAnimFinished();

private:

    void updateUI();
    void updateHoldersUI();

    void animateFadeOut();
    void animateFadeIn();

    void showPopUp( const QString& aImageUrl );


private:
    QGraphicsPixmapItem *iBackground;

    QList<Photo>  iWallpapersList;

    QNetworkAccessManager*  iNam;
    QList<QNetworkReply*>   iNetworkReplyList;

    QGraphicsWidget*    iLayoutWidget;

    WallpaperPreviewPopup*  iPreviewPopup;

    int iThumbLoadedCount;
};

#endif // WALLPAPERPAGE_H
