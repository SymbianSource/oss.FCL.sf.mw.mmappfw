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
* Description:  Implement the operation: Copy Object
*
*/


#include <mtp/mmtpdataproviderframework.h>
#include <mtp/cmtptypeobjectproplist.h>
#include <mtp/cmtptypestring.h>
#include <mtp/cmtptypearray.h>

#include "cmediamtpdataprovidercopyobject.h"
#include "mediamtpdataproviderconst.h"
#include "mmmtpdplogger.h"
#include "mmmtpdputility.h"
#include "cmmmtpdpmetadataaccesswrapper.h"
#include "mmmtpdpdefs.h"
#include "mmmtpdpconfig.h"

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderCopyObject::NewL
// Two-phase construction method
// -----------------------------------------------------------------------------
//
MMmRequestProcessor* CMediaMtpDataProviderCopyObject::NewL( MMTPDataProviderFramework& aFramework,
    MMTPConnection& aConnection,
    MMmMtpDpConfig& aDpConfig )
    {
    CMediaMtpDataProviderCopyObject* self =
        new ( ELeave ) CMediaMtpDataProviderCopyObject( aFramework,
            aConnection,
            aDpConfig );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderCopyObject::ConstructL
// Two-phase construction method
// -----------------------------------------------------------------------------
//
void CMediaMtpDataProviderCopyObject::ConstructL()
    {
    CCopyObject::ConstructL();
    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderCopyObject::~CMediaMtpDataProviderCopyObject
// Destructor
// -----------------------------------------------------------------------------
//
CMediaMtpDataProviderCopyObject::~CMediaMtpDataProviderCopyObject()
    {

    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderCopyObject::CMediaMtpDataProviderCopyObject
// Standard c++ constructor
// -----------------------------------------------------------------------------
//
CMediaMtpDataProviderCopyObject::CMediaMtpDataProviderCopyObject( MMTPDataProviderFramework& aFramework,
    MMTPConnection& aConnection,
    MMmMtpDpConfig& aDpConfig ) :
    CCopyObject( aFramework, aConnection, aDpConfig )
    {

    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderCopyObject::ServiceSpecificObjectPropertyL
//
// -----------------------------------------------------------------------------
//
void CMediaMtpDataProviderCopyObject::ServiceGetSpecificObjectPropertyL( TUint16 aPropCode,
    TUint32 aHandle,
    const CMTPObjectMetaData& aObject )
    {
    PRINT( _L( "MM MTP => CMediaMtpDataProviderCopyObject::ServiceSpecificObjectPropertyL" ) );

    CMTPTypeString* textData = NULL;
    CMTPTypeArray* desData = NULL;
    TInt err = KErrNone;

    if ( iPropertyElement )
        {
        delete iPropertyElement;
        iPropertyElement = NULL;
        }

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
            textData = CMTPTypeString::NewLC();   // + textData

            TRAP( err, iDpConfig.GetWrapperL().GetObjectMetadataValueL( aPropCode,
                *textData,
                aObject ) );

            PRINT1( _L( "MM MTP <> CMediaMtpDataProviderCopyObject::ServiceSpecificObjectPropertyL err = %d" ), err );

            if ( err == KErrNone )
                {
                iPropertyElement = &(iPropertyList->ReservePropElemL(aHandle, aPropCode));
                iPropertyElement->SetStringL( CMTPTypeObjectPropListElement::EValue, textData->StringChars());
                
//                iPropertyElement = CMTPTypeObjectPropListElement::NewL( aHandle,
//                    aPropCode,
//                    *textData );
                }

            CleanupStack::PopAndDestroy( textData );  // - textData
            }
            break;

        case EMTPObjectPropCodeDescription:
            {
            desData = CMTPTypeArray::NewLC( EMTPTypeAUINT16 );   // + desData

            TRAP( err, iDpConfig.GetWrapperL().GetObjectMetadataValueL( aPropCode,
                *desData,
                aObject ) );

            PRINT1( _L( "MM MTP <> CMediaMtpDataProviderCopyObject::ServiceSpecificObjectPropertyL err = %d" ), err );

            if ( err == KErrNone )
                {
                iPropertyElement = &(iPropertyList->ReservePropElemL(aHandle, aPropCode));
                iPropertyElement->SetArrayL( CMTPTypeObjectPropListElement::EValue, *desData);
                
//                iPropertyElement = CMTPTypeObjectPropListElement::NewL( aHandle,
//                    aPropCode,
//                    *desData );
                }

            CleanupStack::PopAndDestroy( desData );  // - desData
            }
            break;

        case EMTPObjectPropCodeTrack:
        case EMTPObjectPropCodeNumberOfChannels:
        case EMTPObjectPropCodeScanType:
        case EMTPObjectPropCodeDRMStatus:
            {
            TMTPTypeUint16 uint16( 0 );
            TRAP( err, iDpConfig.GetWrapperL().GetObjectMetadataValueL( aPropCode,
                uint16,
                aObject ) );
            PRINT1( _L( "MM MTP <> CMediaMtpDataProviderCopyObject::ServiceSpecificObjectPropertyL err = %d" ), err );

            if ( err == KErrNone )
                {
                iPropertyElement = &(iPropertyList->ReservePropElemL(aHandle, aPropCode));
                iPropertyElement->SetUint16L( CMTPTypeObjectPropListElement::EValue, uint16.Value());
                
//                iPropertyElement = CMTPTypeObjectPropListElement::NewL( aHandle,
//                    aPropCode,
//                    uint16 );
                }

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
        case EMTPObjectPropCodeVideoBitRate:
        case EMTPObjectPropCodeFramesPerThousandSeconds:
        case EMTPObjectPropCodeKeyFrameDistance:
            {
            TMTPTypeUint32 uint32 = 0;
            TRAP( err, iDpConfig.GetWrapperL().GetObjectMetadataValueL( aPropCode,
                uint32,
                aObject ) );
            PRINT1( _L( "MM MTP <> CMediaMtpDataProviderCopyObject::ServiceSpecificObjectPropertyL err = %d" ), err );

            if ( err == KErrNone )
                {
                iPropertyElement = &(iPropertyList->ReservePropElemL(aHandle, aPropCode));
                iPropertyElement->SetUint32L( CMTPTypeObjectPropListElement::EValue, uint32.Value());
//                iPropertyElement = CMTPTypeObjectPropListElement::NewL( aHandle,
//                    aPropCode,
//                    uint32 );
                }
            }
            break;

        case EMTPExtObjectPropCodeOmaDrmStatus:
            {
            TInt drmStatus = MmMtpDpUtility::GetDrmStatus( aObject.DesC( CMTPObjectMetaData::ESuid ) );
            TMTPTypeUint8 result;
            result.Set( 0 );

            if ( drmStatus == EMTPDrmStatusProtected )
                result.Set( 1 );

            iPropertyElement = &(iPropertyList->ReservePropElemL(aHandle, aPropCode));
            iPropertyElement->SetUint16L( CMTPTypeObjectPropListElement::EDatatype, EMTPTypeUINT8);
            iPropertyElement->SetUint8L( CMTPTypeObjectPropListElement::EValue, result.Value());
            
//            iPropertyElement = CMTPTypeObjectPropListElement::NewL( aHandle,
//                aPropCode,
//                EMTPTypeUINT8,
//                result );
            }
            break;

        default:
            {
            User::Leave( KErrNotSupported );
            }
            break;
        }

    // centralize logic to handle error, doing so, that's on need to trap the
    // leave for KErrNotSupported in base class like previous version.
    if ( ( err == KErrNotFound )
        || ( ( err == KErrNotSupported )
        && ( ( aObject.Uint( CMTPObjectMetaData::EFormatCode ) == EMTPFormatCodeASF )
        || ( aObject.Uint( CMTPObjectMetaData::EFormatCode ) == EMTPFormatCodeMP4Container )
        || ( aObject.Uint( CMTPObjectMetaData::EFormatCode ) == EMTPFormatCode3GPContainer ) ) ) )
        {
        iPropertyElement = NULL;
        }
    else
        {
        User::LeaveIfError( err );
        }
    PRINT( _L( "MM MTP <= CMediaMtpDataProviderCopyObject::ServiceSpecificObjectPropertyL" ) );
    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderCopyObject::ServiceSetSpecificObjectPropertyL
//
// -----------------------------------------------------------------------------
//
TMTPResponseCode CMediaMtpDataProviderCopyObject::ServiceSetSpecificObjectPropertyL( TUint16 aPropCode, const CMTPObjectMetaData& aObject,
    const CMTPTypeObjectPropListElement& aElement )
    {
    PRINT( _L( "MM MTP => CMediaMtpDataProviderCopyObject::ServiceSetSpecificObjectPropertyL" ) );
    TMTPResponseCode responseCode( EMTPRespCodeOK );

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

            responseCode = ServiceMetaDataToWrapper( aPropCode,
                *stringData,
                aObject );

            CleanupStack::PopAndDestroy( stringData ); // - stringData
            }
            break;

        case EMTPObjectPropCodeVideoBitRate:
            {
            responseCode = EMTPRespCodeAccessDenied;
            }
            break;

        case EMTPObjectPropCodeDescription:
            {
            CMTPTypeArray*  desData = CMTPTypeArray::NewLC( EMTPTypeAUINT16 ); // + desData
//            aElement.GetL( CMTPTypeObjectPropListElement::EValue, *desData );
            desData->SetByDesL(aElement.ArrayL(CMTPTypeObjectPropListElement::EValue));
            //desData(aElement.ArrayL(CMTPTypeObjectPropListElement::EValue));
            
            responseCode = ServiceMetaDataToWrapper( aPropCode,
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
            responseCode = ServiceMetaDataToWrapper( aPropCode,
                uint32,
                aObject );
            }
            break;

        case EMTPObjectPropCodeTrack:
        case EMTPObjectPropCodeNumberOfChannels:
        case EMTPObjectPropCodeScanType:
        case EMTPObjectPropCodeDRMStatus:
            {
            TMTPTypeUint16 uint16( aElement.Uint16L( CMTPTypeObjectPropListElement::EValue ));
            responseCode = ServiceMetaDataToWrapper( aPropCode,
                uint16,
                aObject );
            }
            break;

        case EMTPExtObjectPropCodeOmaDrmStatus:
            {
            TInt drmStatus = MmMtpDpUtility::GetDrmStatus( aObject.DesC( CMTPObjectMetaData::ESuid ) );

            if (drmStatus == EMTPDrmStatusUnknown)
                {
                responseCode = EMTPRespCodeAccessDenied;
                }
            else
                {
                TMTPTypeUint8 newValue( aElement.Uint8L( CMTPTypeObjectPropListElement::EValue ) );

                // there's no DB field to remember the value, so return an error
                // if there's a mismatch to CAF protection status
                if ( ( ( drmStatus == EMTPDrmStatusProtected) && ( newValue.Value() == 0 ) ) ||
                    ( ( drmStatus == EMTPDrmStatusNotProtected ) && ( newValue.Value() == 1 ) ) )
                    {
                    responseCode = EMTPRespCodeAccessDenied;
                    }
                }
            }
            break;

        default:
            {
            User::Leave( KErrNotSupported );
            }
            break;
        }

    PRINT( _L( "MM MTP <= CMediaMtpDataProviderCopyObject::ServiceSetSpecificObjectPropertyL" ) );

    return responseCode;
    }

// end of file
