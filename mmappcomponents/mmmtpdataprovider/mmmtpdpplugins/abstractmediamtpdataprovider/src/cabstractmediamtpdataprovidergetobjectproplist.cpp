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


#include "cabstractmediamtpdataprovidergetobjectproplist.h"
#include "abstractmediamtpdataproviderconst.h"
#include "mmmtpdplogger.h"
#include "mmmtpdpdefs.h"
#include "mmmtpdpconfig.h"

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProviderGetObjectPropList::NewL
// Two-phase construction method
// -----------------------------------------------------------------------------
//
MMmRequestProcessor* CAbstractMediaMtpDataProviderGetObjectPropList::NewL( MMTPDataProviderFramework& aFramework,
    MMTPConnection& aConnection,
    MMmMtpDpConfig& aDpConfig )
    {
    CAbstractMediaMtpDataProviderGetObjectPropList* self =
        new (ELeave) CAbstractMediaMtpDataProviderGetObjectPropList( aFramework,
            aConnection,
            aDpConfig );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProviderGetObjectPropList::~CAbstractMediaMtpDataProviderGetObjectPropList
// Destructor
// -----------------------------------------------------------------------------
//
CAbstractMediaMtpDataProviderGetObjectPropList::~CAbstractMediaMtpDataProviderGetObjectPropList()
    {

    }

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProviderGetObjectPropList::CAbstractMediaMtpDataProviderGetObjectPropList
// Standard C++ Constructor
// -----------------------------------------------------------------------------
//
CAbstractMediaMtpDataProviderGetObjectPropList::CAbstractMediaMtpDataProviderGetObjectPropList( MMTPDataProviderFramework& aFramework,
    MMTPConnection& aConnection,
    MMmMtpDpConfig& aDpConfig ) :
    CGetObjectPropList( aFramework, aConnection, aDpConfig )
    {

    }

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProviderGetObjectPropList::ConstructL
// 2nd Phase Constructor
// -----------------------------------------------------------------------------
//
void CAbstractMediaMtpDataProviderGetObjectPropList::ConstructL()
    {
    CGetObjectPropList::ConstructL();
    }

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProviderGetObjectPropList::ServiceSpecificObjectPropertyL
// do nothing here, just leave, as this should never be invoked or invalid propcode
// -----------------------------------------------------------------------------
//
TInt CAbstractMediaMtpDataProviderGetObjectPropList::ServiceSpecificObjectPropertyL( TUint16 /*aPropCode*/,
    TUint32 /*aHandle*/ )
    {
    PRINT( _L( "MM MTP <> CAbstractMediaMtpDataProviderGetObjectPropList::ServiceSpecificObjectPropertyL, leave with KErrNotSupported" ) );
    return KErrNotSupported;
    }

// end of file
