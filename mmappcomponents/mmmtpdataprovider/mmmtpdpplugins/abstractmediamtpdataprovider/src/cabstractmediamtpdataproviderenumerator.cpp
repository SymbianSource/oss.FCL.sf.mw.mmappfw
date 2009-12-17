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


#include <bautils.h>
#include <mtp/cmtpobjectmetadata.h>
#include <mtp/mmtpdataproviderframework.h>
#include <mtp/mmtpobjectmgr.h>
#include <mtp/mmtpstoragemgr.h>
#include <mtp/mmtpreferencemgr.h>
#include <mpxmediaarray.h>
#include <mpxmedia.h>

#include "abstractmediamtpdataproviderconst.h"
#include "cabstractmediamtpdataproviderenumerator.h"
#include "cabstractmediamtpdataprovider.h"
#include "mmmtpdplogger.h"
#include "mmmtpdputility.h"
#include "cmmmtpdpmetadataaccesswrapper.h"
#include "cmmmtpdpmetadatampxaccess.h"


/** Number of objects to insert into the object manager in one go*/
const TInt KMTPDriveGranularity = 5;

_LIT( KPlaylistFilePath, "Playlists\\" );

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProviderEnumerator::NewL
// Two phase constructor
// -----------------------------------------------------------------------------
//
CAbstractMediaMtpDataProviderEnumerator* CAbstractMediaMtpDataProviderEnumerator::NewL( MMTPDataProviderFramework& aFramework,
    CAbstractMediaMtpDataProvider& aDataProvider )
    {
    PRINT( _L( "MM MTP => CAbstractMediaMtpDataProviderEnumerator::NewL" ) );
    CAbstractMediaMtpDataProviderEnumerator* self = new ( ELeave ) CAbstractMediaMtpDataProviderEnumerator( aFramework,
        aDataProvider );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    PRINT( _L( "MM MTP <= CAbstractMediaMtpDataProviderEnumerator::NewL" ) );
    return self;
    }

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProviderEnumerator::CAbstractMediaMtpDataProviderEnumerator
// Standard C++ Constructor
// -----------------------------------------------------------------------------
//
CAbstractMediaMtpDataProviderEnumerator::CAbstractMediaMtpDataProviderEnumerator( MMTPDataProviderFramework& aFramework,
    CAbstractMediaMtpDataProvider& aDataProvider ) :
    CActive( EPriorityLow ),
    iFramework( aFramework ),
    iObjectMgr( aFramework.ObjectMgr() ),
    iDataProviderId( aFramework.DataProviderId() ),
    iDataProvider( aDataProvider ),
    iStorages( 2 ),
    iAbstractMedias( NULL ),
    iCount( 0 ),
    iCurrentIndex( 0 )
    {
    PRINT1( _L( "MM MTP <> CAbstractMediaMtpDataProviderEnumerator::CAbstractMediaMtpDataProviderEnumerator, iDataProviderId = %d" ), iDataProviderId );
    }

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProviderEnumerator::ConstructL
//
// -----------------------------------------------------------------------------
//
void CAbstractMediaMtpDataProviderEnumerator::ConstructL()
    {
    CActiveScheduler::Add( this );

#if defined(_DEBUG) || defined(MMMTPDP_PERFLOG)
    iPerfLog = CMmMtpDpPerfLog::NewL( _L( "CAbstractMediaMtpDataProviderEnumerator" ) );
#endif
    }

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProviderEnumerator::~CAbstractMediaMtpDataProviderEnumerator
// destructor
// -----------------------------------------------------------------------------
//
CAbstractMediaMtpDataProviderEnumerator::~CAbstractMediaMtpDataProviderEnumerator()
    {
    PRINT( _L( "MM MTP => CAbstractMediaMtpDataProviderEnumerator::~CAbstractMediaMtpDataProviderEnumerator" ) );

    Cancel();
    iStorages.Close();

    delete iAbstractMedias;
    iAbstractMedias = NULL;
#if defined(_DEBUG) || defined(MMMTPDP_PERFLOG)
    delete iPerfLog;
#endif // _DEBUG
    PRINT( _L( "MM MTP <= CAbstractMediaMtpDataProviderEnumerator::~CAbstractMediaMtpDataProviderEnumerator" ) );
    }

// -----------------------------------------------------------------------------
// AbstractMediaDpMtpEnumerator::StartL
// Kick off the enumeration on the specified storage
// -----------------------------------------------------------------------------
//
void CAbstractMediaMtpDataProviderEnumerator::StartL( TUint32 aStorageId )
    {
    PRINT1( _L( "MM MTP => CAbstractMediaMtpDataProviderEnumerator::StartL aStorageId = 0x%x" ), aStorageId );

    MMTPStorageMgr& storageMgr( iFramework.StorageMgr() );
    if ( aStorageId == KMTPStorageAll )
        {
        // Retrieve the available logical StorageIDs
        RPointerArray<const CMTPStorageMetaData> storages;
        CleanupClosePushL( storages );      // + storages
        TMTPStorageMgrQueryParams params( KNullDesC,
            CMTPStorageMetaData::ESystemTypeDefaultFileSystem );

        storageMgr.GetLogicalStoragesL( params, storages );

        // Construct the StorageIDs list.
        for ( TInt i = 0; i < storages.Count(); i++ )
            {
            iStorages.AppendL( storages[i]->Uint(
                CMTPStorageMetaData::EStorageId ) );
            }
        CleanupStack::PopAndDestroy( &storages );   // - storages
        }
    else if ( aStorageId != KMTPNotSpecified32 )
        {
        __ASSERT_DEBUG( storageMgr.ValidStorageId( aStorageId ), User::Invariant() );
        const CMTPStorageMetaData& storage(
             storageMgr.StorageL( aStorageId ) );
        if ( storage.Uint( CMTPStorageMetaData::EStorageSystemType ) ==
                CMTPStorageMetaData::ESystemTypeDefaultFileSystem )
            {
            if ( storageMgr.LogicalStorageId( aStorageId ) != KMTPNotSpecified32 )
                {
                // Logical StorageID.
                iStorages.AppendL( aStorageId );
                }
            else
                {
                // Physical StorageID. Enumerate all eligible logical storages.
                const RArray<TUint>& logicalIds( storage.UintArray(
                    CMTPStorageMetaData::EStorageLogicalIds ) );

                TInt countLogicalIds = logicalIds.Count();

                for ( TInt i = 0; i < countLogicalIds; i++ )
                    {
                    iStorages.AppendL( logicalIds[i] );
                    }
                }
            }
        }

    // keep in mind for notification when enumeration complete
    iStorageId = aStorageId;

    if ( iStorages.Count() > 0 )
        {
        ScanStorageL( iStorages[0] );
        }
    else
        {
        iStorages.Reset();
        SignalCompleteL( iDataProvider );
        }

    PRINT( _L( "MM MTP <= CAbstractMediaMtpDataProviderEnumerator::StartL" ) );
    }

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProviderEnumerator::ScanStorageL
// Find out all AbstractMedia file according to storage id
// -----------------------------------------------------------------------------
//
void CAbstractMediaMtpDataProviderEnumerator::ScanStorageL( TUint32 aStorageId )
    {
    PRINT1( _L( "MM MTP => CAbstractMediaMtpDataProviderEnumerator::ScanStorageL aStorageId = 0x%x" ), aStorageId );
    const CMTPStorageMetaData& storage(
        iFramework.StorageMgr().StorageL( aStorageId ) );

    __ASSERT_DEBUG( ( storage.Uint( CMTPStorageMetaData::EStorageSystemType ) ==
        CMTPStorageMetaData::ESystemTypeDefaultFileSystem ), User::Invariant() );

    TFileName root( storage.DesC( CMTPStorageMetaData::EStorageSuid ) );
    PRINT1( _L( "MM MTP <> CAbstractMediaMtpDataProviderEnumerator::ScanStorageL StorageSuid = %S" ), &root );

    // created by windows media player, or else return responsecode is Access denied
    // Create abstract media directory if it does not exist
    HBufC* tempBuf = HBufC::NewLC( KMaxFileName );  // + tempBuf
    TPtr folder = tempBuf->Des();
    folder.Zero();
    folder.Append( root );
    folder.Append( KPlaylistFilePath );
    TBool ret = BaflUtils::FileExists( iFramework.Fs(), folder );
    PRINT2( _L( "MM MTP <> CAbstractMediaMtpDataProviderEnumerator::ScanStorageL ret = %d, folder = %S" ), ret, &folder );
    if( !ret )
        {
        TInt err = iFramework.Fs().MkDirAll( folder );
        PRINT2( _L( "MM MTP <> CAbstractMediaMtpDataProviderEnumerator::ScanStorageL Creating folder (%S) returned error %d" ), tempBuf, err );

        // add this new folder to framework metadata DB
        CMTPObjectMetaData* object = CMTPObjectMetaData::NewLC( 0, // Dev Dp hard code
                EMTPFormatCodeAssociation,
                aStorageId,
                folder );  // + object
        object->SetUint( CMTPObjectMetaData::EParentHandle, KMTPHandleNoParent );
        object->SetUint( CMTPObjectMetaData::EFormatSubCode, EMTPAssociationTypeGenericFolder );
        PERFLOGSTART( KObjectManagerInsert );
        iObjectMgr.InsertObjectL( *object );
        PERFLOGSTOP( KObjectManagerInsert );
        CleanupStack::PopAndDestroy( object ); // - object
        }
    CleanupStack::PopAndDestroy( tempBuf ); // - tempBuf

    // find all abstract medias stored in MPX
    delete iAbstractMedias;
    iAbstractMedias = NULL;
    PERFLOGSTART( KMpxGetAllPlaylist );
    TRAPD( err, iDataProvider.GetWrapperL().GetAllPlaylistL( root, &iAbstractMedias ) );
    PERFLOGSTOP( KMpxGetAllPlaylist );

    if ( iAbstractMedias != NULL && err == KErrNone )
        {
        iCount = iAbstractMedias->Count();
        iCurrentIndex = 0;

        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, iStatus.Int() );
        SetActive();
        }
    else
        {
        iCount = 0;
        iCurrentIndex = 0;
        ScanNextL();
        }

    PRINT( _L( "MM MTP <= CAbstractMediaMtpDataProviderEnumerator::ScanStorageL" ) );
    }

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProviderEnumerator::ScanNextStorageL
//
// -----------------------------------------------------------------------------
//
void CAbstractMediaMtpDataProviderEnumerator::ScanNextL()
    {
    PRINT1( _L( "MM MTP = > CAbstractMediaMtpDataProviderEnumerator::ScanNextStorageL iStorages.Count = %d" ), iStorages.Count() );
    if ( iCurrentIndex < iCount )
        {
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, iStatus.Int() );
        SetActive();

        PRINT2( _L( "MM MTP <> Current storage is still being scanned, current index = %d, total AbstractMedia count = %d" ),
            iCurrentIndex,
            iCount );
        }
    // If there are one or more unscanned storages left
    // (the currently scanned one is still on the list)
    else if ( iStorages.Count() > 1 )
        {
        iStorages.Remove( 0 );
        ScanStorageL( iStorages[0] );
        }
    else
        {
        // We are done
        PRINT( _L( "MM MTP <> Objects enumeration completed 2" ) );
        iStorages.Reset();
        SignalCompleteL( iDataProvider );
        }
    }

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProviderEnumerator::RunL
// NOTE: preserve for performance improvement
// -----------------------------------------------------------------------------
//
void CAbstractMediaMtpDataProviderEnumerator::RunL()
    {
    TBuf<KMaxFileName> playlist;

    // insert all playlists into handle db of framework
    CMPXMedia* media = ( *iAbstractMedias )[iCurrentIndex];
    PERFLOGSTART( KMpxGetPlaylistName );
    iDataProvider.GetWrapperL().GetPlaylistNameL( media, playlist );
    PERFLOGSTOP( KMpxGetPlaylistName );
    AddEntryL( playlist );

    // find all reference of each playlist and create dummy files for each playlist
    CDesCArray* references = new ( ELeave ) CDesCArrayFlat( KMTPDriveGranularity );
    CleanupStack::PushL( references ); // + references

    PERFLOGSTART( KMpxQueryPlaylistReference );
    iDataProvider.GetWrapperL().GetAllReferenceL( media, *references );
    PERFLOGSTOP( KMpxQueryPlaylistReference );

    // insert references into reference db
    AddReferencesL( playlist, *references );

    CleanupStack::PopAndDestroy( references ); // - references
    iCurrentIndex++;

    ScanNextL();
    }

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProviderEnumerator::RunError
// NOTE: preserve for performance improvement
// -----------------------------------------------------------------------------
//
TInt CAbstractMediaMtpDataProviderEnumerator::RunError( TInt aError )
    {
    if ( aError != KErrNone )
        PRINT1( _L( "MM MTP <> CAbstractMediaMtpDataProviderEnumerator::RunError with error %d" ), aError );

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProviderEnumerator::DoCancel()
// Cancel the enumeration process
// NOTE: preserve for performance improvement
// -----------------------------------------------------------------------------
//
void CAbstractMediaMtpDataProviderEnumerator::DoCancel()
    {

    }

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProviderEnumerator::SignalCompleteL
// Called when the enumeration is completed
// -----------------------------------------------------------------------------
//
void CAbstractMediaMtpDataProviderEnumerator::SignalCompleteL( MMTPEnumerationCallback& aCallback,
    TInt aError )
    {
    PRINT( _L( "MM MTP <> CAbstractMediaMtpDataProviderEnumerator::SignalCompleteL" ) );
    // Enumeration completed on this drive
    aCallback.NotifyEnumerationCompleteL( iStorageId, aError );
    }

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProviderEnumerator::AddEntryL
// Add a file entry to the object store
// -----------------------------------------------------------------------------
//
void CAbstractMediaMtpDataProviderEnumerator::AddEntryL( const TDesC& aSuid )
    {
    PRINT1( _L("MM MTP => CAbstractMediaMtpDataProviderEnumerator::AddEntryL AbstractMedia name = %S"), &aSuid );

    TMTPFormatCode format = MmMtpDpUtility::FormatFromFilename( aSuid );
    CMTPObjectMetaData* object = CMTPObjectMetaData::NewLC( iDataProviderId,
        format,
        iStorages[0],
        aSuid );  // + object

    TParsePtrC parser( aSuid );
    PERFLOGSTART( KObjectManagerObjectUid );
    TUint32 parentHandle = iFramework.ObjectMgr().HandleL( parser.DriveAndPath() );
    PERFLOGSTOP( KObjectManagerObjectUid );
    object->SetUint( CMTPObjectMetaData::EParentHandle, parentHandle );

    PERFLOGSTART(KObjectManagerInsert);
    iObjectMgr.InsertObjectL( *object );
    PERFLOGSTOP(KObjectManagerInsert);

    CleanupStack::PopAndDestroy( object );// - object

    iDataProvider.GetWrapperL().CreateDummyFile( aSuid );

    // remember the abstract media file for clean up
    iDataProvider.GetWrapperL().AddDummyFileL( aSuid );

    PRINT( _L( "MM MTP <= CAbstractMediaMtpDataProviderEnumerator::AddEntryL" ) );
    }

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProviderEnumerator::AddReferencesL
// Add references into reference db according to abstract media name
// -----------------------------------------------------------------------------
//
void CAbstractMediaMtpDataProviderEnumerator::AddReferencesL( const TDesC& aAbstractMediaName,
    CDesCArray& aReferences )
    {
    TInt count = aReferences.Count();
    PRINT2( _L("MM MTP => CAbstractMediaMtpDataProviderEnumerator::AddReferencesL AbstractMedia name = %S, ref count = %d"), &aAbstractMediaName, count );

    // check if references are valid
    CMTPObjectMetaData* object = CMTPObjectMetaData::NewLC();   // + object
    MMTPObjectMgr& objectMgr = iFramework.ObjectMgr();

    TInt removeCount = 0;
    for ( TInt i = 0; i < count; i++ )
        {
        TInt index = i - removeCount;
        PRINT2( _L( "MM MTP <> CAbstractMediaMtpDataProviderEnumerator::AddReferencesL ref[%d]'s name = %S" ), index, &( aReferences[index] ) );
        PERFLOGSTART( KObjectManagerHandle );
        TUint32 handle = iFramework.ObjectMgr().HandleL( aReferences[index] );
        PERFLOGSTOP( KObjectManagerHandle );
        if ( handle == KMTPHandleNone ) // object doesn't exist
            {
            TPtrC temp( aReferences[index] );
            PRINT1( _L( "MM MTP <> CAbstractMediaMtpDataProviderEnumerator::AddReferencesL, [%S] doesn't existed in handle db, remove this from reference array" ), &temp );

            // if handle is invalid, remove from reference array
            aReferences.Delete( index, 1 );
            removeCount++;
            }
        }
    CleanupStack::PopAndDestroy( object );  // - object

    // add all references into references db
    MMTPReferenceMgr& referenceMgr = iFramework.ReferenceMgr();
    PERFLOGSTART( KReferenceManagerSetReference );
    referenceMgr.SetReferencesL( aAbstractMediaName, aReferences );
    PERFLOGSTOP( KReferenceManagerSetReference );

    PRINT( _L( "MM MTP <= CAbstractMediaMtpDataProviderEnumerator::AddReferencesL" ) );
    }

//end of file
