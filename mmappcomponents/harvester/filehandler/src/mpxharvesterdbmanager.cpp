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
* Description:  Harvester database manager
*
*/


#include <e32base.h>
#ifdef RD_MULTIPLE_DRIVE
#include <pathinfo.h>
#include <driveinfo.h>
#endif //RD_MULTIPLE_DRIVE
#include <mpxlog.h>
#include "mpxharvesterdbmanager.h"
#include "mpxharvesterdb.h"
#include "mpxharvesterdbtable.h"

// ---------------------------------------------------------------------------
// C++ Constructor
// ---------------------------------------------------------------------------
//
CMPXHarvesterDatabaseManager::CMPXHarvesterDatabaseManager( RFs& aFs )
                                                                  : iFs( aFs )
    {

    }


// ---------------------------------------------------------------------------
// 2nd Phase Constructor
// ---------------------------------------------------------------------------
//
void CMPXHarvesterDatabaseManager::ConstructL()
    {
    User::LeaveIfError( iDBSession.Connect() );
    }


// ---------------------------------------------------------------------------
// Two-Phased Constructor
// ---------------------------------------------------------------------------
//
CMPXHarvesterDatabaseManager* CMPXHarvesterDatabaseManager::NewL( RFs& aFs )
    {
    CMPXHarvesterDatabaseManager* self =
                             new( ELeave ) CMPXHarvesterDatabaseManager( aFs );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMPXHarvesterDatabaseManager::~CMPXHarvesterDatabaseManager()
    {
    iDatabases.ResetAndDestroy();
    iDatabases.Close();
    iDBSession.Close();
    }

// ---------------------------------------------------------------------------
// CMPXHarvesterDatabaseManager::OpenAllDatabasesL
// ---------------------------------------------------------------------------
//
TInt CMPXHarvesterDatabaseManager::OpenAllDatabasesL()
    {
    MPX_DEBUG1("CMPXHarvesterDatabaseManager::OpenAllDatabasesL <---");

    TInt rtn (KErrNone);

    // Open drives we are interested in.
    //
#ifdef RD_MULTIPLE_DRIVE
    TDriveList driveList;
    TInt driveCount(0);
    User::LeaveIfError( DriveInfo::GetUserVisibleDrives(
           iFs, driveList, driveCount ) );

    TInt check(KErrNone);
    for( TInt driveNum = EDriveA; driveNum <= EDriveZ; driveNum++ )
        {
        if (driveList[driveNum] && !IsRemoteDrive(static_cast<TDriveNumber>(driveNum)))
            {
            TFileName drivePath;
            User::LeaveIfError(
                PathInfo::GetRootPath( drivePath, driveNum ) );
            MPX_DEBUG2("CMPXHarvesterDatabaseManager::OpenAllDatabasesL: opening database in %S drive",
                &drivePath);
            TRAP( check, GetDatabaseL(static_cast<TDriveNumber>(driveNum)) );
            if( check == KErrNotFound )
                {
                CMPXHarvesterDB* dB = CMPXHarvesterDB::NewL(
                    static_cast<TDriveNumber>(driveNum), iFs );
                CleanupStack::PushL( dB );
                TRAPD(openError, rtn |= dB->OpenL() );  //lint !e665
                if(openError == KErrNone)
                    {
                     iDatabases.AppendL( dB );
                     CleanupStack::Pop( dB );
                    }
                else
                    {
                    CleanupStack::PopAndDestroy( dB );
                    }
                }
            else if( check == KErrNone )
                {
                TRAPD(openError, GetDatabaseL(static_cast<TDriveNumber>(driveNum)).OpenL() );
                if(openError != KErrNone)
                    {
                    TRAP_IGNORE( RemoveDatabaseL(static_cast<TDriveNumber>(driveNum)));
                    }
                }
            }
        }
#else
    TInt check(KErrNone);
    TRAP( check, GetDatabaseL(EDriveC) );
    if( check == KErrNotFound )
        {
        CMPXHarvesterDB* dB = CMPXHarvesterDB::NewL( EDriveC, iFs );
        CleanupStack::PushL( dB );
        iDatabases.AppendL( dB );
        CleanupStack::Pop( dB );
        TRAP_IGNORE( rtn = dB->OpenL() ); //lint !e665
        }
    else if( check == KErrNone )
        {
        TRAPD(openError, GetDatabaseL(EDriveC).OpenL() );
        if(openError != KErrNone)
            {
            TRAP_IGNORE( RemoveDatabaseL(EDriveC));
            }
        }
    TRAP( check, GetDatabaseL(EDriveE) );  //lint !e961
    if( check == KErrNotFound )
        {
        CMPXHarvesterDB* dB = CMPXHarvesterDB::NewL( EDriveE, iFs );
        CleanupStack::PushL( dB );
        TRAPD(openError, rtn |= dB->OpenL() );  //lint !e665
        if(openError == KErrNone)
            {
             iDatabases.AppendL( dB );
             CleanupStack::Pop( dB );
            }
        else
            {
            CleanupStack::PopAndDestroy( dB );
            }
        }
    else if( check == KErrNone )
        {
        TRAPD(openError,GetDatabaseL(EDriveE).OpenL() );
        if(openError != KErrNone)
            {
            TRAP_IGNORE( RemoveDatabaseL(EDriveE));
            }
        }
#endif // RD_MULTIPLE_DRIVE

    MPX_DEBUG1("CMPXHarvesterDatabaseManager::OpenAllDatabasesL --->");  //lint !e961
    return rtn;
    }

// ---------------------------------------------------------------------------
// CMPXHarvesterDatabaseManager::OpenDatabaseL
// ---------------------------------------------------------------------------
//
void CMPXHarvesterDatabaseManager::OpenDatabaseL( TDriveNumber aDrive )
    {
    MPX_DEBUG1("CMPXHarvesterDatabaseManager::OpenDatabaseL <---");

    // Re-open a specific database
    //
    if (!IsRemoteDrive(aDrive))
        {
    TInt count( iDatabases.Count() );
    for( TInt i=0; i<count; ++i )
        {
        CMPXHarvesterDB* db = (CMPXHarvesterDB*) iDatabases[i];
        if( db->GetDbDrive() == aDrive )
            {
            db->OpenL();
            break;
            }
        }
        }
    MPX_DEBUG1("CMPXHarvesterDatabaseManager::OpenDatabaseL --->");
    }

// ---------------------------------------------------------------------------
// CMPXHarvesterDatabaseManager::CloseAllDatabase
// ---------------------------------------------------------------------------
//
void CMPXHarvesterDatabaseManager::CloseAllDatabase()
    {
    // Close all databases for shutdown
    iDatabases.ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
// CMPXHarvesterDatabaseManager::CloseDatabase
// ---------------------------------------------------------------------------
//
void CMPXHarvesterDatabaseManager::CloseDatabase( TDriveNumber aDrive )
    {
     if (!IsRemoteDrive(aDrive))
        {
        MPX_DEBUG2("CMPXHarvesterDatabaseManager::CloseDatabase drive %d <---", aDrive );
        TInt count = iDatabases.Count();
        for ( TInt i=0; i<count; ++i)
            {
            CMPXHarvesterDB* db = (CMPXHarvesterDB*) iDatabases[i];
            if ( db->GetDbDrive() == aDrive)
                {
                db->Close();
                break;
                }
            }
        }
    MPX_DEBUG1("CMPXHarvesterDatabaseManager::CloseDatabase --->");
    }

// ---------------------------------------------------------------------------
// CMPXHarvesterDatabaseManager::GetDatabase
// ---------------------------------------------------------------------------
//
CMPXHarvesterDB& CMPXHarvesterDatabaseManager::GetDatabaseL( TDriveNumber aDrive )
    {
    CMPXHarvesterDB* db( NULL );

    // Find the database
    TInt count = iDatabases.Count();
    for( TInt i=0; i<count; ++i )
        {
        CMPXHarvesterDB* tmp = (CMPXHarvesterDB*) iDatabases[i];
        if( tmp->GetDbDrive() == aDrive )
            {
            db = tmp;
            break;
            }
        }

    // Not found, so we leave
    if( db == NULL )
        {
        User::Leave( KErrNotFound );
        }
    return *db;
    }

// ---------------------------------------------------------------------------
// CMPXHarvesterDatabaseManager::RemoveDatabase
// ---------------------------------------------------------------------------
//
void CMPXHarvesterDatabaseManager::RemoveDatabaseL( TDriveNumber aDrive )
    {

    TBool bFound(EFalse);
    // Find the database
    TInt count = iDatabases.Count();
    for(TInt index=0; index<count; ++index )
        {
        if((iDatabases[index]!=NULL) && ( iDatabases[index]->GetDbDrive() == aDrive ))
            {
            bFound = ETrue;
            delete iDatabases[index];
            iDatabases.Remove(index);
            break;
            }
        }

    // Not found, so we leave
    if( !bFound )
        {
        User::Leave( KErrNotFound );
        }
    }

// ---------------------------------------------------------------------------
// CMPXHarvesterDatabaseManager::Count
// ---------------------------------------------------------------------------
//
TInt CMPXHarvesterDatabaseManager::Count()
    {
    return iDatabases.Count();
    }

// ---------------------------------------------------------------------------
// CMPXHarvesterDatabaseManager::GetDatabaseL
// ---------------------------------------------------------------------------
//
CMPXHarvesterDB& CMPXHarvesterDatabaseManager::GetDatabaseL( TInt aDb )
    {
    if( aDb > iDatabases.Count() )
        {
        User::Leave(KErrNotFound/*KErrArgument*/);
        }
    return *iDatabases[aDb];
    }

// ---------------------------------------------------------------------------
// CMPXHarvesterDatabaseManager::RecreateDatabase
// ---------------------------------------------------------------------------
//
void CMPXHarvesterDatabaseManager::RecreateDatabases()
    {
    MPX_DEBUG1("CMPXHarvesterDatabaseManager::RecreateDatabases <--");
    TInt count( iDatabases.Count() );
    for( TInt i=0; i<count; ++i )
        {
        // Close db, delete and recreate
        //
        MPX_DEBUG2("RecreateDatabasesL() -- %i", i);
        CMPXHarvesterDB* cur = (CMPXHarvesterDB*)iDatabases[i];
        cur->Close();
        cur->DeleteDatabase();
        // trap leave just in case 1 db had err
        //
        TRAP_IGNORE( cur->OpenL() );
        }
    }


// ---------------------------------------------------------------------------
// CMPXHarvesterDatabaseManager::IsRemoteDrive
// ---------------------------------------------------------------------------
//
TBool CMPXHarvesterDatabaseManager::IsRemoteDrive(TDriveNumber aDrive)
    {
    TDriveInfo driveInfo;
    TBool isRemoteDrive(EFalse);
    if (iFs.Drive(driveInfo, aDrive) == KErrNone)
        {
        isRemoteDrive = driveInfo.iDriveAtt & KDriveAttRemote;
        }
    return isRemoteDrive;
    }

// ---------------------------------------------------------------------------
// CMPXHarvesterDatabaseManager::BeginL
// ---------------------------------------------------------------------------
// 
void CMPXHarvesterDatabaseManager::BeginL()
    {
    MPX_DEBUG1("CMPXHarvesterDatabaseManager::BeginL <--");
    TInt count( iDatabases.Count() );
    for( TInt i=0; i<count; ++i )
        {
        CMPXHarvesterDB* db = (CMPXHarvesterDB*) iDatabases[i];
        if(!db->InTransaction())
             {
        TRAPD(err, db->BeginL());
        User::LeaveIfError(err);
             }
        }
    MPX_DEBUG1("CMPXHarvesterDatabaseManager::BeginL -->");
    }

// ---------------------------------------------------------------------------
// CMPXHarvesterDatabaseManager::CommitL
// ---------------------------------------------------------------------------
//
void CMPXHarvesterDatabaseManager::CommitL()
    {
    MPX_DEBUG1("CMPXHarvesterDatabaseManager::CommitL <--");
    TInt count( iDatabases.Count() );
    for( TInt i=0; i<count; ++i )
        {
        CMPXHarvesterDB* db = (CMPXHarvesterDB*) iDatabases[i];
        
        // commit only on databases that are in transaction
        if(db->InTransaction())
            {
            TRAPD(err, db->CommitL());
            User::LeaveIfError(err);
            }
        }
    MPX_DEBUG1("CMPXHarvesterDatabaseManager::CommitL -->");
    }

// ---------------------------------------------------------------------------
// CMPXHarvesterDatabaseManager::Rollback
// ---------------------------------------------------------------------------
//
void CMPXHarvesterDatabaseManager::Rollback()
    {
    MPX_DEBUG1("CMPXHarvesterDatabaseManager::Rollback <--");
    TInt count( iDatabases.Count() );
    for( TInt i=0; i<count; ++i )
        {
        CMPXHarvesterDB* db = (CMPXHarvesterDB*) iDatabases[i];
        
        // rollback only on databases that are in transaction
        if(db->InTransaction())
            {
            db->Rollback();
            }
        }      
    MPX_DEBUG1("CMPXHarvesterDatabaseManager::Rollback -->");
    }

// END OF FILE

