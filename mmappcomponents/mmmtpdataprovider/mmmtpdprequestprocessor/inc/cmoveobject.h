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
* Description:  Move object operation
*
*/


#ifndef CMOVEOBJECT_H
#define CMOVEOBJECT_H

#include "crequestprocessor.h"

class CFileMan;
class CMTPObjectMetaData;
class CMmMtpDpMetadataAccessWrapper;
class CMTPTypeObjectPropList;
class CMTPTypeObjectPropListElement;
class MMmMtpDpConfig;

/**
* Defines MoveObject request processor
*/
class CMoveObject : public CRequestProcessor
    {
public:
    /**
    * Two-phase construction method
    * @param aFramework The data provider framework
    * @param aConnection  The connection from which the request comes
    * @param aWrapper    medadata access interface
    * @return a pointer to the created request processor object
    */
    //IMPORT_C static MMmRequestProcessor* NewL(
    //        MMTPDataProviderFramework& aFramework,
    //        MMTPConnection& aConnection,
    //        CMmMtpDpMetadataAccessWrapper& aWrapper );

    /**
    * Destructor
    */
    IMPORT_C virtual ~CMoveObject();

protected:
    /**
    * Standard c++ constructor
    */
    IMPORT_C CMoveObject( MMTPDataProviderFramework& aFramework,
            MMTPConnection& aConnection,
            MMmMtpDpConfig& aDpConfig );

    /**
    * Second phase constructor
    */
    IMPORT_C void ConstructL();

protected:
    // from CRequestProcessor
    /**
    * MoveObject request handler
    */
    IMPORT_C void ServiceL();

    // from CActive
    IMPORT_C void RunL();

    IMPORT_C TInt RunError( TInt aError );


protected:
    // new virtuals
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
            const CMTPObjectMetaData& aObject ) = 0;

    virtual TMTPResponseCode ServiceSetSpecificObjectPropertyL( TUint16 aPropCode,
                const CMTPObjectMetaData& aObject,
                const CMTPTypeObjectPropListElement& aElement ) = 0;

private:
    /**
    * Retrieve the parameters of the request
    */
    void GetParametersL();

    /**
    * Get a default parent object, ff the request does not specify a parent object,
    */
    void SetDefaultParentObjectL();

    /**
    * move object operations
    * @return A valid MTP response code.
    */
    TMTPResponseCode MoveObjectL();

    /**
    * Check if we can move the file to the new location
    */
    TMTPResponseCode CanMoveObjectL( const TDesC& aOldName,
            const TDesC& aNewName ) const;

    /**
    * Save the object properties before moving
    */
    void GetPreviousPropertiesL( const CMTPObjectMetaData& aObject );

    /**
    * Set the object properties after moving
    */

    void SetPreviousPropertiesL( const CMTPObjectMetaData& aObject );
    /**
    * A helper function of MoveObjectL.
    * @param aNewFileName the new file name after the object is moved.
    */
    void MoveFileL( const TDesC& aNewFileName );

    /**
    * A helper function of MoveObjectL
    */
    void MoveFolderL();

    /* Move the objects through iterations of RunL. It currently move 20 objects
    * at a time and the number can be adjusted by changing
    * "KMoveObjectGranularity"
    */
    TBool MoveOwnedObjectsL();

    /**
    * Set the object properties in the object property store.
    */
    void SetPropertiesL( const TDesC& aOldFileName,
        const TDesC& aNewFileName,
        const CMTPObjectMetaData& aNewObject );

    /*
    * This function will actually delete the orginal folders from the file system.
    */
    TMTPResponseCode FinalPhaseMove();

    /*
    * Generate the list of handles that need to be moved to the new location.
    */
    void GenerateObjectHandleListL( TUint32 aParentHandle );

    /*
    * Move a single object and update the database
    */
    void MoveAndUpdateL( TUint32 objectHandle );


protected:
    CMTPTypeObjectPropListElement* iPropertyElement;
    MMmMtpDpConfig& iDpConfig;
    CMTPTypeObjectPropList* iPropertyList;
private:
    TBool iSameStorage;
    CFileMan* iFileMan;
    CMTPObjectMetaData* iObjectInfo; //Not owned.
    HBufC* iDest;
    HBufC* iNewRootFolder;
    TUint32 iNewParentHandle;
    TUint32 iStorageId;
    TTime iPreviousModifiedTime;
    HBufC* iPathToMove;
    RArray<TUint> iObjectHandles;
    TInt iMoveObjectIndex;
    TInt iNumberOfObjects;
    };

#endif // CMOVEOBJCT_H
