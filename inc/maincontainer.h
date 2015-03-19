#ifndef MAINCONTAINER_H
#define MAINCONTAINER_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QAbstractAnimation>
#include <QPropertyAnimation>
#include <QTime>

#include "gesturebox.h"

class QNetworkAccessManager;

class WallpaperPage;
class Button;
class WallpaperProvider;
class Loading;
class Notification;
class SearchBox;

class PageChangeGestureBox : public QObject, public GestureBox
{
    Q_OBJECT
public:
    PageChangeGestureBox(QGraphicsItem *parent = 0);
    void setActivate(bool active);
signals:
    void moveLeft();
    void moveRight();
protected:
    void gestureMousePress(QPointF pos, bool &startGesture, bool &acceptClick);
    void gestureStart(QPointF pos);
    void gestureMove(QPointF pos, QPointF movement, QPointF speed);
    void gestureEnd(QPointF pos, QPointF speed);
private:
    bool m_active;
    QPointF m_startPoint;
    QTime m_startTime;
};

class MainContainer : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

    /* this property is used to animate the flick to next/previous views */
    Q_PROPERTY(QPointF position READ position WRITE setPosition);

public:
    explicit MainContainer(QObject *parent = 0);
    virtual ~MainContainer(){}

    void add(WallpaperPage *);
    void updateUI();

    static Notification* notification();
    static Loading* loading();

    void recognizeGestures(bool);

public slots:
    void moveLeft();
    void moveRight();

    void wallpaperListUpdated(bool);

private slots:
    void animationFinished();
    void pageBusy(bool);

    void firstPageSlideIntoViewAnimationFinished();
    void searchBoxPulledUp(bool);
    void previewPopupOpen(bool);

private:
    QPointF position() const { return iPageContainer->pos(); }
    void setPosition(QPointF pos) { iPageContainer->setPos(pos); }

    void adjustListsPositions();

    void navigatePage(bool leftToRight);
    QAbstractAnimation* navigatePageAnimation(bool aNext);

    void addPageToHead(WallpaperPage *);
    void createWallpaperPages();

    void loadWallpapersForCurrentPage();

private:

    QList<WallpaperPage*> iLists;
    QAbstractAnimation* iAnimation;

    /* contains all the wallpapers */
    QGraphicsRectItem* iPageContainer;

    int iCurrentPageLogically;
    int iCurrentPage;

    QGraphicsPixmapItem* iTitle;
    QGraphicsTextItem* iPageNumber;
    Button* iCloseButton;

    WallpaperProvider* iWallpaperProvider;
    QNetworkAccessManager* iNam;
    PageChangeGestureBox* iPageChangeListBox;
    SearchBox*  iSearchBox;

    static Loading*    iLoading;
    static Notification*   iNotification;

};

#endif // MAINCONTAINER_H
