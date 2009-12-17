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
* Description:  Implement the operation: getobjectproplist
*
*/


#include <mtp/mmtpdataproviderframework.h>
#include <mtp/cmtptypeobjectproplist.h>

#include "cabstractmediamtpdataprovidersetobjectproplist.h"
#include "abstractmediamtpdataproviderconst.h"
#include "mmmtpdplogger.h"
#include "mmmtpdpconfig.h"

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProviderSetObjectPropList::NewL
// Two-phase construction method
// -----------------------------------------------------------------------------
//
MMmRequestProcessor* CAbstractMediaMtpDataProviderSetObjectPropList::NewL( MMTPDataProviderFramework& aFramework,
    MMTPConnection& aConnection,
    MMmMtpDpConfig& aDpConfig )
    {
    CAbstractMediaMtpDataProviderSetObjectPropList* self =
        new (ELeave) CAbstractMediaMtpDataProviderSetObjectPropList( aFramework,
            aConnection,
            aDpConfig );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProviderSetObjectPropList::~CAbstractMediaMtpDataProviderSetObjectPropList
// Destructor
// -----------------------------------------------------------------------------
//
CAbstractMediaMtpDataProviderSetObjectPropList::~CAbstractMediaMtpDataProviderSetObjectPropList()
    {

    }

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProviderSetObjectPropList::CAbstractMediaMtpDataProviderSetObjectPropList
// Standard C++ Constructor
// -----------------------------------------------------------------------------
//
CAbstractMediaMtpDataProviderSetObjectPropList::CAbstractMediaMtpDataProviderSetObjectPropList( MMTPDataProviderFramework& aFramework,
    MMTPConnection& aConnection,
    MMmMtpDpConfig& aDpConfig ) :
    CSetObjectPropList( aFramework, aConnection, aDpConfig )
    {

    }

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProviderSetObjectPropList::ConstructL
// 2nd Phase Constructor
// -----------------------------------------------------------------------------
//
void CAbstractMediaMtpDataProviderSetObjectPropList::ConstructL()
    {
    CSetObjectPropList::ConstructL();
    }

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProviderSetObjectPropList::ServiceSpecificObjectPropertyL
// no need to do anything here, just leave, as this should never be invoked or
// invalid propcode
// -----------------------------------------------------------------------------
//
TMTPResponseCode CAbstractMediaMtpDataProviderSetObjectPropList::ServiceSpecificObjectPropertyL( TUint16 /*aPropCode*/,
    const CMTPObjectMetaData& /*aObject*/,
    const CMTPTypeObjectPropListElement& /*aElement*/ )
    {
    PRINT( _L( "MM MTP <> CAbstractMediaMtpDataProviderSetObjectPropList::ServiceSpecificObjectPropertyL, leave with KErrNotSupported" ) );
    User::Leave( KErrNotSupported );

    return EMTPRespCodeOK;
    }

TInt CAbstractMediaMtpDataProviderSetObjectPropList::HandleSpecificWrapperError( TInt aError,
    const CMTPObjectMetaData& /*aObject*/ )
    {
    // do nothing, simply return the error
    return aError;
    }

// end of file
