#ifndef PHOTO_H
#define PHOTO_H

#include <QString>

class Photo
{
public:

    void setThumbNailUrl(const QString& aUrl) { iThumbNailUrl = aUrl; }
    void setImageUrl(const QString& aUrl) { iImageUrl = aUrl; }

    QString thumbNailUrl() const { return iThumbNailUrl;}
    QString imageUrl() const { return iImageUrl; }

    void setTitle(const QString& aTitle) { iTitle = aTitle; }
    QString title() const { return iTitle; }

    bool operator==(const Photo &aPhoto) const
    {
        return (aPhoto.iThumbNailUrl == iThumbNailUrl &&
            aPhoto.iImageUrl == iImageUrl);
    }
private:

    QString iTitle;

    QString iThumbNailUrl;
    QString iImageUrl;

};
#endif // PHOTO_H
