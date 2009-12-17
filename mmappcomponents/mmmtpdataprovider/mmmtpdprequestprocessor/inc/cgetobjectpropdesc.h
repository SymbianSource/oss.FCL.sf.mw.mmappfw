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
* Description:  Get object properties descript operation
*
*/


#ifndef CGETOBJECTPROPDESC_H
#define CGETOBJECTPROPDESC_H

#include "crequestprocessor.h"

// forward declaration
class CMTPTypeObjectPropDesc;
class MMmMtpDpConfig;

/**
* Defines GetObjectPropDesc request processor for common
*/
class CGetObjectPropDesc : public CRequestProcessor
    {
public:
    /**
    * Destructor
    */
    IMPORT_C virtual ~CGetObjectPropDesc();

protected:
    /**
    * Standard c++ constructor
    * @param aFramework    The data provider framework
    * @param aConnection   The connection from which the request comes
    * @param aWrapper      Medadata access interface
    */
    IMPORT_C CGetObjectPropDesc( MMTPDataProviderFramework& aFramework,
        MMTPConnection& aConnection,
        MMmMtpDpConfig& aDpConfig );
    
    /**
    * Second phase constructor
    */
    IMPORT_C void ConstructL();

protected:
    //from CRequestProcessor
    /**
    * GetObjectPropDesc request handler
    */
    IMPORT_C void ServiceL();

    /**
    * Check the current request
    * @return EMTPRespCodeOK if the reqeust is good, otherwise,
    *     one of the error response codes
    */
    IMPORT_C TMTPResponseCode CheckRequestL();

protected:
    // new virtuals
    virtual void ServiceSpecificObjectPropertyL( TUint16 aPropCode ) = 0;

private:
    /**
    * Ensures the object format operation parameter is valid
    * (done for CheckRequestL)
    * @return Response code of the check result
    */
    TMTPResponseCode CheckFormatL() const;

    /**
    * Ensures the object prop code operation parameter is valid
    * (done for CheckRequestL)
    * @return Response code of the check
    */
    TMTPResponseCode CheckPropCodeL() const;

    /**
    * Create list of possible protection status and create new ObjectPropDesc
    */
    void ServiceProtectionStatusL();

    /**
    * Create Regular expression for a file name and create new ObjectPropDesc
    */
    void ServiceFileNameL();

    /**
    * Create list of possible nonConsumable values and create new ObjectPropDesc
    */
    void ServiceNonConsumableL();

    /*
     * Get group code according to property code
     * @param aPropCode Specify the property code of which the group code needed
     * @return Group code of specified property
     */
    TUint32 GetGroupCode( TUint16 aPropCode );


protected:
    // Property object to return with the desc value
    CMTPTypeObjectPropDesc* iObjectProperty;

    //Format code
    TUint32 iFormatCode;

    MMmMtpDpConfig& iDpConfig;
    };

#endif // CGETOBJECTPROPDESC_H
