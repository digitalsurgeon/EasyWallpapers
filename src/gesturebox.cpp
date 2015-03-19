/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: openBossa - INdT (renato.chencarek@openbossa.org)
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** the openBossa stream from INdT (renato.chencarek@openbossa.org).
** $QT_END_LICENSE$
**
****************************************************************************/

#include "gesturebox.h"

#include <QApplication>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QDebug>
#include <QPointer>
#include <QTime>

#include "gesturebox_p.h"

// GestureData

#ifdef QT_ARCH_ARM
static const int move_threshold = 30;
#else
static const int move_threshold = 15;
#endif

GestureData::GestureData(QPoint pos, const QList<GestureBox*> &boxes,
                         QGraphicsView *view, QList<QEvent*> &ignoreList, bool sendClickEvents)
    : m_state(Pressed)
    , m_view(view)
    , m_pressPos(pos)
    , m_scenePressPos(m_view->mapToScene(m_pressPos))
    , m_boxes(boxes)
    , m_ignoreList(ignoreList)
    , m_time(QTime::currentTime())
    , m_currentScenePos(m_scenePressPos)
    , m_speed(0.0, 0.0)
    , m_sendClickEvents(sendClickEvents)
{
}

void GestureData::sendClick()
{
    if (m_sendClickEvents) {
        QMouseEvent *press = new QMouseEvent(QEvent::MouseButtonPress,
                                              m_pressPos, Qt::LeftButton,
                                              Qt::LeftButton, Qt::NoModifier);
        QMouseEvent *release = new QMouseEvent(QEvent::MouseButtonRelease,
                                               m_pressPos, Qt::LeftButton,
                                               Qt::LeftButton, Qt::NoModifier);
        m_ignoreList << press;
        m_ignoreList << release;
        QApplication::postEvent(m_view->viewport(), press);
        QApplication::postEvent(m_view->viewport(), release);
    }
}

void GestureData::release(QPoint pos)
{
    Q_UNUSED(pos);
    switch (m_state) {
        case Pressed:
            sendClick();
            break;
        case Moving:
            foreach(GestureBox *box, m_boxes)
                box->gestureEnd(box->mapFromScene(m_currentScenePos), m_speed);
            break;
        default:
            break;
    }
    m_state = Ended;
    deleteLater();
}

void GestureData::move(QPoint pos)
{
    if (m_state == Pressed &&
        abs(pos.x() - m_pressPos.x()) < move_threshold &&
        abs(pos.y() - m_pressPos.y()) < move_threshold) {
        return;
    }
    QTime time(QTime::currentTime());
    QPointF scenePos = m_view->mapToScene(pos);
    QPointF move = scenePos - m_currentScenePos;
    qreal secs = (qreal)(m_time.msecsTo(time)) / 1000;
    m_currentScenePos = scenePos;
    m_speed = QPointF(move.x() / secs, move.y() / secs);
    m_time = time;

    switch (m_state) {
        case Pressed:
            foreach(GestureBox *box, m_boxes) {
                box->gestureStart(box->mapFromScene(m_scenePressPos));
                box->gestureMove(box->mapFromScene(m_currentScenePos), move, m_speed);
            }
            m_state = Moving;
            break;
        case Moving:
            foreach(GestureBox *box, m_boxes)
                box->gestureMove(box->mapFromScene(m_currentScenePos), move, m_speed);
            break;
        default:
            break;
    }
}

// ViewData

ViewData::~ViewData()
{
    if (m_gesture)
        m_gesture->deleteLater();
}

bool ViewData::ignoreEvent(QEvent *event)
{
    return m_ignored.removeAll(event);
}

bool ViewData::startGesture(GestureBox *box, QPointF scenePos, bool &sendClick)
{
    bool result = true;
    bool click = true;
    box->gestureMousePress(box->mapFromScene(scenePos), result, click);
    if (result && !click)
        sendClick = false;
    return result;
}

bool ViewData::press(QPoint pos)
{
    if (m_gesture && !m_gesture->ended()) {
        qWarning() << "ERROR: unexpected mouse press during gesture.";
        return false;
    }

    if (!m_view || !m_view->scene())
        return false;

    QList<GestureBox*> boxes;
    bool sendClick = true;
    QPointF scenePos = m_view->mapToScene(pos);

    QList<QGraphicsItem*> itemList = m_view->scene()->items(scenePos,
                                                            Qt::IntersectsItemBoundingRect,
                                                            Qt::AscendingOrder);

    foreach(QGraphicsItem *item, itemList) {
        GestureBox *box = dynamic_cast<GestureBox*>(item);
        if (box && startGesture(box, scenePos, sendClick))
            boxes.append(box);
    }

    if (boxes.isEmpty())
        return false;

    m_gesture = new GestureData(pos, boxes, m_view, m_ignored, sendClick);
    return true;

}

bool ViewData::release(QPoint pos)
{
    if (!m_gesture || m_gesture->ended())
        return false;

    m_gesture->release(pos);
    return true;
}

bool ViewData::move(QPoint pos)
{
    if (!m_gesture || m_gesture->ended())
        return false;

    m_gesture->move(pos);
    return true;
}

QList<GestureBox*> ViewData::getBoxList(QPoint pos) {
    QList<GestureBox*> result;
    if (m_view->scene()) {
        QPointF scenePos = m_view->mapToScene(pos);
        QGraphicsItem *item = m_view->scene()->itemAt(scenePos);
        while (item) {
            GestureBox *box = dynamic_cast<GestureBox*>(item);
            if (box && box->boundingRect().contains(box->mapFromScene(scenePos)))
                result.append(box);
            item = item->parentItem();
        }
    }
    return result;
}

// GestureObserver

GestureObserver *GestureObserver::instance()
{
    static GestureObserver *obj = new GestureObserver();
    return obj;
}

void GestureObserver::addBox(GestureBox *box)
{
    if (box->scene()) {
        foreach(QGraphicsView *view, box->scene()->views()) {
            ViewData *data = getViewData(view);
            if (data->m_boxes.indexOf(box) < 0) {
                data->m_boxes.append(box);
                m_boxes.insertMulti(box, data);
            }
        }
    }
}

void GestureObserver::removeBox(GestureBox *box)
{
    QList<ViewData*> dataList = m_boxes.values(box);
    foreach(ViewData* data, dataList) {
        data->m_boxes.removeAll(box);
        if (data->m_boxes.count() == 0) {
            m_views.remove(data->view()->viewport());
            data->view()->viewport()->removeEventFilter(this);
            delete data;
        }
    }
    m_boxes.remove(box);
}


ViewData *GestureObserver::getViewData(QGraphicsView *view)
{
    if (m_views.contains(view->viewport()))
        return m_views.value(view->viewport());
    ViewData *data = new ViewData(view);
    view->viewport()->installEventFilter(this);
    connect(view->viewport(), SIGNAL(destroyed()), this, SLOT(viewDestroyed()));
    m_views.insert(view->viewport(), data);
    return data;
}

void GestureObserver::viewDestroyed()
{
    if (!sender()->isWidgetType())
        return;
    QWidget *viewport = qobject_cast<QWidget*>(sender());
    if (viewport || m_views.contains(viewport)) {
        ViewData *data = m_views.take(viewport);
        if (data) {
            foreach (GestureBox *box, data->m_boxes) {
                QList<ViewData*> boxes = m_boxes.values(box);
                m_boxes.remove(box);

                foreach(ViewData *viewData, boxes) {
                    if (viewData->widget() != viewport)
                        m_boxes.insertMulti(box, viewData);
                }
            }
            delete data;
        }
    }

}

bool GestureObserver::eventFilter(QObject *object, QEvent *event)
{
    if (!object->isWidgetType())
        return false;

    QEvent::Type type = event->type();
    if (type != QEvent::MouseButtonPress && type != QEvent::MouseButtonRelease &&
        type != QEvent::MouseMove)
        return false;

    QMouseEvent *mouseEvent = dynamic_cast<QMouseEvent*>(event);
    if (!mouseEvent || mouseEvent->modifiers() != Qt::NoModifier)
        return false;

    QWidget *view = dynamic_cast<QWidget*>(object);
    if (!view || !m_views.contains(view))
        return false;

    ViewData *data = m_views[view];
    if (!data || data->ignoreEvent(event))
        return false;

    bool consumed = false;

    switch (mouseEvent->type()) {
        case QEvent::MouseButtonPress:
            consumed = mouseEvent->buttons() == Qt::LeftButton && data->press(mouseEvent->pos());
            break;
        case QEvent::MouseButtonRelease:
            consumed = data->release(mouseEvent->pos());
            break;
        case QEvent::MouseMove:
            consumed = data->move(mouseEvent->pos());
            break;
        default:
            break;
    }
    return consumed;
}

// GestureBox

GestureBox::GestureBox(QGraphicsItem *parent)
    : QGraphicsItem(parent)
{
    GestureObserver::instance()->addBox(this);
    setFlag(QGraphicsItem::ItemHasNoContents, true);
}

GestureBox::~GestureBox()
{
    GestureObserver::instance()->removeBox(this);
}

QRectF GestureBox::rect() const
{
    QRectF result(m_boundingRect);
    result.moveTo(pos());
    return result;
}

void GestureBox::setRect(QRectF value)
{
    setPos(value.topLeft());
    value.moveTo(0.0, 0.0);
    if (value != m_boundingRect) {
        prepareGeometryChange();
        m_boundingRect = value;
        update();
    }
}

void GestureBox::setRect(qreal x, qreal y, qreal width, qreal height)
{
    setRect(QRectF(x, y, width, height));
}

QRectF GestureBox::boundingRect() const
{
    return m_boundingRect;
}

void GestureBox::paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(painter);
    Q_UNUSED(option);
    Q_UNUSED(widget);
}

QVariant GestureBox::itemChange(GraphicsItemChange change, const QVariant &value)
{
    switch (change) {
        case QGraphicsItem::ItemSceneHasChanged:
        case QGraphicsItem::ItemParentHasChanged:
            GestureObserver::instance()->addBox(this);
            break;
        default:
            break;
    }
    return value;
}


