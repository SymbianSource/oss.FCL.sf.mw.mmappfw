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

#include <qtms.h>
//#include "qtmsclientsourceimpl.h"
#include "tmsutility.h"
#include "qtmssourceimpl.h"
//#include "qtmsmodemsourceimpl.h"
//#include "qtmsmicsourceimpl.h"

using namespace QTMS;

gint QTMSSourceImpl::Create(QTMSSourceType /*sourcetype*/,
        QTMSSource*& /*qtmssource*/)
    {
    gint ret(QTMS_RESULT_INSUFFICIENT_MEMORY);
  /*  TRACE_PRN_FN_ENT;
    switch (sourcetype)
        {
        case QTMS_SOURCE_CLIENT:
            ret = QTMSClientSourceImpl::Create(QTMSsource);
            break;
        case QTMS_SOURCE_MODEM:
            ret = QTMSModemSourceImpl::Create(QTMSsource);
            break;
        case QTMS_SOURCE_MIC:
            ret = QTMSMicSourceImpl::Create(QTMSsource);
            break;
        default:
            ret = QTMS_RESULT_SOURCE_TYPE_NOT_SUPPORTED;
            break;
        }

    TRACE_PRN_FN_EXT;
    */
    return ret;
    }

gint QTMSSourceImpl::Delete(QTMSSource*& /*qtmssource*/)
    {
    gint ret(QTMS_RESULT_INVALID_ARGUMENT);
    TRACE_PRN_FN_ENT;
 /*   QTMSSourceType sourcetype;
    ret = QTMSsource->GetType(sourcetype);
    switch (sourcetype)
        {
        case QTMS_SOURCE_CLIENT:
            delete (QTMSClientSourceImpl*) (QTMSsource);
            QTMSsource = NULL;
            ret = QTMS_RESULT_SUCCESS;
            break;
        case QTMS_SOURCE_MODEM:
            {
            delete (QTMSModemSourceImpl*) (QTMSsource);
            QTMSsource = NULL;
            ret = QTMS_RESULT_SUCCESS;
            }
            break;
        case QTMS_SOURCE_MIC:
            {
            delete (QTMSMicSourceImpl*) (QTMSsource);
            QTMSsource = NULL;
            ret = QTMS_RESULT_SUCCESS;
            }
            break;
        default:
            ret = QTMS_RESULT_SOURCE_TYPE_NOT_SUPPORTED;
            break;
        }

    TRACE_PRN_FN_EXT;
    */
    return ret;
    }

// End of file
