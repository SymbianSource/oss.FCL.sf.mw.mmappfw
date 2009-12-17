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
* Description:  Set object references operation.
*
*/


#ifndef CABSTRACTMEDIAMTPDATAPROVIDERSETOBJECTREFERENCES_H
#define CABSTRACTMEDIAMTPDATAPROVIDERSETOBJECTREFERENCES_H

#include "csetobjectreferences.h"

/**
* Defines abstract media data provider SetObjectReferences request processor
*/
class CAbstractMediaMtpDataProviderSetObjectReferences : public CSetObjectReferences
    {
public:
    /**
    * Two-phase construction method
    * @param aFramework    The data provider framework
    * @param aConnection    The connection from which the request comes
    * @param aWrapper    medadata access interface
    * @return a pointer to the created request processor object
    */
    static MMmRequestProcessor* NewL( MMTPDataProviderFramework& aFramework,
        MMTPConnection& aConnection,
        MMmMtpDpConfig& aDpConfig );

    /**
    * Destructor
    */
    ~CAbstractMediaMtpDataProviderSetObjectReferences();

private:
    /**
    * Standard c++ constructor
    * @param aFramework    The data provider framework
    * @param aConnection    The connection from which the request comes
    * @param aWrapper    medadata access interface
    */
    CAbstractMediaMtpDataProviderSetObjectReferences( MMTPDataProviderFramework& aFramework,
        MMTPConnection& aConnection,
        MMmMtpDpConfig& aDpConfig );

    /**
    * two-phase construction
    */
    void ConstructL();

protected:
    // from CSetObjectPropValue
    /**
    * set references to DB
    * @param aWrapper          medadata access interface
    * @param aObjectFormat     the format of the file
    * @param aSrcFileName      file name, from
    * @param aRefFileArray     the array of file names, to
    */
    void DoSetObjectReferencesL( CMmMtpDpMetadataAccessWrapper& aWrapper,
            TUint16 aObjectFormat,
            const TDesC& aSrcFileName,
            CDesCArray& aRefFileArray );

    };

#endif // CABSTRACTMEDIAMTPDATAPROVIDERSETOBJECTREFERENCES_H
