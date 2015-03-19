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

#ifndef GESTUREBOX_H
#define GESTUREBOX_H

#include <QGraphicsItem>

class GestureBox : public QGraphicsItem
{
public:
    GestureBox(QGraphicsItem *parent = 0);
    ~GestureBox();

    QRectF rect() const;
    void setRect(QRectF);
    void setRect(qreal, qreal, qreal, qreal);
    QRectF boundingRect() const;

protected:
    virtual void gestureMousePress(QPointF pos, bool &startGesture, bool &acceptClick) = 0;
    virtual void gestureStart(QPointF pos) = 0;
    virtual void gestureMove(QPointF pos, QPointF movement, QPointF speed) = 0;
    virtual void gestureEnd(QPointF pos, QPointF speed) = 0;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

private:
    friend class GestureData;
    friend class ViewData;
    QRectF m_boundingRect;

};

#endif // GESTUREBOX_H
