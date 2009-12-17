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
* Description:
*
*/
#include <mtp/cmtptypeinterdependentpropdesc.h>

#include "cabstractmediamtpdataprovidergetformatcapabilities.h"


// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProviderGetFormatCapabilities::NewL
// Two-phase construction method
// -----------------------------------------------------------------------------
//

MMmRequestProcessor* CAbstractMediaMtpDataProviderGetFormatCapabilities::NewL( MMTPDataProviderFramework& aFramework,
    MMTPConnection& aConnection,
    MMmMtpDpConfig& aDpConfig )
    {
    CAbstractMediaMtpDataProviderGetFormatCapabilities* self =
        new ( ELeave ) CAbstractMediaMtpDataProviderGetFormatCapabilities( aFramework,
            aConnection,
            aDpConfig );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProviderGetFormatCapabilities::ConstructL
// Two-phase construction method
// -----------------------------------------------------------------------------
//
void CAbstractMediaMtpDataProviderGetFormatCapabilities::ConstructL()
    {
    CGetFormatCapabilities::ConstructL();
    }

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProviderGetFormatCapabilities::~CAbstractMediaMtpDataProviderGetFormatCapabilities()
// Destructor
// -----------------------------------------------------------------------------
//
CAbstractMediaMtpDataProviderGetFormatCapabilities::~CAbstractMediaMtpDataProviderGetFormatCapabilities()
    {

    }


// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProviderGetFormatCapabilities::CAbstractMediaMtpDataProviderGetFormatCapabilities
// Standard c++ constructor
// -----------------------------------------------------------------------------
//
CAbstractMediaMtpDataProviderGetFormatCapabilities::CAbstractMediaMtpDataProviderGetFormatCapabilities( MMTPDataProviderFramework& aFramework,
    MMTPConnection& aConnection,
    MMmMtpDpConfig& aDpConfig ) :
    CGetFormatCapabilities( aFramework, aConnection, aDpConfig )
    {

    }

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProviderGetFormatCapabilities::ServiceInterdepentPropDesc
//
// -----------------------------------------------------------------------------
//
void CAbstractMediaMtpDataProviderGetFormatCapabilities::ServiceInterdepentPropDescL()
    {
    iInterdependentPropDesc = CMTPTypeInterdependentPropDesc::NewL();
    }

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProviderGetFormatCapabilities::ServiceSpecificObjectPropertyL
// Service specific object property
// -----------------------------------------------------------------------------
//
CMTPTypeObjectPropDesc* CAbstractMediaMtpDataProviderGetFormatCapabilities::ServiceSpecificPropertyDescL( TUint16 aPropCode )
    {
    return NULL;
    }


// end of file
