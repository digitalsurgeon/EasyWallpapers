#ifndef THUMBNAILHOLDER_H
#define THUMBNAILHOLDER_H

#include <QGraphicsWidget>
#include <QPainter>

class ThumbnailHolder : public QGraphicsWidget
{
    Q_OBJECT
public:
    ThumbnailHolder();
    ~ThumbnailHolder();

    void setPixmap(const QPixmap &pixmap);

    void setImageUrl ( const QString& aUrl);
    QString imageUrl() const;

    QRectF boundingRect() const;

signals:
    void thmbnailTapped();

private:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

private:

    QString iImageUrl;
    QPixmap* iPixmap;
};

#endif // THUMBNAILHOLDER_H
