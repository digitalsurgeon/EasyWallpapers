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

#ifndef GESTUREBOX_P_H
#define GESTUREBOX_P_H

#include <QObject>
#include <QPointer>
#include <QTime>
#include <QPoint>
#include <QPointF>
#include <QGraphicsView>

#include "gesturebox.h"

class GestureData : public QObject
{
public:
    inline GestureData(QPoint pos, const QList<GestureBox*> &boxes, QGraphicsView *view,
                       QList<QEvent*> &ignoreList, bool sendClickEvents);

    inline void release(QPoint pos);
    inline void move(QPoint pos);
    inline bool ended() const { return m_state == Ended; }

private:
    enum State {Pressed, Moving, Ended};
    State m_state;
    QGraphicsView * const m_view;
    const QPoint m_pressPos;
    const QPointF m_scenePressPos;
    const QList<GestureBox*> m_boxes;
    QList<QEvent*> &m_ignoreList;
    QTime m_time;
    QPointF m_currentScenePos;
    QPointF m_speed;
    bool m_sendClickEvents;

    void sendClick();
};

class ViewData
{
public:
    ViewData(QGraphicsView *view) :  m_view(view), m_gesture(0), m_widget(view->viewport()) {}
    inline ~ViewData();

    inline bool press(QPoint pos);
    inline bool release(QPoint pos);
    inline bool move(QPoint pos);

    QGraphicsView *view() { return m_view; }
    QWidget *widget() { return m_widget; }
    inline bool ignoreEvent(QEvent *event);

    QList<GestureBox*> m_boxes;

private:
    QGraphicsView *const m_view;
    QPointer<GestureData> m_gesture;
    QList<QEvent*> m_ignored;
    QWidget * const m_widget;

    inline QList<GestureBox*> getBoxList(QPoint pos);
    inline bool startGesture(GestureBox *box, QPointF scenePos, bool &sendClick);
};

class GestureObserver : public QObject
{
    Q_OBJECT
public:
    static inline GestureObserver *instance();
    inline void addBox(GestureBox *box);
    inline void removeBox(GestureBox *box);

protected:
    bool eventFilter(QObject *object, QEvent *event);

private:
    QHash<QWidget*, ViewData*> m_views;
    QHash<GestureBox*, ViewData*> m_boxes;

    GestureObserver() {}
    ~GestureObserver() {}
    ViewData *getViewData(QGraphicsView *view);

private slots:
    void viewDestroyed();

};

#endif // GESTUREBOX_P_H
