#include "maincontainer.h"
#include "button.h"
#include "wallpaperpage.h"
#include "wallpaperprovider.h"
#include "loading.h"
#include "notification.h"
#include "searchbox.h"

#include <QDebug>
#include <QFont>
#include <QParallelAnimationGroup>
#include <QNetworkAccessManager>

static const int kTopMargin = 5;
static const qreal kPageContainerTopMargin = 17.0;
static const qreal kListOffset = -1;

Notification* MainContainer::iNotification = NULL;
Loading*    MainContainer::iLoading = NULL;

// PageChangeGestureBox

static const int max_gesture_time_ms = 500;
static const qreal degrees = 30.0;
static const qreal limit_angle_tg = tan(degrees * 3.141592 / 180.0);
static const qreal min_gesture_length = 60.0;

PageChangeGestureBox::PageChangeGestureBox(QGraphicsItem *parent)
    : GestureBox(parent)
    , m_active(true)
    , m_startPoint(0.0, 0.0)
{
}

void PageChangeGestureBox::setActivate(bool active)
{
    m_active = active;
}

void PageChangeGestureBox::gestureMousePress(QPointF pos, bool &startGesture, bool &acceptClick)
{
    Q_UNUSED(pos);
    Q_UNUSED(acceptClick);
    startGesture = m_active;
}

void PageChangeGestureBox::gestureStart(QPointF pos)
{
    m_startPoint = pos;
    m_startTime = QTime::currentTime();
}

void PageChangeGestureBox::gestureMove(QPointF pos, QPointF movement, QPointF speed)
{
    Q_UNUSED(pos);
    Q_UNUSED(movement);
    Q_UNUSED(speed);
}

void PageChangeGestureBox::gestureEnd(QPointF pos, QPointF speed)
{
    Q_UNUSED(speed);
    qreal x = pos.x() - m_startPoint.x();
    qreal y = pos.y() - m_startPoint.y();
    if (!m_active
        || fabs(y / x) > limit_angle_tg
        || m_startTime.msecsTo(QTime::currentTime()) > max_gesture_time_ms
        || fabs(x) < min_gesture_length) {
        return;
    }

    if (x > 0)
        emit moveLeft();
    else
        emit moveRight();
}

MainContainer::MainContainer(QObject *parent) :
    QObject(parent),
    iLists(),
    iAnimation ( NULL ),
    iPageContainer ( new QGraphicsRectItem(this) ),
    iCurrentPageLogically ( 0 ),
    iCurrentPage ( 0 ),
    iTitle ( new QGraphicsPixmapItem(QPixmap(":/images/title.png"),this) ),
    iPageNumber ( new QGraphicsTextItem(this)),
    iCloseButton ( new Button(QPixmap (":/images/button_close.png"),this) ),
    iWallpaperProvider ( new WallpaperProvider(this)),
    iNam ( new QNetworkAccessManager( this )),
    iPageChangeListBox ( new PageChangeGestureBox(this)),
    iSearchBox( new SearchBox(iWallpaperProvider, this))
{
    MainContainer::iNotification  = new Notification(this);
    MainContainer::iLoading = new Loading(this);

#if defined(Q_OS_LINUX)
    QPixmap backgroundPixmap(":/images/background_meego.png");
#else
    QPixmap backgroundPixmap(":/images/background_symbian.png");
#endif

    setPixmap(backgroundPixmap);

    iPageContainer->setFlags(QGraphicsItem::ItemHasNoContents);
    iPageContainer->setPos(kPageContainerTopMargin, 0.0);

    connect (iWallpaperProvider, SIGNAL(wallpaperListUpdated(bool)), this, SLOT(wallpaperListUpdated(bool)));
    iWallpaperProvider->init();

    connect (iPageChangeListBox, SIGNAL(moveLeft()), this, SLOT(moveLeft()));
    connect(iPageChangeListBox, SIGNAL(moveRight()), this, SLOT(moveRight()));

    connect(iCloseButton,SIGNAL(clicked()), QCoreApplication::instance(), SLOT(quit()) );
    connect( iSearchBox, SIGNAL(pulledUp(bool)), this, SLOT(searchBoxPulledUp(bool)));

    updateUI();

}

void MainContainer::recognizeGestures(bool aVal)
{
    iPageChangeListBox->setActivate(aVal);
    iPageChangeListBox->setVisible(aVal);
}

void MainContainer::moveLeft()
{
    navigatePage(false);
}

void MainContainer::moveRight()
{
    navigatePage(true);
}

void MainContainer::updateUI()
{
    QRectF containerRect = boundingRect();

    {
        const QSize closeButtonSize = iCloseButton->boundingRect().size().toSize();
        iCloseButton->setPos(containerRect.width() - closeButtonSize.width(), 0);
    }

    {
        qreal titleLeft = (containerRect.width() - iTitle->boundingRect().width()) / 2;
        iTitle->setPos(titleLeft, kTopMargin);
    }

    iLoading->setPos(0,0);

    iSearchBox->setZValue(11);
    iSearchBox->setPos((boundingRect().width() - iSearchBox->boundingRect().width())/2,
                       boundingRect().height()-40);
}

void MainContainer::add(WallpaperPage* aWallpaperPage)
{
    if (iLists.indexOf(aWallpaperPage) >= 0)
        return;

    iLists.append(aWallpaperPage);
    adjustListsPositions();
}

void MainContainer::addPageToHead(WallpaperPage* aWallpaperPage)
{
    if (iLists.indexOf(aWallpaperPage) >= 0)
        return;

    WallpaperPage* firstPage = iLists.first();
    const qreal post = firstPage->pos().x() - firstPage->boundingRect().width() - kListOffset ;

    aWallpaperPage->setPos(post, 0.0);
    iLists.insert(0, aWallpaperPage);

}

void MainContainer::firstPageSlideIntoViewAnimationFinished()
{
    const QRectF firstPageRect ( iLists[0]->pos(), iLists[0]->boundingRect().size());
    iPageChangeListBox->setRect( firstPageRect );
}

void MainContainer::adjustListsPositions()
{
    qreal listPos = iLists.first()->pos().x();

    if (iLists.count() == 1)
    {
        WallpaperPage* page  = iLists[0];
        page->setPosition(QPointF(listPos, 1000));

        QPropertyAnimation* anim = new QPropertyAnimation(page, "position");
        anim->setEasingCurve(QEasingCurve::OutExpo);
        anim->setDuration(500);
        anim->setEndValue(QPointF(listPos, 0));
        anim->start(QAbstractAnimation::DeleteWhenStopped);

        connect ( anim, SIGNAL(finished()), this, SLOT(firstPageSlideIntoViewAnimationFinished()) );
        return;
    }

    for (int i = 0; i < iLists.count(); ++i) {
        if (i!=0)
            iLists[i]->setPos(listPos, 0);
        listPos += iLists[i]->boundingRect().width() + kListOffset;
    }
}

void MainContainer::navigatePage(bool leftToRight)
{
    if (iAnimation || iLists[iCurrentPage]->previewPopupOpen())
        return;

    recognizeGestures(false);

    const int totalNumberOfPages = iWallpaperProvider->count()
                                   /iWallpaperProvider->wallpaperPerPageCount();

    if (iCurrentPage+1 == iLists.count() && iCurrentPageLogically <= totalNumberOfPages )
    {
        iCurrentPage--;

        WallpaperPage* page = iLists.first(); iLists.removeFirst();
        add(page);
    }
    else if (iCurrentPage-1 == 0 && iCurrentPageLogically >= 0 )
    {
        iCurrentPage ++;

        WallpaperPage* page = iLists.last(); iLists.removeLast();
        addPageToHead(page);
    }

    const int newCurrentPageLogicalPosition = iCurrentPageLogically + (leftToRight ? 1 : -1);
    if (newCurrentPageLogicalPosition < 0 || newCurrentPageLogicalPosition > totalNumberOfPages)
        return;

    iCurrentPageLogically = newCurrentPageLogicalPosition;
    int target = iCurrentPage + (leftToRight ? 1 : -1);

    QAbstractAnimation* anim = navigatePageAnimation(leftToRight);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
    iAnimation = anim;
    iCurrentPage = target;

    loadWallpapersForCurrentPage();

    if (iCurrentPage>0)
        iLists[iCurrentPage-1]->activate(false);

    if (iCurrentPage+1 < iLists.count())
        iLists[iCurrentPage+1]->activate(false);

    iLists[iCurrentPage]->activate(true);

}

QAbstractAnimation* MainContainer::navigatePageAnimation(bool aNext)
{
    QParallelAnimationGroup* group = new QParallelAnimationGroup();

    qreal left = iLists[iCurrentPage]->boundingRect().width() + kListOffset;
    left = iPageContainer->pos().x() +(aNext ? -left : left);

    QPropertyAnimation* anim = new QPropertyAnimation(this, "position");
    anim->setEasingCurve(QEasingCurve::OutExpo);
    anim->setDuration(500);
    anim->setEndValue(QPointF(left, 0));

    group->addAnimation(anim);

    connect(group, SIGNAL(finished()), this, SLOT(animationFinished()));

    return group;
}

void MainContainer::animationFinished()
{
    recognizeGestures(true);
    iAnimation = NULL;
}

void MainContainer::wallpaperListUpdated(bool aResult)
{
    if (aResult)
    {
        if (iLists.count() == 0)
        {
            createWallpaperPages();
        }

        loadWallpapersForCurrentPage();
    }
    else
    {
        notification()->show("Unable to load wallpapers list", false);
        connect(notification(), SIGNAL(dismissed()), QCoreApplication::instance(), SLOT(quit()));
    }

}

void MainContainer::createWallpaperPages()
{
    for ( int i = 0 ; i < 4; i ++)
    {
        WallpaperPage* page = new WallpaperPage(this, iPageContainer, iNam);
        connect ( page, SIGNAL(busy(bool)), this, SLOT(pageBusy(bool)));
        add(page);

        connect(page,SIGNAL(previewPopupOpen(bool)), this, SLOT(previewPopupOpen(bool)));
    }

    recognizeGestures(true);
}

void MainContainer::loadWallpapersForCurrentPage()
{
    QList<Photo> wallpapers;
    if (iWallpaperProvider->wallpapersForPage(iCurrentPageLogically, wallpapers))
        iLists[iCurrentPage]->wallpapersForPage(wallpapers);
}

void MainContainer::pageBusy(bool aBusy)
{
    if (aBusy)
    {
        iLoading->start();
    }
    else
    {
        iLoading->stop();
    }
}

Notification* MainContainer::notification()
{
        return MainContainer::iNotification;
}

Loading* MainContainer::loading()
{
    return MainContainer::iLoading;
}

void MainContainer::searchBoxPulledUp(bool aVal)
{
    recognizeGestures(!aVal);
}

void MainContainer::previewPopupOpen(bool a)
{
    recognizeGestures(!a);
    iSearchBox->usable(!a);
}
