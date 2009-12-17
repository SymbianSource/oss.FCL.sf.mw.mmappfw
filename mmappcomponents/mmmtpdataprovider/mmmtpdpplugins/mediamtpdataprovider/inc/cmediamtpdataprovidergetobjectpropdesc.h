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


#ifndef CMEDIAMTPDATAPROVIDERGETOBJECTPROPDESC_H
#define CMEDIAMTPDATAPROVIDERGETOBJECTPROPDESC_H

#include "cgetobjectpropdesc.h"

/**
* Defines media data provider GetObjectPropDesc request processor
*/
class CMediaMtpDataProviderGetObjectPropDesc: public CGetObjectPropDesc
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
    ~CMediaMtpDataProviderGetObjectPropDesc();

protected:
    // from CGetObjectPropDesc, override from baseclass
    void ServiceSpecificObjectPropertyL( TUint16 aPropCode );

private:

    /**
    * Standard C++ Constructor
    * @param aFramework, The data provider framework
    * @param aConnection, The connection from which the request comes
    * @param aWrapper Medadata access
    */
    CMediaMtpDataProviderGetObjectPropDesc( MMTPDataProviderFramework& aFramework,
        MMTPConnection& aConnection,
        MMmMtpDpConfig& aDpConfig );
    /**
    * two-phase construction
    */
    void ConstructL();

    void ServiceNumberOfChannelsL();
    void ServiceCodeSampleRateL();
    void ServiceCodeWaveCodecL();
    void ServiceCodeAudioBitrateL();
    void ServiceDurationL();
    void ServiceDescriptionL();
    void ServiceScanTypeDescriptionL();
    void ServiceVideoFourCCCodecL();
    void ServiceVideoBitrateL();
    void ServiceEncodingProfileDescriptionL();
    void ServiceDRMStatusL();
    void ServiceOMADRMStatusL();

    void ServiceRangeFormDescriptionL( TUint16 aPropCode,
        TUint32 aMinValue,
        TUint32 aMaxValue,
        TUint32 aStepValue,
        TBool aIsReadOnly = EFalse );

    };

#endif // CMEDIAMTPDATAPROVIDERGETOBJECTPROPDESC_H
