#include "mainview.h"
#include "maincontainer.h"
#include "notification.h"

#include <QKeyEvent>
#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>

static const QString kWindowTitle = "Easy Wallpapers";

MainView::MainView(MainContainer& aContainer) :
    QGraphicsView(),
    iContainer(aContainer)
{
    setRenderHints(QPainter::TextAntialiasing);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFrameShape(QFrame::NoFrame);
    setWindowTitle(kWindowTitle);
    setViewportUpdateMode(BoundingRectViewportUpdate);
}


void MainView::keyPressEvent(QKeyEvent *event)
{
    event->ignore();
    if (event->key() == Qt::Key_F5)
        adjustScreenSize();
    if (event->key() == Qt::Key_Left)
        iContainer.moveLeft();
    if (event->key() == Qt::Key_Right)
        iContainer.moveRight();
    else
        QGraphicsView::keyPressEvent(event);
}


void MainView::adjustScreenSize()
{

    QSizeF screenSize(iContainer.boundingRect().size());

    resize(screenSize.toSize());
    if (scene())
        scene()->setSceneRect(0.0, 0.0, screenSize.width(), screenSize.height());
    centerOn(&iContainer);
    iContainer.updateUI();
}

void MainView::resized(int /*screen*/)
{
    adjustScreenSize();
}
