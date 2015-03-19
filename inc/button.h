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

#ifndef BUTTON_H
#define BUTTON_H

#include <QGraphicsWidget>

QT_BEGIN_NAMESPACE
class QGraphicsSceneMouseEvent;
QT_END_NAMESPACE


class ButtonPrivate;

class Button : public QGraphicsWidget
{
    Q_OBJECT

public:
    enum State { NormalState, PressedState, DisabledState };

    Button(const QPixmap &normal, QGraphicsItem *parent = 0);

    Button(const QPixmap &normal, const QPixmap &pressed,
           QGraphicsItem *parent = 0);

    Button(const QPixmap &normal, const QPixmap &pressed,
           const QPixmap &disabled, QGraphicsItem *parent = 0);

    ~Button();

    QString text() const;
    void setText(const QString &value);

    void setPixmap(State state, const QPixmap &pixmap);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = 0);

signals:
    void clicked();
    void pressed();
    void released();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *e);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *e);

private:
    ButtonPrivate *d;
};

#endif
