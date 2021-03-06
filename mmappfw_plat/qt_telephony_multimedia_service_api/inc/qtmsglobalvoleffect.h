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

#ifndef QTMS_GLOBAL_VOL_EFFECT_H
#define QTMS_GLOBAL_VOL_EFFECT_H

#include <qtms.h>
#include <qtmseffect.h>
#include <QObject>
#include <qglobal.h>
#include "qtmswrapperexport.h"

// FORWARD DECLARATIONS
namespace TMS
{
class TMSEffect;
}

namespace QTMS
{

/**
 * QTMSGlobalVolEffect class
 *
 * This class provides volume level control in the global QTMS scope. Volume
 * level changes done by any client are adjusted accross all calls serviced
 * by the QTMS server.
 *
 * @lib QTMSapi.lib
 *
 */
class QTMS_WRAPPER_DLL_EXPORT QTMSGlobalVolEffect : public QObject,
                                                    public QTMSEffect
{
    Q_OBJECT
public:
    /**
     * Destructor
     */
    virtual ~QTMSGlobalVolEffect();

    /**
     * Set global audio volume level. The volume level should be between 0
     * and the max value returned by GetMaxLevel().
     *
     * @param  level
     *      the volume level to be set.
     * @return
     *      QTMS_RESULT_SUCCESS if the operation was successful.
     */
    gint SetLevel(const guint level);

    /**
     * Return current global audio volume level.
     *
     * @param  level
     *      Volume level to be set.
     *
     * @return
     *      QTMS_RESULT_SUCCESS if the operation was successful.
     *
     */
    gint GetLevel(guint& level);

    /**
     * Return max volume level supported by the device.
     *
     * @param  level
     *      Max volume level.
     *
     * @return
     *      QTMS_RESULT_SUCCESS if the operation was successful.
     *
     */
    gint GetMaxLevel(guint& level);

    /**
     * Return effect type.
     *
     * @param  effecttype
     *      Type of the effect object (QTMS_EFFECT_GLOBAL_VOL).
     *
     * @return
     *      QTMS_RESULT_SUCCESS if the operation was successful.
     *
     */
    virtual gint GetType(QTMSEffectType& effecttype);

    Q_SIGNALS:
    void EffectsEvent(const QTMSEffect& tmseffect, QTMSSignalEvent event);

protected:
    /**
     * Constructor
     */
    QTMSGlobalVolEffect();

protected:
    TMS::TMSEffect* iEffect;
};

} //namespace QTMS

#endif //QTMS_GLOBAL_VOL_EFFECT_H
