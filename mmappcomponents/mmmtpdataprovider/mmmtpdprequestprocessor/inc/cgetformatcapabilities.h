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
* Description:  Get Format Capabilities operation
*
*/
#ifndef CGETFORMATCAPABILITLIST_H
#define CGETFORMATCAPABILITLIST_H

#include <mtp/cmtptypeformatcapabilitylist.h>

#include "crequestprocessor.h"
#include "mmmtpdplogger.h"
#include "mmmtpdpconfig.h"

// forward declaration
class MMTPRequestProcessor;
class MMmMtpDpConfig;
class CMTPTypeObjectPropDesc;
class CMTPTypeInterdependentPropDesc;
class CMTPTypeObjectPropDesc;
class CMTPTypeString;

/**
Implements the file data provider CMTPGetFormatCapabilities request processor.
@internalComponent
*/
class CGetFormatCapabilities : public CRequestProcessor
    {
public:
    /**
    * Destructor
    */
    IMPORT_C virtual ~CGetFormatCapabilities();

protected:
    /**
    * Standard c++ constructor
    * @param aFramework    The data provider framework
    * @param aConnection   The connection from which the request comes
    * @param aDpConfig, The interface of dataprovider configuration
    */
    IMPORT_C CGetFormatCapabilities( MMTPDataProviderFramework& aFramework,
        MMTPConnection& aConnection,
        MMmMtpDpConfig& aDpConfig );

    /**
    * Constructor
    * Second phase constructor
    */
    IMPORT_C void ConstructL();

protected:
    // from CMTPRequestProcessor
    /**
    * CGetFormatCapabilities request handler
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
    /**
    * Service specific object property
    * @param aPropCode, The prop code
    * @return MTPType object propdesc
    */
    virtual CMTPTypeObjectPropDesc* ServiceSpecificPropertyDescL( TUint16 aPropCode ) = 0;

    /**
    * Service interdepent propdesc
    * @param aFormatCode, The format code
    * @return MTPTypeInterdependentPropDesc
    */
    virtual void ServiceInterdepentPropDescL() = 0;

private:
    /**
    * Get  PropDesc via PropCode
    * @param aPropCode    The Property Code
    * @return The MTPTypeObject PropDesc
    */
    CMTPTypeObjectPropDesc* ServicePropDescL( TUint16 aPropCode );

    /**
    * Get PropDesc with Protection status
    * @return The MTPTypeObject PropDesc
    */
    CMTPTypeObjectPropDesc*  ServiceProtectionStatusL();

    /**
    * Get PropDesc with file name
    * @return The MTPTypeObject PropDesc
    */
    CMTPTypeObjectPropDesc*  ServiceFileNameL();

    /**
    * Get PropDesc with nonconsumable
    * @return The MTPTypeObject PropDesc
    */
    CMTPTypeObjectPropDesc*  ServiceNonConsumableL();

    /**
    * Get group code
    * @param aPropCode Specify the property code of which the group code needed
    * @return Group code of specified property
    */
    TUint32 GetGroupCode( TUint32 aPropCode );

protected:
    /*
     * Format code.
     */
    TUint iFormatCode;

    CMTPTypeInterdependentPropDesc* iInterdependentPropDesc;

private: // Owned
    /*
     * CMTPTypeCapabilityList dataset.
     */
    CMTPTypeFormatCapabilityList* iCapabilityList;

    /*
     * MMmMtpDpConfig.
     */
    MMmMtpDpConfig& iDpConfig;
    };

#endif // CMTPGETFORMATCAPABILITLIST_H
