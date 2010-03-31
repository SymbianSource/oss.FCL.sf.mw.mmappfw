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
* Description:  Meta data access singleton
*
*/


#include <e32base.h>    // CObject

#include "cmmmtpdpaccesssingleton.h"
#include "cmmmtpdpmetadataaccesswrapper.h"
#include "mmmtpdplogger.h"

// -----------------------------------------------------------------------------
// CMmMtpDpAccessSingleton::~CMmMtpDpAccessSingleton
// destructor
// -----------------------------------------------------------------------------
//
CMmMtpDpAccessSingleton::~CMmMtpDpAccessSingleton()
    {
    delete iWrapper;
    }

// -----------------------------------------------------------------------------
// CMmMtpDpAccessSingleton::GetAccessWrapperL
// get wrapper instance
// -----------------------------------------------------------------------------
//
EXPORT_C CMmMtpDpMetadataAccessWrapper& CMmMtpDpAccessSingleton::GetAccessWrapperL()
    {
    CMmMtpDpAccessSingleton* self = CMmMtpDpAccessSingleton::Instance();
    User::LeaveIfNull( self );

    return *( self->iWrapper );
    }

// -----------------------------------------------------------------------------
// CMmMtpDpAccessSingleton::CreateL
// create singleton instance
// -----------------------------------------------------------------------------
//
EXPORT_C void CMmMtpDpAccessSingleton::CreateL( MMTPDataProviderFramework& aFramework )
    {
    CMmMtpDpAccessSingleton* self = reinterpret_cast<CMmMtpDpAccessSingleton*>( Dll::Tls() );

    if ( self == NULL )
        {
        self = CMmMtpDpAccessSingleton::NewL( aFramework );
        Dll::SetTls( reinterpret_cast<TAny*>( self ) );
        }
    else
        {
        self->Inc();
        }

    PRINT1( _L("MM MTP <> CMmMtpDpAccessSingleton::CreateL, AccessCount: %d"),
        self->AccessCount() );
    }

// -----------------------------------------------------------------------------
// CMmMtpDpAccessSingleton::Release
// release singleton instance
// -----------------------------------------------------------------------------
//
EXPORT_C void CMmMtpDpAccessSingleton::Release()
    {
    CMmMtpDpAccessSingleton* self = reinterpret_cast<CMmMtpDpAccessSingleton*>( Dll::Tls() );
    if ( self != NULL )
        {
        PRINT1( _L("MM MTP <> CMmMtpDpAccessSingleton::Release, singleton != NULL, AccessCount: %d"),
            self->AccessCount() );

        self->Dec();
        if ( self->AccessCount() == 0 )
            {
            PRINT( _L("MM MTP <> CMmMtpDpAccessSingleton::Release, AccessCount == 0, delete it") );
            delete self;
            Dll::SetTls( NULL );
            }
        }
    }

// -----------------------------------------------------------------------------
// CMmMtpDpAccessSingleton::OpenSessionL
// do some special process with assess DBs when receives opensession command
// -----------------------------------------------------------------------------
//
EXPORT_C void CMmMtpDpAccessSingleton::OpenSessionL()
    {
    PRINT( _L("MM MTP => CMmMtpDpAccessSingleton::OpenSessionL") );
    CMmMtpDpAccessSingleton* self = CMmMtpDpAccessSingleton::Instance();
    User::LeaveIfNull( self );
    self->GetAccessWrapperL().OpenSessionL();
    PRINT( _L("MM MTP <= CMmMtpDpAccessSingleton::OpenSessionL") );
    }

// -----------------------------------------------------------------------------
// CMmMtpDpAccessSingleton::CloseSessionL
// do some special process with assess DBs when receives closesession command
// -----------------------------------------------------------------------------
//
EXPORT_C void CMmMtpDpAccessSingleton::CloseSessionL()
    {
    PRINT( _L("MM MTP => CMmMtpDpAccessSingleton::CloseSessionL") );
    CMmMtpDpAccessSingleton* self = CMmMtpDpAccessSingleton::Instance();
    User::LeaveIfNull( self );
    self->GetAccessWrapperL().CloseSessionL();
    PRINT( _L("MM MTP <= CMmMtpDpAccessSingleton::CloseSessionL") );
    }

// -----------------------------------------------------------------------------
// CMmMtpDpAccessSingleton::Instance
// get singleton instance, for internal use
// -----------------------------------------------------------------------------
//
CMmMtpDpAccessSingleton* CMmMtpDpAccessSingleton::Instance()
    {
    return reinterpret_cast<CMmMtpDpAccessSingleton*>( Dll::Tls() );
    }

// -----------------------------------------------------------------------------
// CMmMtpDpAccessSingleton::NewL
// two-phase construction
// -----------------------------------------------------------------------------
//
CMmMtpDpAccessSingleton* CMmMtpDpAccessSingleton::NewL( MMTPDataProviderFramework& aFramework )
    {
    CMmMtpDpAccessSingleton* self = new(ELeave) CMmMtpDpAccessSingleton;
    CleanupStack::PushL( self );
    self->ConstructL( aFramework );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CMmMtpDpAccessSingleton::ConstructL
// two-phase construction
// -----------------------------------------------------------------------------
//
void CMmMtpDpAccessSingleton::ConstructL( MMTPDataProviderFramework& aFramework )
    {
    iWrapper = CMmMtpDpMetadataAccessWrapper::NewL( aFramework );
    }


// end of file
