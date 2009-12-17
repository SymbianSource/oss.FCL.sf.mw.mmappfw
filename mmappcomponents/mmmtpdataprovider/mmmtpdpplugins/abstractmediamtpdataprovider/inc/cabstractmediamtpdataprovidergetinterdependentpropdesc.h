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


#ifndef CABSTRACTMEDIAMTPDATAPROVIDERGETINTERDEPENTENTPROPDESC_H
#define CABSTRACTMEDIAMTPDATAPROVIDERGETINTERDEPENTENTPROPDESC_H

#include "crequestprocessor.h"

class CMTPTypeInterdependentPropDesc;
class MMmMtpDpConfig;

/**
* Defines abstract media data provider GetInterdependentPropDesc request processor
*/
class CAbstractMediaMtpDataProviderGetInterdependentPropDesc: public CRequestProcessor
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
    ~CAbstractMediaMtpDataProviderGetInterdependentPropDesc();

private:
    /**
    * Standard C++ Constructor
    * @param aFramework, The data provider framework
    * @param aConnection, The connection from which the request comes
    */
    CAbstractMediaMtpDataProviderGetInterdependentPropDesc( MMTPDataProviderFramework& aFramework,
        MMTPConnection& aConnection );

    /**
    * two-phase construction
    */
    void ConstructL();

protected:
    // from CRequestProcessor
    TMTPResponseCode CheckRequestL();

    void ServiceL();

private:
    void ServiceAudioWaveCodecL();

    void ServiceAudioBitrateL();

    void SetFormForResponseL( TUint16 aPropertyCode, const MMTPType& aForm );

private:
    /** Dataset to store the prop descriptions*/
    CMTPTypeInterdependentPropDesc* iDataset;

    TUint32 iFormatCode;

    };

#endif // CABSTRACTMEDIAMTPDATAPROVIDERGETINTERDEPENTENTPROPDESC_H
