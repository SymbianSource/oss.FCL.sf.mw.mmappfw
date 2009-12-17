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


#ifndef CABSTRACTMEDIAMTPDATAPROVIDERGETOBJECTPROPDESC_H
#define CABSTRACTMEDIAMTPDATAPROVIDERGETOBJECTPROPDESC_H

#include "cgetobjectpropdesc.h"

/**
* Defines abstract media data provider GetObjectPropDesc request processor
*/
class CAbstractMediaMtpDataProviderGetObjectPropDesc: public CGetObjectPropDesc
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
    ~CAbstractMediaMtpDataProviderGetObjectPropDesc();

private:
    /**
    * Standard C++ Constructor
    * @param aFramework, The data provider framework
    * @param aConnection, The connection from which the request comes
    * @param aWrapper Medadata access
    */
    CAbstractMediaMtpDataProviderGetObjectPropDesc( MMTPDataProviderFramework& aFramework,
        MMTPConnection& aConnection,
        MMmMtpDpConfig& aDpConfig );

    /**
    * second-phase construction
    */
    void ConstructL();

protected:
    // from CGetObjectPropDesc
    void ServiceSpecificObjectPropertyL( TUint16 aPropCode );

    };

#endif // CABSTRACTMEDIAMTPDATAPROVIDERGETOBJECTPROPDESC_H
