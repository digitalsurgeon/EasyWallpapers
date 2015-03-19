#include "wallpaperprovider.h"
#include "qtflickr.h"
#include "photo.h"
#include "maincontainer.h"
#include "loading.h"

static const QString kBackgroundsFlickrGroupId = "1316065@N23";
static const int kPhotosPerFlickrDataSet = 40;
static const int kWallpapersPerPage = 10;

static const QString kApiKey = "960da859756afda2b5a957d432de2000";
static const QString kApiSecret = "cb3bf4763d34ea2e";

WallpaperProvider::WallpaperProvider(QObject *parent) :
    QObject(parent),
    iFlickr ( new QtFlickr ( kApiKey, kApiSecret,this ) ),    
    iTotalImages (0), iTotalSearchResults ( 0 ),
    iWallpapersPerPage ( kWallpapersPerPage ), iSearchMode ( false )

{
}

void WallpaperProvider::init()
{
    connect ( iFlickr, SIGNAL ( requestFinished ( int, QtfResponse, QtfError, void* ) ),
              this, SLOT ( requestFinished ( int, QtfResponse, QtfError, void* ) ) );
    
    getPhotosDataFromFlickr(1);
}

void WallpaperProvider::requestFinished ( int /*reqId*/, QtfResponse data, QtfError err, void* /*userData*/ )
{
    MainContainer::loading()->stop();

    if ( !err.code )
    {
        bool res;

        ( iSearchMode ? iTotalSearchResults : iTotalImages) =
                data.tags.values("photos").at(0).attrs.value("total").toInt(&res);


        const QList<QtfTag> list = data.tags.values("photo");
        for ( int i = 0; i < list.size(); ++i )
        {
            Photo p;
            p.setTitle( list.at(i).attrs.value("title") );

            QString thumbUrl = list.at(i).attrs.value("url_s");
            if (thumbUrl.length() < 1 )
            {
                thumbUrl = list.at(i).attrs.value("url_t");
                if (thumbUrl.length() < 1)
                    thumbUrl = list.at(i).attrs.value("url_sq");
            }
            p.setThumbNailUrl(thumbUrl);

            QString imageUrl = list.at(i).attrs.value("url_m");
            if (imageUrl.length() < 1 )
            {
                imageUrl = list.at(i).attrs.value("url_o");
                if (imageUrl.length() < 1)
                {
                    imageUrl = list.at(i).attrs.value("url_sq");
                }
            }
            p.setImageUrl(imageUrl);

            (iSearchMode ? iSearchPhotoList : iPhotoList) << p;
        }

        emit (iSearchMode ? searchResultsUpdated(true) : wallpaperListUpdated(true));
    }
    else
    {
        emit (iSearchMode ? searchResultsUpdated(false) : wallpaperListUpdated(false));
    }

}

bool WallpaperProvider::wallpapersForPage(int aPage, QList<Photo>& aList)
{

    const int index = aPage * wallpaperPerPageCount();
    const int total = index + wallpaperPerPageCount();

    if (index >= iPhotoList.count())
    {
        getPhotosDataFromFlickr(aPage);
        return false;
    }

    for ( int i = index; i < total; i ++)
    {
        aList << ( iSearchMode ? iSearchPhotoList[i] : iPhotoList[i]);
    }

    return true;
}


int WallpaperProvider::wallpaperPerPageCount() const
{
    return iWallpapersPerPage;
}

void WallpaperProvider::getPhotosDataFromFlickr(int aPage, const QString aTag)
{
    MainContainer::loading()->start();

    QtfMethod method;
    method.method = "flickr.groups.pools.getPhotos";
    method.args.insert( "group_id", kBackgroundsFlickrGroupId );
    method.args.insert( "page", QString().setNum(aPage) );
    method.args.insert( "per_page", QString().setNum( kPhotosPerFlickrDataSet ) );
    method.args.insert( "extras", "url_sq,url_t,url_s,url_m,url_o" );

    /* flickr currently supports only one tag */
    if (aTag.length())
        method.args.insert( "tags" , aTag);

    QtfRequest request;
    request.requests.insert( "photo", "url_sq,url_t,url_s,url_m,url_o,title" );
    request.requests.insert( "photos", "total" );
    iFlickr->get ( method,request );
}

int WallpaperProvider::count() const
{
    return (iSearchMode ? iTotalSearchResults : iTotalImages);
}

void WallpaperProvider::searchForWallpapers(const QString& aSearchString)
{
    iSearchMode = true;
    getPhotosDataFromFlickr(1, aSearchString);
}
void WallpaperProvider::resetSearchResults()
{
    iSearchMode = false;
    iTotalSearchResults = 0;
}
