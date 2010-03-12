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
* Description:  Media dp specific property description construction utility.
*              Used by all request processors which related to property description
*
*/

#include <mtp/cmtptypeobjectpropdesc.h>
#include <mtp/cmtptypestring.h>

#include "cmediamtpdataproviderdescriptionutility.h"
#include "mediamtpdataproviderconst.h"
#include "tobjectdescription.h"
#include "mmmtpdpconfig.h"
#include "mmmtpdplogger.h"


// -----------------------------------------------------------------------------
// CMediaMtpDataProviderDescriptionUtility::NewL
//
// -----------------------------------------------------------------------------
//
CMediaMtpDataProviderDescriptionUtility* CMediaMtpDataProviderDescriptionUtility::NewL()
    {
    CMediaMtpDataProviderDescriptionUtility* self =
        new( ELeave ) CMediaMtpDataProviderDescriptionUtility();

    return self;
    }

CMediaMtpDataProviderDescriptionUtility::CMediaMtpDataProviderDescriptionUtility()
    {
    // Do nothing
    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderDescriptionUtility::NewInterdepentPropDescL
// Construct media dp specific interdependent property description.
// -----------------------------------------------------------------------------
//
CMTPTypeInterdependentPropDesc* CMediaMtpDataProviderDescriptionUtility::NewInterdepentPropDescL( MMmMtpDpConfig& aDpConfig, TUint aFormatCode )
    {
    PRINT( _L( "MM MTP => CMediaMtpDataProviderDescriptionUtility::NewInterdepentPropDescL" ) );

    TBool audioWaveCodecSupported = EFalse;
    TBool audioBitRateSupported = EFalse;
    TBool videoFourCcCodecSupported = EFalse;
    TBool videoBitRateSupported = EFalse;

    const RArray<TUint>* properties = aDpConfig.GetSupportedPropertiesL( aFormatCode );

    TInt count = properties->Count();
    for ( TInt i = 0; i < count; i++ )
        {
        switch ( ( *properties )[i] )
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

    CMTPTypeInterdependentPropDesc* interdependentPropDesc = CMTPTypeInterdependentPropDesc::NewL();

    if ( audioWaveCodecSupported && audioBitRateSupported )
        {
        CMTPTypeInterdependentProperties* properties = CMTPTypeInterdependentProperties::NewL();
        properties->AppendL( NewAudioWaveCodecPropDescL( aFormatCode ) );
        properties->AppendL( NewAudioBitratePropDescL( aFormatCode ) );
        interdependentPropDesc->AppendL( properties );
        }

    if ( videoFourCcCodecSupported && videoBitRateSupported )
        {
        CMTPTypeInterdependentProperties* properties = CMTPTypeInterdependentProperties::NewL();
        properties->AppendL( NewVideoFourCCCodecPropDescL( aFormatCode ) );
        properties->AppendL( NewVideoBitratePropDescL( aFormatCode ) );
        interdependentPropDesc->AppendL( properties );
        }

    PRINT( _L( "MM MTP <= CMediaMtpDataProviderDescriptionUtility::NewInterdepentPropDescL" ) );
    return interdependentPropDesc;
    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderDescriptionUtility::NewSpecificPropDescL
// Construct media dp specific property description.
// -----------------------------------------------------------------------------
//
CMTPTypeObjectPropDesc* CMediaMtpDataProviderDescriptionUtility::NewSpecificPropDescL( TUint aFormatCode, TUint16 aPropCode )
    {
    CMTPTypeObjectPropDesc* propertyDesc = NULL;

    switch ( aPropCode )
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
            propertyDesc = CMTPTypeObjectPropDesc::NewL( aPropCode );
            break;

        // Number of Channels (m)
        case EMTPObjectPropCodeNumberOfChannels:
            propertyDesc = NewNumberOfChannelsPropDescL();
            break;

        // Sample Rate (HAS MINIMUM AND MAX VALUE)
        case EMTPObjectPropCodeSampleRate:
            propertyDesc = NewCodeSampleRatePropDescL();
            break;

        // Audio Wave Codec (MAY REQUIRED LIMITED NUMBER TO BE PRODUCED)
        case EMTPObjectPropCodeAudioWAVECodec:
            propertyDesc = NewAudioWaveCodecPropDescL( aFormatCode );
            break;

        // Audio Bit Rate (MAY REQUIRED LIMITED NUMBER TO BE PRODUCED)
        case EMTPObjectPropCodeAudioBitRate:
            propertyDesc = NewAudioBitratePropDescL( aFormatCode );
            break;

        // Duration
        case EMTPObjectPropCodeDuration:
            propertyDesc = NewRangeFormDescriptionL( aPropCode,
                EMTPMinDuration,
                EMTPMaxDuration,
                EMTPDurationStep );
            break;

        // Description
        case EMTPObjectPropCodeDescription:
            propertyDesc = NewDescriptionPropDescL();
            break;

        case EMTPObjectPropCodeWidth:
            propertyDesc = NewRangeFormDescriptionL( aPropCode,
                EMTPMinWidth,
                EMTPMaxWidth,
                EMTPStepWidth );
            break;

        case EMTPObjectPropCodeHeight:
            propertyDesc = NewRangeFormDescriptionL( aPropCode,
                EMTPMinHeight,
                EMTPMaxHeight,
                EMTPStepHeight );
            break;

        case EMTPObjectPropCodeScanType:
            propertyDesc = NewScanTypeDescriptionPropDescL();
            break;

        case EMTPObjectPropCodeVideoFourCCCodec:
            propertyDesc = NewVideoFourCCCodecPropDescL( aFormatCode );
            break;

        case EMTPObjectPropCodeVideoBitRate:
            propertyDesc = NewVideoBitratePropDescL( aFormatCode );
            break;

        case EMTPObjectPropCodeFramesPerThousandSeconds:
            propertyDesc = NewRangeFormDescriptionL( aPropCode,
                EMTPMinFramesPerThousandSeconds,
                EMTPMaxFramesPerThousandSeconds,
                EMTPStepFramesPerThousandSeconds );
            break;

        case EMTPObjectPropCodeKeyFrameDistance:
            propertyDesc = NewRangeFormDescriptionL( aPropCode,
                EMTPMinKeyFrameDistance,
                EMTPMaxKeyFrameDistance,
                EMTPStepKeyFrameDistance );
            break;

        case EMTPObjectPropCodeEncodingProfile:
            propertyDesc = NewEncodingProfileDescriptionPropDescL();
            break;

        case EMTPObjectPropCodeDRMStatus:
            propertyDesc = NewDRMStatusPropDescL();
            break;

        case EMTPExtObjectPropCodeOmaDrmStatus:
            propertyDesc = NewOMADRMStatusPropDescL();
            break;

        default:
            // do nothing
            break;
        }
    return propertyDesc;
    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderDescriptionUtility::NewAudioWaveCodecPropDescL
// Construct audio wave codec property description.
// -----------------------------------------------------------------------------
//
CMTPTypeObjectPropDesc* CMediaMtpDataProviderDescriptionUtility::NewAudioWaveCodecPropDescL( TUint aFormatCode )
    {
    CMTPTypeObjectPropDescEnumerationForm* form =
        CMTPTypeObjectPropDescEnumerationForm::NewLC( EMTPTypeUINT32 );    // + form

    switch( aFormatCode )
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

    CMTPTypeObjectPropDesc* propertyDesc = CMTPTypeObjectPropDesc::NewL( EMTPObjectPropCodeAudioWAVECodec, *form );

    CleanupStack::PopAndDestroy( form ); // - form
    PRINT( _L( "MM MTP <= CMediaMtpDataProviderDescriptionUtility::NewAudioWaveCodecPropDescL" ) );

    return propertyDesc;
    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderDescriptionUtility::NewAudioBitratePropDescL
// Construct audio bitrate property description.
// -----------------------------------------------------------------------------
//
CMTPTypeObjectPropDesc* CMediaMtpDataProviderDescriptionUtility::NewAudioBitratePropDescL( TUint aFormatCode )
    {
    PRINT1( _L( "MM MTP => CMediaMtpDataProviderDescriptionUtility::NewAudioBitratePropDescL, iFormatCode = 0x%x" ), aFormatCode );
    CMTPTypeObjectPropDesc* propertyDesc = NULL;
    switch( aFormatCode )
        {
        case EMTPFormatCodeWMA:
        case EMTPFormatCodeWMV:
        case EMTPFormatCodeASF:
            propertyDesc = NewRangeFormDescriptionL( EMTPObjectPropCodeAudioBitRate,
                EMTPWMAMinBitrate,
                EMTPWMAMaxBitrate,
                EMTPAudioBitrateStep );
            break;

        case EMTPFormatCodeMP3:
            propertyDesc = NewRangeFormDescriptionL( EMTPObjectPropCodeAudioBitRate,
                EMTPMP3MinBitrate,
                EMTPMP3MaxBitrate,
                EMTPAudioBitrateStep );
            break;

        case EMTPFormatCodeAAC:
            propertyDesc = NewRangeFormDescriptionL( EMTPObjectPropCodeAudioBitRate,
                EMTPAACMinBitrate,
                EMTPAACMaxBitrate,
                EMTPAudioBitrateStep );
            break;

        case EMTPFormatCodeWAV:
            propertyDesc = NewRangeFormDescriptionL( EMTPObjectPropCodeAudioBitRate,
                EMTPPCMMinBitrate,
                EMTPPCMMaxBitrate,
                EMTPAudioBitrateStep );
            break;

        case EMTPFormatCodeMP4Container:
        case EMTPFormatCode3GPContainer:
            propertyDesc = NewRangeFormDescriptionL( EMTPObjectPropCodeAudioBitRate,
                EMTPAACPlusMinBitrate,
                EMTPAACPlusMaxBitrate,
                EMTPAudioBitrateStep );
            break;

        default:
            // do nothing
            break;
        }
    PRINT( _L( "MM MTP <= CMediaMtpDataProviderDescriptionUtility::NewAudioBitratePropDescL" ) );

    return propertyDesc;
    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderDescriptionUtility::NewVideoFourCCCodecPropDescL
// Construct property description of video fourCC codec.
// -----------------------------------------------------------------------------
//
CMTPTypeObjectPropDesc* CMediaMtpDataProviderDescriptionUtility::NewVideoFourCCCodecPropDescL( TUint aFormatCode )
    {
    PRINT( _L( "MM MTP => CMediaMtpDataProviderDescriptionUtility::NewVideoFourCCCodecPropDescL" ) );

    CMTPTypeObjectPropDesc* propertyDesc = NULL;

    if ( ( EMTPFormatCodeWMV == aFormatCode )
        || ( EMTPFormatCodeASF == aFormatCode ) )
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

        propertyDesc = CMTPTypeObjectPropDesc::NewL( EMTPObjectPropCodeVideoFourCCCodec,
            *expectedForm );

        CleanupStack::PopAndDestroy( expectedForm ); // - expectedForm
        }

    PRINT( _L( "MM MTP <= CMediaMtpDataProviderDescriptionUtility::NewVideoFourCCCodecPropDescL" ) );
    return propertyDesc;
    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderDescriptionUtility::NewVideoBitratePropDescL
// Construct property description of VideoBitrate.
// -----------------------------------------------------------------------------
//
CMTPTypeObjectPropDesc* CMediaMtpDataProviderDescriptionUtility::NewVideoBitratePropDescL( TUint aFormatCode )
    {
    PRINT( _L( "MM MTP => CMediaMtpDataProviderDescriptionUtility::NewVideoBitratePropDescL" ) );

    CMTPTypeObjectPropDesc* propertyDesc = NULL;

    if ( ( EMTPFormatCodeWMV == aFormatCode )
        || ( EMTPFormatCodeASF == aFormatCode ) )
        {
        propertyDesc = NewRangeFormDescriptionL( EMTPObjectPropCodeVideoBitRate,
            EMTPWMVMinBitrate,
            EMTPWMVMaxBitrate,
            EMTPVideoBitrateStep,
            ETrue );
        }

    PRINT( _L( "MM MTP <= MmMtpDpDescriptionUtiliNewviceVideoBitratePropDescL" ) );

    return propertyDesc;
    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderDescriptionUtility::NewNumberOfChannelsPropDescL()
// Construct property description of number of channels.
// -----------------------------------------------------------------------------
//
CMTPTypeObjectPropDesc* CMediaMtpDataProviderDescriptionUtility::NewNumberOfChannelsPropDescL()
    {
    PRINT( _L( "MM MTP => CMediaMtpDataProviderDescriptionUtility::NewNumberOfChannelsPropDescL" ) );

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

    PRINT( _L( "MM MTP <= CMediaMtpDataProviderDescriptionUtility::NewNumberOfChannelsPropDescL" ) );

    return propertyDesc;
    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderDescriptionUtility::NewCodeSampleRatePropDescL
// Construct property description of code sample rate.
// -----------------------------------------------------------------------------
//
CMTPTypeObjectPropDesc* CMediaMtpDataProviderDescriptionUtility::NewCodeSampleRatePropDescL()
    {
    PRINT( _L( "MM MTP => CMediaMtpDataProviderDescriptionUtility::NewCodeSampleRatePropDescL" ) );

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

    PRINT( _L( "MM MTP <= CMediaMtpDataProviderDescriptionUtility::NewCodeSampleRatePropDescL" ) );

    return propertyDesc;
    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderDescriptionUtility::NewDescriptionPropDescL()
// Construct property description of description.
// -----------------------------------------------------------------------------
//
CMTPTypeObjectPropDesc* CMediaMtpDataProviderDescriptionUtility::NewDescriptionPropDescL()
    {
    PRINT( _L( "MM MTP => CMediaMtpDataProviderDescriptionUtility::NewDescriptionPropDescL" ) );

    TMTPTypeUint32 uint32Data( KMTPMaxDescriptionLen );
    CMTPTypeObjectPropDesc* propertyDesc = CMTPTypeObjectPropDesc::NewL( EMTPObjectPropCodeDescription,
        CMTPTypeObjectPropDesc::ELongStringForm,
        &uint32Data );

    PRINT( _L( "MM MTP <= CMediaMtpDataProviderDescriptionUtility::NewDescriptionPropDescL" ) );

    return propertyDesc;
    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderDescriptionUtility::NewScanTypeDescriptionPropDescL
// Construct property description of scan type description.
// -----------------------------------------------------------------------------
//
CMTPTypeObjectPropDesc* CMediaMtpDataProviderDescriptionUtility::NewScanTypeDescriptionPropDescL()
    {
    PRINT( _L( "MM MTP => CMediaMtpDataProviderDescriptionUtility::NewScanTypeDescriptionPropDescL" ) );

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

    PRINT( _L( "MM MTP <= CMediaMtpDataProviderDescriptionUtility::NewScanTypeDescriptionPropDescL" ) );

    return propertyDesc;
    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderDescriptionUtility::NewEncodingProfileDescriptionPropDescL
// Construct property description of encoding profile description.
// -----------------------------------------------------------------------------
//
CMTPTypeObjectPropDesc* CMediaMtpDataProviderDescriptionUtility::NewEncodingProfileDescriptionPropDescL()
    {
    PRINT( _L( "MM MTP => CMediaMtpDataProviderDescriptionUtility::NewEncodingProfileDescriptionPropDescL" ) );

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

    PRINT( _L( "MM MTP <= CMediaMtpDataProviderDescriptionUtility::NewEncodingProfileDescriptionPropDescL" ) );

    return propertyDesc;
    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderDescriptionUtility::NewDRMStatusPropDescL
// Construct property description of DRM status.
// -----------------------------------------------------------------------------
//
CMTPTypeObjectPropDesc* CMediaMtpDataProviderDescriptionUtility::NewDRMStatusPropDescL()
    {
    PRINT( _L( "MM MTP => CMediaMtpDataProviderDescriptionUtility::NewDRMStatusPropDescL" ) );

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

    PRINT( _L( "MM MTP <= CMediaMtpDataProviderDescriptionUtility::NewDRMStatusPropDescL" ) );

    return propertyDesc;
    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderDescriptionUtility::NewOMADRMStatusPropDescL
// Construct property description of OMADRM status.
// -----------------------------------------------------------------------------
//
CMTPTypeObjectPropDesc* CMediaMtpDataProviderDescriptionUtility::NewOMADRMStatusPropDescL()
    {
    PRINT( _L( "MM MTP => CMediaMtpDataProviderGetFormatCapabilities::NewOMADRMStatusPropDescL" ) );

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

    PRINT( _L( "MM MTP <= CMediaMtpDataProviderGetFormatCapabilities::NewOMADRMStatusPropDescL" ) );

    return propertyDesc;
    }

// end of file
