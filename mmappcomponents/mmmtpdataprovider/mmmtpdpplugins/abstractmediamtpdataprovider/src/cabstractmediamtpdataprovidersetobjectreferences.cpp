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
* Description:  Implement the operation: SetObjectReferences
*
*/


#include "cabstractmediamtpdataprovidersetobjectreferences.h"
#include "abstractmediamtpdataproviderconst.h"
#include "cmmmtpdpmetadataaccesswrapper.h"
#include "mmmtpdplogger.h"

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProviderSetObjectReferences::NewL
// Two-phase construction method
// -----------------------------------------------------------------------------
//
MMmRequestProcessor* CAbstractMediaMtpDataProviderSetObjectReferences::NewL( MMTPDataProviderFramework& aFramework,
    MMTPConnection& aConnection,
    MMmMtpDpConfig& aDpConfig )
    {
    CAbstractMediaMtpDataProviderSetObjectReferences* self =
        new ( ELeave ) CAbstractMediaMtpDataProviderSetObjectReferences( aFramework,
            aConnection,
            aDpConfig );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProviderSetObjectReferences::ConstructL
// Two-phase construction method
// -----------------------------------------------------------------------------
//
void CAbstractMediaMtpDataProviderSetObjectReferences::ConstructL()
    {

    }

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProviderSetObjectReferences::~CAbstractMediaMtpDataProviderSetObjectReferences
// Destructor
// -----------------------------------------------------------------------------
//
CAbstractMediaMtpDataProviderSetObjectReferences::~CAbstractMediaMtpDataProviderSetObjectReferences()
    {

    }

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProviderSetObjectReferences::CAbstractMediaMtpDataProviderSetObjectReferences
// Standard c++ constructor
// -----------------------------------------------------------------------------
//
CAbstractMediaMtpDataProviderSetObjectReferences::CAbstractMediaMtpDataProviderSetObjectReferences( MMTPDataProviderFramework& aFramework,
    MMTPConnection& aConnection,
    MMmMtpDpConfig& aDpConfig ) :
    CSetObjectReferences( aFramework, aConnection, aDpConfig )
    {

    }

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProviderSetObjectReferences::DoSetObjectReferencesL
// set references to DB
// -----------------------------------------------------------------------------
//
void CAbstractMediaMtpDataProviderSetObjectReferences::DoSetObjectReferencesL( CMmMtpDpMetadataAccessWrapper& aWrapper,
    TUint16 aObjectFormat,
    const TDesC& aSrcFileName,
    CDesCArray& aRefFileArray )
    {
    TInt count = sizeof( KAbstractMediaMtpDataProviderSupportedFormats ) / sizeof (TUint16);
    TBool supported = EFalse;

    for( TInt i = 0; i < count; i++ )
        {
        if( KAbstractMediaMtpDataProviderSupportedFormats[i] == aObjectFormat )
            {
            supported = ETrue;
            break;
            }
        }

    if ( supported )
        {
        aWrapper.SetPlaylistL( aSrcFileName, aRefFileArray );
        }
    }

// end of file
