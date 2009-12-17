/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Monitor for PS Key changes from SysAp ( ME specific )
*
*/


#include <e32base.h>
#include <e32property.h>
#include <mpxpskeywatcher.h>
#include <mpxlog.h>
#include "mpxmmcejectmonitor.h"

// CONSTANTS
const TUid  KMmcEjectAppUid = { 0x101FFAFC };
const TUint KKeyMmcEjectOperation = 10;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CMPXMMCEjectMonitor::CMPXMMCEjectMonitor( MMPXSystemEventObserver& aObserver ) :
                                        iObserver( aObserver )
    {
    
    }


// ---------------------------------------------------------------------------
// 2nd phased Constructor
// ---------------------------------------------------------------------------
//
void CMPXMMCEjectMonitor::ConstructL()
    {
    MPX_DEBUG1(_L("CMPXMMCEjectMonitor::ConstructL <---"));
    
    iPSKeyWatcher = CMPXPSKeyWatcher::NewL( KMmcEjectAppUid, 
                                            KKeyMmcEjectOperation,
                                            this );
    MPX_DEBUG1(_L("CMPXMMCEjectMonitor::ConstructL --->"));
    }


// ---------------------------------------------------------------------------
// Two-Phase Constructor
// ---------------------------------------------------------------------------
//
CMPXMMCEjectMonitor* CMPXMMCEjectMonitor::NewL( MMPXSystemEventObserver& aObserver )
    {
    CMPXMMCEjectMonitor* self = new( ELeave ) CMPXMMCEjectMonitor( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMPXMMCEjectMonitor::~CMPXMMCEjectMonitor()
    {
    delete iPSKeyWatcher;
    }

// ---------------------------------------------------------------------------
// Callback when a cenrep key is modified
// ---------------------------------------------------------------------------
//
void CMPXMMCEjectMonitor::HandlePSEvent( TUid /*aUid*/, TInt /*aKey*/ )
    {
    MPX_DEBUG1(_L("CMPXMMCEjectMonitor::HandlePSEvent <---"));
    TInt value;
    iPSKeyWatcher->GetValue( value );
    
    // MMC About to be ejected from power key menu
    //
    if( value == 1 )
        {
        TRAP_IGNORE( iObserver.HandleSystemEventL( EPowerKeyEjectEvent, 0 ) );
        }
    MPX_DEBUG1(_L("CMPXMMCEjectMonitor::HandlePSEvent --->"));
    }

// END OF FILE
