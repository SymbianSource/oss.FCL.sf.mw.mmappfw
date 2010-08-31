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
* Description:  Folder scanning class to scan files in the file system
*
*/

#include <e32base.h>
#include <mpxlog.h>
#include "mpxfolderscanner.h"
#include "mpxfileadditionobserver.h"
#include "mpxfilescanstateobserver.h"
#include "mpxfhcommon.h"

// CONSTANTS
const TInt KFileNumBreakCount = 5;

// ======== LOCAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// RArray compare function to compare strings
// ---------------------------------------------------------------------------
//
static TInt CompareString(const TPath& aFirst,
                          const TPath& aSecond)
    {
    return aFirst.Compare( aSecond );
    }
    
// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CMPXFolderScanner::CMPXFolderScanner( MMPXFileAdditionObserver& aObserver,
                                      MMPXFileScanStateObserver& aStateObs,
                                      RFs& aFs  ) : CActive(EPriorityNull),
                                                  iObserver( aObserver ),
                                                  iStateObserver( aStateObs ),
                                                  iFs( aFs )
                                                    
    {
    CActiveScheduler::Add(this);
    }

// ---------------------------------------------------------------------------
// 2nd Phase Constructor
// ---------------------------------------------------------------------------
//
void CMPXFolderScanner::ConstructL()
    {
    iDirScan = CDirScan::NewL(iFs);
    }

// ---------------------------------------------------------------------------
// Two Phased Constructor
// ---------------------------------------------------------------------------
//
CMPXFolderScanner* CMPXFolderScanner::NewL( MMPXFileAdditionObserver& aObserver,
                                            MMPXFileScanStateObserver& aStateObs,
                                            RFs& aFs )
    {
    CMPXFolderScanner* self = new( ELeave ) CMPXFolderScanner( aObserver, 
                                                               aStateObs,
                                                               aFs );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMPXFolderScanner::~CMPXFolderScanner()
    {
    Cancel();
    
    delete iDirScan;
    delete iDir;
    
    iDrivesToScan.Close();
    }
    
// ---------------------------------------------------------------------------
// Scans a list of drives for files
// ---------------------------------------------------------------------------
//
void CMPXFolderScanner::ScanL( RArray<TPath>& aDrives )
    {
    MPX_DEBUG1("CMPXFolderScanner::ScanL <---");
    
    // Copy all the other drives we want to scan
    //
    TInt count( aDrives.Count() );
    for( TInt i=0; i<count; ++i )
        {
        // Check if we are already scanning this drive
        TInt found( iDrivesToScan.FindInOrder( aDrives[i], CompareString ) ); 
        if( found == KErrNotFound )
            {
            iDrivesToScan.Append( aDrives[i] );
            }
        }
    
    // If we were already scanning, don't do it again
    //
    if( !iScanning )
        {
        // Setup the next drive to scan
        //
        if( !SetupNextDriveToScanL() )
            {
            // Kick off the scanning
            iStatus = KRequestPending;
            SetActive();
            TRequestStatus* status = &iStatus;
            User::RequestComplete( status, KErrNone );
            
            // We've started scanning
            iScanning = ETrue;
            }
        else
            {
            // Nothing to scan
            DoScanCompleteL(KErrNone);    
            }        
        }
    MPX_DEBUG1("CMPXFolderScanner::ScanL --->");
    }

// ---------------------------------------------------------------------------
// Continue Scanning for more files
// ---------------------------------------------------------------------------
//
TBool CMPXFolderScanner::DoScanL()
    {
    MPX_DEBUG1("CMPXFolderScanner::DoScanL <---");
    TBool done (EFalse);
    
    // Check each file in each directory
    TInt numFiles( iDir->Count() );
    while( iCount < numFiles )
        {
        TEntry entry = (*iDir)[iCount];

        // Construct the full path and file name
        TParse fullEntry;
        TPtrC dirPath(iDirScan->FullPath());
        fullEntry.Set(entry.iName, &dirPath, NULL);
        
        TPtrC fullname = fullEntry.FullName();
        TInt index = iObserver.IsMediaFileL( fullname );
        if( KErrNotFound != index )
            {
            iObserver.HandleFileAdditionL( fullname, index );
            }
        
        // Break if we have scanned enough files
        //
        ++iCount;
        if( iCount%KFileNumBreakCount == 0 )
            {
            return EFalse;
            }
        }
    
    // All files from this directory scanned, so move onto next
    //
    TInt err( KErrNone );
    TBool blocked (EFalse);
    if( iCount == numFiles )
        {
        // Get next Folder
        //
        iCount = 0;          
        delete iDir;
        iDir = NULL;
        do
            {
            TRAP(err, iDirScan->NextL(iDir));
            blocked = iObserver.IsPathBlockedL( iDirScan->FullPath() );
            if( blocked )
                {
                delete iDir;
                iDir = NULL;
                }
            if( err == KErrNotReady )
                {
                delete iDir;
                iDir = NULL;
                break;
                }
            }
        while ( err == KErrPathNotFound || blocked );  
        
        // No more directories to scan on this drive
        //
        if( !iDir )
            {
            done = SetupNextDriveToScanL();  
            }
        }
        
    MPX_DEBUG1("CMPXFolderScanner::DoScanL --->");
    return done;
    }

// ---------------------------------------------------------------------------
// Setup the object to scan the next directory
// ---------------------------------------------------------------------------
//
TBool CMPXFolderScanner::SetupNextDriveToScanL()
    {
    MPX_DEBUG1("CMPXFolderScanner::SetupNextDriveToScanL <---");
    
    TBool done(EFalse);
    TBool blocked(EFalse);
    // Scan next drive
    while( iDir == NULL && !done )
        {
        if( !iDrivesToScan.Count() )
            {
            // No more drives or folders that we are interested in
            done = ETrue;
            }
        else
            {
            iDirScan->SetScanDataL(iDrivesToScan[0], KEntryAttNormal, ESortNone);
            iCount = 0;
            TInt err(KErrNone);
            do
                {
                MPX_DEBUG1("CMPXFolderScanner::SetupNextDriveToScanL iDirScan->NextL()");
                TRAP(err, iDirScan->NextL(iDir));
                MPX_DEBUG2("CMPXFolderScanner::SetupNextDriveToScanL path %S", &iDirScan->FullPath());
                blocked = iObserver.IsPathBlockedL( iDirScan->FullPath() );
                MPX_DEBUG2("CMPXFolderScanner::SetupNextDriveToScanL path blocked %i", blocked);
                if( blocked )
                    {
                    delete iDir;
                    iDir = NULL;
                    }
                }
            while (err == KErrPathNotFound || blocked );  
            
            // If there was something to scan
            //
            if( iDir != NULL )
                {
                // Inform Observer of the new drive that we are scanning
                iObserver.HandleOpenDriveL( ::ExtractDrive(iDrivesToScan[0]), 
                                            iDrivesToScan[0] );
                }
                
            // Remove the 0th element
            iDrivesToScan.Remove(0); 
            iDrivesToScan.Compress();
            }
        }
    
    
    MPX_DEBUG1("CMPXFolderScanner::SetupNextDriveToScanL --->");
    return done;
    }
    
// ---------------------------------------------------------------------------
// Handle when scanning is complete
// ---------------------------------------------------------------------------
//
void CMPXFolderScanner::DoScanCompleteL( TInt aErr )
    {
    MPX_DEBUG1("CMPXFolderScanner::DoScanCompleteL <---");
    
    // Reset all arrays and data
    iDrivesToScan.Reset();
    
    // All done!
    iScanning = EFalse;
    
    delete iDir;
    iDir = NULL;
    
    // Callback to observer
    iStateObserver.HandleScanStateCompleteL( MMPXFileScanStateObserver::EScanFiles, 
                                              aErr );
    
    MPX_DEBUG1("CMPXFolderScanner::DoScanCompleteL --->");
    }
     
// ---------------------------------------------------------------------------
// From CActive
// ---------------------------------------------------------------------------
//
void CMPXFolderScanner::RunL()
    {
    MPX_DEBUG1("CMPXFolderScanner::RunL <---");
    
    // Do more scanning
    TBool done(EFalse);
    TRAPD( err, done = DoScanL() );   
     
    // We are all done
    //
    if( KErrNone != err || done )
        {
        DoScanCompleteL( err );
        }
    else // if( !done )
        {
        MPX_DEBUG1("CMPXFolderScanner::RunL -- Run again");
        iStatus = KRequestPending;
        SetActive();
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, KErrNone );
        }
    }

// ---------------------------------------------------------------------------
// From CActive
// ---------------------------------------------------------------------------
//
void CMPXFolderScanner::DoCancel()
    {
    if( iScanning )
        {
        // Callback to observer with the partial list?
        TRAP_IGNORE( DoScanCompleteL( KErrCancel ) );
        }
    }
    
// ----------------------------------------------------------------------------
// Handles a leave occurring in the request completion event handler RunL()
// ----------------------------------------------------------------------------
//
TInt CMPXFolderScanner::RunError(TInt aError)
    {
    MPX_DEBUG2("CMPXFolderScanner::RunError(%d)", aError );
    
    TRAP_IGNORE( DoScanCompleteL( aError ) );
    
    return KErrNone;
    }        
