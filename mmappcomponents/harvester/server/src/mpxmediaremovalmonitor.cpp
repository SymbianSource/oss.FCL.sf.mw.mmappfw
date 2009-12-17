/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Monitors for Media removal
*
*/


#include <e32base.h>
#include <f32file.h>
#include <mpxlog.h>
#include "mpxmediaremovalmonitor.h"

// ---------------------------------------------------------------------------
// C++ Constructor
// ---------------------------------------------------------------------------
//
CMPXMediaRemovalMonitor::CMPXMediaRemovalMonitor
                  ( TInt aDrive, RFs& aFs, MMPXSystemEventObserver& aObserver ) 
                                                       : CActive(EPriorityHigh),
                                                         iDrive( aDrive ),
                                                         iFs( aFs ),
                                                         iDiskRemoved( EFalse ),
                                                         iObserver( aObserver )
                                                         
    {
    CActiveScheduler::Add(this);
    }


// ---------------------------------------------------------------------------
// 2nd Phase Constructor
// ---------------------------------------------------------------------------
//
void CMPXMediaRemovalMonitor::ConstructL()
    {
    MPX_DEBUG1(_L("CMPXMediaRemovalMonitor::ConstructL <---"));
    
    // Initial state
    TDriveInfo drive;
	User::LeaveIfError(iFs.Drive(drive, TInt(iDrive)));
   	iDiskRemoved = (drive.iType == EMediaNotPresent);

    // Start listening
    TNotifyType notType(ENotifyDisk);
    iFs.NotifyChange( notType, iStatus );
    SetActive();
    
    MPX_DEBUG1(_L("CMPXMediaRemovalMonitor::ConstructL --->"));
    }


// ---------------------------------------------------------------------------
// Two-Phased Constructor
// ---------------------------------------------------------------------------
//
CMPXMediaRemovalMonitor* CMPXMediaRemovalMonitor::NewL
                ( TInt aDrive, RFs& aFs, MMPXSystemEventObserver& aObserver )
    {
    CMPXMediaRemovalMonitor* self
                               = CMPXMediaRemovalMonitor::NewLC( aDrive,
                                                                 aFs,
                                                                 aObserver );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Two-Phased Constructor
// ---------------------------------------------------------------------------
//
CMPXMediaRemovalMonitor* CMPXMediaRemovalMonitor::NewLC
               ( TInt aDrive, RFs& aFs, MMPXSystemEventObserver& aObserver )
    {
    CMPXMediaRemovalMonitor* self = 
                          new( ELeave ) CMPXMediaRemovalMonitor( aDrive,
                                                                 aFs,
                                                                 aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMPXMediaRemovalMonitor::~CMPXMediaRemovalMonitor()
    {
    Cancel();
    }

    
// ---------------------------------------------------------------------------
// Service the request
// ---------------------------------------------------------------------------
//
void CMPXMediaRemovalMonitor::RunL()
    {
    MPX_DEBUG1(_L("CMPXMediaRemovalMonitor::RunL <---"));
    
    // Re-subscribe to event.
    TNotifyType notType(ENotifyDisk);
    iFs.NotifyChange( notType, iStatus );
    SetActive();
    
    // Check state
    TDriveInfo drive;
	User::LeaveIfError(iFs.Drive(drive, TInt(iDrive)));
	
	// Notify Observer
    switch(drive.iType)
        {
        case EMediaNotPresent:
            {
            if (!iDiskRemoved)
                {
                iObserver.HandleSystemEventL( EDiskRemovedEvent, iDrive );
                }
            iDiskRemoved = ETrue;
            break;
            }
        default:
            {
            if ( iDiskRemoved &&
        		 ( drive.iMediaAtt & ( KMediaAttLockable|KMediaAttLocked|KMediaAttHasPassword ) ) != 
 				 ( KMediaAttLockable|KMediaAttLocked|KMediaAttHasPassword ) ) 
                {
                iObserver.HandleSystemEventL( EDiskInsertedEvent, iDrive );
                iDiskRemoved = EFalse;
                }
            break;
            }
        }
    

    MPX_DEBUG1(_L("CMPXMediaRemovalMonitor::RunL --->"));
    }
    
// ---------------------------------------------------------------------------
// Cancel NotifyChange request from file system
// ---------------------------------------------------------------------------
//
void CMPXMediaRemovalMonitor::DoCancel()
    {
    iFs.NotifyChangeCancel();
    }
    
// ----------------------------------------------------------------------------
// Handles a leave occurring in the request completion event handler RunL()
// Don't care if client has a User::Leave() in RunL(), keep monitoring for events
// ----------------------------------------------------------------------------
//
TInt CMPXMediaRemovalMonitor::RunError(TInt aError)
    {
    MPX_DEBUG2("CMPXMediaRemovalMonitor::RunError(%d)", aError );
    (void) aError;  // avoid compile warning in urel
    
    return KErrNone;
    }           
