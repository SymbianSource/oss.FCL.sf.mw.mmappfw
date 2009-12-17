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
* Description:  SendObjectInfo/SendObjectPropList/SendObject operation
*
*/


#ifndef CSENDOBJECT_H
#define CSENDOBJECT_H

#include "crequestprocessor.h"

// forward declaration
class RFs;
class CMTPTypeObjectInfo;
class MMTPObjectMgr;
class CMTPTypeFile;
class CMTPTypeObjectPropListElement;
class CMTPTypeObjectPropList;
class TMTPTypeUint32;
class CMTPObjectMetaData;
class CMTPObjectPropertyMgr;
class MMmMtpDpConfig;

/**
* Defines SendObjectInfo/SendObject request processor
*/
class CSendObject : public CRequestProcessor
    {
public:
    /**
    * Destructor
    */
    IMPORT_C virtual ~CSendObject();

protected:
    /**
    * Standard C++ Constructor
    * @param aFramework    The data provider framework
    * @param aConnection   The connection from which the request comes
    * @param aWrapper      Medadata access interface
    */
    IMPORT_C CSendObject( MMTPDataProviderFramework& aFramework,
        MMTPConnection& aConnection,
        MMmMtpDpConfig& aDpConfig );

    /**
    * 2nd Phase Constructor
    */
    IMPORT_C void ConstructL();

protected:
    // from CRequestProcessor
    /**
    * Override to match both the SendObjectInfo and SendObject requests
    * @param aRequest    The request to match
    * @param aConnection The connection from which the request comes
    * @return ETrue if the processor can handle the request, otherwise EFalse
    */
    IMPORT_C TBool Match( const TMTPTypeRequest& aRequest,
        MMTPConnection& aConnection ) const;

    /**
    * Verify the reqeust
    * @return EMTPRespCodeOK if request is verified, otherwise one of the
    *     error response codes
    */
    IMPORT_C TMTPResponseCode CheckRequestL();

    /**
    * Override from CRequestProcessor, can not be neglected
    * @return ETrue
    */
    IMPORT_C TBool HasDataphase() const;

    /**
    * SendObjectInfo/SendObject request handler
    */
    IMPORT_C void ServiceL();

    /**
    * Response Phase Handler
    */
    IMPORT_C TBool DoHandleResponsePhaseL();

    /**
    * Completeing phase Handler
    */
    IMPORT_C TBool DoHandleCompletingPhaseL();

    /**
    * Override from CRequestProcessor, can not be neglected
    */
    IMPORT_C void UsbDisconnect();

    // new protected function
    /**
     * Called by dp derived processor
     */
    IMPORT_C TMTPResponseCode SetMetaDataToWrapperL( const TUint16 aPropCode,
        MMTPType& aNewData,
        const CMTPObjectMetaData& aObjectMetaData );

protected:
    // new virtual functions
    /**
     *
     */
    virtual TMTPResponseCode SetSpecificObjectPropertyL( TUint16 aPropCode,
        const CMTPObjectMetaData& aObject,
        const CMTPTypeObjectPropListElement& aElement ) = 0;

    /**
     * Check datatypes for DP specific PropCodes
     */
    virtual TMTPResponseCode CheckSepecificPropType( TUint16 aPropCode, TUint16 aDataType ) = 0;

    virtual TInt HandleSpecificWrapperError(TInt aError,
            const CMTPObjectMetaData& aObject) = 0;

private:
    /**
    * Verify if the SendObject request comes after SendObjectInfo request
    * @return EMTPRespCodeOK if SendObject request comes after a valid
    *     SendObjectInfo request, otherwise
    * EMTPRespCodeNoValidObjectInfo
    */
    TMTPResponseCode CheckSendingStateL();

    /**
    * Handle request phase of SendObjectInfo operation
    */
    void ServiceInfoL();

    /**
     * Handle request phase of SendObjectPropList operation
     */
    void ServicePropListL();

    /**
    * Handle request phase of SendObject operation
    */
    void ServiceObjectL();

    /**
    * Handle response phase of SendObjectInfo operation
    */
    TBool DoHandleResponsePhaseInfoL();

    /**
     * Handle response phase of SendObject operation
     */
    TBool DoHandleResponsePhasePropListL();

    /**
    * Handle response phase of SendObject operation
    */
    TBool DoHandleResponsePhaseObjectL();

    /**
     *
     */
    TMTPResponseCode VerifyObjectPropListL(TInt& aInvalidParameterIndex);

    /**
     *
     */
    TMTPResponseCode CheckPropCodeL(const CMTPTypeObjectPropListElement& aElement);

    /**
     *
     */
    TMTPResponseCode ExtractPropertyL(const CMTPTypeObjectPropListElement& aElement);

    /*
     * Handle response phase of SetObjectPropListL operation
     */
    TMTPResponseCode SetObjectPropListL( const CMTPTypeObjectPropList& aPropList );

    /**
     *
     */
    TMTPResponseCode MatchStoreAndParentL();

    /**
    * Check if the object is too large
    * @return ETrue if yes, otherwise EFalse
    */
    TBool IsTooLarge( TUint32 aObjectSize ) const;

    /**
    * Check if we can store the file on the storage
    * @return ETrue if yes, otherwise EFalse
    */
    TBool CanStoreFileL( TUint32 aStorageId, TInt64 aObjectSize ) const;

    /**
    * Get the full path name of the object to be saved
    * @param aFileName, on entry, contains the file name of the object,
    * on return, contains the full path name of the object to be saved
    */
    TBool GetFullPathNameL( const TDesC& aFileName );

    /**
    * Check if the file already exists on the storage.
    * @return ETrue if file is exists, otherwise EFalse
    */
    TBool ExistsL( const TDesC& aName ) const;

    /**
    * Reserve object information before SendObject
    */
    void ReserveObjectL();

    /**
    * Set protection status of object which could be read/write-only
    */
    void SetProtectionStatusL();

    /**
    * Save object information of object whose size is zero
    */
    void SaveEmptyFileL();


    /**
    * Add file information into database in response phase of SendObject
    */
    void AddMediaToStoreL();

    /**
    * delete the file, which transfer incompletely
    */
    void Rollback();


private:
    enum TMTPSendingObjectState
        {
        EObjectNone,
        EObjectInfoInProgress,
        EObjectInfoSucceed,
        EObjectInfoFail,
        ESendObjectInProgress,
        ESendObjectSucceed,
        ESendObjectFail
        };

private:
    RFs& iFs;
    MMTPObjectMgr& iObjectMgr;
    MMmMtpDpConfig& iDpConfig;

    // received dataset
    CMTPTypeObjectInfo* iObjectInfo;
    CMTPTypeObjectPropList* iObjectPropList;
    CMTPTypeFile* iFileReceived;

    // object which need to reserved
    CMTPObjectMetaData* iReceivedObjectInfo;

    // progress of request
    TMTPSendingObjectState iProgress;

    // request params
    TUint16 iOperationCode;
    TUint32 iStorageId;
    TUint32 iParentHandle;
    TUint16 iObjectFormat;
    TUint64 iObjectSize;

    // S60 feature doesn't include this.
    // Reserve for future
    TUint16 iProtectionStatus;
    HBufC* iDateMod;

    // cache info during SendObjectInfo
    TUint32 iWidth;
    TUint32 iHeight;

    TFileName iFullPath;
    HBufC* iParentSuid;

    // params which is needed for reserve handle
    TMTPTypeRequest iExpectedSendObjectRequest;

    TBool iNoRollback;

    TUint32 iPreviousTransactionID;
    TUint32 iPreviousOperation;
    };

#endif // CSENDOBJECT_H
