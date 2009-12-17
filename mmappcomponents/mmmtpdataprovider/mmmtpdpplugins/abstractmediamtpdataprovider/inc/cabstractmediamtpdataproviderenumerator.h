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


#ifndef CABSTRACTMEDIAMTPDATAPROVIDERENUMERATOR_H
#define CABSTRACTMEDIAMTPDATAPROVIDERENUMERATOR_H

#include <e32base.h>
#include <d32dbms.h>

#include "mmmtpenumerationcallback.h"
#include "cmmmtpdpperflog.h"

// keep here to avoid warning on urel
_LIT( KMpxGetAllPlaylist, "MpxGetAllPlaylist" );
_LIT( KMpxGetPlaylistName, "MpxGetPlaylistName" );
_LIT( KMpxQueryPlaylistReference, "MpxQueryPlaylistReference" );
_LIT( KObjectManagerObjectUid, "ObjectManagerObjectUid" );
_LIT( KObjectManagerInsert, "ObjectManagerInsert" );
_LIT( KObjectManagerHandle, "ObjectManagerHandle" );
_LIT( KReferenceManagerSetReference, "ReferenceManagerSetReference" );

// Forward declarations
class MMTPDataProviderFramework;
class MMTPObjectMgr;
class CAbstractMediaMtpDataProvider;
class CMTPObjectMetaData;
class CMmMtpDpMetadataAccessWrapper;
class CMPXMediaArray;

/**
* Defines file enumerator.  Enumerates all files/directories under a
* specified path or storage
*/
class CAbstractMediaMtpDataProviderEnumerator: public CActive
    {
public:
    /**
    * Two phase constructor
    * @param aFramework Reference to MMTPDataProviderFramework
    * @param aDataProvider Reference to Abstract Media DataProvider
    * @return A pointer to a new instance of the object
    */
    static CAbstractMediaMtpDataProviderEnumerator* NewL( MMTPDataProviderFramework& aFramework,
        CAbstractMediaMtpDataProvider& aDataProvider );

    /**
    * destructor
    */
    ~CAbstractMediaMtpDataProviderEnumerator();

    /**
    * Kick off the enumeration on the specified storage
    * @param aStorageId storage to be enumerated
    */
    void StartL( TUint32 aStorageId );

protected: // from CActive
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
    * @param aDataProvider Reference to Abstract Media DataProvider
    */
    CAbstractMediaMtpDataProviderEnumerator( MMTPDataProviderFramework& aFramework,
        CAbstractMediaMtpDataProvider& aDataProvider );

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
    void ScanNextL();

    /**
    * Add a file entry to the object store
    * @param aPath current scanned file path
    */
    void AddEntryL( const TDesC& aSuid );

    /**
    * Add references of specified abstract media into reference db
    * @param aAbstractMediaName, specify the abstract media of which references should be added into db
    * @param aReferences, suid array which stored references
    */
    void AddReferencesL( const TDesC& aAbstractMediaName, CDesCArray& aReferences );

private:
    // Owned
    /** MMTPDataProviderFramework used to access MTP components*/
    MMTPDataProviderFramework& iFramework;

    /** MTP Object Manager */
    MMTPObjectMgr& iObjectMgr;

    /** MTP Data Provider ID */
    TUint iDataProviderId;

    /** Provides Callback mechanism for completion of enumeration*/
    CAbstractMediaMtpDataProvider& iDataProvider;

    TParse iPath;
    TEntryArray iEntries;
    TInt iFirstUnprocessed;
    RArray<TUint> iStorages;
    TUint32 iStorageId;
    TUint32 iParentHandle;

    CMPXMediaArray* iAbstractMedias;
    TInt iCount;
    TInt iCurrentIndex;

#if defined(_DEBUG) || defined(MMMTPDP_PERFLOG)
    CMmMtpDpPerfLog* iPerfLog;
#endif

    };

#endif // CABSTRACTMEDIAMTPDATAPROVIDERENUMERATOR_H
