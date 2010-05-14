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
#include "tmsutility.h"
#include "qtmsmembuffer.h"
#include "qtmsbufferimpl.h"

using namespace QTMS;

gint QTMSBufferImpl::Create(QTMSBufferType buffertype, guint size,
        QTMSBuffer*& tmsbuffer)
    {
    gint ret(TMS_RESULT_INSUFFICIENT_MEMORY);
    TRACE_PRN_FN_ENT;
    switch (buffertype)
        {
        case QTMS_BUFFER_MEMORY:
            ret = QTMSMemBuffer::Create(size, tmsbuffer);
            break;
        default:
            ret = QTMS_RESULT_BUFFER_TYPE_NOT_SUPPORTED;
            break;
        }

    TRACE_PRN_FN_EXT;
    return ret;
    }

gint QTMSBufferImpl::Delete(QTMSBuffer*& tmsbuffer)
    {
    gint ret(TMS_RESULT_INVALID_ARGUMENT);
    TRACE_PRN_FN_ENT;
    QTMSBufferType buffertype;
    ret = tmsbuffer->GetType(buffertype);
    switch (buffertype)
        {
        case QTMS_BUFFER_MEMORY:
            delete (QTMSMemBuffer*) (tmsbuffer);
            tmsbuffer = NULL;
            ret = QTMS_RESULT_SUCCESS;
            break;
        default:
            ret = QTMS_RESULT_BUFFER_TYPE_NOT_SUPPORTED;
            break;
        }

    TRACE_PRN_FN_EXT;
    return ret;
    }

// End of file
