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
* Description:  Implement the operation: GetInterdenpendentPropDesc
*
*/


#include "cmediamtpdataprovidergetinterdependentpropdesc.h"
#include "mediamtpdataproviderconst.h"
#include "mmmtpdplogger.h"
#include "tobjectdescription.h"
#include "mmmtpdpdefs.h"
#include "mmmtpdpconfig.h"

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderGetInterDependentPropDesc::NewL
// Two-phase construction method
// -----------------------------------------------------------------------------
//
MMmRequestProcessor* CMediaMtpDataProviderGetInterDependentPropDesc::NewL( MMTPDataProviderFramework& aFramework,
    MMTPConnection& aConnection,
    MMmMtpDpConfig& aDpConfig )
    {
    CMediaMtpDataProviderGetInterDependentPropDesc* self =
        new ( ELeave ) CMediaMtpDataProviderGetInterDependentPropDesc( aFramework,
            aConnection );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CMediaDpMtpGetInterDependentPropDesc::ConstructL
// Two-phase construction method
// -----------------------------------------------------------------------------
//
void CMediaMtpDataProviderGetInterDependentPropDesc::ConstructL()
    {

    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderGetInterDependentPropDesc::~CMediaMtpDataProviderGetInterDependentPropDesc
// Destructor
// -----------------------------------------------------------------------------
//
CMediaMtpDataProviderGetInterDependentPropDesc::~CMediaMtpDataProviderGetInterDependentPropDesc()
    {
    delete iDataset;
    delete iProperties;
    }

// -----------------------------------------------------------------------------
// CMTPGetInterDependentPropDesc::CMTPGetInterDependentPropDesc
// Standard c++ constructor
// -----------------------------------------------------------------------------
//
CMediaMtpDataProviderGetInterDependentPropDesc::CMediaMtpDataProviderGetInterDependentPropDesc( MMTPDataProviderFramework& aFramework,
    MMTPConnection& aConnection ) :
    CRequestProcessor( aFramework, aConnection, 0, NULL )
    {
    PRINT( _L( "Operation: GetInterdependentPropDesc(0x9807)" ) );
    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderGetInterDependentPropDesc::CheckRequestL
// Verify the reqeust and returns it
// -----------------------------------------------------------------------------
//
TMTPResponseCode CMediaMtpDataProviderGetInterDependentPropDesc::CheckRequestL()
    {
    TMTPResponseCode responseCode = CRequestProcessor::CheckRequestL();
    iFormatCode = Request().Uint32( TMTPTypeRequest::ERequestParameter1 );

    if ( EMTPRespCodeOK == responseCode )
        {
        TInt count = sizeof( KMediaMtpDataProviderSupportedFormats ) / sizeof( TUint16 );

        responseCode = EMTPRespCodeInvalidObjectFormatCode;

        for ( TInt i = 0; i < count; i++ )
            {
            if ( iFormatCode == KMediaMtpDataProviderSupportedFormats[i] )
                {
                responseCode = EMTPRespCodeOK;
                break;
                }
            }
        }

    return responseCode;
    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderGetInterDependentPropDesc::ServiceL
// service a request at request phase
// -----------------------------------------------------------------------------
//
void CMediaMtpDataProviderGetInterDependentPropDesc::ServiceL()
    {
    PRINT( _L( "MM MTP => CMediaMtpDataProviderGetInterDependentPropDesc::ServiceL" ) );

    // Clear the data set.
    delete iDataset;
    iDataset = NULL;
    iDataset = CMTPTypeInterdependentPropDesc::NewL();

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
                        PRINT( _L( "MM MTP <> CMediaMtpDataProviderGetInterDependentPropDesc::ServiceL default" ) );
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
                switch (KMmMtpDpSupportedPropMandatoryWMV[i] )
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
            PRINT1( _L( "MM MTP <> CMediaMtpDataProviderGetInterDependentPropDesc::ServiceL leave because of invalid formatcode = 0x%x" ), iFormatCode );
            User::Leave( KErrNotSupported );
            }
            break;
        }

    if ( audioWaveCodecSupported && audioBitRateSupported )
        {
        if ( videoFourCcCodecSupported && videoBitRateSupported )
            {
            iProperties = CMTPTypeInterdependentProperties::NewL();
            ServiceVideoFourCCCodecL();
            ServiceVideoBitrateL();
            iDataset->AppendL( iProperties );
//            CleanupStack::Pop();
            iProperties = NULL;
            }

        iProperties = CMTPTypeInterdependentProperties::NewL();
        ServiceAudioWaveCodecL();
        ServiceAudioBitrateL();
        iDataset->AppendL( iProperties );
//        CleanupStack::Pop();
        iProperties = NULL;
        }

    // Send the dataset.
    SendDataL( *iDataset );
    PRINT( _L( "MM MTP <= CMediaMtpDataProviderGetInterDependentPropDesc::ServiceL" ) );
    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderGetInterDependentPropDesc::ServiceAudioWaveCodecL
//
// -----------------------------------------------------------------------------
//
void CMediaMtpDataProviderGetInterDependentPropDesc::ServiceAudioWaveCodecL()
    {
    CMTPTypeObjectPropDescEnumerationForm* form =
        CMTPTypeObjectPropDescEnumerationForm::NewLC( EMTPTypeUINT32 );

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
            PRINT1( _L( "MM MTP <> CMediaMtpDataProviderGetInterDependentPropDesc::ServiceAudioWaveCodecL leave because of invalid formatcode = 0x%x" ), iFormatCode );
            User::Leave( KErrNotSupported );
            break;
        }

    CMTPTypeObjectPropDesc::TPropertyInfo propInfo;
    propInfo.iDataType = EMTPTypeUINT32;
    propInfo.iFormFlag = CMTPTypeObjectPropDesc::EEnumerationForm;
    propInfo.iGetSet = CMTPTypeObjectPropDesc::EReadWrite;
    SetFormForResponseL( EMTPObjectPropCodeAudioWAVECodec, propInfo, form );
    CleanupStack::PopAndDestroy( form );
    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderGetInterDependentPropDesc::ServiceAudioBitrateL
//
// -----------------------------------------------------------------------------
//
void CMediaMtpDataProviderGetInterDependentPropDesc::ServiceAudioBitrateL()
    {
    switch( iFormatCode )
        {
        case EMTPFormatCodeWMA:
        case EMTPFormatCodeWMV:
        case EMTPFormatCodeASF:
            ServiceRangeFormDescriptionL( EMTPObjectPropCodeAudioBitRate,
                EMTPWMAMinBitrate,
                EMTPWMAMaxBitrate,
                EMTPAudioBitrateStep );
            break;

        case EMTPFormatCodeMP3:
            ServiceRangeFormDescriptionL( EMTPObjectPropCodeAudioBitRate,
                EMTPMP3MinBitrate,
                EMTPMP3MaxBitrate,
                EMTPAudioBitrateStep );
            break;

        case EMTPFormatCodeAAC:
            ServiceRangeFormDescriptionL( EMTPObjectPropCodeAudioBitRate,
                EMTPAACMinBitrate,
                EMTPAACMaxBitrate,
                EMTPAudioBitrateStep );
            break;

        case EMTPFormatCodeWAV:
            ServiceRangeFormDescriptionL( EMTPObjectPropCodeAudioBitRate,
                EMTPPCMMinBitrate,
                EMTPPCMMaxBitrate,
                EMTPAudioBitrateStep );
            break;

        case EMTPFormatCodeMP4Container:
        case EMTPFormatCode3GPContainer:
            ServiceRangeFormDescriptionL( EMTPObjectPropCodeAudioBitRate,
                EMTPAACPlusMinBitrate,
                EMTPAACPlusMaxBitrate,
                EMTPAudioBitrateStep );
            break;

        default:
            PRINT1( _L( "MM MTP <> CMediaMtpDataProviderGetInterDependentPropDesc::ServiceAudioBitrateL leave because of invalid formatcode = 0x%x" ), iFormatCode );
            User::Leave( KErrNotSupported );
            break;
        }
    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderGetInterDependentPropDesc::ServiceVideoFourCCCodecL
// Create Description and create new ObjectPropDesc
// -----------------------------------------------------------------------------
//
void CMediaMtpDataProviderGetInterDependentPropDesc::ServiceVideoFourCCCodecL()
    {
    switch( iFormatCode )
        {
        case EMTPFormatCodeWMV:
        case EMTPFormatCodeASF:
            {
            CMTPTypeObjectPropDescEnumerationForm* form =
                CMTPTypeObjectPropDescEnumerationForm::NewL( EMTPTypeUINT32 );
            CleanupStack::PushL( form ); // + form

            TUint32 values[] =
                {
                EMTPVideoFourCCCodecWMV3
                };

            TInt numValues = sizeof ( values ) / sizeof ( values[0] ) ;
            for ( TInt i = 0; i < numValues; i++ )
                {
                TMTPTypeUint32 data( values[i] );
                form->AppendSupportedValueL( data );
                }
            // TODO:
            CMTPTypeObjectPropDesc::TPropertyInfo propInfo;
            propInfo.iDataType = EMTPTypeUINT32;
            propInfo.iFormFlag = CMTPTypeObjectPropDesc::EEnumerationForm;
            propInfo.iGetSet = CMTPTypeObjectPropDesc::EReadWrite;
            SetFormForResponseL( EMTPObjectPropCodeVideoFourCCCodec, propInfo, form );
            CleanupStack::PopAndDestroy( form ); // - form
            }
            break;

        default:
            {
            PRINT1( _L( "MM MTP <> CMediaMtpDataProviderGetInterDependentPropDesc::ServiceVideoFourCCCodecL leave because of invalid formatcode = 0x%x" ), iFormatCode );
            User::Leave( KErrNotSupported );
            }
            break;
        }
    }

void CMediaMtpDataProviderGetInterDependentPropDesc::ServiceVideoBitrateL()
    {
    switch( iFormatCode )
        {
        case EMTPFormatCodeWMV:
        case EMTPFormatCodeASF:
            ServiceRangeFormDescriptionL( EMTPObjectPropCodeVideoBitRate,
                EMTPWMVMinBitrate,
                EMTPWMVMaxBitrate,
                EMTPVideoBitrateStep,
                ETrue );
            break;

        default:
            PRINT1( _L( "MM MTP <> CMediaMtpDataProviderGetInterDependentPropDesc::ServiceVideoBitrateL leave because of invalid formatcode = 0x%x" ), iFormatCode );
            User::Leave( KErrNotSupported );
            break;
        }
    }

void CMediaMtpDataProviderGetInterDependentPropDesc::ServiceRangeFormDescriptionL( TUint16 aPropCode,
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

    CMTPTypeObjectPropDesc::TPropertyInfo propInfo;
    propInfo.iDataType = EMTPTypeUINT32;
    propInfo.iFormFlag = CMTPTypeObjectPropDesc::ERangeForm;
    propInfo.iGetSet = !aIsReadOnly;
    SetFormForResponseL( aPropCode, propInfo, form );
    CleanupStack::PopAndDestroy( form ); // - form
    }

void CMediaMtpDataProviderGetInterDependentPropDesc::SetFormForResponseL( TUint16 aPropertyCode,
    CMTPTypeObjectPropDesc::TPropertyInfo& aPropInfo,
    const MMTPType* aForm )
    {
    CMTPTypeObjectPropDesc* propertyDesc = CMTPTypeObjectPropDesc::NewLC( aPropertyCode,
        aPropInfo,
        aForm );
    // Set group code
    propertyDesc->SetUint32L( CMTPTypeObjectPropDesc::EGroupCode, EGroupCodeMediaDB );

    iProperties->AppendL( propertyDesc );
    CleanupStack::Pop( propertyDesc );
    }

// end of file
