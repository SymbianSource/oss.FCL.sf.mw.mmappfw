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


const TInt KMTPDriveGranularity = 5;
const TInt KAbstractMediaDpArrayGranularity = 2;

_LIT( KPlaylistFilePath, "Playlists\\" );

#if defined(_DEBUG) || defined(MMMTPDP_PERFLOG)
_LIT( KMpxGetAllAbstractMedia, "MpxGetAllAbstractMedia" );
_LIT( KMpxQueryAbstractMediaReference, "MpxQueryAbstractMediaReference" );
_LIT( KObjectManagerObjectUid, "ObjectManagerObjectUid" );
_LIT( KObjectManagerInsert, "ObjectManagerInsert" );
_LIT( KObjectManagerHandle, "ObjectManagerHandle" );
_LIT( KReferenceManagerSetReference, "ReferenceManagerSetReference" );
#endif

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
    iStorages( KAbstractMediaDpArrayGranularity ),
    iAbstractMedias( NULL ),
    iCount( 0 ),
    iCurrentIndex( 0 ),
    iEnumState ( EEnumPlaylist )
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
    iEnumState = EEnumPlaylist;
    if ( iStorages.Count() > 0 )
        {
        InitStorageL();
        }
    ScanNextL();

    PRINT( _L( "MM MTP <= CAbstractMediaMtpDataProviderEnumerator::StartL" ) );
    }

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProviderEnumerator::InitStorageL
// Initialize iAbstractMedia, iCount and iCurrentIndex according to iStorages and iEnumState
// -----------------------------------------------------------------------------
//
void CAbstractMediaMtpDataProviderEnumerator::InitStorageL()
    {
    __ASSERT_DEBUG(iStorages.Count() > 0 && iEnumState < EEnumCount, User::Invariant() );
    PRINT2( _L( "MM MTP => CAbstractMediaMtpDataProviderEnumerator::InitStorageL storageId = 0x%x, state=%d" ), iStorages[0], iEnumState );
    const CMTPStorageMetaData& storage(
        iFramework.StorageMgr().StorageL( iStorages[0] ) );

    __ASSERT_DEBUG( ( storage.Uint( CMTPStorageMetaData::EStorageSystemType ) ==
        CMTPStorageMetaData::ESystemTypeDefaultFileSystem ), User::Invariant() );

    TFileName root( storage.DesC( CMTPStorageMetaData::EStorageSuid ) );
    PRINT1( _L( "MM MTP <> CAbstractMediaMtpDataProviderEnumerator::InitStorageL StorageSuid = %S" ), &root );

    if ( iEnumState == EEnumPlaylist )
        {
        // created by windows media player, or else return responsecode is Access denied
        // Create playlist directory if it does not exist
        // NOTE: Only playlist need to create directory here, for the dummy files
        HBufC* tempBuf = HBufC::NewLC( KMaxFileName );  // + tempBuf
        TPtr folder = tempBuf->Des();
        folder.Zero();
        folder.Append( root );
        folder.Append( KPlaylistFilePath );
        TBool ret = BaflUtils::FileExists( iFramework.Fs(), folder );
        PRINT2( _L( "MM MTP <> CAbstractMediaMtpDataProviderEnumerator::InitStorageL ret = %d, folder = %S" ), ret, &folder );
        if( !ret )
            {
            TInt err = iFramework.Fs().MkDirAll( folder );
            PRINT2( _L( "MM MTP <> CAbstractMediaMtpDataProviderEnumerator::InitStorageL Creating folder (%S) returned error %d" ), tempBuf, err );

            // add this new folder to framework metadata DB
            CMTPObjectMetaData* object = CMTPObjectMetaData::NewLC( 0, // Dev Dp hard code
                    EMTPFormatCodeAssociation,
                    iStorages[0],
                    folder );  // + object
            object->SetUint( CMTPObjectMetaData::EParentHandle, KMTPHandleNoParent );
            object->SetUint( CMTPObjectMetaData::EFormatSubCode, EMTPAssociationTypeGenericFolder );
            PERFLOGSTART( KObjectManagerInsert );
            iObjectMgr.InsertObjectL( *object );
            PERFLOGSTOP( KObjectManagerInsert );
            CleanupStack::PopAndDestroy( object ); // - object
            }
        CleanupStack::PopAndDestroy( tempBuf ); // - tempBuf
        }

    // find all abstract medias stored in MPX
    delete iAbstractMedias;
    iAbstractMedias = NULL;
    TMPXGeneralCategory category = ( iEnumState == EEnumPlaylist ) ? EMPXPlaylist : EMPXAbstractAlbum;
    PERFLOGSTART( KMpxGetAllAbstractMedia );
    TRAPD( err, iDataProvider.GetWrapperL().GetAllAbstractMediaL( root, &iAbstractMedias, category  ) );
    PERFLOGSTOP( KMpxGetAllAbstractMedia );

    if ( iAbstractMedias != NULL && err == KErrNone )
        {
        iCount = iAbstractMedias->Count();
        }
    else
        {
        iCount = 0;
        }
    iCurrentIndex = 0;

    PRINT( _L( "MM MTP <= CAbstractMediaMtpDataProviderEnumerator::InitStorageL" ) );
    }

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProviderEnumerator::ScanNextL
// Proceed to next abstract media item
// -----------------------------------------------------------------------------
//
void CAbstractMediaMtpDataProviderEnumerator::ScanNextL()
    {
    while (iStorages.Count() > 0)
        {
        while ( iEnumState < EEnumCount )
            {
            if ( iCurrentIndex < iCount )
                {
                TRequestStatus* status = &iStatus;
                User::RequestComplete( status, iStatus.Int() );
                SetActive();
                PRINT2( _L( "MM MTP <> Current storage is still being scanned, current index = %d, total AbstractMedia count = %d" ),
                    iCurrentIndex,
                    iCount );
                return;
                }
            ++iEnumState;
            if (iEnumState == EEnumCount)
                {
                break;
                }
            InitStorageL();
            }
        iStorages.Remove( 0 );
        if (iStorages.Count() == 0)
            {
            break;
            }
        iEnumState = EEnumPlaylist;
        InitStorageL();
        }
    // We are done
    PRINT( _L( "MM MTP <> Objects enumeration completed" ) );
    delete iAbstractMedias;
    iAbstractMedias = NULL;
    iStorages.Reset();
    SignalCompleteL( iDataProvider );
    }

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProviderEnumerator::RunL
// NOTE: preserve for performance improvement
// -----------------------------------------------------------------------------
//
void CAbstractMediaMtpDataProviderEnumerator::RunL()
    {
    PRINT( _L( "MM MTP => CAbstractMediaMtpDataProviderEnumerator::RunL" ) );

    // insert all abstract medias into handle db of framework
    CMPXMedia* media = ( *iAbstractMedias )[iCurrentIndex];
    
    // Increase the index first in case of leave
    iCurrentIndex++;
    TMPXGeneralCategory category = ( iEnumState == EEnumPlaylist ) ? EMPXPlaylist : EMPXAbstractAlbum;
    HBufC* abstractMedia = iDataProvider.GetWrapperL().GetAbstractMediaNameL( media, category );
    CleanupStack::PushL( abstractMedia ); // + abstractMedia
    TBool ret = ETrue;
    if ( iEnumState == EEnumAbstractAlbum )
        {
        //The abstract album may be removed, check it first
        ret = BaflUtils::FileExists( iFramework.Fs(), *abstractMedia );
        PRINT2( _L( "MM MTP <> BaflUtils::FileExists( RFs,%S ) ret = %d" ), abstractMedia, ret );
        }

    if ( ret )
        {
        PRINT1( _L( "MM MTP <> CAbstractMediaMtpDataProviderEnumerator::RunL abstractMedia=%S" ), abstractMedia );
        AddEntryL( *abstractMedia );

        // find all reference of each abstract media
        CDesCArray* references = new ( ELeave ) CDesCArrayFlat( KMTPDriveGranularity );
        CleanupStack::PushL( references ); // + references
    
        PERFLOGSTART( KMpxQueryAbstractMediaReference );
        iDataProvider.GetWrapperL().GetAllReferenceL( media, *references );
        PERFLOGSTOP( KMpxQueryAbstractMediaReference );
    
        // insert references into reference db
        AddReferencesL( *abstractMedia, *references );
    
        CleanupStack::PopAndDestroy( references ); // - references
        }     
    CleanupStack::PopAndDestroy( abstractMedia ); // - abstractMedia

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
    // Do nothing
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

    PERFLOGSTART( KObjectManagerInsert );
    iObjectMgr.InsertObjectL( *object );
    PERFLOGSTOP( KObjectManagerInsert );

    CleanupStack::PopAndDestroy( object );// - object

    //Only remember the playlist file for clean up
    if( iEnumState == EEnumPlaylist )
        {
        iDataProvider.GetWrapperL().CreateDummyFile( aSuid );

        // remember the abstract media file for clean up
        iDataProvider.GetWrapperL().AddDummyFileL( aSuid );
        }

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
        TPtrC temp( aReferences[index] );
        PRINT2( _L( "MM MTP <> CAbstractMediaMtpDataProviderEnumerator::AddReferencesL ref[%d]'s name = %S" ), index, &temp );
        PERFLOGSTART( KObjectManagerHandle );
        TUint32 handle = iFramework.ObjectMgr().HandleL( temp );
        PERFLOGSTOP( KObjectManagerHandle );
        if ( handle == KMTPHandleNone ) // object doesn't exist
            {
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
