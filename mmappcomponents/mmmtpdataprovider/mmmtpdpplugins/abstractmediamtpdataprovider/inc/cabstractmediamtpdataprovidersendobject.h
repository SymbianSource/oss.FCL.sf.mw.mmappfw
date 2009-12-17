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
* Description:   *
*/

#ifndef CABSTRACTMEDIAMTPDATAPROVIDERSENDOBJECT_H_
#define CABSTRACTMEDIAMTPDATAPROVIDERSENDOBJECT_H_

#include "csendobject.h"

/**
* Defines media data provider SetObjectPropsList request processor
*/
class CAbstractMediaMtpDataProviderSendObject: public CSendObject
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
    ~CAbstractMediaMtpDataProviderSendObject();

private:
    /**
    * Standard C++ Constructor
    * @param aFramework, The data provider framework
    * @param aConnection, The connection from which the request comes
    * @param aWrapper Medadata access
    */
    CAbstractMediaMtpDataProviderSendObject( MMTPDataProviderFramework& aFramework,
        MMTPConnection& aConnection,
        MMmMtpDpConfig& aDpConfig );

    /**
    * 2nd Phase Constructor
    */
    void ConstructL();

protected:
    // from CSendObjectInfo
    TMTPResponseCode SetSpecificObjectPropertyL( TUint16 aPropCode,
        const CMTPObjectMetaData& aObject,
        const CMTPTypeObjectPropListElement& aElement );

    TMTPResponseCode CheckSepecificPropType( TUint16 aPropCode, TUint16 aDataType );

    TInt HandleSpecificWrapperError(TInt aError,
            const CMTPObjectMetaData& aObject);

    };

#endif /*CABSTRACTMEDIAMTPDATAPROVIDERSENDOBJECT_H_*/
