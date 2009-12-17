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
* Description:  Active object to extract metadata 
*  Version     : %version: da1mmcf#16.2.3.1.5 % 
*
*/


#include <e32base.h>
#include <f32file.h>
#include <badesca.h>
#include <apgcli.h>
#include <mpxmedia.h>
#include <mpxmediaarray.h>
#include <mpxcollectionpath.h>
#include <mpxcollectiontype.h>
#include <mpxdrmmediautility.h>
#include <mpxlog.h>
#include <mpxmetadataextractor.h>
#include "mpxmetadatascanner.h"
#include "mpxmetadatascanobserver.h"
#include "mpxfilescanstateobserver.h"

// CONSTANTS
const TInt KLoopCount = 10;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Private Constructor
// ---------------------------------------------------------------------------
//
CMPXMetadataScanner::CMPXMetadataScanner( MMPXMetadataScanObserver& aObs,
                                          MMPXFileScanStateObserver& aStateObs )
                                      : CActive( EPriorityNull ),
                                        iObserver( aObs ),
                                        iStateObserver( aStateObs ) 
    {
    CActiveScheduler::Add( this );
    }


// ---------------------------------------------------------------------------
// 2nd Phase Constructor
// ---------------------------------------------------------------------------
//
void CMPXMetadataScanner::ConstructL( RFs& aFs, 
                                      RApaLsSession& aAppArc,
                                      RPointerArray<CMPXCollectionType>& aTypesAry )
    {
    iExtractor = CMPXMetadataExtractor::NewL( aFs, aAppArc, aTypesAry );
    iNewFileProps = CMPXMediaArray::NewL();
    iModifiedFileProps = CMPXMediaArray::NewL();
    }


// ---------------------------------------------------------------------------
// Two-Phased Constructor
// ---------------------------------------------------------------------------
//
CMPXMetadataScanner* CMPXMetadataScanner::NewL( RFs& aFs,
                                 RApaLsSession& aAppArc,
                                 RPointerArray<CMPXCollectionType>& aTypesAry,
                                 MMPXMetadataScanObserver& aObs,
                                 MMPXFileScanStateObserver& aStateObs )
    {
    CMPXMetadataScanner* self = CMPXMetadataScanner::NewLC( aFs,
                                                            aAppArc,
                                                            aTypesAry, 
                                                            aObs,
                                                            aStateObs );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Two-Phased Constructor
// ---------------------------------------------------------------------------
//
CMPXMetadataScanner* CMPXMetadataScanner::NewLC( RFs& aFs,
                                  RApaLsSession& aAppArc,
                                  RPointerArray<CMPXCollectionType>& aTypesAry,
                                  MMPXMetadataScanObserver& aObs,
                                  MMPXFileScanStateObserver& aStateObs )
    {
    CMPXMetadataScanner* self = new( ELeave ) CMPXMetadataScanner( aObs,
                                                                   aStateObs );
    CleanupStack::PushL( self );
    self->ConstructL( aFs, aAppArc, aTypesAry );
    return self;
    }


// ---------------------------------------------------------------------------
// Virtual Destructor
// ---------------------------------------------------------------------------
//
CMPXMetadataScanner::~CMPXMetadataScanner()
    {
    Cancel();
    Reset();
    iNewFiles.Close();
    iModifiedFiles.Close();
    
    delete iNewFileProps;
    delete iModifiedFileProps;
   
    delete iExtractor;
    }
    
// ---------------------------------------------------------------------------
// Resets this object
// ---------------------------------------------------------------------------
//
void CMPXMetadataScanner::Reset()
    {
    iNewFiles.ResetAndDestroy();
    iModifiedFiles.ResetAndDestroy();
    if(iNewFileProps)
        {
    iNewFileProps->Reset();
        }
    if(iModifiedFileProps)
        {
    iModifiedFileProps->Reset();
        }
    }

// ---------------------------------------------------------------------------
// Starts the metadata extraction process
// ---------------------------------------------------------------------------
//    
void CMPXMetadataScanner::Start()
    {
    MPX_DEBUG1("MPXMetadataScanner::StartL <---");
    if( !IsActive() )
        {
        // Setup
        iAryPos = 0;
        iExtractType = ENewFiles;
        iExtracting = ETrue;        
        
        // Set Active
        iStatus = KRequestPending;
        SetActive();
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, KErrNone );    
        MPX_DEBUG1("MPXMetadataScanner::StartL --->");
        }
    }
// ---------------------------------------------------------------------------
// Stops the metadata extraction process
// ---------------------------------------------------------------------------
//
void CMPXMetadataScanner::Stop()
    {
    MPX_DEBUG1("MPXMetadataScanner::Stop <---");
    DoCancel();
    MPX_DEBUG1("MPXMetadataScanner::Stop --->");
    }

// ---------------------------------------------------------------------------
// Add a file to the scanning list
// ---------------------------------------------------------------------------
//  
void CMPXMetadataScanner::AddNewFileToScanL( const TDesC& aFile )
    {
    HBufC* file = aFile.AllocLC();
    iNewFiles.AppendL( file );
    CleanupStack::Pop( file );
    }

// ---------------------------------------------------------------------------
// Add a file to the modified list
// ---------------------------------------------------------------------------
//      
void CMPXMetadataScanner::AddModifiedFileToScanL( const TDesC& aFile )
    {
    HBufC* file = aFile.AllocLC();
    iModifiedFiles.AppendL( file );
    CleanupStack::Pop( file );
    }

// ---------------------------------------------------------------------------
// Extract media properties for a file
// ---------------------------------------------------------------------------
//
CMPXMedia* CMPXMetadataScanner::ExtractFileL( const TDesC& aFile )
    {
    CMPXMedia* media;
    iExtractor->CreateMediaL( aFile, media );
    return media;
    }
    
// ---------------------------------------------------------------------------
// Cancel the operation
// ---------------------------------------------------------------------------
//   
void CMPXMetadataScanner::DoCancel()
    {
    if( iExtracting )
        {
        // Callback to observer 
        Reset();
        TRAP_IGNORE( iStateObserver.HandleScanStateCompleteL( MMPXFileScanStateObserver::EScanMetadata,
                                                              KErrCancel ) );
        iExtracting = EFalse;
        }    
    }
    
// ---------------------------------------------------------------------------
// RunL function
// ---------------------------------------------------------------------------
//   
void CMPXMetadataScanner::RunL()
    {
    if ( iExtracting )
        {
        TBool done(EFalse);
        TRAPD( err, done = DoExtractL() );
        if ( !iExtracting )
            {
            // If DoCancel() was called during DoExtractL(), do nothing.
            MPX_DEBUG1("CMPXMetadataScanner::RunL - Cancel during RunL");
            }
        else if( KErrNone != err || done )
            {
            // Callback to observer 
            TRAP_IGNORE( iStateObserver.HandleScanStateCompleteL( MMPXFileScanStateObserver::EScanMetadata,
                                                              err ) );
            iExtracting = EFalse;
            }
        else
            {
            MPX_DEBUG1("CMPXMetadataScanner::RunL -- Run again");
            iStatus = KRequestPending;
            SetActive();
            TRequestStatus* status = &iStatus;
            User::RequestComplete( status, KErrNone );    
            }
        }
    }

// ---------------------------------------------------------------------------
// Extract metadata
// ---------------------------------------------------------------------------
//   
TBool CMPXMetadataScanner::DoExtractL()
    {
    MPX_DEBUG1("CMPXMetadataScanner::DoExtractL <---");
    TBool done(EFalse);
    TExtractType curType = (TExtractType) iExtractType;
    
    // Pointer re-direction to generalize extraction
    RPointerArray<HBufC>* source;
    CMPXMediaArray* mptarget;
    if( curType == ENewFiles )
        {
        source = &iNewFiles;
        mptarget = iNewFileProps;
        }
    else if( curType == EModFiles )
        {
        source = &iModifiedFiles;
        mptarget = iModifiedFileProps;
        }
    else // All done!
        {
        return ETrue;    
        }
    
    // Process at most KLoopCount number of files 
    //
    mptarget->Reset();
    if( source->Count() != 0 )
        {
        for( TInt i=0; i<KLoopCount; ++i )
            {
            CMPXMedia* media(NULL);
            
            // TRAP to keep scanning if 1 file fails 
            TRAPD( err, iExtractor->CreateMediaL( *(*source)[iAryPos], media ) );
            if ( !iExtracting )
                {
                // In case DoCancel() was called while processing iExtractor->CreateMediaL
                MPX_DEBUG1("CMPXMetadataScanner::DoExtractL - Cancel during CreateMediaL");
                delete media;
                return ETrue;
                }
            
            if( err == KErrNone )
                {
                CleanupStack::PushL( media );
                mptarget->AppendL( media );
                CleanupStack::Pop( media );
                }
            
            iAryPos++;
            if( iAryPos == source->Count() )
                {
                iAryPos = 0;
                iExtractType++;
                break;
                }
            }    
        }
    else // No item in the array
        {
        iAryPos = 0;
        iExtractType++;    
        }
    
    // After extraction, get observer to add files to the collection
    //
    switch( curType )
        {
        case ENewFiles:
            {
            if( iNewFileProps->Count() )
                {
                iObserver.AddFilesToCollectionL( *iNewFileProps );
                }
            break;
            }
        case EModFiles:
            {
            if( iModifiedFileProps->Count() )
                {
                iObserver.UpdatesFilesInCollectionL( *iModifiedFileProps );
                }
            break;
            }
        case EMaxFile:  // All done.
            done = ETrue;
            break;
        default:
            ASSERT(0); 
        }
        
    MPX_DEBUG1("CMPXMetadataScanner::DoExtractL --->");    
    return done; 
    }
