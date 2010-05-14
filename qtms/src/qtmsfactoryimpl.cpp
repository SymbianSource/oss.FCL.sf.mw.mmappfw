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

#include <tms.h>
#include <tmsfactory.h>
#include <tmsformat.h>
#include <qtms.h>
#include <qtmscall.h>
#include <qtmsformat.h>
#include <qtmseffect.h>
#include <qtmsbuffer.h>
#include <qtmssource.h>
#include <qtmssink.h>
#include <qtmsringtone.h>
#include <qtmsdtmf.h>
#include <qtmsinbandtone.h>
#include "tmsutility.h"
#include "qtmscallimpl.h"
#include "qtmsglobalroutingimpl.h"
#include "qtmsglobalvoleffectimpl.h"
#include "qtmsglobalgaineffectimpl.h"
#include "qtmsvolumeeffectimpl.h"
#include "qtmsgaineffectimpl.h"
#include "qtmsclientsinkimpl.h"
#include "qtmsmodemsinkimpl.h"
#include "qtmsspeakersinkimpl.h"
#include "qtmsclientsourceimpl.h"
#include "qtmsmodemsourceimpl.h"
#include "qtmsmicsourceimpl.h"
#include "qtmsringtoneimpl.h"
#include "qtmsdtmfimpl.h"
#include "qtmsinbandtoneimpl.h"
#include "qtmsfactoryimpl.h"
#include "qtmspcmimpl.h"
#include "qtmsamrimpl.h"
#include "qtmsg711impl.h"
#include "qtmsg729impl.h"
#include "qtmsilbcimpl.h"
#include "qtmsmembuffer.h"

using namespace QTMS;
using namespace TMS;

QTMSFactoryImpl::QTMSFactoryImpl()
    {
    TMSVer* ver = NULL;
    TMSFactory::CreateFactory(iFactory, *ver);
    }

QTMSFactoryImpl::~QTMSFactoryImpl()
    {
    delete iFactory;
    }

gint QTMSFactoryImpl::CreateCall(QTMSCallType ctype, QTMSCall*& qtmscall,
        guint /*ctxid*/)
    {
    gint ret(QTMS_RESULT_UNINITIALIZED_OBJECT);

    TMSCall* tmscall(NULL);
    if (iFactory)
        {
        ret = iFactory->CreateCall((TMSCallType) ctype, tmscall);

        if (ret == TMS_RESULT_SUCCESS)
            {
            ret = QTMSCallImpl::Create(qtmscall, tmscall);
            }
        }
    return ret;
    }

gint QTMSFactoryImpl::DeleteCall(QTMSCall*& qtmscall)
    {
    gint ret(QTMS_RESULT_SUCCESS);
    delete qtmscall;
    qtmscall = NULL;
    return ret;
    }

gint QTMSFactoryImpl::IsCallTypeSupported(QTMSCallType ctype, gboolean& flag)
    {
    gint ret(QTMS_RESULT_SUCCESS);

    switch (ctype)
        {
        case QTMS_CALL_CS:
        case QTMS_CALL_IP:
            flag = ETrue;
            break;
        case QTMS_CALL_ECS: //from TB 10.1
        default:
            flag = EFalse;
            break;
        }
    return ret;
    }

gint QTMSFactoryImpl::GetSupportedFormats(const QTMSStreamType strmtype,
        FormatVector& fmtlist)
    {
    gint ret(QTMS_RESULT_SUCCESS);
    TMS::FormatVector tmsfmtlist;
    if(iFactory)
        {
        iFactory->GetSupportedFormats(strmtype,tmsfmtlist);
        }

    std::vector<TMSFormat*>::iterator itCodecs = tmsfmtlist.begin();
    TMSFormatType fmttype;
    for (; itCodecs < tmsfmtlist.end(); itCodecs++)
        {
        (*itCodecs)->GetType(fmttype);
        QTMSFormat* qtmsfmt(NULL);
        switch (fmttype)
            {
            case QTMS_FORMAT_PCM:
                ret = QTMSPCMFormatImpl::Create(qtmsfmt,*itCodecs);
                break;
            case QTMS_FORMAT_AMR:
                ret = QTMSAMRFormatImpl::Create(qtmsfmt,*itCodecs);
                break;
            case QTMS_FORMAT_G711:
                ret = QTMSG711FormatImpl::Create(qtmsfmt,*itCodecs);
                break;
            case QTMS_FORMAT_G729:
                ret = QTMSG729FormatImpl::Create(qtmsfmt,*itCodecs);
                break;
            case QTMS_FORMAT_ILBC:
                ret = QTMSILBCFormatImpl::Create(qtmsfmt,*itCodecs);
                break;
            default:
                break;
            }

        if(qtmsfmt)
            {
            fmtlist.push_back(qtmsfmt);
            }
        }

    return ret;
    }

gint QTMSFactoryImpl::CreateFormat(QTMSFormatType fmttype,
        QTMSFormat*& qtmsfmt)
    {
    gint ret(QTMS_RESULT_SUCCESS);

    TMSFormat* tmsformat(NULL);

    if (iFactory)
        {
        ret = iFactory->CreateFormat((TMSFormatType) fmttype, tmsformat);

        if (ret == TMS_RESULT_SUCCESS)
            {
            switch (fmttype)
                {
                case QTMS_FORMAT_PCM:
                    ret = QTMSPCMFormatImpl::Create(qtmsfmt, tmsformat);
                    break;
                case QTMS_FORMAT_AMR:
                    ret = QTMSAMRFormatImpl::Create(qtmsfmt, tmsformat);
                    break;
                case QTMS_FORMAT_G711:
                    ret = QTMSG711FormatImpl::Create(qtmsfmt, tmsformat);
                    break;
                case QTMS_FORMAT_G729:
                    ret = QTMSG729FormatImpl::Create(qtmsfmt, tmsformat);
                    break;
                case QTMS_FORMAT_ILBC:
                    ret = QTMSILBCFormatImpl::Create(qtmsfmt, tmsformat);
                    break;
                default:
                    break;
                }
            }
        }

    return ret;
    }

gint QTMSFactoryImpl::DeleteFormat(QTMSFormat*& qtmsfmt)
    {
    gint ret(QTMS_RESULT_FORMAT_TYPE_NOT_SUPPORTED);

    QTMSFormatType fmttype;
    ret = qtmsfmt->GetType(fmttype);
    switch (fmttype)
        {
        case QTMS_FORMAT_PCM:
            delete (static_cast<QTMSPCMFormatImpl*>(qtmsfmt));
            qtmsfmt = NULL;
            ret = QTMS_RESULT_SUCCESS;
            break;
        case QTMS_FORMAT_AMR:
            delete (static_cast<QTMSAMRFormatImpl*>(qtmsfmt));
            qtmsfmt = NULL;
            ret = QTMS_RESULT_SUCCESS;
            break;
        case QTMS_FORMAT_G711:
            delete (static_cast<QTMSG711FormatImpl*>(qtmsfmt));
            qtmsfmt = NULL;
            ret = QTMS_RESULT_SUCCESS;
            break;
        case QTMS_FORMAT_G729:
            delete (static_cast<QTMSG729FormatImpl*>(qtmsfmt));
            qtmsfmt = NULL;
            ret = QTMS_RESULT_SUCCESS;
            break;
        case QTMS_FORMAT_ILBC:
            delete (static_cast<QTMSILBCFormatImpl*>(qtmsfmt));
            qtmsfmt = NULL;
            ret = QTMS_RESULT_SUCCESS;
            break;
        default:
            break;
        }

    return ret;
    }

gint QTMSFactoryImpl::CreateEffect(QTMSEffectType tmseffecttype,
        QTMSEffect*& qtmseffect)
    {
    //gint ret(QTMS_RESULT_UNINITIALIZED_OBJECT);
    gint ret(TMS_RESULT_SUCCESS);
    TMSEffect* tmseffect(NULL);
    if (iFactory)
        {
        ret = iFactory->CreateEffect((TMSEffectType) tmseffecttype, tmseffect);

        if (ret == TMS_RESULT_SUCCESS)
            {
            switch (tmseffecttype)
                {
                case TMS_EFFECT_VOLUME:
                    ret = QTMSVolumeEffectImpl::Create(qtmseffect, tmseffect);
                    break;
                case TMS_EFFECT_GAIN:
                    ret = QTMSGainEffectImpl::Create(qtmseffect, tmseffect);
                    break;
                case TMS_EFFECT_GLOBAL_VOL:
                    ret = QTMSGlobalVolEffectImpl::Create(qtmseffect,
                            tmseffect);
                    break;
                case TMS_EFFECT_GLOBAL_GAIN:
                    ret = QTMSGlobalGainEffectImpl::Create(qtmseffect,
                            tmseffect);
                    break;
                default:
                    ret = TMS_RESULT_EFFECT_TYPE_NOT_SUPPORTED;
                    break;
                }
            }
        }

    return ret;
    }

gint QTMSFactoryImpl::DeleteEffect(QTMSEffect*& qtmseffect)
    {
    gint ret(QTMS_RESULT_SUCCESS);

    QTMSEffectType effecttype;
    ret = qtmseffect->GetType(effecttype);
    switch (effecttype)
        {
        case QTMS_EFFECT_VOLUME:
            delete (static_cast<QTMSVolumeEffectImpl*> (qtmseffect));
            qtmseffect = NULL;
            break;
        case QTMS_EFFECT_GAIN:
            delete (static_cast<QTMSGainEffectImpl*> (qtmseffect));
            qtmseffect = NULL;
            break;
        case QTMS_EFFECT_GLOBAL_VOL:
            delete (static_cast<QTMSGlobalVolEffectImpl*> (qtmseffect));
            qtmseffect = NULL;
            break;
        case QTMS_EFFECT_GLOBAL_GAIN:
            delete (static_cast<QTMSGlobalGainEffectImpl*> (qtmseffect));
            qtmseffect = NULL;
            break;
        default:
            ret = QTMS_RESULT_EFFECT_TYPE_NOT_SUPPORTED;
            break;
        }

    return ret;
    }

gint QTMSFactoryImpl::CreateBuffer(QTMSBufferType buffertype, guint size,
        QTMSBuffer*& qtmsbuffer)
    {
    gint ret(TMS_RESULT_INSUFFICIENT_MEMORY);
    TRACE_PRN_FN_ENT;

    switch (buffertype)
        {
        case QTMS_BUFFER_MEMORY:
            ret = QTMSMemBuffer::Create(size, qtmsbuffer);
            break;
        default:
            ret = QTMS_RESULT_BUFFER_TYPE_NOT_SUPPORTED;
            break;
        }

    TRACE_PRN_FN_EXT;
    return ret;
    }

gint QTMSFactoryImpl::DeleteBuffer(QTMSBuffer*& qtmsbuffer)
    {
    delete (static_cast<QTMSMemBuffer*> (qtmsbuffer));
    qtmsbuffer = NULL;
    return QTMS_RESULT_SUCCESS;
    }

gint QTMSFactoryImpl::CreateSource(QTMSSourceType srctype,
        QTMSSource*& qtmssrc)
    {
    gint ret(QTMS_RESULT_SUCCESS);

    TMS::TMSSource* tmssource(NULL);
    if (iFactory)
        {
        ret = iFactory->CreateSource(srctype, tmssource);

        if (ret == TMS_RESULT_SUCCESS)
            {
            switch (srctype)
                {
                case QTMS_SOURCE_CLIENT:
                    ret = QTMSClientSourceImpl::Create(qtmssrc, tmssource);
                    break;
                case QTMS_SOURCE_MODEM:
                    ret = QTMSModemSourceImpl::Create(qtmssrc, tmssource);
                    break;
                case QTMS_SOURCE_MIC:
                    ret = QTMSMicSourceImpl::Create(qtmssrc, tmssource);
                    break;
                default:
                    break;
                }
            }
        }

    return ret;
    }

gint QTMSFactoryImpl::DeleteSource(QTMSSource*& qtmssrc)
    {
    gint ret(TMS_RESULT_INVALID_ARGUMENT);

    QTMSSourceType sourcetype;
    ret = qtmssrc->GetType(sourcetype);
    switch (sourcetype)
        {
        case QTMS_SOURCE_CLIENT:
            delete (static_cast<QTMSClientSourceImpl*>(qtmssrc));
            qtmssrc = NULL;
            ret = QTMS_RESULT_SUCCESS;
            break;
        case QTMS_SOURCE_MODEM:
            {
            delete (static_cast<QTMSModemSourceImpl*>(qtmssrc));
            qtmssrc = NULL;
            ret = TMS_RESULT_SUCCESS;
            }
            break;
        case TMS_SOURCE_MIC:
            {
            delete (static_cast<QTMSMicSourceImpl*>(qtmssrc));
            qtmssrc = NULL;
            ret = TMS_RESULT_SUCCESS;
            }
            break;
        default:
            ret = TMS_RESULT_SOURCE_TYPE_NOT_SUPPORTED;
            break;
        }

    TRACE_PRN_FN_EXT;
    return ret;
    }

gint QTMSFactoryImpl::CreateSink(QTMSSinkType sinktype, QTMSSink*& qtmssink)
    {
    gint ret(QTMS_RESULT_SUCCESS);

    TMS::TMSSink* tmssink(NULL);

    if (iFactory)
        {
        ret = iFactory->CreateSink(sinktype, tmssink);

        if (ret == TMS_RESULT_SUCCESS)
            {
            switch (sinktype)
                {
                case QTMS_SINK_CLIENT:
                    ret = QTMSClientSinkImpl::Create(qtmssink, tmssink);
                    break;
                case QTMS_SINK_MODEM:
                    ret = QTMSModemSinkImpl::Create(qtmssink, tmssink);
                    break;
                case QTMS_SINK_SPEAKER:
                    ret = QTMSSpeakerSinkImpl::Create(qtmssink, tmssink);
                    break;
                default:
                    break;
                }
            }
        }

    return ret;
    }

gint QTMSFactoryImpl::DeleteSink(QTMSSink*& qtmssink)
    {
    gint ret(QTMS_RESULT_INVALID_ARGUMENT);

    QTMSSinkType sinktype;
    ret = qtmssink->GetType(sinktype);
    switch (sinktype)
        {
        case QTMS_SINK_CLIENT:
            {
            delete (static_cast<QTMSClientSinkImpl*>(qtmssink));
            qtmssink = NULL;
            ret = QTMS_RESULT_SUCCESS;
            }
            break;
        case QTMS_SINK_MODEM:
            {
            delete (static_cast<QTMSModemSinkImpl*>(qtmssink));
            qtmssink = NULL;
            ret = QTMS_RESULT_SUCCESS;
            }
            break;
        case QTMS_SINK_SPEAKER:
            {
            delete (static_cast<QTMSSpeakerSinkImpl*>(qtmssink));
            qtmssink = NULL;
            ret = QTMS_RESULT_SUCCESS;
            }
            break;
        default:
            ret = QTMS_RESULT_SINK_TYPE_NOT_SUPPORTED;
            break;
        }

    return ret;
    }

gint QTMSFactoryImpl::CreateGlobalRouting(QTMSGlobalRouting*& qrouting)
    {
    gint ret(QTMS_RESULT_SUCCESS);

    TMS::TMSGlobalRouting* tmsrouting(NULL);
    if (iFactory)
        {
        ret = iFactory->CreateGlobalRouting(tmsrouting);

        if (ret == TMS_RESULT_SUCCESS)
            {
            ret = QTMSGlobalRoutingImpl::Create(qrouting, tmsrouting);
            }
        }
    return ret;
    }

gint QTMSFactoryImpl::DeleteGlobalRouting(QTMSGlobalRouting*& globalrouting)
    {
    gint ret(QTMS_RESULT_SUCCESS);
    delete (static_cast<QTMSGlobalRoutingImpl*>(globalrouting));
    globalrouting = NULL;
    return ret;
    }

gint QTMSFactoryImpl::CreateDTMF(QTMSStreamType streamtype, QTMSDTMF*& qdtmf)
    {
    gint ret(QTMS_RESULT_SUCCESS);

    TMS::TMSDTMF* tmsdtmf = NULL;

    if (iFactory)
        {
        ret = iFactory->CreateDTMF(streamtype, tmsdtmf);

        if (ret == TMS_RESULT_SUCCESS)
            {
            ret = QTMSDTMFImpl::Create(qdtmf, tmsdtmf);
            }
        }
    return ret;
    }

gint QTMSFactoryImpl::DeleteDTMF(QTMSDTMF*& dtmf)
    {
    gint ret(QTMS_RESULT_SUCCESS);
    delete (static_cast<QTMSDTMFImpl*>(dtmf));
    dtmf = NULL;
    return ret;
    }

gint QTMSFactoryImpl::CreateRingTonePlayer(QTMSRingTone*& rt)
    {
    gint ret(QTMS_RESULT_SUCCESS);

    TMS::TMSRingTone* tmsrt = NULL;

    if (iFactory)
        {
        ret = iFactory->CreateRingTonePlayer(tmsrt);

        if (ret == TMS_RESULT_SUCCESS)
            {
            ret = QTMSRingToneImpl::Create(rt, tmsrt);
            }
        }
    return ret;
    }

gint QTMSFactoryImpl::DeleteRingTonePlayer(QTMSRingTone*& rt)
    {
    gint ret(QTMS_RESULT_SUCCESS);
    delete (static_cast<QTMSRingToneImpl*>(rt));
    rt = NULL;
    return ret;
    }

gint QTMSFactoryImpl::CreateInbandTonePlayer(QTMSInbandTone*& qinbandtone)
    {
    gint ret(QTMS_RESULT_SUCCESS);

    TMS::TMSInbandTone* tmsinband = NULL;
    if (iFactory)
        {
        ret = iFactory->CreateInbandTonePlayer(tmsinband);

        if (ret == TMS_RESULT_SUCCESS)
            {
            ret = QTMSInbandToneImpl::Create(qinbandtone, tmsinband);
            }
        }
    return ret;
    }

gint QTMSFactoryImpl::DeleteInbandTonePlayer(QTMSInbandTone*& inbandtone)
    {
    gint ret(QTMS_RESULT_SUCCESS);
    delete (static_cast<QTMSInbandToneImpl*>(inbandtone));
    inbandtone = NULL;
    return ret;
    }

// End of file
