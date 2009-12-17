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
* Description:  Get object references.
*
*/


#ifndef CGETOBJECTREFERENCES_H
#define CGETOBJECTREFERENCES_H

#include "crequestprocessor.h"

// forward declaration
class CMTPTypeArray;
class MMmMtpDpConfig;

/**
* Defines GetObjectReferences request processor
*/
class CGetObjectReferences : public CRequestProcessor
    {
public:
    /**
    * Two-phase construction method
    * @param aFramework    The data provider framework
    * @param aConnection   The connection from which the request comes
    * @param aWrapper      Medadata access interface
    * @return a pointer to the created request processor object
    */
    IMPORT_C static MMmRequestProcessor* NewL( MMTPDataProviderFramework& aFramework,
        MMTPConnection& aConnection,
        MMmMtpDpConfig& aDpConfig );

    /**
    * Destructor
    */
    IMPORT_C ~CGetObjectReferences();

protected:
    // From CRequestProcessor
    /**
    * GetObjectReferences request handler
    */
    void ServiceL();

private:
    /**
    * Standard c++ constructor
    * @param aFramework    The data provider framework
    * @param aConnection   The connection from which the request comes
    * @param aWrapper      Medadata access interface
    */
    CGetObjectReferences( MMTPDataProviderFramework& aFramework,
        MMTPConnection& aConnection );

    /**
    * Two-phase construction
    */
    void ConstructL();

private:
    // Array object to store object references
    CMTPTypeArray* iReferences;

    };

#endif // CGETOBJECTREFERENCES_H
