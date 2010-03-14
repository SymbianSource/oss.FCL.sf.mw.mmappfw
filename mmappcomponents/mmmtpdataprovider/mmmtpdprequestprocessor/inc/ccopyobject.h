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
* Description:  Copy object operation
*
*/


#ifndef CCOPYOBJECT_H
#define CCOPYOBJECT_H

#include "crequestprocessor.h"

class RFs;
class CMTPObjectMetaData;
class CMTPTypeObjectPropList;
class CMTPTypeObjectPropListElement;
class MMmMtpDpConfig;

/**
* Defines CopyObject request processor
*/
class CCopyObject : public CRequestProcessor
    {
public:
    /**
    * Two-phase construction method
    * @param aFramework    The data provider framework
    * @param aConnection    The connection from which the request comes
    * @param aWrapper    medadata access interface
    * @return a pointer to the created request processor object
    */
    //IMPORT_C static MMmRequestProcessor* NewL( MMTPDataProviderFramework& aFramework,
    //        MMTPConnection& aConnection,
    //        CMmMtpDpMetadataAccessWrapper& aWrapper );

    /**
    * Destructor
    */
    IMPORT_C virtual ~CCopyObject();

protected:
    /**
    * Standard c++ constructor
    */
    IMPORT_C CCopyObject( MMTPDataProviderFramework& aFramework,
            MMTPConnection& aConnection,
            MMmMtpDpConfig& aDpConfig );

    /**
    * Second phase constructor
    */
    IMPORT_C void ConstructL();

protected:
    // from CMTPRequestProcessor
    /**
    * CopyObject request handler
    */
    IMPORT_C void ServiceL();

    virtual void ServiceGetSpecificObjectPropertyL( TUint16 aPropCode,
        TUint32 aHandle,
        const CMTPObjectMetaData& aObjectMetaData ) = 0;

private:
    /**
    * Retrieve the parameters of the request
    */
    void GetParametersL();

    void SetDefaultParentObjectL();

    /**
    * Copy object operation
    */
    void CopyObjectL();

    /**
    * Check if we can copy the file to the new location
    */
    TMTPResponseCode CanCopyObjectL( const TDesC& aOldName,
        const TDesC& aNewName ) const;

    /**
    * Save the object properties before doing the copy
    */
    void GetPreviousPropertiesL( const CMTPObjectMetaData& aObject );

    /**
    * Set the object properties after doing the copy
    */
    void SetPreviousPropertiesL( const CMTPObjectMetaData& aObject );

    /**
    * A helper function of CopyObjectL.
    * @param aNewFileName the new full filename after copy.
    * @return objectHandle of new copy of object.
    */
    TUint32 CopyFileL( const TDesC& aNewFileName );

    /**
    * Update object info in the database.
    */
    TUint32 AddObjectToStoreL( const TDesC& aOldObjectName,
        const TDesC& aNewObjectName );

protected:
    CMTPTypeObjectPropListElement* iPropertyElement;
    MMmMtpDpConfig& iDpConfig;
    CMTPTypeObjectPropList* iPropertyList;

private:
    TUint32 iStorageId;
    CMTPObjectMetaData* iObjectInfo; // Not owned
    HBufC* iDest;
    TUint32 iNewParentHandle;
    TTime iPreviousModifiedTime;

    };

#endif // CCOPYOBJECT_H
