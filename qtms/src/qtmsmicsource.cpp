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

#include <tmsmicsource.h>
#include <qtmsmicsource.h>
#include "qtmsmicsourceimpl.h"
#include "qtmswrapperexport.h"

using namespace QTMS;
using namespace TMS;

QTMSMicSource::QTMSMicSource() :
    iSource(NULL)
{
}

QTMSMicSource::~QTMSMicSource()
{
    delete (static_cast<TMSMicSource*> (iSource));
}

gint QTMSMicSource::GetType(QTMSSourceType& sourcetype)
{
    gint status(QTMS_RESULT_UNINITIALIZED_OBJECT);
    if (iSource) {
        //status = iSource->GetType(sourcetype);
        status = static_cast<TMSMicSource*> (iSource)->GetType(sourcetype);
    }
    return status;
}

// End of file
