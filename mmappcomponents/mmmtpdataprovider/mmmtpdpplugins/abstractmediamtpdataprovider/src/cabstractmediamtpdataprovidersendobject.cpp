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
* Description:  Implement the operation: SendObjectInfo/SendObjectPropList/SendObject
*
*/


#include <mtp/mmtpdataproviderframework.h>
#include <mtp/cmtptypeobjectproplist.h>

#include "cabstractmediamtpdataprovidersendobject.h"
#include "abstractmediamtpdataproviderconst.h"
#include "mmmtpdplogger.h"
#include "mmmtpdpconfig.h"

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProviderSendObject::NewL
// Two-phase construction method
// -----------------------------------------------------------------------------
//
MMmRequestProcessor* CAbstractMediaMtpDataProviderSendObject::NewL( MMTPDataProviderFramework& aFramework,
    MMTPConnection& aConnection,
    MMmMtpDpConfig& aDpConfig )
    {
    CAbstractMediaMtpDataProviderSendObject* self =
        new (ELeave) CAbstractMediaMtpDataProviderSendObject( aFramework,
            aConnection,
            aDpConfig );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProviderSendObject::~CAbstractMediaMtpDataProviderSendObject
// Destructor
// -----------------------------------------------------------------------------
//
CAbstractMediaMtpDataProviderSendObject::~CAbstractMediaMtpDataProviderSendObject()
    {

    }

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProviderSendObject::CAbstractMediaMtpDataProviderSendObject
// Standard C++ Constructor
// -----------------------------------------------------------------------------
//
CAbstractMediaMtpDataProviderSendObject::CAbstractMediaMtpDataProviderSendObject( MMTPDataProviderFramework& aFramework,
    MMTPConnection& aConnection,
    MMmMtpDpConfig& aDpConfig ) :
    CSendObject( aFramework, aConnection, aDpConfig )
    {

    }

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProviderSendObject::ConstructL
// 2nd Phase Constructor
// -----------------------------------------------------------------------------
//
void CAbstractMediaMtpDataProviderSendObject::ConstructL()
    {
    CSendObject::ConstructL();
    }

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProviderSendObject::ServiceSpecificObjectPropertyL
// no need to do anything here, just leave, as this should never be invoked or
// invalid propcode
// -----------------------------------------------------------------------------
//
TMTPResponseCode CAbstractMediaMtpDataProviderSendObject::SetSpecificObjectPropertyL( TUint16 /*aPropCode*/,
    const CMTPObjectMetaData& /*aObject*/,
    const CMTPTypeObjectPropListElement& /*aElement*/ )
    {
    PRINT( _L( "MM MTP <> CAbstractMediaMtpDataProviderSendObject::ServiceSpecificObjectPropertyL, leave with KErrNotSupported" ) );
    User::Leave( KErrNotSupported );

    return EMTPRespCodeOK;
    }

TMTPResponseCode CAbstractMediaMtpDataProviderSendObject::CheckSepecificPropType( TUint16 aPropCode, TUint16 aDataType )
    {
    PRINT( _L( "MM MTP <> CAbstractMediaMtpDataProviderSendObject::ServiceSpecificCheckDataTypeL, leave with KErrNotSupported" ) );
    // TODO: User::Leave( KErrNotSupported );
    return EMTPRespCodeOK;
    }

TInt CAbstractMediaMtpDataProviderSendObject::HandleSpecificWrapperError( TInt aError,
    const CMTPObjectMetaData& /*aObject*/ )
    {
    // do nothing, simply return the error
    return aError;
    }

// end of file
