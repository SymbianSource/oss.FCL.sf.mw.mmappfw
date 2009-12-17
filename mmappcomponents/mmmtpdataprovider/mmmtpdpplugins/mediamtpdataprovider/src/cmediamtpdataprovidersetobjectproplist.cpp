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
* Description:  Implement the operation: getobjectproplist
*
*/


#include <mtp/mmtpdataproviderframework.h>
#include <mtp/cmtptypeobjectproplist.h>
#include <mtp/cmtptypestring.h>
#include <mtp/cmtptypearray.h>

#include "cmediamtpdataprovidersetobjectproplist.h"
#include "mediamtpdataproviderconst.h"
#include "mmmtpdplogger.h"
#include "cmmmtpdpmetadataaccesswrapper.h"
#include "mmmtpdputility.h"

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderSetObjectPropList::NewL
// Two-phase construction method
// -----------------------------------------------------------------------------
//
MMmRequestProcessor* CMediaMtpDataProviderSetObjectPropList::NewL( MMTPDataProviderFramework& aFramework,
    MMTPConnection& aConnection,
    MMmMtpDpConfig& aDpConfig )
    {
    CMediaMtpDataProviderSetObjectPropList* self =
        new ( ELeave ) CMediaMtpDataProviderSetObjectPropList( aFramework, aConnection, aDpConfig );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderSetObjectPropList::~CMediaMtpDataProviderSetObjectPropList
// Destructor
// -----------------------------------------------------------------------------
//
CMediaMtpDataProviderSetObjectPropList::~CMediaMtpDataProviderSetObjectPropList()
    {

    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderSetObjectPropList::CMediaMtpDataProviderSetObjectPropList
// Standard C++ Constructor
// -----------------------------------------------------------------------------
//
CMediaMtpDataProviderSetObjectPropList::CMediaMtpDataProviderSetObjectPropList( MMTPDataProviderFramework& aFramework,
    MMTPConnection& aConnection,
    MMmMtpDpConfig& aDpConfig ) :
    CSetObjectPropList( aFramework, aConnection, aDpConfig )
    {

    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderSetObjectPropList::ConstructL
// 2nd Phase Constructor
// -----------------------------------------------------------------------------
//
void CMediaMtpDataProviderSetObjectPropList::ConstructL()
    {
    CSetObjectPropList::ConstructL();
    }

TMTPResponseCode CMediaMtpDataProviderSetObjectPropList::ServiceSpecificObjectPropertyL( TUint16 aPropCode, const CMTPObjectMetaData& aObject,
    const CMTPTypeObjectPropListElement& aElement )
    {
    PRINT( _L( "MM MTP => CMediaMtpDataProviderSetObjectPropList::ServiceSpecificObjectPropertyL" ) );
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

            responseCode = ServiceMetaDataToWrapperL( aPropCode,
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
            responseCode = ServiceMetaDataToWrapperL( aPropCode,
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
            responseCode = ServiceMetaDataToWrapperL( aPropCode,
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
            responseCode = ServiceMetaDataToWrapperL( aPropCode,
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

    PRINT( _L( "MM MTP <= CMediaMtpDataProviderSetObjectPropList::ServiceSpecificObjectPropertyL" ) );

    return responseCode;
    }

TInt CMediaMtpDataProviderSetObjectPropList::HandleSpecificWrapperError( TInt aError,
    const CMTPObjectMetaData& aObject)
    {
    TInt err = aError;

    if ( ( err == KErrNotSupported )
        && ( ( aObject.Uint( CMTPObjectMetaData::EFormatCode ) == EMTPFormatCodeASF )
        || ( aObject.Uint( CMTPObjectMetaData::EFormatCode ) == EMTPFormatCodeMP4Container )
        || ( aObject.Uint( CMTPObjectMetaData::EFormatCode ) == EMTPFormatCode3GPContainer ) ) )
        {
        // do nothing, handle gracefully
        // should only happens for container case where metadata field does not match in different DB
        err = KErrNone;
        PRINT( _L( "MM MTP <> CMediaMtpDataProviderSetObjectPropList::HandleSpecificWrapperError, gracefully dealt with error" ) );
        }

    return err;
    }

// end of file
