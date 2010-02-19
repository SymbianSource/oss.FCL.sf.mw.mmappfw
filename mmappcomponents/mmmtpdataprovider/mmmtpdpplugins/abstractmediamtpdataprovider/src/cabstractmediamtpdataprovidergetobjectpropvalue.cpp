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
* Description:  Implement the operation: getobjectpropvalue
*
*/


#include "cabstractmediamtpdataprovidergetobjectpropvalue.h"
#include "abstractmediamtpdataproviderconst.h"
#include "mmmtpdplogger.h"
#include "mmmtpdpdefs.h"
#include "mmmtpdpconfig.h"

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProviderGetObjectPropValue::NewL
// Two-phase construction method
// -----------------------------------------------------------------------------
//
MMmRequestProcessor* CAbstractMediaMtpDataProviderGetObjectPropValue::NewL( MMTPDataProviderFramework& aFramework,
    MMTPConnection& aConnection,
    MMmMtpDpConfig& aDpConfig )
    {
    CAbstractMediaMtpDataProviderGetObjectPropValue* self =
        new ( ELeave ) CAbstractMediaMtpDataProviderGetObjectPropValue( aFramework,
            aConnection,
            aDpConfig );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProviderGetObjectPropValue::ConstructL
// Two-phase construction method
// -----------------------------------------------------------------------------
//
void CAbstractMediaMtpDataProviderGetObjectPropValue::ConstructL()
    {
    // Do nothing
    }

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProviderGetObjectPropValue::~CAbstractMediaMtpDataProviderGetObjectPropValue
// Destructor
// -----------------------------------------------------------------------------
//
CAbstractMediaMtpDataProviderGetObjectPropValue::~CAbstractMediaMtpDataProviderGetObjectPropValue()
    {
    // Do nothing
    }

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProviderGetObjectPropValue::CAbstractMediaMtpDataProviderGetObjectPropValue
// Standard c++ constructor
// -----------------------------------------------------------------------------
//
CAbstractMediaMtpDataProviderGetObjectPropValue::CAbstractMediaMtpDataProviderGetObjectPropValue( MMTPDataProviderFramework& aFramework,
    MMTPConnection& aConnection,
    MMmMtpDpConfig& aDpConfig ) :
    CGetObjectPropValue( aFramework, aConnection, aDpConfig )
    {
    // Do nothing
    }

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProviderGetObjectPropValue::ServiceSpecificObjectProperty
// no need to do anything here, just leave, as this should never be
// invoked or invalid propcode
// -----------------------------------------------------------------------------
//
void CAbstractMediaMtpDataProviderGetObjectPropValue::ServiceSpecificObjectPropertyL( TUint16 /*aPropCode*/ )
    {
    PRINT( _L( "MM MTP <> CAbstractMediaMtpDataProviderGetObjectPropValue::ServiceSpecificObjectPropertyL, leave with KErrNotSupported"));

    // May need add implementation here for further extension.

    User::Leave( KErrNotSupported );
    }

// end of file
