/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/

#ifndef ABSTRACTMEDIAMTPDATAPROVIDERCONST_H
#define ABSTRACTMEDIAMTPDATAPROVIDERCONST_H

#include <mtp/mtpprotocolconstants.h>

const TUint16 EMTPExtOpCodeAbstractMediaPreference = 0x9203;

/**
* define all the operations that are supported by the abstractmedia data provider
*/
static const TUint16 KAbstractMediaMtpDataProviderSupportedFormats[] =
    {
    EMTPFormatCodeM3UPlaylist,
    EMTPFormatCodeAbstractAudioVideoPlaylist
    };

/**
* define all the operations that are supported by the abstract media data provider
*/
static const TUint16 KAbstractMediaMtpDataProviderSupportedOperations[] =
    {
    // Required
    EMTPOpCodeGetObjectInfo,
    EMTPOpCodeGetObject,
    EMTPOpCodeGetPartialObject,
    EMTPOpCodeDeleteObject,
    EMTPOpCodeSendObjectInfo,
    EMTPOpCodeSendObject,
    EMTPOpCodeGetObjectPropsSupported,
    EMTPOpCodeGetObjectPropDesc,
    EMTPOpCodeGetObjectPropValue,
    EMTPOpCodeSetObjectPropValue,
    EMTPOpCodeGetObjectReferences,
    EMTPOpCodeSetObjectReferences,

    // Recommended
    EMTPOpCodeGetObjectPropList,
    EMTPOpCodeSetObjectPropList,
    EMTPOpCodeGetInterdependentPropDesc,

    EMTPExtOpCodeAbstractMediaPreference, // 0x9203

    EMTPOpCodeCopyObject,
    EMTPOpCodeMoveObject,

    EMTPOpCodeSendObjectPropList,
    EMTPOpCodeGetFormatCapabilities
    };

#endif // ABSTRACTMEDIAMTPDATAPROVIDERCONST_H
