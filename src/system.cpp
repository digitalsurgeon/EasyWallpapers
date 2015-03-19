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

#include <QTimer>
#include <QWidget>

#include "system.h"

#ifdef Q_OS_SYMBIAN
#include <eikenv.h>
#include <coemain.h>
#include <aknappui.h>
#endif


System::System()
    : QObject()
{

}

System::~System()
{

}

System *System::instance()
{
    static System result;
    return &result;
}

void System::setViewMode(QWidget *window, ViewMode mode)
{
#if defined(Q_OS_SYMBIAN)
    Q_UNUSED(window);

    // we need to use singleshot since CAknAppUi object is only
    // available in the main loop
    if (mode == PortraitMode)
        QTimer::singleShot(0, instance(), SLOT(setPortraitMode()));
    else
        QTimer::singleShot(0, instance(), SLOT(setLandscapeMode()));

#elif defined(Q_WS_MAEMO_5)
    if (mode == PortraitMode)
        window->setAttribute(Qt::WA_Maemo5PortraitOrientation, true);
    else if (mode == LandscapeMode)
        window->setAttribute(Qt::WA_Maemo5LandscapeOrientation, true);
    else
        window->setAttribute(Qt::WA_Maemo5AutoOrientation, true);
#else
    Q_UNUSED(window);
    Q_UNUSED(mode);
#endif
}

#ifdef Q_OS_SYMBIAN

void System::setPortraitMode()
{
    CAknAppUi *aknAppUi = dynamic_cast<CAknAppUi *>(CEikonEnv::Static()->AppUi());

    if (aknAppUi)
        aknAppUi->SetOrientationL(CAknAppUi::EAppUiOrientationPortrait);
}

void System::setLandscapeMode()
{
    CAknAppUi *aknAppUi = dynamic_cast<CAknAppUi *>(CEikonEnv::Static()->AppUi());

    if (aknAppUi)
        aknAppUi->SetOrientationL(CAknAppUi::EAppUiOrientationLandscape);
}

#endif
