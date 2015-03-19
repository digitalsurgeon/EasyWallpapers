#ifndef SEARCHBOX_H
#define SEARCHBOX_H

#include <QGraphicsRectItem>

class Button;
class QLineEdit;
class QGraphicsProxyWidget;
class WallpaperProvider;

class SearchBox : public QGraphicsObject
{
    Q_OBJECT

public:
    SearchBox(WallpaperProvider*,QGraphicsItem *parent = 0);
    QRectF boundingRect() const;


public slots:
    void usable(bool);

signals:
    void pulledUp(bool);

private slots:
    void pullUpButtonClicked();
    void startSearchButtonClicked();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event){}

private:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0){}

    QGraphicsPixmapItem* iBackground;

    Button* iPullUpButton;
    Button* iStartSearchButton;
    qreal   iInitialTopPoint;

    QLineEdit*  iSearchEdit; QGraphicsProxyWidget* iSearchEditProxyWidget;
    WallpaperProvider* iWallpaperProvider;

    enum mode { down = 0, up, search };
    mode    iMode;
};

#endif // SEARCHBOX_H
