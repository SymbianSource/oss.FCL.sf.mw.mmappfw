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
#ifndef CMEDIAMTPDATAPROVIDERGETFORMATCAPABILITIES_H
#define CMEDIAMTPDATAPROVIDERGETFORMATCAPABILITIES_H

#include "cgetformatcapabilities.h"

class CMTPTypeObjectPropDesc;
/**
* Defines media data provider GetFormatCapabilities
*/
class CMediaMtpDataProviderGetFormatCapabilities : public CGetFormatCapabilities
    {
public:

    /**
    * @param aFramework, The data provider framework
    * @param aConnection, The connection from which the request comes
    * @param aWrapper Medadata access
    * @return A pointer to the created request processor object
    */
    static MMmRequestProcessor* NewL( MMTPDataProviderFramework& aFramework,
        MMTPConnection& aConnection,
        MMmMtpDpConfig& aDpConfig );

    /**
    * Destructor
    */
    ~CMediaMtpDataProviderGetFormatCapabilities();

protected:
    // from CGetFormatCapabilities, override from baseclass
    /**
    * Service interdepent propdesc
    */
    void ServiceInterdepentPropDescL();

    /**
    * Service specific object property
    * @param aPropCode, The prop code
    * @return MTPType object propdesc
    */
    CMTPTypeObjectPropDesc* ServiceSpecificPropertyDescL( TUint16 aPropCode );

private:
    /**
    * Standard C++ Constructor
    * @param aFramework, The data provider framework
    * @param aConnection, The connection from which the request comes
    * @param aWrapper Medadata access
    */
    CMediaMtpDataProviderGetFormatCapabilities( MMTPDataProviderFramework& aFramework,
        MMTPConnection& aConnection,
        MMmMtpDpConfig& aDpConfig );

    /**
    * two-phase construction
    */
    void ConstructL();

    //functions used by ServiceInterdepentPropDesc
    /**
    * Service AudioWaveCodec
    * @param aFormatCode, The format code
    * @return MTPTypeInterdependentProperties
    */
    CMTPTypeObjectPropDesc* ServiceAudioWaveCodecL();

    /**
    * Service AudioBitrate
    * @param aFormatCode, The format code
    * @return MTPTypeInterdependentProperties
    */
    CMTPTypeObjectPropDesc* ServiceAudioBitrateL();

    /**
    * Service videoFourCCCodec
    * @param aFormatCode, The format code
    * @return MTPTypeInterdependentProperties
    */
    CMTPTypeObjectPropDesc* ServiceVideoFourCCCodecL();

    /**
    * Service VideoBitrate
    * @param aFormatCode, The format code
    * @return MTPTypeInterdependentProperties
    */
    CMTPTypeObjectPropDesc* ServiceVideoBitrateL();

    /**
    * Service RangeForm Description
    * @param aMinValue The minimal value
    * @param aMaxValue The maximal value
    * @param aStepValue The step value
    * @return MTPTypeObjectPropDescRangeForm
    */
    CMTPTypeObjectPropDesc* ServiceRangeFormDescriptionL( TUint16 aPropCode,
        TUint32 aMinValue,
        TUint32 aMaxValue,
        TUint32 aStepValue,
        TBool aIsReadOnly = EFalse );

    //functions used by ServiceSpecificObjectPropertyL
    /**
    * Service number of channels
    * @return MTPTypeObjectPropDesc
    */
    CMTPTypeObjectPropDesc* ServiceNumberOfChannelsL();

    /**
    * Service code sample rate
    * @return MTPTypeObjectPropDesc
    */
    CMTPTypeObjectPropDesc* ServiceCodeSampleRateL();

    /**
    * Service description
    * @return MTPTypeObjectPropDesc
    */
    CMTPTypeObjectPropDesc* ServiceDescriptionL();

    /**
    * Service scan type description
    * @return MTPTypeObjectPropDesc
    */
    CMTPTypeObjectPropDesc* ServiceScanTypeDescriptionL();

    /**
    * Service encoding profile description
    * @return MTPTypeObjectPropDesc
    */
    CMTPTypeObjectPropDesc* ServiceEncodingProfileDescriptionL();

    /**
    * Service DRM status
    * @return MTPTypeObjectPropDesc
    */
    CMTPTypeObjectPropDesc* ServiceDRMStatusL();

    /**
    * Service OMADRM status
    * @return MTPTypeObjectPropDesc
    */
    CMTPTypeObjectPropDesc* ServiceOMADRMStatusL();


    };

#endif // CMEDIAMTPDATAPROVIDERGETFORMATCAPABILITIES_H
