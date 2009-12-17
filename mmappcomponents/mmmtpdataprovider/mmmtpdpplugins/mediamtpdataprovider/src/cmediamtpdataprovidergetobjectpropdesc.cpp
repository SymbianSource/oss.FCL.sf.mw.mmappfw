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
* Description:  Implement the operation: GetObjectPropDesc
*
*/

#include <mtp/cmtptypeobjectpropdesc.h>
#include <mtp/mtpprotocolconstants.h>
#include <mtp/cmtptypestring.h>

#include "cmediamtpdataprovidergetobjectpropdesc.h"
#include "mediamtpdataproviderconst.h"
#include "mmmtpdplogger.h"
#include "mmmtpdpdefs.h"

const TUint32 KMTPDescriptionLen = 0x00000200;

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderGetObjectPropDesc::NewL
// Two-phase construction method
// -----------------------------------------------------------------------------
//
MMmRequestProcessor* CMediaMtpDataProviderGetObjectPropDesc::NewL( MMTPDataProviderFramework& aFramework,
    MMTPConnection& aConnection,
    MMmMtpDpConfig& aDpConfig )
    {
    CMediaMtpDataProviderGetObjectPropDesc* self =
        new ( ELeave ) CMediaMtpDataProviderGetObjectPropDesc( aFramework,
            aConnection,
            aDpConfig );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderGetObjectPropDesc::ConstructL
// Two-phase construction method
// -----------------------------------------------------------------------------
//
void CMediaMtpDataProviderGetObjectPropDesc::ConstructL()
    {
    CGetObjectPropDesc::ConstructL();
    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderGetObjectPropDesc::~CMediaMtpDataProviderGetObjectPropDesc
// Destructor
// -----------------------------------------------------------------------------
//
CMediaMtpDataProviderGetObjectPropDesc::~CMediaMtpDataProviderGetObjectPropDesc()
    {

    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderGetObjectPropDesc::CMediaMtpDataProviderGetObjectPropDesc
// Standard c++ constructor
// -----------------------------------------------------------------------------
//
CMediaMtpDataProviderGetObjectPropDesc::CMediaMtpDataProviderGetObjectPropDesc( MMTPDataProviderFramework& aFramework,
    MMTPConnection& aConnection,
    MMmMtpDpConfig& aDpConfig ) :
    CGetObjectPropDesc( aFramework, aConnection, aDpConfig )
    {

    }

void CMediaMtpDataProviderGetObjectPropDesc::ServiceSpecificObjectPropertyL(TUint16 aPropCode)
    {
    switch (aPropCode)
        {
        // format specific
        case EMTPObjectPropCodeArtist: // Artist
        case EMTPObjectPropCodeTrack: // Track
        case EMTPObjectPropCodeGenre: // Genre
        case EMTPObjectPropCodeUseCount: // Use Count
        case EMTPObjectPropCodeAlbumName: // Album Name
        case EMTPObjectPropCodeAlbumArtist: // Album Artist
        case EMTPObjectPropCodeOriginalReleaseDate: // Original Release Date
        case EMTPObjectPropCodeComposer: // Composer
        case EMTPObjectPropCodeParentalRating:
            iObjectProperty = CMTPTypeObjectPropDesc::NewL( aPropCode );
            break;

        // Number of Channels (m)
        case EMTPObjectPropCodeNumberOfChannels:
            ServiceNumberOfChannelsL();
            break;

        // Sample Rate (HAS MINIMUM AND MAX VALUE)
        case EMTPObjectPropCodeSampleRate:
            ServiceCodeSampleRateL();
            break;

        // Audio Wave Codec (MAY REQUIRED LIMITED NUMBER TO BE PRODUCED)
        case EMTPObjectPropCodeAudioWAVECodec:
            ServiceCodeWaveCodecL();
            break;

        // Audio Bit Rate (MAY REQUIRED LIMITED NUMBER TO BE PRODUCED)
        case EMTPObjectPropCodeAudioBitRate:
            ServiceCodeAudioBitrateL();
            break;

        // Duration
        case EMTPObjectPropCodeDuration:
            ServiceDurationL();
            break;

        // Description
        case EMTPObjectPropCodeDescription:
            ServiceDescriptionL();
            break;

        case EMTPObjectPropCodeWidth:
            ServiceRangeFormDescriptionL( aPropCode,
                EMTPMinWidth,
                EMTPMaxWidth,
                EMTPStepWidth );
            break;

        case EMTPObjectPropCodeHeight:
            ServiceRangeFormDescriptionL( aPropCode,
                EMTPMinHeight,
                EMTPMaxHeight,
                EMTPStepHeight );
            break;

        case EMTPObjectPropCodeScanType:
            ServiceScanTypeDescriptionL();
            break;

        case EMTPObjectPropCodeVideoFourCCCodec:
            ServiceVideoFourCCCodecL();
            break;

        case EMTPObjectPropCodeVideoBitRate:
            ServiceRangeFormDescriptionL( aPropCode,
                EMTPWMVMinBitrate,
                EMTPWMVMaxBitrate,
                EMTPVideoBitrateStep,
                ETrue );
            break;

        case EMTPObjectPropCodeFramesPerThousandSeconds:
            ServiceRangeFormDescriptionL( aPropCode,
                EMTPMinFramesPerThousandSeconds,
                EMTPMaxFramesPerThousandSeconds,
                EMTPStepFramesPerThousandSeconds );
            break;

        case EMTPObjectPropCodeKeyFrameDistance:
            ServiceRangeFormDescriptionL( aPropCode,
                EMTPMinKeyFrameDistance,
                EMTPMaxKeyFrameDistance,
                EMTPStepKeyFrameDistance );
            break;

        case EMTPObjectPropCodeEncodingProfile:
            ServiceEncodingProfileDescriptionL();
            break;

        case EMTPObjectPropCodeDRMStatus:
            ServiceDRMStatusL();
            break;

        case EMTPExtObjectPropCodeOmaDrmStatus:
            ServiceOMADRMStatusL();
            break;

        default:
            User::Leave( KErrNotSupported );
            break;
        }
    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderGetObjectPropDesc::ServiceDescriptionL()
// Create Description and create new ObjectPropDesc
// -----------------------------------------------------------------------------
//
void CMediaMtpDataProviderGetObjectPropDesc::ServiceDescriptionL()
    {
    TMTPTypeUint32 uint32Data( KMTPDescriptionLen );

    // Althrough iObjectProperty is released in ServiceL(),
    // release it here maybe a more safer way
    if ( iObjectProperty != NULL )
        {
        delete iObjectProperty;
        iObjectProperty = NULL;
        }

    iObjectProperty = CMTPTypeObjectPropDesc::NewL( EMTPObjectPropCodeDescription,
        CMTPTypeObjectPropDesc::ELongStringForm,
        &uint32Data );
    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderGetObjectPropDesc::ServiceNumberOfChannelsL
// Create list of possible Channel numbers and create new ObjectPropDesc
// -----------------------------------------------------------------------------
//
void CMediaMtpDataProviderGetObjectPropDesc::ServiceNumberOfChannelsL()
    {
    PRINT( _L( "MM MTP => CMediaMtpDataProviderGetObjectPropDesc::ServiceCodeSampleRateL" ) );

    CMTPTypeObjectPropDescEnumerationForm* expectedForm = CMTPTypeObjectPropDescEnumerationForm::NewL( EMTPTypeUINT16 );
    CleanupStack::PushL( expectedForm ); // + expectedForm

    TUint16 values[] =
        {
        EMTPChannelMono,
        EMTPChannelStereo
        };

    TInt numValues =  sizeof ( values ) / sizeof ( values[0] ) ;

    for ( TInt i = 0; i < numValues; i++ )
        {
        TMTPTypeUint16 data( values[i] );
        expectedForm->AppendSupportedValueL( data );
        }

    // Althrough iObjectProperty is released in ServiceL(),
    // release it here maybe a more safer way :)
    if ( iObjectProperty != NULL )
        {
        delete iObjectProperty;
        iObjectProperty = NULL;
        }

    iObjectProperty = CMTPTypeObjectPropDesc::NewL( EMTPObjectPropCodeNumberOfChannels, *expectedForm );
    CleanupStack::PopAndDestroy( expectedForm ); // - expectedForm

    PRINT( _L( "MM MTP <= CMediaMtpDataProviderGetObjectPropDesc::ServiceCodeSampleRateL" ) );
    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderGetObjectPropDesc::ServiceCodeSampleRateL
// Create list of possible Sample rate list and create new ObjectPropDesc
// -----------------------------------------------------------------------------
//
void CMediaMtpDataProviderGetObjectPropDesc::ServiceCodeSampleRateL()
    {
    // if some format does not support the sample rate,
    // here still need an instance of CMTPTypeObjectPropDesc.
    PRINT( _L( "MM MTP => CMediaMtpDataProviderGetObjectPropDesc::ServiceCodeSampleRateL" ) );

    CMTPTypeObjectPropDescEnumerationForm* expectedForm =
        CMTPTypeObjectPropDescEnumerationForm::NewLC( EMTPTypeUINT32 ); // + expectedForm

    TUint32 values[] =
        {
        EMTPSampleRate8K ,
        EMTPSampleRate16K,
        EMTPSampleRate22_05K,
        EMTPSampleRate24K,
        EMTPSampleRate32K,
        EMTPSampleRate44_1K,
        EMTPSampleRate48K
        };

    TInt numValues = sizeof( values ) / sizeof( values[0] ) ;

    for ( TInt i = 0; i < numValues; i++ )
        {
        TMTPTypeUint32 data( values[i] );
        expectedForm->AppendSupportedValueL( data );
        }

    // Althrough iObjectProperty is released in ServiceL(),
    // release it here maybe a more safer way :)
    if ( iObjectProperty != NULL )
        {
        delete iObjectProperty;
        iObjectProperty = NULL;
        }

    iObjectProperty = CMTPTypeObjectPropDesc::NewL( EMTPObjectPropCodeSampleRate, CMTPTypeObjectPropDesc::EEnumerationForm, expectedForm );
    CleanupStack::PopAndDestroy( expectedForm ); // - expectedForm

    PRINT( _L( "MM MTP <= CMediaMtpDataProviderGetObjectPropDesc::ServiceCodeSampleRateL" ) );
    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderGetObjectPropDesc::ServiceWaveCodecL
// Create list of possible bitrate types and create new ObjectPropDesc
// -----------------------------------------------------------------------------
//
void CMediaMtpDataProviderGetObjectPropDesc::ServiceCodeWaveCodecL()
    {
    PRINT1( _L( "MM MTP => CMediaMtpDataProviderGetObjectPropDesc::ServiceCodeWaveCodecL, format = 0x%x" ), iFormatCode );

    CMTPTypeObjectPropDescEnumerationForm* form =
    CMTPTypeObjectPropDescEnumerationForm::NewLC( EMTPTypeUINT32 ); // + form

    switch (iFormatCode)
        {
        case EMTPFormatCodeWMA:
        case EMTPFormatCodeWMV:
        case EMTPFormatCodeASF:
            {
            PRINT1( _L( "MM MTP <> CMediaMtpDataProviderGetObjectPropDesc::ServiceCodeWaveCodecL format = 0x%x" ), iFormatCode );
            TMTPTypeUint32 data( EMTPAudioWAVECodecWMA );
            form->AppendSupportedValueL( data );
            }
            break;

        case EMTPFormatCodeMP3:
            {
            PRINT1( _L( "MM MTP <> CMediaMtpDataProviderGetObjectPropDesc::ServiceCodeWaveCodecL format = 0x%x" ), iFormatCode );
            TMTPTypeUint32 data( EMTPAudioWAVECodecMP3 );
            form->AppendSupportedValueL( data );
            }
            break;

        case EMTPFormatCodeMP4Container:
        case EMTPFormatCode3GPContainer:
        case EMTPFormatCodeAAC:
            {
            PRINT1( _L( "MM MTP <> CMediaMtpDataProviderGetObjectPropDesc::ServiceCodeWaveCodecL format = 0x%x" ), iFormatCode );
            TMTPTypeUint32 data( EMTPAudioWAVECodecAAC );
            form->AppendSupportedValueL( data );
            }
            break;

        case EMTPFormatCodeWAV:
            {
            PRINT1( _L("MM MTP <> CMediaMtpDataProviderGetObjectPropDesc::ServiceCodeWaveCodecL format = 0x%x"), iFormatCode );
            TMTPTypeUint32 data( EMTPAudioWAVECodecPCM );
            form->AppendSupportedValueL( data );
            }
            break;

        default:
            {
            PRINT1( _L( "MM MTP <> CMediaMtpDataProviderGetObjectPropDesc::ServiceCodeWaveCodecL leave because of invalid formatcode = 0x%x" ), iFormatCode );
            User::Leave(KErrNotSupported);
            }
            break;
        }

    // Althrough iObjectProperty is released in ServiceL(),
    // release it here maybe a more safer way :)
    if ( iObjectProperty != NULL )
        {
        delete iObjectProperty;
        iObjectProperty = NULL;
        }

    iObjectProperty = CMTPTypeObjectPropDesc::NewL( EMTPObjectPropCodeAudioWAVECodec, *form );
    CleanupStack::PopAndDestroy( form ); // - form

    PRINT( _L( "MM MTP <= CMediaMtpDataProviderGetObjectPropDesc::ServiceCodeWaveCodecL" ) );
    }

void CMediaMtpDataProviderGetObjectPropDesc::ServiceCodeAudioBitrateL()
    {
    PRINT1( _L("MM MTP => CMediaMtpDataProviderGetObjectPropDesc::ServiceCodeAudioBitrate, format = 0x%x"), iFormatCode );

    switch (iFormatCode)
        {
        case EMTPFormatCodeWMA:
        case EMTPFormatCodeWMV:
        case EMTPFormatCodeASF:
            {
            ServiceRangeFormDescriptionL( EMTPObjectPropCodeAudioBitRate,
                EMTPWMAMinBitrate,
                EMTPWMAMaxBitrate,
                EMTPAudioBitrateStep );
            }
            break;

        case EMTPFormatCodeMP3:
            {
            ServiceRangeFormDescriptionL( EMTPObjectPropCodeAudioBitRate,
                EMTPMP3MinBitrate,
                EMTPMP3MaxBitrate,
                EMTPAudioBitrateStep );
            }
            break;

        case EMTPFormatCodeAAC:
            {
            ServiceRangeFormDescriptionL( EMTPObjectPropCodeAudioBitRate,
                EMTPAACMinBitrate,
                EMTPAACMaxBitrate,
                EMTPAudioBitrateStep );
            }
            break;

        case EMTPFormatCodeWAV:
            {
            ServiceRangeFormDescriptionL( EMTPObjectPropCodeAudioBitRate,
                EMTPPCMMinBitrate,
                EMTPPCMMaxBitrate,
                EMTPAudioBitrateStep );
            }
            break;

        case EMTPFormatCodeMP4Container:
        case EMTPFormatCode3GPContainer:
            {
            ServiceRangeFormDescriptionL( EMTPObjectPropCodeAudioBitRate,
                EMTPAACPlusMinBitrate,
                EMTPAACPlusMaxBitrate,
                EMTPAudioBitrateStep );
            }
            break;

        default:
            {
            PRINT1( _L( "MM MTP <> CMediaMtpDataProviderGetObjectPropDesc::ServiceCodeAudioBitRateL leave because of invalid formatcode = 0x%x" ), iFormatCode );
            User::Leave( KErrNotSupported );
            }
        }

    PRINT( _L( "MM MTP <= CMediaMtpDataProviderGetObjectPropDesc::ServiceCodeAudioBitRateL" ) );
    }

// -----------------------------------------------------------------------------
// CGetObjectPropDesc::ServiceDurationL()
// Create list of Duration types and create new ObjectPropDesc
// -----------------------------------------------------------------------------
//
void CMediaMtpDataProviderGetObjectPropDesc::ServiceDurationL()
    {

    PRINT( _L( "MM MTP => CMediaMtpDataProviderGetObjectPropDesc::ServiceDurationL" ) );

    CMTPTypeObjectPropDescRangeForm* form = CMTPTypeObjectPropDescRangeForm::NewLC( EMTPTypeUINT32 ); // + form

    // Set expected values
    form->SetUint32L( CMTPTypeObjectPropDescRangeForm::EMinimumValue, EMTPMinDuration );
    form->SetUint32L( CMTPTypeObjectPropDescRangeForm::EMaximumValue, EMTPMaxDuration );
    form->SetUint32L( CMTPTypeObjectPropDescRangeForm::EStepSize, EMTPDurationStep );

    // Althrough iObjectProperty is released in ServiceL(),
    // release it here maybe a more safer way
    if ( iObjectProperty != NULL )
        {
        delete iObjectProperty;
        iObjectProperty = NULL;
        }

    iObjectProperty = CMTPTypeObjectPropDesc::NewL( EMTPObjectPropCodeDuration, *form );
    CleanupStack::PopAndDestroy( form ); // - form

    PRINT( _L( "MM MTP <= CMediaMtpDataProviderGetObjectPropDesc::ServiceDurationL" ) );
    }

void CMediaMtpDataProviderGetObjectPropDesc::ServiceScanTypeDescriptionL()
    {
    CMTPTypeObjectPropDescEnumerationForm* expectedForm =
        CMTPTypeObjectPropDescEnumerationForm::NewL( EMTPTypeUINT16 );
    CleanupStack::PushL( expectedForm ); // + expectedForm

    TUint16 values[] =
        {
        EMTPScanTypeProgressive
        };

    TInt numValues = sizeof ( values ) / sizeof ( values[0] ) ;
    for ( TInt i = 0; i < numValues; i++ )
        {
        TMTPTypeUint16 data( values[i] );
        expectedForm->AppendSupportedValueL( data );
        }

    // Althrough iObjectProperty is released in ServiceL(),
    // release it here maybe a more safer way :)
    if ( iObjectProperty != NULL )
        {
        delete iObjectProperty;
        iObjectProperty = NULL;
        }

    iObjectProperty = CMTPTypeObjectPropDesc::NewL( EMTPObjectPropCodeScanType, *expectedForm );
    CleanupStack::PopAndDestroy( expectedForm ); // - expectedForm
    }

void CMediaMtpDataProviderGetObjectPropDesc::ServiceVideoFourCCCodecL()
    {
    switch( iFormatCode )
        {
        case EMTPFormatCodeWMV:
        case EMTPFormatCodeASF:
            {
            CMTPTypeObjectPropDescEnumerationForm* expectedForm =
                CMTPTypeObjectPropDescEnumerationForm::NewL( EMTPTypeUINT32 );
            CleanupStack::PushL( expectedForm ); // + expectedForm

            TUint32 values[] =
                {
                EMTPVideoFourCCCodecWMV3
                };

            TInt numValues = sizeof ( values ) / sizeof ( values[0] ) ;
            for ( TInt i = 0; i < numValues; i++ )
                {
                TMTPTypeUint32 data( values[i] );
                expectedForm->AppendSupportedValueL( data );
                }

            // comment out asf fourcc for mp2a and mp4a, not sure if this is needed
            /*if ( iFormatCode == EMTPFormatCodeASF )
                {
                TUint32 additionalValues[] =
                    {
                    EMTPVideoFourCCCodecMP2A,
                    EMTPVideoFourCCCodecMP4A
                    };

                for ( TInt j = 0; i < sizeof(additionalValues) / sizeof(additionalValues[0]) )
                    {
                    TMTPTypeUint32 data( additionalValues[i] );
                    expectedForm->AppendSupportedValueL( data );
                    }
                }*/

            // Althrough iObjectProperty is released in ServiceL(),
            // release it here maybe a more safer way :)
            if ( iObjectProperty != NULL )
                {
                delete iObjectProperty;
                iObjectProperty = NULL;
                }

            iObjectProperty = CMTPTypeObjectPropDesc::NewL( EMTPObjectPropCodeVideoFourCCCodec, *expectedForm );
            CleanupStack::PopAndDestroy( expectedForm ); // - expectedForm
            }
            break;

        default:
            {
            PRINT1( _L( "MM MTP <> CMediaMtpDataProviderGetObjectPropDesc::ServiceVideoFourCCCodecL leave because of invalid formatcode = 0x%x" ), iFormatCode );
            User::Leave( KErrNotSupported );
            }
            break;
        }
    }

void CMediaMtpDataProviderGetObjectPropDesc::ServiceVideoBitrateL()
    {
    switch( iFormatCode )
        {
        case EMTPFormatCodeWMV:
        case EMTPFormatCodeASF:
            {
            ServiceRangeFormDescriptionL( EMTPObjectPropCodeAudioBitRate,
                EMTPWMVMinBitrate,
                EMTPWMVMaxBitrate,
                EMTPVideoBitrateStep,
                ETrue );
            }
            break;

        default:
            {
            PRINT1( _L( "MM MTP <> CMediaMtpDataProviderGetObjectPropDesc::ServiceVideoBitrateL leave because of invalid formatcode = 0x%x" ), iFormatCode );
            User::Leave( KErrNotSupported );
            }
            break;
        }
    }

void CMediaMtpDataProviderGetObjectPropDesc::ServiceEncodingProfileDescriptionL()
    {
    CMTPTypeObjectPropDescEnumerationForm* expectedForm =
        CMTPTypeObjectPropDescEnumerationForm::NewLC( EMTPTypeString ); // + expectedForm

    CMTPTypeString* string = CMTPTypeString::NewLC( _L( "SP@LL" ) );
    expectedForm->AppendSupportedValueL( *string );
    string->SetL(_L("SP@ML"));
    expectedForm->AppendSupportedValueL( *string );
    string->SetL(_L("MP@LL"));
    expectedForm->AppendSupportedValueL( *string );

    CMTPTypeObjectPropDesc::TPropertyInfo propertyInfo;
    propertyInfo.iDataType = EMTPTypeString;
    propertyInfo.iFormFlag = CMTPTypeObjectPropDesc::EEnumerationForm;
    propertyInfo.iGetSet = CMTPTypeObjectPropDesc::EReadWrite;
    iObjectProperty = CMTPTypeObjectPropDesc::NewL( EMTPObjectPropCodeEncodingProfile,
        propertyInfo,
        expectedForm );

    CleanupStack::PopAndDestroy( string );
    CleanupStack::PopAndDestroy( expectedForm ); // - expectedForm
    }

void CMediaMtpDataProviderGetObjectPropDesc::ServiceRangeFormDescriptionL( TUint16 aPropCode,
    TUint32 aMinValue,
    TUint32 aMaxValue,
    TUint32 aStepValue,
    TBool aIsReadOnly )
    {
    CMTPTypeObjectPropDescRangeForm* form =
        CMTPTypeObjectPropDescRangeForm::NewLC( EMTPTypeUINT32 ); // + form

    // Set expected values
    form->SetUint32L( CMTPTypeObjectPropDescRangeForm::EMinimumValue, aMinValue );
    form->SetUint32L( CMTPTypeObjectPropDescRangeForm::EMaximumValue, aMaxValue );
    form->SetUint32L( CMTPTypeObjectPropDescRangeForm::EStepSize, aStepValue );

    // Althrough iObjectProperty is released in ServiceL(),
    // release it here maybe a more safer way
    if ( iObjectProperty != NULL )
        {
        delete iObjectProperty;
        iObjectProperty = NULL;
        }

    CMTPTypeObjectPropDesc::TPropertyInfo propertyInfo;
    propertyInfo.iDataType = EMTPTypeUINT32;
    propertyInfo.iFormFlag = CMTPTypeObjectPropDesc::ERangeForm;
    propertyInfo.iGetSet = !aIsReadOnly;
    iObjectProperty = CMTPTypeObjectPropDesc::NewL( aPropCode, propertyInfo, form );

    CleanupStack::PopAndDestroy( form ); // - form
    }

// -----------------------------------------------------------------------------
// CGetObjectPropDesc::ServiceDRMStatusL
// Create list of possible DRM status and create new ObjectPropDesc
// -----------------------------------------------------------------------------
//
void CMediaMtpDataProviderGetObjectPropDesc::ServiceDRMStatusL()
    {
    CMTPTypeObjectPropDescEnumerationForm* expectedForm =
        CMTPTypeObjectPropDescEnumerationForm::NewL( EMTPTypeUINT16 );
    CleanupStack::PushL( expectedForm ); // + expectedForm

    TUint16 values[] =
        {
        EMTPDrmNoProtection,
        EMTPDrmProtection,
        EMTPDrmReserveForMTP,
        EMTPDrmVenderExtension
        };

    TInt numValues = sizeof ( values ) / sizeof ( values[0] ) ;
    for ( TInt i = 0; i < numValues; i++ )
        {
        TMTPTypeUint16 data( values[i] );
        expectedForm->AppendSupportedValueL( data );
        }

    // Althrough iObjectProperty is released in ServiceL(),
    // release it here maybe a more safer way :)
    if ( iObjectProperty != NULL )
        {
        delete iObjectProperty;
        iObjectProperty = NULL;
        }

    iObjectProperty = CMTPTypeObjectPropDesc::NewL( EMTPObjectPropCodeDRMStatus, *expectedForm );
    CleanupStack::PopAndDestroy( expectedForm ); // - expectedForm
    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderGetObjectPropDesc::ServiceOMADRMStatusL
// Create list of possible OMA DRM status and create new ObjectPropDesc
// -----------------------------------------------------------------------------
//
void CMediaMtpDataProviderGetObjectPropDesc::ServiceOMADRMStatusL()
    {
    CMTPTypeObjectPropDescEnumerationForm* expectedForm =
        CMTPTypeObjectPropDescEnumerationForm::NewL( EMTPTypeUINT8 );
    CleanupStack::PushL( expectedForm ); // + expectedForm

    TUint8 values[] =
        {
        EMTPOMADrmNoProtection,
        EMTPOMADrmProtection,
        };

    TInt numValues = sizeof ( values ) / sizeof ( values[0] ) ;
    for ( TInt i = 0; i < numValues; i++ )
        {
        TMTPTypeUint8 data( values[i] );
        expectedForm->AppendSupportedValueL( data );
        }

    // Althrough iObjectProperty is released in ServiceL(),
    // release it here maybe a more safer way :)
    if ( iObjectProperty != NULL )
        {
        delete iObjectProperty;
        iObjectProperty = NULL;
        }

    CMTPTypeObjectPropDesc::TPropertyInfo propertyInfo;
    propertyInfo.iDataType = EMTPTypeUINT8;
    propertyInfo.iFormFlag = CMTPTypeObjectPropDesc::EEnumerationForm;

    iObjectProperty = CMTPTypeObjectPropDesc::NewL( EMTPExtObjectPropCodeOmaDrmStatus,
        propertyInfo,
        expectedForm );
    CleanupStack::PopAndDestroy( expectedForm ); // - expectedForm
    }

// end of file
