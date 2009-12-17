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
#include <mtp/cmtptypeobjectpropdesc.h>
#include <mtp/mtpprotocolconstants.h>
#include <mtp/cmtptypestring.h>

#include "cmediamtpdataprovidergetobjectpropdesc.h"
#include "mediamtpdataproviderconst.h"
#include "cmediamtpdataprovidergetformatcapabilities.h"
#include "mmmtpdplogger.h"

const TUint32 KMTPDescriptionLen = 0x00000200;

// forward declaration
class MMmMtpDpConfig;
class CMTPTypeObjectPropDesc;
class CMTPTypeInterdependentPropDesc;
class CMTPTypeObjectPropDesc;
class CMTPTypeString;
class CGetFormatCapabilities;

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderGetFormatCapabilities::NewL
// Two-phase construction method
// -----------------------------------------------------------------------------
//
MMmRequestProcessor* CMediaMtpDataProviderGetFormatCapabilities::NewL( MMTPDataProviderFramework& aFramework,
    MMTPConnection& aConnection,
    MMmMtpDpConfig& aDpConfig )
    {
    CMediaMtpDataProviderGetFormatCapabilities* self =
        new ( ELeave ) CMediaMtpDataProviderGetFormatCapabilities( aFramework,
            aConnection,
            aDpConfig );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderGetFormatCapabilities::ConstructL
// Two-phase construction method
// -----------------------------------------------------------------------------
//
void CMediaMtpDataProviderGetFormatCapabilities::ConstructL()
    {
    CGetFormatCapabilities::ConstructL();
    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderGetFormatCapabilities::~CMediaMtpDataProviderGetFormatCapabilities()
// Destructor
// -----------------------------------------------------------------------------
//
CMediaMtpDataProviderGetFormatCapabilities::~CMediaMtpDataProviderGetFormatCapabilities()
    {
    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderGetFormatCapabilities::CMediaMtpDataProviderGetFormatCapabilities
// Standard c++ constructor
// -----------------------------------------------------------------------------
//
CMediaMtpDataProviderGetFormatCapabilities::CMediaMtpDataProviderGetFormatCapabilities( MMTPDataProviderFramework& aFramework,
    MMTPConnection& aConnection,
    MMmMtpDpConfig& aDpConfig ) :
    CGetFormatCapabilities( aFramework, aConnection, aDpConfig )
    {

    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderGetFormatCapabilities::ServiceInterdepentPropDescL
//
// -----------------------------------------------------------------------------
//
void CMediaMtpDataProviderGetFormatCapabilities::ServiceInterdepentPropDescL()
    {
    PRINT( _L( "MM MTP => CMediaMtpDataProviderGetFormatCapabilities::ServiceInterdepentPropDescL" ) );

    iInterdependentPropDesc = CMTPTypeInterdependentPropDesc::NewL();

    TBool audioWaveCodecSupported = EFalse;
    TBool audioBitRateSupported = EFalse;
    TBool videoFourCcCodecSupported = EFalse;
    TBool videoBitRateSupported = EFalse;

    switch ( iFormatCode )
        {
        case EMTPFormatCodeWMA:
        case EMTPFormatCodeMP3:
        case EMTPFormatCodeAAC:
        case EMTPFormatCodeWAV:
            {
            TInt count = sizeof( KMmMtpDpSupportedPropMandatoryAudio ) / sizeof( TUint16 );

            for ( TInt i = 0; i < count; i++ )
                {
                switch ( KMmMtpDpSupportedPropMandatoryAudio[i] )
                    {
                    case EMTPObjectPropCodeAudioWAVECodec:
                        audioWaveCodecSupported = ETrue;
                        break;
                    case EMTPObjectPropCodeAudioBitRate:
                        audioBitRateSupported = ETrue;
                        break;
                    default:
                        // do nothing
                        break;
                    }
                }
            }
            break;

        case EMTPFormatCodeMP4Container:
            {
            TInt count = sizeof( KMmMtpDpSupportedPropMandatoryAudio ) / sizeof( TUint16 );

            for ( TInt i = 0; i < count; i++ )
                {
                switch ( KMmMtpDpSupportedPropMandatoryAudio[i] )
                    {
                    case EMTPObjectPropCodeAudioWAVECodec:
                        audioWaveCodecSupported = ETrue;
                        break;
                    case EMTPObjectPropCodeAudioBitRate:
                        audioBitRateSupported = ETrue;
                        break;
                    default:
                        // do nothing
                        break;
                    }
                }
            }
            break;

        case EMTPFormatCode3GPContainer:
            {
            TInt count = sizeof( KMmMtpDpSupportedPropMandatoryAudio ) / sizeof( TUint16 );

            for ( TInt i = 0; i < count; i++ )
                {
                switch ( KMmMtpDpSupportedPropMandatoryAudio[i] )
                    {
                    case EMTPObjectPropCodeAudioWAVECodec:
                        audioWaveCodecSupported = ETrue;
                        break;
                    case EMTPObjectPropCodeAudioBitRate:
                        audioBitRateSupported = ETrue;
                        break;
                    default:
                        // do nothing
                        break;
                    }
                }
            }
            break;

        case EMTPFormatCodeWMV:
        case EMTPFormatCodeASF:
            {
            TInt count = sizeof( KMmMtpDpSupportedPropMandatoryWMV ) / sizeof( TUint16 );

            for ( TInt i = 0; i < count; i++ )
                {
                switch ( KMmMtpDpSupportedPropMandatoryWMV[i] )
                    {
                    case EMTPObjectPropCodeAudioWAVECodec:
                        audioWaveCodecSupported = ETrue;
                        break;
                    case EMTPObjectPropCodeAudioBitRate:
                        audioBitRateSupported = ETrue;
                        break;
                    case EMTPObjectPropCodeVideoFourCCCodec:
                        videoFourCcCodecSupported = ETrue;
                        break;
                    case EMTPObjectPropCodeVideoBitRate:
                        videoBitRateSupported = ETrue;
                        break;
                    default:
                        // do nothing
                        break;
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


    if ( audioWaveCodecSupported && audioBitRateSupported )
        {
        CMTPTypeInterdependentProperties* properties = CMTPTypeInterdependentProperties::NewL();
        if ( videoFourCcCodecSupported && videoBitRateSupported )
            {
            // TODO: need to confirm
            properties->AppendL( ServiceVideoFourCCCodecL() );
            properties->AppendL( ServiceVideoBitrateL() );
            iInterdependentPropDesc->AppendL( properties );
            }
        properties = CMTPTypeInterdependentProperties::NewL();
        properties->AppendL( ServiceAudioWaveCodecL() );
        properties->AppendL( ServiceAudioBitrateL() );
        iInterdependentPropDesc->AppendL( properties );
        }

    PRINT( _L( "MM MTP <= CMediaMtpDataProviderGetFormatCapabilities::ServiceInterdepentPropDescL" ) );
    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderGetFormatCapabilities::ServiceAudioWaveCodecL
//
// -----------------------------------------------------------------------------
//
CMTPTypeObjectPropDesc* CMediaMtpDataProviderGetFormatCapabilities::ServiceAudioWaveCodecL()
    {
    PRINT1( _L( "MM MTP => CMediaMtpDataProviderGetFormatCapabilities::ServiceAudioWaveCodecL, iFormatCode = 0x%X" ), iFormatCode );
    CMTPTypeObjectPropDescEnumerationForm* form =
        CMTPTypeObjectPropDescEnumerationForm::NewLC( EMTPTypeUINT32 );    // + form

    switch( iFormatCode )
        {
        case EMTPFormatCodeWMA:
        case EMTPFormatCodeWMV:
        case EMTPFormatCodeASF:
            form->AppendSupportedValueL( TMTPTypeUint32( EMTPAudioWAVECodecWMA ) );
            break;

        case EMTPFormatCodeMP3:
            form->AppendSupportedValueL( TMTPTypeUint32( EMTPAudioWAVECodecMP3 ) );
            break;

        case EMTPFormatCodeMP4Container:
        case EMTPFormatCode3GPContainer:
        case EMTPFormatCodeAAC:
            form->AppendSupportedValueL( TMTPTypeUint32( EMTPAudioWAVECodecAAC ) );
            break;

        case EMTPFormatCodeWAV:
            form->AppendSupportedValueL( TMTPTypeUint32( EMTPAudioWAVECodecPCM ) );
            break;

        default:
            User::Leave( KErrNotSupported );
            break;
        }

    CMTPTypeObjectPropDesc* propertyDesc = CMTPTypeObjectPropDesc::NewLC( EMTPObjectPropCodeAudioWAVECodec, *form );    // + propertyDesc

    // Set group code
    propertyDesc->SetUint32L( CMTPTypeObjectPropDesc::EGroupCode,
        EGroupCodeMediaDB );
    CleanupStack::Pop( propertyDesc );    // - propertyDesc
    CleanupStack::PopAndDestroy( form ); // - form

    PRINT( _L( "MM MTP <= CMediaMtpDataProviderGetFormatCapabilities::ServiceAudioWaveCodecL" ) );

    return propertyDesc;
    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderGetFormatCapabilities::ServiceAudioBitrateL
//
// -----------------------------------------------------------------------------
//
CMTPTypeObjectPropDesc* CMediaMtpDataProviderGetFormatCapabilities::ServiceAudioBitrateL()
    {
    PRINT1( _L( "MM MTP => CMediaMtpDataProviderGetFormatCapabilities::ServiceAudioBitrateL, iFormatCode = 0x%X" ), iFormatCode );
    CMTPTypeObjectPropDesc* propertyDesc = NULL;
    switch( iFormatCode )
        {
        case EMTPFormatCodeWMA:
        case EMTPFormatCodeWMV:
        case EMTPFormatCodeASF:
            propertyDesc = ServiceRangeFormDescriptionL( EMTPObjectPropCodeAudioBitRate,
                EMTPWMAMinBitrate,
                EMTPWMAMaxBitrate,
                EMTPAudioBitrateStep );
            break;

        case EMTPFormatCodeMP3:
            propertyDesc = ServiceRangeFormDescriptionL( EMTPObjectPropCodeAudioBitRate,
                EMTPMP3MinBitrate,
                EMTPMP3MaxBitrate,
                EMTPAudioBitrateStep );
            break;

        case EMTPFormatCodeAAC:
            propertyDesc = ServiceRangeFormDescriptionL( EMTPObjectPropCodeAudioBitRate,
                EMTPAACMinBitrate,
                EMTPAACMaxBitrate,
                EMTPAudioBitrateStep );
            break;

        case EMTPFormatCodeWAV:
            propertyDesc = ServiceRangeFormDescriptionL( EMTPObjectPropCodeAudioBitRate,
                EMTPPCMMinBitrate,
                EMTPPCMMaxBitrate,
                EMTPAudioBitrateStep );
            break;

        case EMTPFormatCodeMP4Container:
        case EMTPFormatCode3GPContainer:
            propertyDesc = ServiceRangeFormDescriptionL( EMTPObjectPropCodeAudioBitRate,
                EMTPAACPlusMinBitrate,
                EMTPAACPlusMaxBitrate,
                EMTPAudioBitrateStep );
            break;

        default:
            User::Leave( KErrNotSupported );
            break;
        }
    PRINT( _L( "MM MTP <= CMediaMtpDataProviderGetFormatCapabilities::ServiceAudioBitrateL" ) );

    return propertyDesc;
    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderGetFormatCapabilities::ServiceVideoFourCCCodecL
// Create Description and create new ObjectPropDesc
// -----------------------------------------------------------------------------
//
CMTPTypeObjectPropDesc* CMediaMtpDataProviderGetFormatCapabilities::ServiceVideoFourCCCodecL()
    {
    PRINT( _L( "MM MTP => CMediaMtpDataProviderGetFormatCapabilities::ServiceVideoFourCCCodecL" ) );
    CMTPTypeObjectPropDesc* propertyDesc = NULL;
    if ( ( EMTPFormatCodeWMV == iFormatCode )
        || ( EMTPFormatCodeASF == iFormatCode ) )
        {
        CMTPTypeObjectPropDescEnumerationForm* expectedForm =
            CMTPTypeObjectPropDescEnumerationForm::NewLC( EMTPTypeUINT32 ); // + expectedForm

        TUint32 values[] =
            {
            EMTPVideoFourCCCodecWMV3
            };

        TInt numValues = sizeof( values ) / sizeof( values[0] );
        for ( TInt i = 0; i < numValues; i++ )
            {
            TMTPTypeUint32 data( values[i] );
            expectedForm->AppendSupportedValueL( data );
            }
        propertyDesc = CMTPTypeObjectPropDesc::NewLC( EMTPObjectPropCodeVideoFourCCCodec,
            *expectedForm );    // + propertyDesc
        // Set group code
        propertyDesc->SetUint32L( CMTPTypeObjectPropDesc::EGroupCode,
            EGroupCodeMediaDB );
        CleanupStack::Pop( propertyDesc ); // - propertyDesc
        CleanupStack::PopAndDestroy( expectedForm ); // - expectedForm
        }

    PRINT( _L( "MM MTP <= CMediaMtpDataProviderGetFormatCapabilities::ServiceVideoFourCCCodecL" ) );
    return propertyDesc;
    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderGetFormatCapabilities::ServiceVideoBitrateL
// Create Description and create new ObjectPropDesc
// -----------------------------------------------------------------------------
//
CMTPTypeObjectPropDesc* CMediaMtpDataProviderGetFormatCapabilities::ServiceVideoBitrateL()
    {
    PRINT( _L( "MM MTP => CMediaMtpDataProviderGetFormatCapabilities::ServiceVideoBitrateL" ) );
    CMTPTypeObjectPropDesc* propertyDesc = NULL;
    if ( ( EMTPFormatCodeWMV == iFormatCode )
        || ( EMTPFormatCodeASF == iFormatCode ) )
        {
        propertyDesc = ServiceRangeFormDescriptionL( EMTPObjectPropCodeVideoBitRate,
            EMTPWMVMinBitrate,
            EMTPWMVMaxBitrate,
            EMTPVideoBitrateStep,
            ETrue );
        }
    PRINT( _L( "MM MTP <= CMediaMtpDataProviderGetFormatCapabilities::ServiceVideoBitrateL" ) );
    return propertyDesc;
    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderGetFormatCapabilities::ServiceRangeFormDescriptionL
//
// -----------------------------------------------------------------------------
//
CMTPTypeObjectPropDesc* CMediaMtpDataProviderGetFormatCapabilities::ServiceRangeFormDescriptionL( TUint16 aPropertyCode,
    TUint32 aMinValue,
    TUint32 aMaxValue,
    TUint32 aStepValue,
    TBool aIsReadOnly )
    {
    PRINT1( _L( "MM MTP => CMediaMtpDataProviderGetFormatCapabilities::ServiceRangeFormDescriptionL, aPropertyCode = 0x%X" ),
        aPropertyCode );
    CMTPTypeObjectPropDescRangeForm* form =
        CMTPTypeObjectPropDescRangeForm::NewLC( EMTPTypeUINT32 ); // + form

    // Set expected values
    form->SetUint32L( CMTPTypeObjectPropDescRangeForm::EMinimumValue, aMinValue );
    form->SetUint32L( CMTPTypeObjectPropDescRangeForm::EMaximumValue, aMaxValue );
    form->SetUint32L( CMTPTypeObjectPropDescRangeForm::EStepSize, aStepValue );

    CMTPTypeObjectPropDesc::TPropertyInfo propertyInfo;
    propertyInfo.iDataType = EMTPTypeUINT32;
    propertyInfo.iFormFlag = CMTPTypeObjectPropDesc::ERangeForm;
    propertyInfo.iGetSet = !aIsReadOnly;

    CMTPTypeObjectPropDesc* propertyDesc = CMTPTypeObjectPropDesc::NewLC( aPropertyCode,
        propertyInfo,
        form );    // + propertyDesc
    // Set group code
    propertyDesc->SetUint32L( CMTPTypeObjectPropDesc::EGroupCode, EGroupCodeMediaDB );

    CleanupStack::Pop( propertyDesc );     // - propertyDesc
    CleanupStack::PopAndDestroy( form ); // - form
    PRINT( _L( "MM MTP <= CMediaMtpDataProviderGetFormatCapabilities::ServiceRangeFormDescriptionL" ) );

    return propertyDesc;
    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderGetFormatCapabilities::ServiceSpecificpropertyDescL
//
// -----------------------------------------------------------------------------
//
CMTPTypeObjectPropDesc* CMediaMtpDataProviderGetFormatCapabilities::ServiceSpecificPropertyDescL( TUint16 aPropertyCode )
    {
    PRINT1( _L( "MM MTP => CMediaMtpDataProviderGetFormatCapabilities::ServiceSpecificPropertyDescL, aPropertyCode = 0x%X" ), aPropertyCode );
    // Property object to return with the desc value
    CMTPTypeObjectPropDesc* propertyDesc = NULL;

    switch ( aPropertyCode )
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
            propertyDesc = CMTPTypeObjectPropDesc::NewL( aPropertyCode );
            break;

            // Number of Channels (m)
        case EMTPObjectPropCodeNumberOfChannels:
            propertyDesc = ServiceNumberOfChannelsL();
            break;

            // Sample Rate (HAS MINIMUM AND MAX VALUE)
        case EMTPObjectPropCodeSampleRate:
            propertyDesc = ServiceCodeSampleRateL();
            break;

            // Audio Wave Codec (MAY REQUIRED LIMITED NUMBER TO BE PRODUCED)
        case EMTPObjectPropCodeAudioWAVECodec:
            propertyDesc = ServiceAudioWaveCodecL();
            break;

            // Audio Bit Rate (MAY REQUIRED LIMITED NUMBER TO BE PRODUCED)
        case EMTPObjectPropCodeAudioBitRate:
            propertyDesc = ServiceAudioBitrateL();
            break;

            // Duration
        case EMTPObjectPropCodeDuration:
            propertyDesc = ServiceRangeFormDescriptionL( aPropertyCode,
                EMTPMinDuration,
                EMTPMaxDuration,
                EMTPDurationStep );
            break;

            // Description
        case EMTPObjectPropCodeDescription:
            propertyDesc = ServiceDescriptionL();
            break;

        case EMTPObjectPropCodeWidth:
            propertyDesc = ServiceRangeFormDescriptionL( aPropertyCode,
                EMTPMinWidth,
                EMTPMaxWidth,
                EMTPStepWidth );
            break;

        case EMTPObjectPropCodeHeight:
            propertyDesc = ServiceRangeFormDescriptionL( aPropertyCode,
                EMTPMinHeight,
                EMTPMaxHeight,
                EMTPStepHeight );
            break;

        case EMTPObjectPropCodeScanType:
            propertyDesc = ServiceScanTypeDescriptionL();
            break;

        case EMTPObjectPropCodeVideoFourCCCodec:
            propertyDesc = ServiceVideoFourCCCodecL();
            break;

        case EMTPObjectPropCodeVideoBitRate:
            propertyDesc = ServiceRangeFormDescriptionL( aPropertyCode,
                EMTPWMVMinBitrate,
                EMTPWMVMaxBitrate,
                EMTPVideoBitrateStep,
                ETrue );
            break;

        case EMTPObjectPropCodeFramesPerThousandSeconds:
            propertyDesc = ServiceRangeFormDescriptionL( aPropertyCode,
                EMTPMinFramesPerThousandSeconds,
                EMTPMaxFramesPerThousandSeconds,
                EMTPStepFramesPerThousandSeconds );
            break;

        case EMTPObjectPropCodeKeyFrameDistance:
            propertyDesc = ServiceRangeFormDescriptionL( aPropertyCode,
                EMTPMinKeyFrameDistance,
                EMTPMaxKeyFrameDistance,
                EMTPStepKeyFrameDistance );
            break;

        case EMTPObjectPropCodeEncodingProfile:
            propertyDesc = ServiceEncodingProfileDescriptionL();
            break;

        case EMTPObjectPropCodeDRMStatus:
            propertyDesc = ServiceDRMStatusL();
            break;

        case EMTPExtObjectPropCodeOmaDrmStatus:
            propertyDesc = ServiceOMADRMStatusL();
            break;

        default:
            User::Leave( KErrNotSupported );
            break;
        }

    PRINT( _L( "MM MTP <= CMediaMtpDataProviderGetFormatCapabilities::ServiceSpecificPropertyDescL" ) );
    return propertyDesc;
    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderGetFormatCapabilities::ServiceNumberOfChannelsL()
// Create list of possible Channel numbers and create new ObjectPropDesc
// -----------------------------------------------------------------------------
//
CMTPTypeObjectPropDesc* CMediaMtpDataProviderGetFormatCapabilities::ServiceNumberOfChannelsL()
    {
    PRINT( _L( "MM MTP => CMediaMtpDataProviderGetFormatCapabilities::ServiceNumberOfChannelsL" ) );
    CMTPTypeObjectPropDescEnumerationForm* expectedForm =
        CMTPTypeObjectPropDescEnumerationForm::NewLC( EMTPTypeUINT16 ); // + expectedForm

    TUint16 values[] =
        {
        EMTPChannelMono,
        EMTPChannelStereo
        };

    TInt numValues = sizeof( values ) / sizeof( values[0] );

    for ( TInt i = 0; i < numValues; i++ )
        {
        TMTPTypeUint16 data( values[i] );
        expectedForm->AppendSupportedValueL( data );
        }

    CMTPTypeObjectPropDesc* propertyDesc = CMTPTypeObjectPropDesc::NewL( EMTPObjectPropCodeNumberOfChannels,
        *expectedForm );
    CleanupStack::PopAndDestroy( expectedForm ); // - expectedForm

    PRINT( _L( "MM MTP <= CMediaMtpDataProviderGetFormatCapabilities::ServiceNumberOfChannelsL" ) );

    return propertyDesc;
    }

// -----------------------------------------------------------------------------
//CMediaMtpDataProviderGetFormatCapabilities::ServiceCodeSampleRateL
// Create list of possible Sample rate list and create new ObjectPropDesc
// -----------------------------------------------------------------------------
//
CMTPTypeObjectPropDesc* CMediaMtpDataProviderGetFormatCapabilities::ServiceCodeSampleRateL()
    {
    PRINT( _L( "MM MTP => CMediaMtpDataProviderGetFormatCapabilities::ServiceCodeSampleRateL" ) );
    CMTPTypeObjectPropDescEnumerationForm* expectedForm =
        CMTPTypeObjectPropDescEnumerationForm::NewLC( EMTPTypeUINT32 ); // + expectedForm

    TUint32 values[] =
        {
            EMTPSampleRate8K,
            EMTPSampleRate16K,
            EMTPSampleRate22_05K,
            EMTPSampleRate24K,
            EMTPSampleRate32K,
            EMTPSampleRate44_1K,
            EMTPSampleRate48K
        };

    TInt numValues = sizeof( values ) / sizeof( values[0] );

    for ( TInt i = 0; i < numValues; i++ )
        {
        TMTPTypeUint32 data( values[i] );
        expectedForm->AppendSupportedValueL( data );
        }

    CMTPTypeObjectPropDesc* propertyDesc = CMTPTypeObjectPropDesc::NewL( EMTPObjectPropCodeSampleRate,
        CMTPTypeObjectPropDesc::EEnumerationForm,
        expectedForm );
    CleanupStack::PopAndDestroy( expectedForm ); // - expectedForm

    PRINT( _L( "MM MTP <= CMediaMtpDataProviderGetFormatCapabilities::ServiceCodeSampleRateL" ) );

    return propertyDesc;
    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderGetFormatCapabilities::ServiceDescriptionL()
// Create Description and create new ObjectPropDesc
// -----------------------------------------------------------------------------
//
CMTPTypeObjectPropDesc* CMediaMtpDataProviderGetFormatCapabilities::ServiceDescriptionL()
    {
    PRINT( _L( "MM MTP => CMediaMtpDataProviderGetFormatCapabilities::ServiceDescriptionL" ) );

    TMTPTypeUint32 uint32Data( KMTPDescriptionLen );
    CMTPTypeObjectPropDesc* propertyDesc = CMTPTypeObjectPropDesc::NewL( EMTPObjectPropCodeDescription,
        CMTPTypeObjectPropDesc::ELongStringForm,
        &uint32Data );

    PRINT( _L( "MM MTP <= CMediaMtpDataProviderGetFormatCapabilities::ServiceDescriptionL" ) );

    return propertyDesc;
    }

// -----------------------------------------------------------------------------
//CMediaMtpDataProviderGetFormatCapabilities::ServiceScanTypeDescriptionL
// Create Description and create new ObjectPropDesc
// -----------------------------------------------------------------------------
//
CMTPTypeObjectPropDesc* CMediaMtpDataProviderGetFormatCapabilities::ServiceScanTypeDescriptionL()
    {
    PRINT( _L( "MM MTP => CMediaMtpDataProviderGetFormatCapabilities::ServiceScanTypeDescriptionL" ) );
    CMTPTypeObjectPropDescEnumerationForm* expectedForm =
        CMTPTypeObjectPropDescEnumerationForm::NewLC( EMTPTypeUINT16 ); // + expectedForm

    TUint16 values[] =
        {
        EMTPScanTypeProgressive
        };

    TInt numValues = sizeof( values ) / sizeof( values[0] );
    for ( TInt i = 0; i < numValues; i++ )
        {
        TMTPTypeUint16 data( values[i] );
        expectedForm->AppendSupportedValueL( data );
        }

    CMTPTypeObjectPropDesc* propertyDesc = NULL;
    propertyDesc = CMTPTypeObjectPropDesc::NewL( EMTPObjectPropCodeScanType,
        *expectedForm );
    CleanupStack::PopAndDestroy( expectedForm ); // - expectedForm

    PRINT( _L( "MM MTP <= CMediaMtpDataProviderGetFormatCapabilities::ServiceScanTypeDescriptionL" ) );

    return propertyDesc;
    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderGetFormatCapabilities::ServiceEncodingProfileDescriptionL
// Create Description and create new ObjectPropDesc
// -----------------------------------------------------------------------------
//
CMTPTypeObjectPropDesc* CMediaMtpDataProviderGetFormatCapabilities::ServiceEncodingProfileDescriptionL()
    {
    PRINT( _L( "MM MTP => CMediaMtpDataProviderGetFormatCapabilities::ServiceEncodingProfileDescriptionL" ) );
    CMTPTypeObjectPropDescEnumerationForm* expectedForm =
        CMTPTypeObjectPropDescEnumerationForm::NewLC( EMTPTypeString ); // + expectedForm

    CMTPTypeString* string = CMTPTypeString::NewLC( _L( "SP@LL" ) );    // + string
    expectedForm->AppendSupportedValueL( *string );
    string->SetL( _L("SP@ML"));
    expectedForm->AppendSupportedValueL( *string );
    string->SetL( _L("MP@LL"));
    expectedForm->AppendSupportedValueL( *string );

    CMTPTypeObjectPropDesc::TPropertyInfo propertyInfo;
    propertyInfo.iDataType = EMTPTypeString;
    propertyInfo.iFormFlag = CMTPTypeObjectPropDesc::EEnumerationForm;
    propertyInfo.iGetSet = CMTPTypeObjectPropDesc::EReadWrite;

    CMTPTypeObjectPropDesc* propertyDesc = CMTPTypeObjectPropDesc::NewL( EMTPObjectPropCodeEncodingProfile,
            propertyInfo,
            expectedForm );

    CleanupStack::PopAndDestroy( string );       // - string
    CleanupStack::PopAndDestroy( expectedForm ); // - expectedForm

    PRINT( _L( "MM MTP <= CMediaMtpDataProviderGetFormatCapabilities::ServiceEncodingProfileDescriptionL" ) );

    return propertyDesc;
    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderGetFormatCapabilities::ServiceDRMStatusL
// Create Description and create new ObjectPropDesc
// -----------------------------------------------------------------------------
//
CMTPTypeObjectPropDesc* CMediaMtpDataProviderGetFormatCapabilities::ServiceDRMStatusL()
    {
    PRINT( _L( "MM MTP => CMediaMtpDataProviderGetFormatCapabilities::ServiceDRMStatusL" ) );
    CMTPTypeObjectPropDescEnumerationForm* expectedForm =
        CMTPTypeObjectPropDescEnumerationForm::NewLC( EMTPTypeUINT16 ); // + expectedForm

    TUint16 values[] =
        {
        EMTPDrmNoProtection,
        EMTPDrmProtection,
        EMTPDrmReserveForMTP,
        EMTPDrmVenderExtension
        };

    TInt numValues = sizeof( values ) / sizeof( values[0] );
    for ( TInt i = 0; i < numValues; i++ )
        {
        TMTPTypeUint16 data( values[i] );
        expectedForm->AppendSupportedValueL( data );
        }

    CMTPTypeObjectPropDesc* propertyDesc = CMTPTypeObjectPropDesc::NewL( EMTPObjectPropCodeDRMStatus,
        *expectedForm );
    CleanupStack::PopAndDestroy( expectedForm ); // - expectedForm
    PRINT( _L( "MM MTP <= CMediaMtpDataProviderGetFormatCapabilities::ServiceDRMStatusL" ) );

    return propertyDesc;
    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderGetFormatCapabilities::ServiceOMADRMStatusL
// Create Description and create new ObjectPropDesc
// -----------------------------------------------------------------------------
//
CMTPTypeObjectPropDesc* CMediaMtpDataProviderGetFormatCapabilities::ServiceOMADRMStatusL()
    {
    PRINT( _L( "MM MTP => CMediaMtpDataProviderGetFormatCapabilities::ServiceOMADRMStatusL" ) );
    CMTPTypeObjectPropDescEnumerationForm* expectedForm =
        CMTPTypeObjectPropDescEnumerationForm::NewLC( EMTPTypeUINT8 );  // + expectedForm

    TUint8 values[] =
        {
        EMTPOMADrmNoProtection,
        EMTPOMADrmProtection,
        };

    TInt numValues = sizeof( values ) / sizeof( values[0] );
    for ( TInt i = 0; i < numValues; i++ )
        {
        TMTPTypeUint8 data( values[i] );
        expectedForm->AppendSupportedValueL( data );
        }

    CMTPTypeObjectPropDesc::TPropertyInfo propertyInfo;
    propertyInfo.iDataType = EMTPTypeUINT8;
    propertyInfo.iFormFlag = CMTPTypeObjectPropDesc::EEnumerationForm;

    CMTPTypeObjectPropDesc* propertyDesc = CMTPTypeObjectPropDesc::NewL( EMTPExtObjectPropCodeOmaDrmStatus,
        propertyInfo,
        expectedForm );
    CleanupStack::PopAndDestroy( expectedForm ); // - expectedForm
    PRINT( _L( "MM MTP <= CMediaMtpDataProviderGetFormatCapabilities::ServiceOMADRMStatusL" ) );

    return propertyDesc;
    }

// end of file
