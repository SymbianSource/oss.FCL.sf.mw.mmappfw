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
* Description:  Enumerator objects
*
*/


#ifndef CMEDIAMTPDATAPROVIDERENUMERATOR_H
#define CMEDIAMTPDATAPROVIDERENUMERATOR_H

#include <e32base.h>
#include <d32dbms.h>

#include "mmmtpenumerationcallback.h"
#include "cmmmtpdpperflog.h"

// Forward declarations
class MMTPDataProviderFramework;
class MMTPObjectMgr;
class CMTPObjectMetaData;
class CMediaMtpDataProvider;

/**
* Defines file enumerator.  Enumerates all files/directories under a
* specified path or storage
*/
class CMediaMtpDataProviderEnumerator: public CActive
    {
public:
    /**
    * Two phase constructor
    * @param aFramework Reference to MMTPDataProviderFramework
    * @param aDataProvider Reference to CMediaMtpDataProvider
    * @return A pointer to a new instance of the object
    */
    static CMediaMtpDataProviderEnumerator* NewL( MMTPDataProviderFramework& aFramework,
        CMediaMtpDataProvider& aDataProvider );

    /**
    * destructor
    */
    ~CMediaMtpDataProviderEnumerator();

    /**
    * Kick off the enumeration on the specified storage
    * @param aStorageId storage to be enumerated
    */
    void StartL( TUint32 aStorageId );

    // introduce to cleanup db at close session
    void SessionClosedL();

protected:
    // from CActive
    /**
    * Cancel the enumeration process
    */
    void DoCancel();

    void RunL();

    /**
    * Ignore the error, continue with the next one
    */
    TInt RunError( TInt aError );

private:
    /**
    * Standard C++ Constructor
    * @param aFramework Reference to MMTPDataProviderFramework
    * @param aDataProvider Reference to CMediaMtpDataProvider
    */
    CMediaMtpDataProviderEnumerator( MMTPDataProviderFramework& aFramework,
        CMediaMtpDataProvider& aDataProvider );

    void ConstructL();

    /**
    * Called when the enumeration is completed
    */
    void SignalCompleteL( MMTPEnumerationCallback& aCallback,
        TInt aError = KErrNone );

    void ScanStorageL( TUint32 aStorageId );

    /**
    * Scan next storage
    */
    void ScanNextStorageL();

    /**
    * Scans directory at aPath recursing into subdirectories on a depth
    * first basis.
    *
    * Directory entries are kept in iDirStack - which is a LIFO stack.
    * The current path, needed since TEntries don't keep track of it,
    * is kept in iPath.
    *
    * The algorithm works as follows:
    *
    * 1. Read directory entries.
    * 2. ProcessEntriesL is called if no error occurs and >= 1 entries are
    *    read.
    * 3. ProcessEntriesL adds entries to database, if entry is directory
    *    add to iDirStack.
    * 4. When all entries are processed pop entry off the dirstack,
    *    if entry is empty TEntry remove one directory from iPath.
    * 5. Append entry name onto iPath - to update path with new depth
    *    (parent/subdir).
    * 6. Push an empty TEntry onto iDirStack - this tells us we have
    *    recursed one, think of it as pushing the '\' separator onto
    *    iDirStack.
    * 7. Repeat 1-7 until iDirStack is empty.
    */
    void ScanDirL();

    /**
    * Recurse into the next directory on the stack
    * and scan it for entries.
    */
    void ScanNextDirL();

    /**
    * Scan next sub dir
    */
    void ScanNextSubdirL();

    /**
    * Iterates iEntries adding entries as needed to object manager and
    *  iDirStack.
    */
    void ProcessEntriesL();

    /**
    * Add a file entry to the object store
    * @param aFullFileName current scanned file path
    */
    void AddEntryL( const TDesC& aFullFileName );

    /**
    * Returns a TMTPFormatCode for the requested object
    */
    TMTPFormatCode GetObjectFormatCode( const TDesC& aFullFileName );

    /**
    * @param aFullFileName the object file name
    * @return the file if needed to be enumeratored?
    */
    TBool IsFileAccepted( const TDesC& aFullFileName );

    /**
    * start count system tick
    */
    void StartEnumerationCount();

    /**
    * check if the system tick more than predefined value
    * @retuan if more than the predefined value, return true
    *         else return false
    */
    TBool IsOverThreshold();

    /**
    * Get modified content and report to Symbian MTP Framework
    */
    void GetModifiedContentL( const TDesC& aStorageRoot );

private:
    // Owned
    /** MMTPDataProviderFramework used to access MTP components*/
    MMTPDataProviderFramework& iFramework;

    /** MTP Object Manager */
    MMTPObjectMgr& iObjectMgr;

    /** MTP Data Provider ID */
    TUint iDataProviderId;

    CMediaMtpDataProvider& iDataProvider;
    TParse iPath;
    RDir iDir;
    TEntryArray iEntries;
    TInt iFirstUnprocessed;
    RPointerArray<TEntry> iDirStack;
    RArray<TUint> iStorages;
    TUint32 iStorageId;
    TUint32 iParentHandle;
    TMTPFormatCode iFormatCode;
    TBool iScanningDir;

    // record the system tick
    TUint iTickCountBegin;
    TInt iTickPeriod;

#if defined(_DEBUG) || defined(MMMTPDP_PERFLOG)
    CMmMtpDpPerfLog* iPerfLog;
#endif

    };

#endif // CMEDIAMTPDATAPROVIDERENUMERATOR_H
