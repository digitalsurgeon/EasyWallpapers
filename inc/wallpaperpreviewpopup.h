#ifndef WALLPAPERPREVIEWPOPUP_H
#define WALLPAPERPREVIEWPOPUP_H

#include <QGraphicsRectItem>
#include <QObject>
#include <QNetworkReply>
#include <QRunnable>

class QNetworkAccessManager;
class QPropertyAnimation;
class ThumbnailHolder;
class Button;

class FileSaveTask : public QObject, public QRunnable
{
   Q_OBJECT

    void run();
public:
    FileSaveTask( const QString& aFilePath, const QImage& aImage, bool aSetAsWallpaper);
    
    bool setAsWallpaper() const ;
    QString wallpaperPath() const;

signals:
    void fileSaved(bool, QString);

private:
    QString iFilePath;
    QImage iImage;
    bool iSetAsWallpaper;
};


class WallpaperPreviewPopup:  public QGraphicsObject
{
    Q_OBJECT

public:
    WallpaperPreviewPopup(QGraphicsItem* parent, const QString& aUrl, QNetworkAccessManager* aNam);

signals:
    void dismissed();

private slots:
    void networkReplyError ( QNetworkReply::NetworkError );
    void networkReplyFinished ();

    void saveButtonTapped();
    void setWallpaperButtonTapped();
    void closeButtonTapped();

    void fileSaved(bool, QString);

private:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
    QRectF boundingRect() const;

    void showLoading(bool);
    void startDownload();
    
    void saveCurrentWallpaper(bool aSetAsWallpaper);

    void updateUI();
    QPropertyAnimation* fadeIn(QObject* aObject) const;

    Button*     iCloseButton;
    Button*     iSaveButton;
    Button*     iSetWallpaperButton;
    QGraphicsPixmapItem*    iImage;
    QGraphicsTextItem*  iImageTitle;

    QGraphicsPixmapItem*    iBackground;

    QNetworkAccessManager* iNam;

    QString iImageUrl;
    QImage iImg;

};

#endif // WALLPAPERPREVIEWPOPUP_H
