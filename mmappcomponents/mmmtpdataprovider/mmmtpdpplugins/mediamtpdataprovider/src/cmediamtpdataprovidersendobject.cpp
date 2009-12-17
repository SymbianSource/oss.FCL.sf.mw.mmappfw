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
 * Description:  Implement the operation: SendObjectInfo/SendObjectPropList/SendObject
 *
 */

#include <mtp/mmtpdataproviderframework.h>
#include <mtp/cmtptypeobjectproplist.h>
#include <mtp/cmtptypestring.h>
#include <mtp/cmtptypearray.h>

#include "cmediamtpdataprovidersendobject.h"
#include "mediamtpdataproviderconst.h"
#include "mmmtpdplogger.h"
#include "cmmmtpdpmetadataaccesswrapper.h"
#include "mmmtpdputility.h"

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderSendObject::NewL
// Two-phase construction method
// -----------------------------------------------------------------------------
//
MMmRequestProcessor* CMediaMtpDataProviderSendObject::NewL( MMTPDataProviderFramework& aFramework,
    MMTPConnection& aConnection,
    MMmMtpDpConfig& aDpConfig)
    {
    CMediaMtpDataProviderSendObject* self = new ( ELeave ) CMediaMtpDataProviderSendObject( aFramework, aConnection, aDpConfig );

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderSendObject::~CMediaMtpDataProviderSendObject
// Destructor
// -----------------------------------------------------------------------------
//
CMediaMtpDataProviderSendObject::~CMediaMtpDataProviderSendObject()
    {

    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderSendObject::CMediaMtpDataProviderSendObject
// Standard C++ Constructor
// -----------------------------------------------------------------------------
//
CMediaMtpDataProviderSendObject::CMediaMtpDataProviderSendObject(
        MMTPDataProviderFramework& aFramework, MMTPConnection& aConnection,
        MMmMtpDpConfig& aDpConfig) :
    CSendObject(aFramework, aConnection, aDpConfig)
    {

    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderSendObject::ConstructL
// 2nd Phase Constructor
// -----------------------------------------------------------------------------
//
void CMediaMtpDataProviderSendObject::ConstructL()
    {
    CSendObject::ConstructL();
    }

TMTPResponseCode CMediaMtpDataProviderSendObject::CheckSepecificPropType( TUint16 aPropCode, TUint16 aDataType )
    {
    PRINT( _L( "MM MTP => CMediaMtpDataProviderSendObject::CheckSepecificPropType" ) );
    TMTPResponseCode responseCode( EMTPRespCodeOK );

    switch ( aPropCode )
        {
        case EMTPObjectPropCodeArtist: // 0xDC46
        case EMTPObjectPropCodeGenre: // 0xDC8C
        case EMTPObjectPropCodeParentalRating: // 0xDC94
        case EMTPObjectPropCodeComposer: // 0xDC96
        case EMTPObjectPropCodeOriginalReleaseDate: // 0xDC99
        case EMTPObjectPropCodeAlbumName: // 0xDC9A
        case EMTPObjectPropCodeEncodingProfile: // 0xDEA1
            if ( aDataType != EMTPTypeString)
                {
                responseCode = EMTPRespCodeInvalidObjectPropFormat;
                }
            break;

        case EMTPObjectPropCodeDescription:
            if ( aDataType != EMTPTypeAUINT16)
                {
                responseCode = EMTPRespCodeInvalidObjectPropFormat;
                }
            break;

        case EMTPObjectPropCodeTrack: // 0xDC8B
        case EMTPObjectPropCodeDRMStatus: // 0xDC9D
        case EMTPObjectPropCodeNumberOfChannels: // 0xDE94
        case EMTPObjectPropCodeScanType: // 0xDE97
            if ( aDataType != EMTPTypeUINT16)
                {
                responseCode = EMTPRespCodeInvalidObjectPropFormat;
                }
            break;

            //Get Data for Int32 objects
        case EMTPObjectPropCodeWidth: // 0xDC87
        case EMTPObjectPropCodeHeight: // 0xDC88
        case EMTPObjectPropCodeDuration: // 0xDC89
        case EMTPObjectPropCodeUseCount: // 0xDC91
        case EMTPObjectPropCodeSampleRate: // 0xDE93
        case EMTPObjectPropCodeAudioWAVECodec: // 0xDE99
        case EMTPObjectPropCodeAudioBitRate: // 0xDE9A
        case EMTPObjectPropCodeVideoFourCCCodec: // 0xDE9B
        case EMTPObjectPropCodeFramesPerThousandSeconds: // 0xDE9D
        case EMTPObjectPropCodeKeyFrameDistance: // 0xDE9E
            if ( aDataType != EMTPTypeUINT32)
                {
                responseCode = EMTPRespCodeInvalidObjectPropFormat;
                }
            break;

        case EMTPExtObjectPropCodeOmaDrmStatus:
            if ( aDataType != EMTPTypeUINT8)
                {
                responseCode = EMTPRespCodeInvalidObjectPropFormat;
                }
            break;

        default:
            // It's not possible to run here.
            responseCode = EMTPRespCodeInvalidObjectPropCode;
            break;

        }
    PRINT1( _L( "MM MTP <= CMediaMtpDataProviderSendObject::CheckSepecificPropType, responseCode = 0x%X" ), responseCode );
    return responseCode;
    }

TMTPResponseCode CMediaMtpDataProviderSendObject::SetSpecificObjectPropertyL( TUint16 aPropCode,
    const CMTPObjectMetaData& aObject,
    const CMTPTypeObjectPropListElement& aElement )
    {
    PRINT( _L( "MM MTP => CMediaMtpDataProviderSendObject::SetSpecificObjectPropertyL" ) );
    TMTPResponseCode responseCode(EMTPRespCodeOK);

    switch ( aPropCode )
        {
        case EMTPObjectPropCodeArtist:
        case EMTPObjectPropCodeGenre:
        case EMTPObjectPropCodeComposer:
        case EMTPObjectPropCodeOriginalReleaseDate:
        case EMTPObjectPropCodeAlbumName:
        case EMTPObjectPropCodeParentalRating:
        case EMTPObjectPropCodeEncodingProfile:
            {
            CMTPTypeString* stringData =
                CMTPTypeString::NewLC( aElement.StringL( CMTPTypeObjectPropListElement::EValue ) );// + stringData

            responseCode = SetMetaDataToWrapperL( aPropCode,
                *stringData,
                aObject );

            CleanupStack::PopAndDestroy( stringData );// - stringData
            }
            break;

        case EMTPObjectPropCodeVideoBitRate:
            // TODO: Does anything need to be done?
            /* spec:
             * Object properties that are get-only (0x00 GET)
             * should accept values during object creation by
             * way of the SendObjectPropList command.
             */
            break;

        case EMTPObjectPropCodeDescription:
            {
            CMTPTypeArray* desData = CMTPTypeArray::NewLC( EMTPTypeAUINT16 ); // + desData
            //            aElement.GetL( CMTPTypeObjectPropListElement::EValue, *desData );
            desData->SetByDesL( aElement.ArrayL( CMTPTypeObjectPropListElement::EValue ) );
            responseCode = SetMetaDataToWrapperL( aPropCode,
                *desData,
                aObject );
            CleanupStack::PopAndDestroy( desData ); // - desData
            }
            break;

        case EMTPObjectPropCodeWidth:
        case EMTPObjectPropCodeHeight:
        case EMTPObjectPropCodeDuration:
        case EMTPObjectPropCodeUseCount:
        case EMTPObjectPropCodeSampleRate:
        case EMTPObjectPropCodeAudioWAVECodec:
        case EMTPObjectPropCodeAudioBitRate:
        case EMTPObjectPropCodeVideoFourCCCodec:
        case EMTPObjectPropCodeFramesPerThousandSeconds:
        case EMTPObjectPropCodeKeyFrameDistance:
            {
            TMTPTypeUint32 uint32( aElement.Uint32L( CMTPTypeObjectPropListElement::EValue ) );
            responseCode = SetMetaDataToWrapperL( aPropCode,
                uint32,
                aObject );
            }
            break;

        case EMTPObjectPropCodeTrack:
        case EMTPObjectPropCodeNumberOfChannels:
        case EMTPObjectPropCodeScanType:
        case EMTPObjectPropCodeDRMStatus:
            {
            TMTPTypeUint16 uint16( aElement.Uint16L( CMTPTypeObjectPropListElement::EValue ) );
            responseCode = SetMetaDataToWrapperL( aPropCode,
                uint16,
                aObject );
            }
            break;

        case EMTPExtObjectPropCodeOmaDrmStatus:
            {
            TInt drmStatus = MmMtpDpUtility::GetDrmStatus( aObject.DesC( CMTPObjectMetaData::ESuid ) );

            if ( drmStatus == EMTPDrmStatusUnknown )
                {
                responseCode = EMTPRespCodeAccessDenied;
                }
            else
                {
                TMTPTypeUint8 newValue( aElement.Uint8L( CMTPTypeObjectPropListElement::EValue ) );

                // there's no DB field to remember the value, so return an error
                // if there's a mismatch to CAF protection status
                if ( ( ( drmStatus == EMTPDrmStatusProtected )
                    && ( newValue.Value() == 0 ) )
                    || ( ( drmStatus == EMTPDrmStatusNotProtected )
                        && ( newValue.Value() == 1 ) ) )
                    {
                    responseCode = EMTPRespCodeAccessDenied;
                    }
                }
            }
            break;

        default:
            {
            PRINT( _L( "MM MTP <> Default CASE leaving CMedia...tInfo::SetSpecificObjectPropertyL" ) );
            User::Leave( KErrNotSupported );
            }
            break;
        }

    PRINT( _L( "MM MTP <= CMediaMtpDataProviderSendObject::ServiceSpecificObjectPropertyL" ) );

    return responseCode;
    }

// TODO: Is it necessary?
TInt CMediaMtpDataProviderSendObject::HandleSpecificWrapperError( TInt aError,
    const CMTPObjectMetaData& aObject )
    {
    PRINT( _L( "MM MTP <> CMediaMtpDataProviderSendObject::HandleSpecificWrapperError" ) );
    TInt err = aError;

    if ( ( err == KErrNotSupported )
        && ( ( aObject.Uint( CMTPObjectMetaData::EFormatCode ) == EMTPFormatCodeASF )
        || ( aObject.Uint( CMTPObjectMetaData::EFormatCode ) == EMTPFormatCodeMP4Container )
        || ( aObject.Uint( CMTPObjectMetaData::EFormatCode ) == EMTPFormatCode3GPContainer ) ) )
        {
        // do nothing, handle gracefully
        // should only happens for container case where metadata field does not match in different DB
        err = KErrNone;
        PRINT( _L( "MM MTP <> CMediaMtpDataProviderSendObject::HandleSpecificWrapperError, gracefully dealt with error" ) );
        }

    return err;
    }

// end of file
