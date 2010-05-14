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
//#include "qtmsclientsinkimpl.h"
#include "tmsutility.h"
#include "qtmssinkimpl.h"
//#include "qtmsmodemsinkimpl.h"
//#include "qtmsspeakersinkimpl.h"

using namespace QTMS;

gint QTMSSinkImpl::Create(QTMSSinkType /*sinktype*/, QTMSSink*& /*qtmssink*/)
    {
    gint ret(QTMS_RESULT_INSUFFICIENT_MEMORY);
    TRACE_PRN_FN_ENT;
  /*  switch (sinktype)
        {
        case QTMS_SINK_CLIENT:
            ret = QTMSClientSinkImpl::Create(QTMSsink);
            break;
        case QTMS_SINK_MODEM:
            ret = QTMSModemSinkImpl::Create(QTMSsink);
            break;
        case QTMS_SINK_SPEAKER:
            ret = QTMSSpeakerSinkImpl::Create(QTMSsink);
            break;
        default:
            ret = QTMS_RESULT_SINK_TYPE_NOT_SUPPORTED;
            break;
        }

    TRACE_PRN_FN_EXT;
    */
    return ret;
    }

gint QTMSSinkImpl::Delete(QTMSSink*& /*qtmssink*/)
    {
    gint ret(QTMS_RESULT_INVALID_ARGUMENT);
    TRACE_PRN_FN_ENT;
   /* QTMSSinkType sinktype;
    ret = QTMSsink->GetType(sinktype);
    switch (sinktype)
        {
        case QTMS_SINK_CLIENT:
            {
            delete (QTMSClientSinkImpl*) (QTMSsink);
            QTMSsink = NULL;
            ret = QTMS_RESULT_SUCCESS;
            }
            break;
        case QTMS_SINK_MODEM:
            {
            delete (QTMSModemSinkImpl*) (QTMSsink);
            QTMSsink = NULL;
            ret = QTMS_RESULT_SUCCESS;
            }
            break;
        case QTMS_SINK_SPEAKER:
            {
            delete (QTMSSpeakerSinkImpl*) (QTMSsink);
            QTMSsink = NULL;
            ret = QTMS_RESULT_SUCCESS;
            }
            break;
        default:
            ret = QTMS_RESULT_SINK_TYPE_NOT_SUPPORTED;
            break;
        }

    TRACE_PRN_FN_EXT;
    */
    return ret;
    }

// End of file
