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
* Description:  MMC Eject monitor
*
*/


#ifndef C_MPXMMCEJECTMONITOR_H
#define C_MPXMMCEJECTMONITOR_H

#include <mpxpskeyobserver.h>
#include "mpxsystemeventobserver.h"

// FORWARD DECLARATIONS
class CMPXPSKeyWatcher;

/**
 *  Monitors a SysAp cenrep key for the "eject" mmc menu option
 *
 *  @lib mpxharvester.exe
 *  @since S60 3.1
 */
NONSHARABLE_CLASS( CMPXMMCEjectMonitor ) : public CBase,
                                           public MMPXPSKeyObserver
    {

public:

    /**
    * Two-phased constructor
    */
    static CMPXMMCEjectMonitor* NewL( MMPXSystemEventObserver& aObserver );

    /**
    * Destructor
    */
    virtual ~CMPXMMCEjectMonitor();

protected: // From Base Class

    /**
    * From MMPXPSKeyObserver
    */
    void HandlePSEvent( TUid aUid, TInt aKey );

private:
    
    /**
    * 2nd phase constructor
    */
    void ConstructL();
    
    /**
    * Private Constructor
    */
    CMPXMMCEjectMonitor( MMPXSystemEventObserver& aObserver );
    
private: // data

    CMPXPSKeyWatcher*  iPSKeyWatcher;  // PS key watcher for USB PS key
   
    /*
    * Observer interface to callback to an observer
    */
    MMPXSystemEventObserver& iObserver; // Not owned
    };

#endif // C_MPXMMCEJECTMONITOR_H
