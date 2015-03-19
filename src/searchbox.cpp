#include "searchbox.h"
#include "button.h"
#include "maincontainer.h"
#include "notification.h"
#include "wallpaperprovider.h"

#include <QPropertyAnimation>
#include <QLineEdit>
#include <QGraphicsProxyWidget>

#define kPullupButtonPixmap ":/images/searchpullup_up.png"
#define kPulldownButtonPixmap ":/images/searchpullup_down.png"
#define kPullbackButtonPixmap ":/images/searchpullup_back.png"
#define kStartSearchButtonPixmap ":/images/searchpullup_up.png"

SearchBox::SearchBox(WallpaperProvider* aProvider, QGraphicsItem *parent) :
    QGraphicsObject(parent),
    iBackground( new QGraphicsPixmapItem(QPixmap(":/images/searchboxbackground.png"),this)),
    iPullUpButton(new Button(QPixmap(kPullupButtonPixmap), this)),
    iStartSearchButton(new Button(QPixmap(kStartSearchButtonPixmap), this)),
    iInitialTopPoint ( 0 ),
    iSearchEdit( new QLineEdit()), iSearchEditProxyWidget(new QGraphicsProxyWidget(this) ),
    iWallpaperProvider ( aProvider ),
    iMode ( down )
{
    setFlag(QGraphicsItem::ItemHasNoContents, true);

    iBackground->setPos(0, iPullUpButton->size().height());
    iBackground->setZValue(0);

    iPullUpButton->setPos(iBackground->boundingRect().width() - iPullUpButton->size().width(),0);
    iPullUpButton->setZValue(1);
    connect(iPullUpButton, SIGNAL(clicked()), this, SLOT(pullUpButtonClicked()) );

    iSearchEdit->setTextMargins(0, 0, 0, 0);
    iSearchEditProxyWidget->setWidget(iSearchEdit);
    iSearchEditProxyWidget->setPos(
            (iBackground->boundingRect().width() - iSearchEditProxyWidget->boundingRect().width())/2,
                                    50);
    iSearchEditProxyWidget->setZValue(1);

    iStartSearchButton->setPos((boundingRect().width()- iStartSearchButton->size().width())/2,
                               iSearchEditProxyWidget->pos().y() + iSearchEditProxyWidget->boundingRect().height() + 5);
    iStartSearchButton->setZValue(1);
    connect (iStartSearchButton, SIGNAL(clicked()), this, SLOT(startSearchButtonClicked()));

}

void SearchBox::pullUpButtonClicked()
{
    QPropertyAnimation* anim = new QPropertyAnimation(this, "pos");

    anim->setEasingCurve(QEasingCurve::InBack);
    anim->setDuration(500);

    QPointF finalPoint (pos().x(), 0);

    switch ( iMode )
    {
    case down:
        iInitialTopPoint = pos().y();
        finalPoint.setY(pos().y() - boundingRect().height()+iPullUpButton->size().height());
        iPullUpButton->setPixmap(Button::NormalState, QPixmap(kPulldownButtonPixmap));
        iMode = up;
        break;

    case up:
    case search:
        finalPoint.setY(iInitialTopPoint);
        iPullUpButton->setPixmap(Button::NormalState, QPixmap(kPullupButtonPixmap));
        iMode = down;
        break;

    default:break;
    }

    anim->setEndValue(finalPoint);
    connect(anim, SIGNAL(finished()), iSearchEdit, SLOT(setFocus()));
    anim->start(QAbstractAnimation::DeleteWhenStopped);

    emit pulledUp(iMode == up);
}

void SearchBox::startSearchButtonClicked()
{
    if (iSearchEdit->text().length() < 1)
    {
        MainContainer::notification()->show("Duuude! nothing to search!");
        return;
    }

    iWallpaperProvider->searchForWallpapers(iSearchEdit->text());

    iMode = search;
    iPullUpButton->setPixmap(Button::NormalState, QPixmap(kPullbackButtonPixmap));

    QPropertyAnimation* anim = new QPropertyAnimation(this, "pos");

    anim->setEasingCurve(QEasingCurve::InBack);
    anim->setDuration(500);

    anim->setEndValue(QPointF(pos().x(),iInitialTopPoint ));
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

QRectF SearchBox::boundingRect() const
{
    QRectF boundingRect = iBackground->boundingRect();
    boundingRect.setHeight( iPullUpButton->boundingRect().height() + boundingRect.height());
    return boundingRect;
}


void SearchBox::usable(bool aUsable)
{
    QPropertyAnimation* anim = new QPropertyAnimation(iPullUpButton, "pos");
    QPointF finalPoint (iPullUpButton->pos().x(), 0);
    if (!aUsable)
    {
        finalPoint.setY(iPullUpButton->size().height());
    }

    anim->setEndValue(finalPoint);
    anim->setEasingCurve(QEasingCurve::InBack);
    anim->setDuration(1000);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}
