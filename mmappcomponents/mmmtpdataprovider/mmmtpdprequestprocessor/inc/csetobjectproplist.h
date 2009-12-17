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
* Description:  Set Object proplist operation.
*
*/


#ifndef CSETOBJECTPROPLIST_H
#define CSETOBJECTPROPLIST_H

#include "crequestprocessor.h"

// forward declaration
class CMTPTypeObjectPropList;
class CMmMtpDpMetadataAccessWrapper;
class MMTPObjectMgr;
class CMTPObjectMetaData;
class MMmMtpDpConfig;

/**
* Defines SetObjectPropList request processor
*/
class CSetObjectPropList : public CRequestProcessor
    {
public:
    /**
    * Two-phase construction method
    * @param aFramework, The data provider framework
    * @param aConnection, The connection from which the request comes
    * @param aWrapper    medadata access interface
    * @return a pointer to the created request processor object
    */
    //IMPORT_C static MMmRequestProcessor* NewL( MMTPDataProviderFramework& aFramework,
    //    MMTPConnection& aConnection,
    //   CMmMtpDpMetadataAccessWrapper& aWrapper );

    /**
    * Destructor
    */
    IMPORT_C virtual ~CSetObjectPropList();

protected:
    /**
    * Standard c++ constructor
    * @param aFramework    The data provider framework
    * @param aConnection    The connection from which the request comes
    * @param aWrapper    medadata access interface
    */
    IMPORT_C CSetObjectPropList( MMTPDataProviderFramework& aFramework,
        MMTPConnection& aConnection,
        MMmMtpDpConfig& aDpConfig );

    /**
    * 2nd Phase Constructor
    */
    IMPORT_C void ConstructL();

protected:
    // from CRequestProcessor
    /**
    * SetObjectPropList request handler
    */
    IMPORT_C void ServiceL();

    /**
    * Completing phase for the request handler
    */
    IMPORT_C TBool DoHandleResponsePhaseL();

    /**
    * Derived from CRequestProcessor, can not be neglected
    * @return ETrue
    */
    IMPORT_C TBool HasDataphase() const;

    // From CActive
    /**
    * Cancel the enumeration process
    */
    IMPORT_C void DoCancel();

    /**
    * Handle completed request
    */
    IMPORT_C void RunL();

    /**
    * Ignore the error, continue with the next one
    */
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
    IMPORT_C TMTPResponseCode ServiceMetaDataToWrapperL( const TUint16 aPropCode,
        MMTPType& aNewData,
        const CMTPObjectMetaData& aObjectMetaData );

    virtual TMTPResponseCode ServiceSpecificObjectPropertyL( TUint16 aPropCode,
        const CMTPObjectMetaData& aObject,
        const CMTPTypeObjectPropListElement& aElement ) = 0;

    virtual TInt HandleSpecificWrapperError( TInt aError,
        const CMTPObjectMetaData& aObject) = 0;

private:
    /*
    * Handle response phase of SetObjectPropListL operation
    */
    TMTPResponseCode SetObjectPropListL( const CMTPTypeObjectPropListElement& aPropListElement );

protected:
    MMTPObjectMgr& iObjectMgr;
    RFs& iFs;
    CMTPTypeObjectPropList* iPropertyList;
    MMmMtpDpConfig& iDpConfig;
    TUint32  iUnprocessedIndex;
    TInt  iElementCount;

    };

#endif // CSETOBJECTPROPLIST_H
