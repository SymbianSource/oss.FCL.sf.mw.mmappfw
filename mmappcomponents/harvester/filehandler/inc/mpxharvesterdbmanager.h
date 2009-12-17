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


#ifndef CMPXHARVESTERDBMANAGER_H
#define CMPXHARVESTERDBMANAGER_H

#include <e32base.h>
#include <d32dbms.h>
#include "mpxharvesterdb.h"

/**
 *  CMPXHarvesterDatabaseManager
 *
 *  Harvester database manager
 *
 *  @lib harvesterfilehandler.lib
 *  @since S60 3.0
 */
NONSHARABLE_CLASS( CMPXHarvesterDatabaseManager ) : public CBase
    {

public:

    /**
    * Two phased constructor
    * @param aFs file session
    * @return CMPXHarvesterDatabasemanager session
    */
    static CMPXHarvesterDatabaseManager* NewL( RFs& aFs );

    /**
    * Virtual destructor
    */
    virtual ~CMPXHarvesterDatabaseManager();

    /**
    * Open All databases
    */
    TInt OpenAllDatabasesL();

    /**
    * Reopen a particular database
    * (For MMC events)
    * @param TDriveNumber aDrive
    */
    void OpenDatabaseL( TDriveNumber aDrive );

    /**
    * Close all databases
    */
    void CloseAllDatabase();

    /**
    * Close a particular DB
    * (For MMC events)
    * @param TDriveNumber the Drive
    */
    void CloseDatabase( TDriveNumber aDrive );

    /**
    * Get a particular database
    * @param TDriveNumber the Drive
    */
    CMPXHarvesterDB& GetDatabaseL( TDriveNumber aDrive );

    /**
     * Remove a particular database from the array
     * @param TDriveNumber the Drive
     */
    void RemoveDatabaseL( TDriveNumber aDrive );

    /**
    * Return the number of databases
    * @return number of databases
    */
    TInt Count();

    /**
    * Get the n'th database
    * @param aDb index to the database
    * @return CMPXHarvesterDB reference
    */
    CMPXHarvesterDB& GetDatabaseL( TInt aDb );

    /**
    * Recreate all databases
    * Deletes old ones and re-creates
    */
    void RecreateDatabases();
    
    /**
    * Begin transaction on all databases
    */
    void BeginL();
    
    /**
    * Commit transaction on all databases
    */
    void CommitL();
    
    /**
    * Rollbacks the current transaction on all databases
    */
    void Rollback();
    
    /**
     * Checks if the spefified drive is a remove drive
     */      
    TBool IsRemoteDrive(TDriveNumber aDrive);
      
private:

    /**
    * Private constructor
    */
    CMPXHarvesterDatabaseManager( RFs& aFs );

    /**
    * Second phase constructor
    */
    void ConstructL();

private: // data
    RDbs                             iDBSession;
    RPointerArray<CMPXHarvesterDB>   iDatabases;

    RFs&                             iFs;   // Not Owned
    };


#endif // CMPXHARVESTERDBMANAGER_H
