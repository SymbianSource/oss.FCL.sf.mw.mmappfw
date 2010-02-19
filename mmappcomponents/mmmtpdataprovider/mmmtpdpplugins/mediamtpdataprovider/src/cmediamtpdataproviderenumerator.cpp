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


#include <mtp/mmtpdataproviderframework.h>
#include <mtp/mmtpobjectmgr.h>
#include <mtp/mmtpstoragemgr.h>
#include <mtp/cmtpobjectmetadata.h>
#include <hal.h>

#include "mediamtpdataproviderconst.h"
#include "cmediamtpdataproviderenumerator.h"
#include "cmediamtpdataprovider.h"
#include "mmmtpdplogger.h"
#include "mmmtpdputility.h"
#include "cmmmtpdpaccesssingleton.h"
#include "cmmmtpdpmetadataaccesswrapper.h"


#if defined(_DEBUG) || defined(MMMTPDP_PERFLOG)
_LIT( KDirectoryScan, "DirectoryScan" );
_LIT( KFormatFilter, "FormatFilter" );
_LIT( KObjectManagerObjectUid, "ObjectManagerObjectUid" );
_LIT( KObjectManagerInsert, "ObjectManagerInsert" );
#endif

// Unit: microsecond
const TInt KThresholdOfEnumerationLoopDuration = 1000 * 1000; // microsecond

const TInt KMTPDriveGranularity = 5;
const TInt KMediaDpArrayGranularity = 2;

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderEnumerator::NewL
// Two phase constructor
// -----------------------------------------------------------------------------
//
CMediaMtpDataProviderEnumerator* CMediaMtpDataProviderEnumerator::NewL( MMTPDataProviderFramework& aFramework,
    CMediaMtpDataProvider& aDataProvider )
    {
    PRINT( _L( "MM MTP => CMediaMtpDataProviderEnumerator::NewL" ) );

    CMediaMtpDataProviderEnumerator* self = new ( ELeave ) CMediaMtpDataProviderEnumerator( aFramework, aDataProvider );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    PRINT( _L( "MM MTP <= CMediaMtpDataProviderEnumerator::NewL" ) );
    return self;
    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderEnumerator::CMediaMtpDataProviderEnumerator
// Standard C++ Constructor
// -----------------------------------------------------------------------------
//
CMediaMtpDataProviderEnumerator::CMediaMtpDataProviderEnumerator( MMTPDataProviderFramework& aFramework,
    CMediaMtpDataProvider& aDataProvider ) :
        CActive( EPriorityLow ), // EPriorityStandard ? sure?
        iFramework( aFramework ),
        iObjectMgr( aFramework.ObjectMgr() ),
        iDataProviderId( aFramework.DataProviderId() ),
        iDataProvider( aDataProvider ),
        iDirStack( KMediaDpArrayGranularity ),
        iStorages( KMediaDpArrayGranularity ),
        iScanningDir( EFalse )
    {
    PRINT1( _L( "MM MTP <> CMediaMtpDataProviderEnumerator::CMediaMtpDataProviderEnumerator, iDataProviderId = %d" ), iDataProviderId );
    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderEnumerator::~CMediaMtpDataProviderEnumerator
// destructor
// -----------------------------------------------------------------------------
//
CMediaMtpDataProviderEnumerator::~CMediaMtpDataProviderEnumerator()
    {
    PRINT( _L( "MM MTP => CMediaMtpDataProviderEnumerator::~CMediaMtpDataProviderEnumerator" ) );
    Cancel();
    iDir.Close();
    iDirStack.Close();
    iStorages.Close();

#if defined(_DEBUG) || defined(MMMTPDP_PERFLOG)
    delete iPerfLog;
#endif
    PRINT( _L( "MM MTP <= CMediaMtpDataProviderEnumerator::~CMediaMtpDataProviderEnumerator" ) );
    }

// -----------------------------------------------------------------------------
// MediaMtpDataProviderEnumerator::StartL
// Kick off the enumeration on the specified storage
// -----------------------------------------------------------------------------
//
void CMediaMtpDataProviderEnumerator::StartL( TUint32 aStorageId )
    {
    PRINT1( _L( "MM MTP => CMediaMtpDataProviderEnumerator::StartL aStorageId = 0x%x" ), aStorageId );

    MMTPStorageMgr& storageMgr( iFramework.StorageMgr() );
    if ( aStorageId == KMTPStorageAll )
        {
        // Retrieve the available logical StorageIDs
        RPointerArray<const CMTPStorageMetaData> storages;
        CleanupClosePushL( storages ); // + storages
        TMTPStorageMgrQueryParams params( KNullDesC,
            CMTPStorageMetaData::ESystemTypeDefaultFileSystem );

        storageMgr.GetLogicalStoragesL( params, storages );

        // Construct the StorageIDs list.
        for ( TInt i = 0; i < storages.Count(); i++ )
            {
            iStorages.AppendL( storages[i]->Uint( CMTPStorageMetaData::EStorageId ) );
            }
        CleanupStack::PopAndDestroy( &storages ); // - storages
        }
    else if ( aStorageId != KMTPNotSpecified32 )
        {
        __ASSERT_DEBUG( storageMgr.ValidStorageId( aStorageId ), User::Invariant() );

        const CMTPStorageMetaData& storage( storageMgr.StorageL( aStorageId ) );

        if ( storage.Uint( CMTPStorageMetaData::EStorageSystemType )
            == CMTPStorageMetaData::ESystemTypeDefaultFileSystem )
            {
            if ( storageMgr.LogicalStorageId( aStorageId ) )
                {
                // Logical StorageID.
                iStorages.AppendL( aStorageId );
                }
            else
                {
                // Physical StorageID. Enumerate all eligible logical storages.
                const RArray<TUint>& logicalIds( storage.UintArray( CMTPStorageMetaData::EStorageLogicalIds ) );

                for ( TInt i = 0; i < logicalIds.Count(); i++ )
                    {
                    iStorages.AppendL( logicalIds[i] );
                    }
                }
            }
        }

    iStorageId = aStorageId;

    if ( iStorages.Count() > 0 )
        {
        ScanStorageL( iStorages[0] );
        }
    else
        {
        SignalCompleteL( iDataProvider );
        }

    PRINT( _L( "MM MTP <= CMediaMtpDataProviderEnumerator::StartL" ) );
    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderEnumerator::DoCancel()
// Cancel the enumeration process
// -----------------------------------------------------------------------------
//
void CMediaMtpDataProviderEnumerator::DoCancel()
    {
    iDir.Close();
    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderEnumerator::ScanStorageL
//
// -----------------------------------------------------------------------------
//
void CMediaMtpDataProviderEnumerator::ScanStorageL( TUint32 aStorageId )
    {
    const CMTPStorageMetaData& storage( iFramework.StorageMgr().StorageL( aStorageId ) );

    __ASSERT_DEBUG( ( storage.Uint( CMTPStorageMetaData::EStorageSystemType ) == CMTPStorageMetaData::ESystemTypeDefaultFileSystem ),
        User::Invariant() );

    TFileName root( storage.DesC( CMTPStorageMetaData::EStorageSuid ) );
    PRINT2( _L("MM MTP <> CMediaMtpDataProviderEnumerator::ScanStorageL aStorageId = 0x%x, StorageSuid = %S"), aStorageId, &root );

    iParentHandle = KMTPHandleNoParent;
    iPath.Set( root, NULL, NULL);
    iDir.Close();
    User::LeaveIfError( iDir.Open( iFramework.Fs(),
        iPath.DriveAndPath(),
        KEntryAttNormal | KEntryAttDir ) );
    ScanDirL();
    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderEnumerator::ScanNextStorageL
//
// -----------------------------------------------------------------------------
//
void CMediaMtpDataProviderEnumerator::ScanNextStorageL()
    {
    // If there are one or more unscanned storages left
    // (the currently scanned one is still on the list)
    if ( iStorages.Count() > 1 )
        {
        // Round trip suppport
        const CMTPStorageMetaData& storage( iFramework.StorageMgr().StorageL( iStorages[0] ) );
        TFileName root( storage.DesC( CMTPStorageMetaData::EStorageSuid ) );
        GetModifiedContentL( root );
        iDataProvider.GetWrapperL().UpdateMusicCollectionL();

        iStorages.Remove( 0 );
        ScanStorageL( iStorages[0] );
        }
    else
        {
        // Round trip suppport
        const CMTPStorageMetaData& storage( iFramework.StorageMgr().StorageL( iStorages[0] ) );
        TFileName root( storage.DesC( CMTPStorageMetaData::EStorageSuid ) );
        GetModifiedContentL( root );

        // We are done
        iStorages.Reset();

        SignalCompleteL( iDataProvider );
        }
    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderEnumerator::ScanDirL
//
// -----------------------------------------------------------------------------
//
void CMediaMtpDataProviderEnumerator::ScanDirL()
    {
    PRINT( _L( "MM MTP => CMediaMtpDataProviderEnumerator::ScanDirL" ) );

    StartEnumerationCount();

    iFirstUnprocessed = 0;

    iScanningDir = ETrue;
    PERFLOGSTART( KDirectoryScan );
    iDir.Read( iEntries, iStatus );
    SetActive();

    PRINT( _L( "MM MTP <= CMediaMtpDataProviderEnumerator::ScanDirL" ) );
    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderEnumerator::ScanNextDirL
// Recurse into the next directory on the stack
// and scan it for entries.
// -----------------------------------------------------------------------------
//
void CMediaMtpDataProviderEnumerator::ScanNextDirL()
    {
    TInt count = iDirStack.Count();

    if ( count == 0 )
        {
        // No more directories on the stack, try the next storage
        ScanNextStorageL();
        }
    else
        {
        TEntry* entry = iDirStack[count - 1];
        if ( entry != NULL )
            {

            // Empty TEntry, no more subdirectories in
            // the current path
            if ( entry->iName == KNullDesC )
                {
                // Remove current dir from path
                iPath.PopDir();
                iDirStack.Remove( count - 1 );
                delete entry;
                entry = NULL;
                iDir.Close();

                // Scan the next directory of the parent
                ScanNextDirL();
                }
            // Going into a subdirectory of current
            else
                {
                // Add directory to path
                iPath.AddDir( entry->iName );
                // Remove directory so we don't think it's a subdirectory
                iDirStack.Remove( count - 1 );
                delete entry;
                entry = NULL;
                ScanNextSubdirL();
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderEnumerator::ScanNextSubdirL
// Scan next sub dir
// -----------------------------------------------------------------------------
//
void CMediaMtpDataProviderEnumerator::ScanNextSubdirL()
    {
    PRINT( _L( "MM MTP => CMediaMtpDataProviderEnumerator::ScanNextSubdirL" ) );

    // A empty (non-constructed) TEntry is our marker telling us to pop a directory
    // from iPath when we see this
    TEntry* entry = new TEntry( TEntry() );

    User::LeaveIfNull( entry );

    iDirStack.AppendL( entry );

    // Leave with KErrNotFound if we don't find the object handle since it shouldn't be on the
    // dirstack if the entry wasn't added
    TPtrC suid = iPath.DriveAndPath().Left( iPath.DriveAndPath().Length() );
    // Update the current parenthandle with object of the directory
    PERFLOGSTART( KObjectManagerObjectUid );
    iParentHandle = iFramework.ObjectMgr().HandleL( suid );
    PERFLOGSTOP( KObjectManagerObjectUid );
    PRINT1( _L( "MM MTP <> iParentHandle = 0x%x" ), iParentHandle );

    // Kick-off a scan of the next directory
    iDir.Close();

    TInt err = iDir.Open( iFramework.Fs(),
        iPath.DriveAndPath(),
        KEntryAttNormal | KEntryAttDir );

    PRINT1( _L( "MM MTP <> CMediaMtpDataProviderEnumerator::ScanNextSubdirL, RDir::Open err = %d" ), err );

    if ( err == KErrNone )
        ScanDirL();
    else
        {
        iEntries = TEntryArray();

        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, iStatus.Int() );
        SetActive();
        }


    PRINT( _L( "MM MTP <= CMediaMtpDataProviderEnumerator::ScanNextSubdirL" ) );
    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderEnumerator::RunL
//
// -----------------------------------------------------------------------------
//
void CMediaMtpDataProviderEnumerator::RunL()
    {
    if ( iScanningDir )
        {
        PERFLOGSTOP( KDirectoryScan );
        }

    iScanningDir = EFalse;
    if ( iEntries.Count() == 0 )
        {
        // No entries to process, scan next dir or storage
        ScanNextDirL();
        }
    else if ( iFirstUnprocessed < iEntries.Count() )
        {
        PRINT1( _L( "MM MTP <> CMediaMtpDataProviderEnumerator::RunL iFirstUnprocessed = %d" ), iFirstUnprocessed);
        TRAPD( err, ProcessEntriesL(););
        if ( err != KErrNone )
            {
            iFirstUnprocessed++;
            }

        // Complete ourselves with current TRequestStatus
        // since we need to run again to either scan a new dir or drive
        // or process more entries
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, iStatus.Int() );
        SetActive();
        }
    else
        {
        switch ( iStatus.Int() )
            {
            case KErrNone:
                // There are still entries left to be read
                ScanDirL();
                break;

            case KErrEof:
                // There are no more entries
            default:
                // Error, ignore and continue with next dir
                ScanNextDirL();
                break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderEnumerator::RunError
//
// -----------------------------------------------------------------------------
//
TInt CMediaMtpDataProviderEnumerator::RunError( TInt aError )
    {
    PRINT1( _L( "MM MTP <> CMediaMtpDataProviderEnumerator::RunError with error %d" ), aError );

    TRAP_IGNORE( SignalCompleteL( iDataProvider ) );

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderEnumerator::ConstructL
//
// -----------------------------------------------------------------------------
//
void CMediaMtpDataProviderEnumerator::ConstructL()
    {
    CActiveScheduler::Add( this );

    TTimeIntervalMicroSeconds32 tickPeriod = 0;
    TInt err = UserHal::TickPeriod(tickPeriod);

    PRINT1( _L( "MM MTP <> CMediaMtpDataProviderEnumerator::ConstructL, UserHal::TickPeriod err = %d" ), err);
    User::LeaveIfError( err );
    iTickPeriod = tickPeriod.Int();

#if defined(_DEBUG) || defined(MMMTPDP_PERFLOG)
    iPerfLog = CMmMtpDpPerfLog::NewL( _L( "MediaMtpDataProviderEnumerator" ) );
#endif
    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderEnumerator::SignalCompleteL
// Called when the enumeration is completed
// -----------------------------------------------------------------------------
//
void CMediaMtpDataProviderEnumerator::SignalCompleteL( MMTPEnumerationCallback& aCallback,
    TInt aError/* = KErrNone*/)
    {
    // Enumeration completed on this drive
    aCallback.NotifyEnumerationCompleteL( iStorageId, aError );
    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderEnumerator::ProcessEntriesL
// Iterates iEntries adding entries as needed to object manager and iDirStack.
// -----------------------------------------------------------------------------
//
void CMediaMtpDataProviderEnumerator::ProcessEntriesL()
    {
    PRINT( _L( "MM MTP => CMediaMtpDataProviderEnumerator::ProcessEntriesL" ) );

    TBuf<KMaxFileName> path = iPath.DriveAndPath();

    while ( !IsOverThreshold() && iFirstUnprocessed < iEntries.Count() )
        {
        const TEntry& entry = iEntries[iFirstUnprocessed];
        path.Append( entry.iName );
        PRINT1( _L( "MM MTP <> path = %S" ), &path );

        TInt len = entry.iName.Length();

        if ( entry.IsDir() )
            {
            path.Append( '\\' );
            ++len;

            // we don't need to process folder, just remember
            // the folder
            TEntry* dirEntry = new TEntry( entry );
            User::LeaveIfNull( dirEntry );
            iDirStack.AppendL( dirEntry );
            }
        else if ( IsFileAccepted( path ) )
            {
            AddEntryL( path );
            }

        // Remove filename part
        path.SetLength( path.Length() - len );

        iFirstUnprocessed++;
        }

    StartEnumerationCount();
    PRINT( _L( "MM MTP <= CMediaMtpDataProviderEnumerator::ProcessEntriesL" ) );
    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderEnumerator::AddEntryL
// Add a file entry to the object store
// -----------------------------------------------------------------------------
//
void CMediaMtpDataProviderEnumerator::AddEntryL( const TDesC& aFullFileName )
    {
    PRINT1( _L( "MM MTP => CMediaDataProviderEnumerator::AddEntryL aFullFileName = %S" ), &aFullFileName );

    CMTPObjectMetaData* object( CMTPObjectMetaData::NewLC( iDataProviderId,
        iFormatCode,
        iStorages[0],
        aFullFileName ) );    // + object

    object->SetUint( CMTPObjectMetaData::EParentHandle, iParentHandle );

    if ( ( iFormatCode == EMTPFormatCodeMP4Container )
        || ( iFormatCode == EMTPFormatCode3GPContainer )
        || ( iFormatCode == EMTPFormatCodeASF ) )
        {
        TMmMtpSubFormatCode subFormatCode;

        // Note: Delay the file parsing until external enumeration phase to avoid time-out
        subFormatCode = EMTPSubFormatCodeUnknown;

        object->SetUint( CMTPObjectMetaData::EFormatSubCode, (TUint)subFormatCode );
        }

    PERFLOGSTART( KObjectManagerInsert );
    iObjectMgr.InsertObjectL( *object );
    PERFLOGSTOP( KObjectManagerInsert );

    CleanupStack::PopAndDestroy( object );  // - object

    PRINT1( _L( "MM MTP <= CMediaMtpDataProviderEnumerator::AddEntryL, entry inserted, iFormatCode = 0x%x" ), iFormatCode);
    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderEnumerator::GetObjectFormatCode
// Returns a TMTPFormatCode for the requested object
// -----------------------------------------------------------------------------
//
TMTPFormatCode CMediaMtpDataProviderEnumerator::GetObjectFormatCode( const TDesC& aFullFileName )
    {
    PRINT1( _L("MM MTP <> CMediaMtpDataProviderEnumerator::GetObjectFormatCodeL aFullFileName = %S"), &aFullFileName );
    TMTPFormatCode formatCode = MmMtpDpUtility::FormatFromFilename( aFullFileName );
    return formatCode;
    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderEnumerator::IsFileAccepted
// Is the file format is supported by this Dp
// -----------------------------------------------------------------------------
//
TBool CMediaMtpDataProviderEnumerator::IsFileAccepted( const TDesC& aFullFileName )
    {
    PERFLOGSTART( KFormatFilter );
    iFormatCode = GetObjectFormatCode( aFullFileName );
    PRINT1( _L( "MM MTP <> CMediaMtpDataProviderEnumerator::IsFileAcceptedL formatCode = 0x%x" ), iFormatCode );
    TBool accepted = EFalse;
    TInt count = sizeof( KMediaMtpDataProviderSupportedFormats ) / sizeof( TUint16 );
    for ( TInt i = 0; i < count; i++ )
        {
        if ( iFormatCode == KMediaMtpDataProviderSupportedFormats[i] )
            {
            accepted = ETrue;
            break;
            }
        }

    PERFLOGSTOP( KFormatFilter );
    return accepted;
    }

// -----------------------------------------------------------------------------
// CMediaDpMtpEnumerator::StartEnumerationCount
// start record system tick
// -----------------------------------------------------------------------------
//
void CMediaMtpDataProviderEnumerator::StartEnumerationCount()
    {
    iTickCountBegin = User::TickCount();
    }

// -----------------------------------------------------------------------------
// CMediaDpMtpEnumerator::IsOverThreshold
// Is system tick more than the predefined value?
// -----------------------------------------------------------------------------
//
TBool CMediaMtpDataProviderEnumerator::IsOverThreshold()
    {
    TUint tcNow = User::TickCount();

    if ( ( tcNow - iTickCountBegin ) * iTickPeriod >= KThresholdOfEnumerationLoopDuration )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

// -----------------------------------------------------------------------------
// CMediaDpMtpEnumerator::GetModifiedContentL()
// Get Modified Content and report to Framework
// -----------------------------------------------------------------------------
//
void CMediaMtpDataProviderEnumerator::GetModifiedContentL( const TDesC& aStorageRoot )
    {
    PRINT( _L( "MM MTP => CMediaMtpDataProviderEnumerator::GetModifiedContentL" ) );
    TInt arrayCount = 0;
    CDesCArray* modifiedContents = new ( ELeave ) CDesCArrayFlat( KMTPDriveGranularity );
    CleanupStack::PushL( modifiedContents ); // + modifiedContents

    iDataProvider.GetWrapperL().GetModifiedContentL( aStorageRoot, arrayCount, *modifiedContents );

    if ( arrayCount > 0 )
        {
        CMTPObjectMetaData* object = CMTPObjectMetaData::NewLC();   // + object
        for ( TInt i = 0; i < arrayCount; i++ )
            {

            if ( iFramework.ObjectMgr().ObjectL( ( *modifiedContents )[i], *object ) )
                {
                object->SetUint( CMTPObjectMetaData::EObjectMetaDataUpdate , 1 );
                iFramework.ObjectMgr().ModifyObjectL( *object );
                }
            }

        CleanupStack::PopAndDestroy( object );  // - object
        }

    CleanupStack::PopAndDestroy( modifiedContents ); // - modifiedContents

    PRINT( _L( "MM MTP <= CMediaMtpDataProviderEnumerator::GetModifiedContentL" ) );
    }

// -----------------------------------------------------------------------------
// CMediaMtpDataProviderEnumerator::SessionClosedL
//
// -----------------------------------------------------------------------------
//
void CMediaMtpDataProviderEnumerator::SessionClosedL()
    {
    iDataProvider.GetWrapperL().UpdateMusicCollectionL();
    iDataProvider.GetWrapperL().CleanupDatabaseL();
    }

//end of file
