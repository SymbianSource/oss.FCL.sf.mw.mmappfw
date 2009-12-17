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


#ifndef CMEDIAMTPDATAPROVIDERGETINTERDEPENTENTPROPDESC_H
#define CMEDIAMTPDATAPROVIDERGETINTERDEPENTENTPROPDESC_H

#include <mtp/cmtptypeobjectpropdesc.h>
#include "crequestprocessor.h"

class CMTPTypeInterdependentPropDesc;
class CMTPTypeInterdependentProperties;
class MMmMtpDpConfig;

/**
* Defines media data provider GetInterDependentPropDesc request processor
*/
class CMediaMtpDataProviderGetInterDependentPropDesc: public CRequestProcessor
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
    ~CMediaMtpDataProviderGetInterDependentPropDesc();

private:
    /**
    * Standard C++ Constructor
    * @param aFramework, The data provider framework
    * @param aConnection, The connection from which the request comes
    */
    CMediaMtpDataProviderGetInterDependentPropDesc( MMTPDataProviderFramework& aFramework,
        MMTPConnection& aConnection );

protected:
    // from CRequestProcessor
    TMTPResponseCode CheckRequestL();
    void ServiceL();

private:
    /**
    * two-phase construction
    */
    void ConstructL();

    void ServiceAudioWaveCodecL();
    void ServiceAudioBitrateL();

    void ServiceVideoFourCCCodecL();
    void ServiceVideoBitrateL();

    void ServiceRangeFormDescriptionL( TUint16 aPropCode,
        TUint32 aMinValue,
        TUint32 aMaxValue,
        TUint32 aStepValue,
        TBool aIsReadOnly = EFalse);

    void SetFormForResponseL( TUint16 aPropertyCode,
                CMTPTypeObjectPropDesc::TPropertyInfo& aPropInfo,
                const MMTPType* aForm );

private:
    /** Dataset to store the prop descriptions*/
    CMTPTypeInterdependentPropDesc* iDataset;

    /**support configuration correctly*/
    CMTPTypeInterdependentProperties* iProperties;

    TUint32 iFormatCode;

    };

#endif // CMEDIAMTPDATAPROVIDERGETINTERDEPENTENTPROPDESC_H
