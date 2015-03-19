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

#include <QPixmap>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>

#if defined(Q_OS_SYMBIAN)
#include <e32std.h>
#include <touchfeedback.h>
#endif

#include "button.h"


class ButtonPrivate
{
public:
    ButtonPrivate(Button *qptr);

    void init();

    Button *q;
    QString text;
    bool isPressed;
    QPixmap normalPixmap;
    QPixmap pressedPixmap;
    QPixmap disabledPixmap;
};

ButtonPrivate::ButtonPrivate(Button *qptr)
    : q(qptr), isPressed(false)
{

}

void ButtonPrivate::init()
{
    q->setMinimumSize(normalPixmap.size());
    q->setMaximumSize(normalPixmap.size());
}


Button::Button(const QPixmap &normal, QGraphicsItem *parent)
    : QGraphicsWidget(parent),
      d(new ButtonPrivate(this))
{
    d->normalPixmap = normal;
    d->init();
}

Button::Button(const QPixmap &normal, const QPixmap &pressed,
               QGraphicsItem *parent)
    : QGraphicsWidget(parent),
      d(new ButtonPrivate(this))
{
    d->normalPixmap = normal;
    d->pressedPixmap = pressed;
    d->init();
}

Button::Button(const QPixmap &normal, const QPixmap &pressed,
               const QPixmap &disabled, QGraphicsItem *parent)
    : QGraphicsWidget(parent),
      d(new ButtonPrivate(this))
{
    d->normalPixmap = normal;
    d->pressedPixmap = pressed;
    d->disabledPixmap = disabled;
    d->init();
}

Button::~Button()
{
    delete d;
}

void Button::setPixmap(State state, const QPixmap &pixmap)
{
    bool dirty = false;

    if (state == NormalState) {
        d->normalPixmap = pixmap;
        dirty = (!d->isPressed && isEnabled());
        setMinimumSize(d->normalPixmap.size());
        setMaximumSize(d->normalPixmap.size());
    } else if (state == PressedState) {
        dirty = d->isPressed;
        d->pressedPixmap = pixmap;
    } else if (state == DisabledState) {
        dirty = !isEnabled();
        d->disabledPixmap = pixmap;
    }

    if (dirty)
        update();
}

QString Button::text() const
{
    return d->text;
}

void Button::setText(const QString &value)
{
    if (d->text != value) {
        d->text = value;
        update();
    }
}

void Button::mousePressEvent(QGraphicsSceneMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        d->isPressed = true;
        update();
        emit pressed();
		
#if defined(Q_OS_SYMBIAN)
            MTouchFeedback* feedBack = MTouchFeedback::Instance();
            if (feedBack)
            {
                feedBack->InstantFeedback(ETouchFeedbackBasic);
            }
#endif
    }
}

void Button::mouseReleaseEvent(QGraphicsSceneMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        const bool isClick = d->isPressed;

        d->isPressed = false;
        update();
        emit released();
		
#if defined(Q_OS_SYMBIAN)
            MTouchFeedback* feedBack = MTouchFeedback::Instance();
            if (feedBack)
            {
                feedBack->InstantFeedback(ETouchFeedbackBasic);
            }
#endif

        if (isClick && contains(e->pos()))
            emit clicked();
    }
}

void Button::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                   QWidget *widget)
{
    Q_UNUSED(widget);

    if (!(option->state & QStyle::State_Enabled)) {
        const bool support = !d->disabledPixmap.isNull();
        painter->drawPixmap(0, 0, support ? d->disabledPixmap : d->normalPixmap);
    } else if (d->isPressed && !d->pressedPixmap.isNull())
        painter->drawPixmap(0, 0, d->pressedPixmap);
    else if (!d->normalPixmap.isNull())
        painter->drawPixmap(0, 0, d->normalPixmap);


    if (!d->text.isEmpty()) {
        const QRect &textRect = boundingRect().toRect();

        QFontMetrics metrics(font());
        const QString &elidedText = metrics.elidedText(d->text, Qt::ElideRight,
                                                       textRect.width());

        painter->setFont(font());
        painter->setPen(Qt::white);
        painter->drawText(textRect, Qt::TextSingleLine | Qt::AlignCenter, elidedText);
    }
}
