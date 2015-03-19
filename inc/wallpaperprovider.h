#ifndef WALLPAPERPROVIDER_H
#define WALLPAPERPROVIDER_H

#include <QObject>

#include "qtflickr.h"
#include "photo.h"

class QtFlickr;


class WallpaperProvider : public QObject
{
    Q_OBJECT
public:
    explicit WallpaperProvider(QObject *parent = 0);

    int wallpaperPerPageCount() const;
    bool wallpapersForPage(int, QList<Photo>&);
    int count() const;

    void searchForWallpapers(const QString&);
    void resetSearchResults();

signals:
    void wallpaperListUpdated(bool);
    void searchResultsUpdated(bool);

public slots:
    void init();

private slots:
    void requestFinished ( int reqId, QtfResponse data, QtfError err, void* userData );

private:
    void getPhotosDataFromFlickr(int, const QString = "");

    QtFlickr*   iFlickr;

    int iTotalImages;
    int iTotalSearchResults;
    int iWallpapersPerPage;

    QList<Photo> iPhotoList;
    QList<Photo> iSearchPhotoList;

    bool iSearchMode;

};

#endif // WALLPAPERPROVIDER_H
