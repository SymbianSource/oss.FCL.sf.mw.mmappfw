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

#ifndef QTMS_MEM_BUFFER_H
#define QTMS_MEM_BUFFER_H

#include <qtmsbuffer.h>

namespace TMS {
class TMSBuffer;
}

namespace QTMS {

// QTMSMemBuffer class
class QTMSMemBuffer : public QTMSBuffer
    {
public:
    virtual ~QTMSMemBuffer();

    /**
     * Creates Memory Buffer object of the given size.
     * The memory is allocated on the heap.
     *
     * @param  size
     *      Size of the buffer to be created.
     *
     * @param  qbuffer
     *      Created QTMSBuffer object.
     *
     * @param  buffer
     *      TMS::TMSBuffer buffer object.
     *
     * @return
     *      TMS_RESULT_SUCCESS if buffer is created successfully.
     *      TMS_RESULT_INSUFFICIENT_MEMORY if buffer creation failed due to
     *      insufficient memory.
     *      TMS_RESULT_BUFFER_TYPE_NOT_SUPPORTED if the buffer type is not
     *      supported.
     *
     */
    static gint Create(guint size, QTMSBuffer*& qbuffer,
            TMS::TMSBuffer*& tmsbuffer);

    /**
     * Return buffer type.
     *
     * @param  buftype
     *      Type of the buffer object (QTMS_BUFFER_MEMORY).
     *
     * @return
     *      QTMS_RESULT_SUCCESS if the operation was successful.
     *
     */
    virtual gint GetType(QTMSBufferType& buftype);

    /**
     Gets the timestamp on the Buffer so that the framework can
     determine the time at which this buffer has to be rendered
     by the output device sink.

     @param ts
     timestamp in microseconds
     */
    virtual gint GetTimeStamp(guint64& ts);

    /**
     Sets the timestamp on the Buffer so that the framework can
     determine the time at which this buffer has to be rendered
     by the output device sink.

     @param ts
     timestamp in milliseconds
     */
    virtual gint SetTimeStamp(const guint64 ts);

    /**
     Gets the size of data in the buffer specified by the client.

     @param size
     size of data in bytes
     */
    virtual gint GetDataSize(guint& size);

    /**
     Sets the size of data in the buffer after the client
     fill it.

     @param size
     size of data in bytes
     */
    virtual gint SetDataSize(const guint size);

    /**
     Gets the pointer to the memory location associated with this
     buffer where the data is stored.

     @param bufptr
     ptr to the data stored in the buffer.
     */
    virtual gint GetDataPtr(guint8*& bufptr);

private:
    QTMSMemBuffer();
    gint PostConstruct();

protected:
    TMS::TMSBuffer* iBuffer;
    };

} //namespace QTMS

#endif //QTMS_MEM_BUFFER_H
