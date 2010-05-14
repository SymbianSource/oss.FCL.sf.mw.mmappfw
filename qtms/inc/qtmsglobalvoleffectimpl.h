/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of "Eclipse Public License v1.0"
 * which accompanies this distribution, and is available
 * at the URL "http://www.eclipse.org/legal/epl-v10.html".
 *
 * Initial Contributors:
 * Nokia Corporation - initial contribution.
 *
 * Contributors:
 *
 * Description: QT Bindings for TMS
 *
 */

#ifndef QTMS_GLOBAL_VOLUME_EFFECT_IMPL_H
#define QTMS_GLOBAL_VOLUME_EFFECT_IMPL_H

#include <qtms.h>
#include <qtmsglobalvoleffect.h>
#include <tmseffectobsrvr.h>

// FORWARD DECLARATIONS
namespace TMS {
class TMSEffect;
}

namespace QTMS {

// FORWARD DECLARATIONS
class QTMSEffect;

// QTMSGlobalVolEffectImpl class
class QTMSGlobalVolEffectImpl : public QTMSGlobalVolEffect,
                                public TMS::TMSEffectObserver
    {
public:

    static gint Create(QTMSEffect*& qglobalvol, TMS::TMSEffect*& tmsglobalvol);
    virtual ~QTMSGlobalVolEffectImpl();

    gint AddObserver(TMS::TMSEffectObserver& obsrvr, gpointer user_data);
    gint RemoveObserver(TMS::TMSEffectObserver& obsrvr);

    //from TMSEffectObserver
    void EffectsEvent(const TMS::TMSEffect& tmseffect,
            TMS::TMSSignalEvent event);

private:
    QTMSGlobalVolEffectImpl();
    gint PostConstruct();
    };

} //namespace QTMS

#endif // QTMS_GLOBAL_VOLUME_EFFECT_IMPL_H

// End of file
