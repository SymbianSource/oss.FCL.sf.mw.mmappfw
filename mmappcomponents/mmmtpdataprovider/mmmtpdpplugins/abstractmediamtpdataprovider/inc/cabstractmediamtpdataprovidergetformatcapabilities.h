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

#ifndef CABSTRACTMEDIAMTPDATAPROVIDERGETFORMATCAPABILITIES_H
#define CABSTRACTMEDIAMTPDATAPROVIDERGETFORMATCAPABILITIES_H

#include "cgetformatcapabilities.h"

// forward declaration
class MMmMtpDpConfig;
class CMTPTypeObjectPropDesc;
class CMTPTypeInterdependentPropDesc;
class CMTPTypeObjectPropDesc;
class CMTPTypeString;
class CGetFormatCapabilities;

/**
* Defines abstract media data provider GetFormatCapabilities
*/
class CAbstractMediaMtpDataProviderGetFormatCapabilities : public CGetFormatCapabilities
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

/*    *
     * @param aFramework, The data provider framework
     * @param aConnection, The connection from which the request comes
     * @param aWrapper Medadata access
     * @return A pointer to the created request processor object

    static MMmRequestProcessor* NewLC( MMTPDataProviderFramework& aFramework,
        MMTPConnection& aConnection,
        MMmMtpDpConfig& aDpConfig );*/

    /**
    * Destructor
    */
    ~CAbstractMediaMtpDataProviderGetFormatCapabilities();

protected:
    // from CGetFormatCapabilities, override from baseclass

    /**
    * Service specific object property
    * @param aPropCode, The prop code
    * @return MTPType object propdesc
    */
    CMTPTypeObjectPropDesc* ServiceSpecificPropertyDescL( TUint16 aPropCode );

    /**
    * Service interdepent propdesc
    * @param aFormatCode, The format code
    * @return MTPTypeInterdependentPropDesc
    */
    void ServiceInterdepentPropDescL();

private:

    /**
    * Standard C++ Constructor
    * @param aFramework, The data provider framework
    * @param aConnection, The connection from which the request comes
    * @param aWrapper Medadata access
    */
    CAbstractMediaMtpDataProviderGetFormatCapabilities( MMTPDataProviderFramework& aFramework,
        MMTPConnection& aConnection,
        MMmMtpDpConfig& aDpConfig );

    /**
    * two-phase construction
    */
    void ConstructL();
    };

#endif // CAbstractMediaMTPDATAPROVIDERGETFORMATCAPABILITIES_H
