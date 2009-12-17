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

#include "cmediamtpdataprovidermoveobject.h"
#include "mediamtpdataproviderconst.h"
#include "mmmtpdplogger.h"
#include "mmmtpdputility.h"
#include "cmmmtpdpmetadataaccesswrapper.h"
#include "mmmtpdpdefs.h"
#include "mmmtpdpconfig.h"

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderMoveObject::NewL
// Two-phase construction method
// -----------------------------------------------------------------------------
//
MMmRequestProcessor* CMediaMtpDataProviderMoveObject::NewL( MMTPDataProviderFramework& aFramework,
    MMTPConnection& aConnection,
    MMmMtpDpConfig& aDpConfig )
    {
    CMediaMtpDataProviderMoveObject* self =
        new ( ELeave ) CMediaMtpDataProviderMoveObject( aFramework,
            aConnection,
            aDpConfig );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderMoveObject::ConstructL
// Two-phase construction method
// -----------------------------------------------------------------------------
//
void CMediaMtpDataProviderMoveObject::ConstructL()
    {
    CMoveObject::ConstructL();
    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderMoveObject::~CMediaMtpDataProviderMoveObject
// Destructor
// -----------------------------------------------------------------------------
//
CMediaMtpDataProviderMoveObject::~CMediaMtpDataProviderMoveObject()
    {

    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderMoveObject::CMediaMtpDataProviderMoveObject
// Standard c++ constructor
// -----------------------------------------------------------------------------
//
CMediaMtpDataProviderMoveObject::CMediaMtpDataProviderMoveObject( MMTPDataProviderFramework& aFramework,
    MMTPConnection& aConnection,
    MMmMtpDpConfig& aDpConfig ) :
    CMoveObject( aFramework, aConnection, aDpConfig )
    {

    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderMoveObject::ServiceSpecificObjectPropertyL
//
// -----------------------------------------------------------------------------
//
void CMediaMtpDataProviderMoveObject::ServiceGetSpecificObjectPropertyL( TUint16 aPropCode,
    TUint32 aHandle,
    const CMTPObjectMetaData& aObject )
    {
    PRINT1( _L( "MM MTP => CMediaMtpDataProviderMoveObject::ServiceSpecificObjectPropertyL aPropCode(0x%x)" ), aPropCode );

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

            PRINT1( _L( "MM MTP <> CMediaMtpDataProviderMoveObject::ServiceSpecificObjectPropertyL err = %d" ), err );

            if ( err == KErrNone )
                {
                iPropertyElement = &(iPropertyList->ReservePropElemL(aHandle, aPropCode));
                iPropertyElement->SetStringL( CMTPTypeObjectPropListElement::EValue, textData->StringChars());
                
//                iPropertyElement = CMTPTypeObjectPropListElement::NewL( aHandle,
//                    aPropCode,
//                    *textData );
                }
            else if ( err == KErrNotFound )
              {
                iPropertyElement = NULL;
              }
            else
              {
                User::Leave( err );
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

            PRINT1( _L( "MM MTP <> CMediaMtpDataProviderMoveObject::ServiceSpecificObjectPropertyL err = %d" ), err );

            if ( err == KErrNone )
                {
                
                iPropertyElement = &(iPropertyList->ReservePropElemL(aHandle, aPropCode));
                iPropertyElement->SetArrayL( CMTPTypeObjectPropListElement::EValue, *desData);
                
//                iPropertyElement = CMTPTypeObjectPropListElement::NewL( aHandle,
//                    aPropCode,
//                    *desData );
                }
            else if ( err == KErrNotFound )
                {
                iPropertyElement = NULL;
                }
            else
                {
                User::Leave( err );
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
            PRINT1( _L( "MM MTP <> CMediaMtpDataProviderMoveObject::ServiceSpecificObjectPropertyL err = %d" ), err );

            if ( err == KErrNone )
                {
                
                iPropertyElement = &(iPropertyList->ReservePropElemL(aHandle, aPropCode));
                iPropertyElement->SetUint16L( CMTPTypeObjectPropListElement::EValue, uint16.Value());
                
//                iPropertyElement = CMTPTypeObjectPropListElement::NewL( aHandle,
//                    aPropCode,
//                    uint16 );
                }
            else if ( err == KErrNotFound )
                {
                iPropertyElement = NULL;
                }
            else
                {
                User::Leave( err );
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
            PRINT1( _L( "MM MTP <> CMediaMtpDataProviderMoveObject::ServiceSpecificObjectPropertyL err = %d" ), err );

            if ( err == KErrNone )
                {
                iPropertyElement = &(iPropertyList->ReservePropElemL(aHandle, aPropCode));
                iPropertyElement->SetUint32L( CMTPTypeObjectPropListElement::EValue, uint32.Value());
                
//                iPropertyElement = CMTPTypeObjectPropListElement::NewL( aHandle,
//                    aPropCode,
//                    uint32 );
                }
            else if ( err == KErrNotFound )
                {
                iPropertyElement = NULL;
                }
            else
                {
                User::Leave( err );
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

    PRINT( _L( "MM MTP <= CMediaMtpDataProviderMoveObject::ServiceSpecificObjectPropertyL" ) );
    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderMoveObject::ServiceSetSpecificObjectPropertyL
//
// -----------------------------------------------------------------------------
//
TMTPResponseCode CMediaMtpDataProviderMoveObject::ServiceSetSpecificObjectPropertyL( TUint16 aPropCode, const CMTPObjectMetaData& aObject,
    const CMTPTypeObjectPropListElement& aElement )
    {
    PRINT( _L( "MM MTP => CMediaMtpDataProviderMoveObject::ServiceSetSpecificObjectPropertyL" ) );
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

            CleanupStack::PopAndDestroy( stringData );// - stringData
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
            
            desData->SetByDesL(aElement.ArrayL(CMTPTypeObjectPropListElement::EValue ));
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
            TInt drmStatus = MmMtpDpUtility::GetDrmStatus( aObject.DesC(
                CMTPObjectMetaData::ESuid ) );

            if ( drmStatus == EMTPDrmStatusUnknown )
                {
                responseCode = EMTPRespCodeAccessDenied;
                }
            else
                {
                TMTPTypeUint8 newValue( aElement.Uint8L( CMTPTypeObjectPropListElement::EValue ) );

                // there's no DB field to remember the value, so return an error
                // if there's a mismatch to CAF protection status
                if ( ( ( drmStatus == EMTPDrmStatusProtected ) && ( newValue.Value() == 0 ) )
                    || ( ( drmStatus == EMTPDrmStatusNotProtected ) && ( newValue.Value() == 1 ) ) )
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

    PRINT( _L( "MM MTP <= CMediaMtpDataProviderMoveObject::ServiceSetSpecificObjectPropertyL" ) );

    return responseCode;
    }

// end of file
