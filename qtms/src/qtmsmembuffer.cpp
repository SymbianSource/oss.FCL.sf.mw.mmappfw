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
#include <tmsbuffer.h>
#include "qtmsmembuffer.h"
#include "tmsutility.h"

using namespace QTMS;
using namespace TMS;

QTMSMemBuffer::QTMSMemBuffer()
    {
    }

QTMSMemBuffer::~QTMSMemBuffer()
    {
    delete iBuffer;
    }

gint QTMSMemBuffer::Create(guint size, QTMSBuffer*& qbuffer,
        TMS::TMSBuffer*& tmsbuffer)
    {
    TRACE_PRN_FN_ENT;
    gint ret(TMS_RESULT_INSUFFICIENT_MEMORY);
    QTMSMemBuffer* self = new QTMSMemBuffer();
    if (self)
        {
        ret = self->PostConstruct();
        if (ret != TMS_RESULT_SUCCESS)
            {
            delete self;
            self = NULL;
            }
        self->iBuffer = tmsbuffer;
        }
    qbuffer = self;
    TRACE_PRN_FN_EXT;
    return ret;
    }

gint QTMSMemBuffer::PostConstruct()
    {
    gint ret(TMS_RESULT_SUCCESS);
    return ret;
    }

gint QTMSMemBuffer::GetType(QTMSBufferType& buffertype)
    {
    gint ret(QTMS_RESULT_UNINITIALIZED_OBJECT);
    if (iBuffer)
        {
        ret = iBuffer->GetType(buffertype);
        }
    return ret;
    }

/**
 * Gets the timestamp on the Buffer so that the framework can
 * determine the time at which this buffer has to be rendered
 * by the output device sink.
 *
 * @param ts timestamp in microseconds
 *
 */
gint QTMSMemBuffer::GetTimeStamp(guint64& ts)
    {
    gint ret(QTMS_RESULT_UNINITIALIZED_OBJECT);
    if (iBuffer)
        {
        ret = iBuffer->GetTimeStamp(ts);
        }
    return ret;
    }

/**
 * Sets the timestamp on the Buffer so that the framework can
 * determine the time at which this buffer has to be rendered
 * by the output device sink.
 *
 * @param ts timestamp in milliseconds
 *
 */
gint QTMSMemBuffer::SetTimeStamp(const guint64 ts)
    {
    gint ret(QTMS_RESULT_UNINITIALIZED_OBJECT);
    if (iBuffer)
        {
        ret = iBuffer->SetTimeStamp(ts);
        }
    return ret;
    }

/**
 * Gets the size of data in the buffer specified by the client.
 *
 * @param size size of data in bytes
 *
 */
gint QTMSMemBuffer::GetDataSize(guint& size)
    {
    gint ret(QTMS_RESULT_UNINITIALIZED_OBJECT);
    if (iBuffer)
        {
        ret = iBuffer->GetDataSize(size);
        }
    return ret;
    }

/**
 * Sets the size of data in the buffer after the client fill it.
 *
 * @param size size of data in bytes
 *
 */
gint QTMSMemBuffer::SetDataSize(const guint size)
    {
    gint ret(QTMS_RESULT_UNINITIALIZED_OBJECT);
    if (iBuffer)
        {
        ret = iBuffer->SetDataSize(size);
        }
    return ret;
    }

/**
 * Gets the pointer to the memory location associated with this
 * buffer where the data is stored.
 *
 * @param bufptr ptr to the data stored in the buffer.
 *
 */
gint QTMSMemBuffer::GetDataPtr(guint8*& bufptr)
    {
    gint ret(QTMS_RESULT_UNINITIALIZED_OBJECT);
    if (iBuffer)
        {
        ret = iBuffer->GetDataPtr(bufptr);
        }
    return ret;
    }
