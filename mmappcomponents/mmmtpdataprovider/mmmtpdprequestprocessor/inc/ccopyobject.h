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
class CFileMan;
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

    /**
    * Set MetaData to CMetadataAccessWrapper, for internal use
    * @param aPropCode, specify property code of aMediaProp
    * @param aNewData, object property value which will be get from
    *    aObjectMetaData
    * @param aObjectMetaData, owner of the property which should be
    *    inserted or updated into database
    * @return response code
    */
    IMPORT_C TMTPResponseCode ServiceMetaDataToWrapper( const TUint16 aPropCode,
            MMTPType& aNewData,
            const CMTPObjectMetaData& aObject );

    virtual void ServiceGetSpecificObjectPropertyL( TUint16 aPropCode,
        TUint32 aHandle,
        const CMTPObjectMetaData& aObjectMetaData ) = 0;

    virtual TMTPResponseCode ServiceSetSpecificObjectPropertyL( TUint16 aPropCode,
        const CMTPObjectMetaData& aObject,
        const CMTPTypeObjectPropListElement& aElement ) = 0;

    // from CActive
    IMPORT_C void RunL();

    IMPORT_C TInt RunError( TInt aError );

private:
    /**
    * Retrieve the parameters of the request
    */
    void GetParametersL();

    void SetDefaultParentObjectL();

    /**
    * Copy object operation
    */
    void CopyObjectL( TUint32& aNewHandle );

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

    /*
    * Generate the list of handles that need to be copied to the new location.
    */
    void GenerateObjectHandleListL( TUint32 aParentHandle );

    /**
    * A helper function of CopyObjectL.
    * @param aNewFolderName the new full file folder name after copy.
    * @return objecthandle of new copy of the folder.
    */
    TUint32 CopyFolderL( const TDesC& aNewFolderName );

    /**
    * Set the object properties in the object property store.
    */
    void SetPropertiesL( const TDesC& aOldFileName,
        const TDesC& aNewFileName,
        const CMTPObjectMetaData& aObject );

    /**
    * Update object info in the database.
    */
    TUint32 UpdateObjectInfoL( const TDesC& aOldObjectName,
            const TDesC& aNewObjectName);

    /*
    * A helper function of CopyObjectL
    * Copy a single object and update the database
    */
    void CopyAndUpdateL( TUint32 objectHandle );

protected:
    CMTPTypeObjectPropListElement* iPropertyElement;
    MMmMtpDpConfig& iDpConfig;
    CMTPTypeObjectPropList* iPropertyList;
private:
    CMTPObjectMetaData* iObjectInfo; // Not owned
    RArray<TUint> iObjectHandles;
    CFileMan* iFileMan;
    HBufC* iDest;
    TUint32 iNewParentHandle;
    TUint32 iHandle;
    TUint32 iStorageId;
    TInt iCopyObjectIndex;
    TInt iNumberOfObjects;
    HBufC* iPathToCopy;
    HBufC* iNewRootFolder;
    TTime iPreviousModifiedTime;

    };

#endif // CCOPYOBJECT_H
