#include "photo.h"

/*

void Photo::downloadThumbNail()
{
    if (!thumbNail().isNull())
    {
        emit thumbNailDownloaded();
        return;
    }

    QNetworkRequest request;

    request.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true);
    request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysNetwork);
    request.setAttribute(QNetworkRequest::CacheSaveControlAttribute, false);

    request.setUrl(QUrl(iThumbNailUrl));

    QNetworkReply* reply = iNam.get(request);

    QObject::connect(reply, SIGNAL(finished()),  this, SLOT(handleSmallImageDownload()));
    QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
                             this, SLOT(networkReplyError(QNetworkReply::NetworkError)));
}

void Photo::downloadImage()
{
    if (!image().isNull())
    {
        emit imageDownloaded();
        return;
    }

    QNetworkRequest request;

    request.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true);
    request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysNetwork);
    request.setAttribute(QNetworkRequest::CacheSaveControlAttribute, false);

    request.setUrl(QUrl(iImageUrl));

    QNetworkReply* reply = iNam.get(request);
    QObject::connect(reply, SIGNAL(downloadProgress(qint64,qint64)), SIGNAL(downloadProgress(qint64,qint64)));
    QObject::connect(reply, SIGNAL(finished()),  this, SLOT(handleOriginalImageDownload()));
    QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
                             this, SLOT(networkReplyError(QNetworkReply::NetworkError)));

}

void Photo::networkReplyError ( QNetworkReply::NetworkError aCode )
{
}

void Photo::handleSmallImageDownload()
{
    QNetworkReply* reply = static_cast<QNetworkReply*> ( sender() );
    QByteArray responseData = reply->readAll();

    iThumbNail = QImage::fromData(responseData);

    reply->deleteLater();

    emit thumbNailDownloaded();
}

void Photo::handleOriginalImageDownload()
{
    QNetworkReply* reply = static_cast<QNetworkReply*> ( sender() );
    QByteArray responseData = reply->readAll();

    iImage = QImage::fromData(responseData);

    reply->deleteLater();

    emit imageDownloaded();
}
*/
