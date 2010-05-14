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

#ifndef QTMS_MIC_SOURCE_H
#define QTMS_MIC_SOURCE_H

#include <qtms.h>
#include <qtmssource.h>
#include <QObject>
#include "qtmswrapperexport.h"

// FORWARD DECLARATIONS
namespace TMS {
class TMSSource;
}

namespace QTMS {

/**
 * QTMSMicSource class
 *
 * This class instantiates QTMS mic source object.
 *
 * @lib QTMSapi.lib
 *
 */
class QTMS_WRAPPER_DLL_EXPORT QTMSMicSource : public QObject,
                                              public QTMSSource
    {
    Q_OBJECT
public:
    /**
     * Destructor
     */
    virtual ~QTMSMicSource();

    /**
     * Return source type.
     *
     * This function can be called at any time.
     *
     * @param  sourcetype
     *      The type of sink object (QTMS_SOURCE_MIC).
     *
     * @return
     *      QTMS_RESULT_SUCCESS if the operation was successful.
     *
     */
    virtual gint GetType(QTMSSourceType& sourcetype);

protected:
    /**
     * Constructor
     */
    QTMSMicSource();

protected:
    TMS::TMSSource *iSource;
    };

} //namespace QTMS

#endif //QTMS_MIC_SOURCE_H

// End of file

