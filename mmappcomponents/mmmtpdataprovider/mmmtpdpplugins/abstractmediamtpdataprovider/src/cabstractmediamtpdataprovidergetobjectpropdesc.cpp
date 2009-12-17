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
* Description:  Implement the operation: GetObjectPropDesc
*
*/

#include <mtp/cmtptypeobjectpropdesc.h>
#include <mtp/mtpprotocolconstants.h>

#include "cabstractmediamtpdataprovidergetobjectpropdesc.h"
#include "mmmtpdplogger.h"
#include "abstractmediamtpdataproviderconst.h"
#include "mmmtpdpdefs.h"

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProviderGetObjectPropDesc::NewL
// Two-phase construction method
// -----------------------------------------------------------------------------
//
MMmRequestProcessor* CAbstractMediaMtpDataProviderGetObjectPropDesc::NewL( MMTPDataProviderFramework& aFramework,
    MMTPConnection& aConnection,
    MMmMtpDpConfig& aDpConfig )
    {
    CAbstractMediaMtpDataProviderGetObjectPropDesc* self =
        new ( ELeave ) CAbstractMediaMtpDataProviderGetObjectPropDesc( aFramework,
            aConnection,
            aDpConfig );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProviderGetObjectPropDesc::ConstructL
// Two-phase construction method
// -----------------------------------------------------------------------------
//
void CAbstractMediaMtpDataProviderGetObjectPropDesc::ConstructL()
    {
    CGetObjectPropDesc::ConstructL();
    }

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProviderGetObjectPropDesc::~CAbstractMediaMtpDataProviderGetObjectPropDesc
// Destructor
// -----------------------------------------------------------------------------
//
CAbstractMediaMtpDataProviderGetObjectPropDesc::~CAbstractMediaMtpDataProviderGetObjectPropDesc()
    {

    }

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProviderGetObjectPropDesc::CAbstractMediaMtpDataProviderGetObjectPropDesc
// Standard c++ constructor
// -----------------------------------------------------------------------------
//
CAbstractMediaMtpDataProviderGetObjectPropDesc::CAbstractMediaMtpDataProviderGetObjectPropDesc( MMTPDataProviderFramework& aFramework,
    MMTPConnection& aConnection,
    MMmMtpDpConfig& aDpConfig ) :
    CGetObjectPropDesc( aFramework, aConnection, aDpConfig )
    {

    }

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProviderGetObjectPropDesc::ServiceSpecificObjectPropertyL
//
// -----------------------------------------------------------------------------
//
void CAbstractMediaMtpDataProviderGetObjectPropDesc::ServiceSpecificObjectPropertyL( TUint16 /*aPropCode*/ )
    {
    PRINT( _L( "MM MTP <> CAbstractMediaMtpDataProviderGetObjectPropDesc::ServiceSpecificObjectPropertyL leave with not supportd" ) );
    User::Leave( KErrNotSupported );
    }

// end of file
