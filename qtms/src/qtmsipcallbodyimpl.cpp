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
#include <qtmsstream.h>
#include "qtmsstreamimpl.h"
#include "tmsutility.h"
#include "qtmsipcallimpl.h"

using namespace QTMS;

QTMSIPCallImpl::QTMSIPCallImpl()
    {
    TRACE_PRN_FN_ENT;
    TRACE_PRN_FN_EXT;
    }

QTMSIPCallImpl::~QTMSIPCallImpl()
    {
    TRACE_PRN_FN_ENT;

    TRACE_PRN_FN_EXT;
    }

/*
gint QTMSIPCallImpl::Create(QTMSCallBody*& bodyimpl)
    {
    gint ret(QTMS_RESULT_INSUFFICIENT_MEMORY);
    QTMSIPCallImpl* self = new QTMSIPCallImpl;

    TRACE_PRN_FN_ENT;
    if (self)
        {
        ret = self->PostConstruct();
        if (ret != QTMS_RESULT_SUCCESS)
            {
            delete self;
            self = NULL;
            }
        }
    bodyimpl = self;

    TRACE_PRN_FN_EXT;
    return ret;
    }
*/
gint QTMSIPCallImpl::PostConstruct()
    {
    gint ret(QTMS_RESULT_SUCCESS);
    TRACE_PRN_FN_ENT;


    TRACE_PRN_FN_EXT;
    return ret;
    }

QTMSCallType QTMSIPCallImpl::GetCallType()
    {
    QTMSCallType ctype(QTMS_CALL_IP);
    return ctype;
    }

gint QTMSIPCallImpl::GetCallContextId(guint& ctxid)
    {
    gint ret(QTMS_RESULT_FEATURE_NOT_SUPPORTED);
    ctxid = 0;
    return ret;
    }

gint QTMSIPCallImpl::CreateStream(QTMSStreamType type, QTMSStream*& strm)
    {
    gint ret(QTMS_RESULT_CALL_TYPE_NOT_SUPPORTED);
    strm = NULL;
    switch (type)
        {
        case QTMS_STREAM_UPLINK:
        case QTMS_STREAM_DOWNLINK:
 //           ret = QTMSStreamImpl::Create(QTMS_CALL_IP, type, iSession, strm);
            break;
        default:
            break;
        }
    TRACE_PRN_FN_EXT;
    return ret;
    }

gint QTMSIPCallImpl::DeleteStream(QTMSStream*& /*strm*/)
    {
    gint ret(QTMS_RESULT_SUCCESS);
    TRACE_PRN_FN_ENT;
   // ret = RemStreamFromList(strm);
    TRACE_PRN_FN_EXT;
    return ret;
    }

gint QTMSIPCallImpl::AddStreamToList(QTMSStream* /*strm*/)
    {
    gint ret(QTMS_RESULT_SUCCESS);
    TRACE_PRN_FN_ENT;
  //  iStreamsVector.push_back(strm);
    TRACE_PRN_FN_EXT;
    return ret;
    }

gint QTMSIPCallImpl::RemStreamFromList(QTMSStream*& /*strm*/)
    {
    gint ret(QTMS_RESULT_DOES_NOT_EXIST);

    TRACE_PRN_FN_EXT;
    return ret;
    }

// End of file
