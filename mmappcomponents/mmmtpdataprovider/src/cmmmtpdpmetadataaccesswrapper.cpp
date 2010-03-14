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
* Description:  Meta data access wrapper
*
*/


#include <mtp/mmtpdataproviderframework.h>
#include <mtp/mmtpobjectmgr.h>
#include <bautils.h>
#include <e32math.h>
#include <e32property.h>
#include <mtpprivatepskeys.h>

#include "cmmmtpdpmetadataaccesswrapper.h"
#include "cmmmtpdpmetadatampxaccess.h"
#include "cmmmtpdpmetadatavideoaccess.h"
#include "mmmtpdputility.h"
#include "mmmtpdplogger.h"

const TInt KMMMTPDummyFileArrayGranularity = 5;

CMmMtpDpMetadataAccessWrapper* CMmMtpDpMetadataAccessWrapper::NewL( RFs& aRfs,
    MMTPDataProviderFramework& aFramework )
    {
    CMmMtpDpMetadataAccessWrapper* me = new (ELeave) CMmMtpDpMetadataAccessWrapper( aRfs, aFramework );
    CleanupStack::PushL( me );
    me->ConstructL();
    CleanupStack::Pop( me );

    return me;
    }

CMmMtpDpMetadataAccessWrapper::CMmMtpDpMetadataAccessWrapper( RFs& aRfs,
    MMTPDataProviderFramework& aFramework ) :
    iRfs( aRfs ),
    iFramework( aFramework )
    {
    // Do nothing
    }

// ---------------------------------------------------------------------------
// CMmMtpDpMetadataAccessWrapper::ConstructL
// Second-phase
// ---------------------------------------------------------------------------
//
void CMmMtpDpMetadataAccessWrapper::ConstructL()
    {
    PRINT( _L( "MM MTP => CMmMtpDpMetadataAccessWrapper::ConstructL" ) );

    iMmMtpDpMetadataMpxAccess = CMmMtpDpMetadataMpxAccess::NewL( iRfs, iFramework );

    iMmMtpDpMetadataVideoAccess = CMmMtpDpMetadataVideoAccess::NewL( iRfs );

    iAbstractMediaArray = new ( ELeave ) CDesCArrayFlat( KMMMTPDummyFileArrayGranularity );

    // Create the PS key to notify subscribers that MTP mode is activated
    _LIT_SECURITY_POLICY_C1(KKeyReadPolicy, ECapabilityReadUserData);
    _LIT_SECURITY_POLICY_C1(KKeyWritePolicy, ECapabilityWriteUserData);
    RProperty::Define( KMtpPSUid,
                       KMtpPSStatus,
                       RProperty::EInt,
                       KKeyReadPolicy,
                       KKeyWritePolicy);

    PRINT( _L( "MM MTP <= CMmMtpDpMetadataAccessWrapper::ConstructL" ) );
    }

// ---------------------------------------------------------------------------
// CMmMtpDpMetadataAccessWrapper::~CMmMtpDpMetadataAccessWrapper
// Destructor
// ---------------------------------------------------------------------------
//
CMmMtpDpMetadataAccessWrapper::~CMmMtpDpMetadataAccessWrapper()
    {
    PRINT( _L( "MM MTP => CMmMtpDpMetadataAccessWrapper::~CMmMtpDpMetadataAccessWrapper" ) );
    RemoveDummyFiles();

    delete iAbstractMediaArray;

    delete iMmMtpDpMetadataVideoAccess;
    delete iMmMtpDpMetadataMpxAccess;

    // unblock MPX
    RProperty::Set( KMtpPSUid,
                    KMtpPSStatus,
                    EMtpPSStatusUninitialized );
    PRINT( _L( "MM MTP <= CMmMtpDpMetadataAccessWrapper::~CMmMtpDpMetadataAccessWrapper" ) );
    }

// -----------------------------------------------------------------------------
// CMmMtpDpMetadataAccessWrapper::SetAbstractMediaL
// Set abstract media to DB
// -----------------------------------------------------------------------------
//
EXPORT_C void CMmMtpDpMetadataAccessWrapper::SetAbstractMediaL( const TDesC& aAbstractMediaFileName, 
    CDesCArray& aRefFileArray, 
    TMPXGeneralCategory aCategory )
    {
    PRINT1( _L( "MM MTP => CMmMtpDpMetadataAccessWrapper::SetAbstractMediaL aAbstractMediaFileName = %S" ), &aAbstractMediaFileName );

    if ( !MmMtpDpUtility::IsVideoL( aAbstractMediaFileName, iFramework ) )
        {
        iMmMtpDpMetadataMpxAccess->SetAbstractMediaL( aAbstractMediaFileName, aRefFileArray, aCategory );
        }

    PRINT( _L( "MM MTP <= CMmMtpDpMetadataAccessWrapper::SetAbstractMediaL" ) );
    }

// ---------------------------------------------------------------------------
// CMmMtpDpMetadataAccessWrapper::GetObjectMetadataValueL
// Gets a piece of metadata from the collection
// ---------------------------------------------------------------------------
//
EXPORT_C void CMmMtpDpMetadataAccessWrapper::GetObjectMetadataValueL( const TUint16 aPropCode,
    MMTPType& aNewData,
    const CMTPObjectMetaData& aObjectMetaData )
    {
    PRINT( _L( "MM MTP => CMmMtpDpMetadataAccessWrapper::GetObjectMetadataValueL" ) );

    TPtrC fullFileName( aObjectMetaData.DesC( CMTPObjectMetaData::ESuid ) );
    if ( !MmMtpDpUtility::IsVideoL( fullFileName, iFramework ) )
        {
        iMmMtpDpMetadataMpxAccess->GetObjectMetadataValueL( aPropCode,
            aNewData,
            aObjectMetaData );
        }
    else
        {
        iMmMtpDpMetadataVideoAccess->GetObjectMetadataValueL( aPropCode,
            aNewData,
            aObjectMetaData );
        }

    PRINT( _L( "MM MTP <= CMmMtpDpMetadataAccessWrapper::GetObjectMetadataValueL" ) );
    }

// ---------------------------------------------------------------------------
// CMmMtpDpMetadataAccessWrapper::SetObjectMetadataValueL
// Sets a piece of metadata in the collection
// ---------------------------------------------------------------------------
//
void CMmMtpDpMetadataAccessWrapper::SetObjectMetadataValueL( const TUint16 aPropCode,
    const MMTPType& aNewData,
    const CMTPObjectMetaData& aObjectMetaData )
    {
    PRINT( _L( "MM MTP => CMmMtpDpMetadataAccessWrapper::SetObjectMetadataValueL" ) );

    // In SendObjectPropList, formatCode has already know, but object handle has not been committed into db
    // In that case, format couldn't be get from object manager
    TUint formatCode = aObjectMetaData.Uint( CMTPObjectMetaData::EFormatCode );
    TUint formatSubCode = aObjectMetaData.Uint( CMTPObjectMetaData::EFormatSubCode );

    TBool isVideo = EFalse;
    isVideo = MmMtpDpUtility::IsVideoL( formatCode, formatSubCode );

    if ( isVideo )
        {
        iMmMtpDpMetadataVideoAccess->SetObjectMetadataValueL( aPropCode,
            aNewData,
            aObjectMetaData );
        }
    else
        {
        iMmMtpDpMetadataMpxAccess->SetObjectMetadataValueL( aPropCode,
            aNewData,
            aObjectMetaData );
        }

    PRINT( _L( "MM MTP <= CMmMtpDpMetadataAccessWrapper::SetObjectMetadataValueL" ) );
    }

// ---------------------------------------------------------------------------
// CMmMtpDpMetadataAccessWrapper::SetObjectMetadataValueL
// Renames the file part of a record in the collection database
// ---------------------------------------------------------------------------
//
EXPORT_C void CMmMtpDpMetadataAccessWrapper::RenameObjectL( const TDesC& aOldFileName,
    const TDesC& aNewFileName )
    {
    PRINT2( _L( "MM MTP => CMmMtpDpMetadataAccessWrapper::RenameObjectL old = %S, new = %S" ),
        &aOldFileName,
        &aNewFileName );

    if ( MmMtpDpUtility::IsVideoL( aOldFileName, iFramework ) )
        {
        iMmMtpDpMetadataVideoAccess->RenameRecordL( aOldFileName, aNewFileName );
        }
    else
        {
        CMTPObjectMetaData* object = CMTPObjectMetaData::NewLC();
        iFramework.ObjectMgr().ObjectL( aOldFileName, *object );
        iMmMtpDpMetadataMpxAccess->RenameObjectL( aOldFileName,
            aNewFileName,
            object->Uint( CMTPObjectMetaData::EFormatCode ) );
        CleanupStack::PopAndDestroy( object );
        }

    PRINT( _L( "MM MTP <= CMmMtpDpMetadataAccessWrapper::RenameObjectL" ) );
    }

// ---------------------------------------------------------------------------
// CMmMtpDpMetadataAccessWrapper::SetObjectMetadataValueL
// Deletes metadata information associated with the object
// ---------------------------------------------------------------------------
//
void CMmMtpDpMetadataAccessWrapper::DeleteObjectL( const TDesC& aFullFileName,
    const TUint aFormatCode )
    {
    PRINT( _L( "MM MTP => CMmMtpDpMetadataAccessWrapper::DeleteObjectL" ) );

    TMPXGeneralCategory category = Category( aFormatCode );

    // Have to do this.  File might not be in file system anymore, have to
    // reply on ObjectManager
    if ( ( aFormatCode == EMTPFormatCodeMP4Container )
        || ( aFormatCode == EMTPFormatCode3GPContainer )
        || ( aFormatCode == EMTPFormatCodeASF ) )
        {
        if ( MmMtpDpUtility::IsVideoL( aFullFileName, iFramework ) )
            {
            category = EMPXVideo;
            }
        else
            {
            category = EMPXSong;
            }
        }

    switch ( category )
        {
        case EMPXPlaylist:
        case EMPXSong:
            {
            iMmMtpDpMetadataMpxAccess->DeleteObjectL( aFullFileName, category );
            }
            break;

        case EMPXVideo:
            {
            iMmMtpDpMetadataVideoAccess->DeleteRecordL( aFullFileName );
            }
            break;

        default:
            PRINT( _L( "MM MTP <> CMmMtpDpMetadataAccessWrapper::DeleteObjectL default" ) );
            break;
        }

    PRINT( _L( "MM MTP <= CMmMtpDpMetadataAccessWrapper::DeleteObjectL" ) );
    }

// ---------------------------------------------------------------------------
// CMmMtpDpMetadataAccessWrapper::SetStorageRootL
// Sets current Drive info
// ---------------------------------------------------------------------------
//
void CMmMtpDpMetadataAccessWrapper::SetStorageRootL( const TDesC& aStorageRoot )
    {
    PRINT( _L( "MM MTP => CMmMtpDpMetadataAccessWrapper::SetStorageRootL" ) );

    iMmMtpDpMetadataVideoAccess->SetStorageRootL( aStorageRoot );
    iMmMtpDpMetadataMpxAccess->SetStorageRootL( aStorageRoot );

    PRINT( _L( "MM MTP <= CMmMtpDpMetadataAccessWrapper::SetStorageRootL" ) );
    }
// -----------------------------------------------------------------------------
// CMmMtpDpMetadataMpxAccess::SetImageObjPropL
// set image specific properties specific to videos
// -----------------------------------------------------------------------------
//
void CMmMtpDpMetadataAccessWrapper::SetImageObjPropL( const TDesC& aFullFileName,
    const TUint32 aWidth,
    const TUint32 aHeight )
    {
    if ( MmMtpDpUtility::IsVideoL( aFullFileName, iFramework ) )
        {
        iMmMtpDpMetadataVideoAccess->SetStorageRootL( aFullFileName );
        iMmMtpDpMetadataVideoAccess->SetImageObjPropL( aFullFileName, aWidth, aHeight );
        }
    }

// -----------------------------------------------------------------------------
// CMmMtpDpMetadataMpxAccess::GetImageObjPropL
// get image specific properties specific to videos
// -----------------------------------------------------------------------------
//
void CMmMtpDpMetadataAccessWrapper::GetImageObjPropL( const TDesC& aFullFileName,
    TUint32& aWidth,
    TUint32& aHeight )
    {
    if ( MmMtpDpUtility::IsVideoL( aFullFileName, iFramework ) )
        {
        iMmMtpDpMetadataVideoAccess->SetStorageRootL( aFullFileName );
        iMmMtpDpMetadataVideoAccess->GetImageObjPropL( aFullFileName, aWidth, aHeight );
        }
    }
// -----------------------------------------------------------------------------
// CMmMtpDpMetadataAccessWrapper::OpenSessionL
// Called when the MTP session is initialised
// -----------------------------------------------------------------------------
//
void CMmMtpDpMetadataAccessWrapper::OpenSessionL()
    {
    iOpenSession = ETrue;
    }

// -----------------------------------------------------------------------------
// CMmMtpDpMetadataAccessWrapper::CloseSessionL
//
// -----------------------------------------------------------------------------
//
void CMmMtpDpMetadataAccessWrapper::CloseSessionL()
    {
    if ( iOpenSession )
        {
        PRINT( _L( "MM MTP <> CMmMtpDpMetadataAccessWrapper::CloseSessionL close" ) );
        iMmMtpDpMetadataVideoAccess->CloseSessionL();
        iOpenSession = EFalse;
        }
    else
        {
        PRINT( _L( "MM MTP <> CMmMtpDpMetadataAccessWrapper::CloseSessionL alreay close" ) );
        }
    }

// ---------------------------------------------------------------------------
// CMmMtpDpMetadataAccessWrapper::Category
// Get category according format code
// ---------------------------------------------------------------------------
//
TMPXGeneralCategory CMmMtpDpMetadataAccessWrapper::Category( const TUint aFormatCode )
    {
    TMPXGeneralCategory category = EMPXNoCategory;
    switch ( aFormatCode )
        {
        case EMTPFormatCodeMP3:
        case EMTPFormatCodeWAV:
        case EMTPFormatCodeWMA:
        case EMTPFormatCodeAAC:
        case EMTPFormatCodeASF:
        case EMTPFormatCodeMP4Container:
        case EMTPFormatCode3GPContainer:
            {
            category = EMPXSong;
            }
            break;

        case EMTPFormatCodeAbstractAudioVideoPlaylist:
            {
            category = EMPXPlaylist;
            }
            break;

        case EMTPFormatCodeWMV:
            {
            category = EMPXVideo;
            }
            break;

        default:
            break;
        }
    return category;
    }

// ---------------------------------------------------------------------------
// CMmMtpDpMetadataAccessWrapper::GetAllAbstractMediaL
//
// ---------------------------------------------------------------------------
EXPORT_C void CMmMtpDpMetadataAccessWrapper::GetAllAbstractMediaL( const TDesC& aStoreRoot, CMPXMediaArray** aAbstractMedias, TMPXGeneralCategory aCategory )
    {
    iMmMtpDpMetadataMpxAccess->GetAllAbstractMediaL( aStoreRoot, aAbstractMedias, aCategory );
    }

// ---------------------------------------------------------------------------
// CMmMtpDpMetadataAccessWrapper::GetAllReferenceL
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CMmMtpDpMetadataAccessWrapper::GetAllReferenceL( CMPXMedia* aAbstractMedia, CDesCArray& aReferences )
    {
    iMmMtpDpMetadataMpxAccess->GetAllReferenceL( aAbstractMedia, aReferences );
    }

// ---------------------------------------------------------------------------
// CMmMtpDpMetadataAccessWrapper::GetAbstractMediaNameL
//
// ---------------------------------------------------------------------------
//
EXPORT_C HBufC* CMmMtpDpMetadataAccessWrapper::GetAbstractMediaNameL( CMPXMedia* aAbstractMedia, TMPXGeneralCategory aCategory )
    {
    return iMmMtpDpMetadataMpxAccess->GetAbstractMediaNameL( aAbstractMedia, aCategory );
    }

// -----------------------------------------------------------------------------
// CMmMtpDpMetadataAccessWrapper::AddObjectL
// Add object (music, video, playlist and abstract media) info to DB
// -----------------------------------------------------------------------------
//
void CMmMtpDpMetadataAccessWrapper::AddObjectL( const TDesC& aFullFileName, TUint aFormatCode, TUint aSubFormatCode )
    {
    PRINT1( _L( "MM MTP => CMmMtpDpMetadataAccessWrapper::AddObjectL aFullFileName = %S" ), &aFullFileName );

    if ( aFullFileName.Length() <= 0 )
        {
        User::Leave( KErrArgument );
        }
    if ( MmMtpDpUtility::IsVideoL( aFormatCode, aSubFormatCode ) )
        {
        PRINT( _L( "MM MTP => CMmMtpDpMetadataAccessWrapper::AddObjectL Addvideo" ) );
        iMmMtpDpMetadataVideoAccess->AddVideoL( aFullFileName );
        }
    else
        {
        if ( aFormatCode == EMTPFormatCodeM3UPlaylist
            || aFormatCode == EMTPFormatCodeMPLPlaylist
            || aFormatCode == EMTPFormatCodeAbstractAudioVideoPlaylist
            || aFormatCode == EMTPFormatCodeAbstractAudioPlaylist
            || aFormatCode == EMTPFormatCodeAbstractVideoPlaylist
            || aFormatCode == EMTPFormatCodeASXPlaylist
            || aFormatCode == EMTPFormatCodePLSPlaylist )
            {
            PRINT( _L( "MM MTP => CMmMtpDpMetadataAccessWrapper::AddObjectL AddPlaylist" ) );
            iMmMtpDpMetadataMpxAccess->AddAbstractMediaL( aFullFileName,
                EMPXPlaylist );
            }
        else
            {
            PRINT( _L( "MM MTP => CMmMtpDpMetadataAccessWrapper::AddObjectL AddSong" ) );
            iMmMtpDpMetadataMpxAccess->AddSongL( aFullFileName );
            }
        }

    PRINT( _L( "MM MTP <= CMmMtpDpMetadataAccessWrapper::AddObjectL" ) );
    }

// ---------------------------------------------------------------------------
// CMmMtpDpMetadataAccessWrapper::GetModifiedContentL
// Get Modified content
// ---------------------------------------------------------------------------
//
EXPORT_C void CMmMtpDpMetadataAccessWrapper::GetModifiedContentL( const TDesC& aStorageRoot,
    TInt& arrayCount,
    CDesCArray& aModifiedcontent )
    {
    iMmMtpDpMetadataMpxAccess->SetStorageRootL( aStorageRoot );
    iMmMtpDpMetadataMpxAccess->GetModifiedContentL( arrayCount, aModifiedcontent );
    }

EXPORT_C void CMmMtpDpMetadataAccessWrapper::CleanupDatabaseL()
    {
    iMmMtpDpMetadataVideoAccess->CleanupDatabaseL();
    }

// ---------------------------------------------------------------------------
// CMmMtpDpMetadataAccessWrapper::IsExistL
// if the playlsit exist in the MPX DB
// ---------------------------------------------------------------------------
//
TBool CMmMtpDpMetadataAccessWrapper::IsExistL( const TDesC& aSuid )
    {
    TParsePtrC parse( aSuid );
    iMmMtpDpMetadataMpxAccess->SetStorageRootL( parse.Drive() );
    return iMmMtpDpMetadataMpxAccess->IsExistL( aSuid );
    }

// -----------------------------------------------------------------------------
// CMmMtpDpMetadataAccessWrapper::AddDummyFile
// Add one dummy file to dummy files array
// -----------------------------------------------------------------------------
//
EXPORT_C void CMmMtpDpMetadataAccessWrapper::AddDummyFileL( const TDesC& aDummyFileName )
    {
    PRINT1( _L( "MM MTP <> CMmMtpDpMetadataAccessWrapper::AddDummyFile aDummyFileName(%S)" ), &aDummyFileName );
    iAbstractMediaArray->AppendL( aDummyFileName );
    }

// -----------------------------------------------------------------------------
// CMmMtpDpMetadataAccessWrapper::DeleteDummyFile
// Delete one dummy file from dummy files array
// -----------------------------------------------------------------------------
//
EXPORT_C void CMmMtpDpMetadataAccessWrapper::DeleteDummyFile( const TDesC& aDummyFileName )
    {
    PRINT1( _L( "MM MTP <> CMmMtpDpMetadataAccessWrapper::DeleteDummyFile aDummyFileName(%S)" ), &aDummyFileName );
    TInt pos = 0;
    if ( iAbstractMediaArray->Count() > 0 )
        {
        if ( 0 == iAbstractMediaArray->Find( aDummyFileName, pos ) )
            {
            PRINT1( _L( "MM MTP <> CMmMtpDpMetadataAccessWrapper::DeleteDummyFile pos = %d" ), pos );
            iAbstractMediaArray->Delete( pos );
            }
        }
    }

// -----------------------------------------------------------------------------
// CMmMtpDpMetadataAccessWrapper::CreateDummyFile
// Create a Dummy File from the virtual playlist URI
// -----------------------------------------------------------------------------
//
EXPORT_C void CMmMtpDpMetadataAccessWrapper::CreateDummyFile( const TDesC& aPlaylistName )
    {
    PRINT1( _L( "MM MTP => CMmMtpDpMetadataAccessWrapper::CreateDummyFile, filename = %S" ), &aPlaylistName );

    if ( MmMtpDpUtility::FormatFromFilename( aPlaylistName ) ==
        EMTPFormatCodeAbstractAudioVideoPlaylist )
        {
        RFile newfile;
        TInt err = newfile.Replace( iFramework.Fs(), aPlaylistName, EFileWrite );

        if ( err != KErrNone )
            {
            newfile.Close();
            PRINT1( _L( "MM MTP <> CMmMtpDpMetadataAccessWrapper::CreateDummyFile err =  %d" ), err );
            }
        else // File created OK
            {
            err = newfile.Flush();
            newfile.Close();
            err = iFramework.Fs().SetAtt( aPlaylistName, KEntryAttSystem | KEntryAttHidden,
                KEntryAttReadOnly | KEntryAttNormal );
            if ( err != KErrNone )
                PRINT1( _L( "MM MTP <> CMmMtpDpMetadataAccessWrapper::CreateDummyFile Dummy Playlist file created. err = %d" ), err );
            }
        }
    }

// -----------------------------------------------------------------------------
// CMmMtpDpMetadataAccessWrapper::RemoveDummyFiles
// Remove all dummy file of which format is "pla", and leave the "m3u"
// -----------------------------------------------------------------------------
//
void CMmMtpDpMetadataAccessWrapper::RemoveDummyFiles()
    {
    PRINT( _L( "MM MTP => CMmMtpDpMetadataAccessWrapper::RemoveDummyFiles" ) );

    TInt count = iAbstractMediaArray->Count();
    // Check if playlist file is a dummy file or an imported file
    for ( TInt i = 0; i < count; i++ )
        {
        if ( MmMtpDpUtility::FormatFromFilename( (*iAbstractMediaArray)[i] ) !=
            EMTPFormatCodeM3UPlaylist )
            {
            // delete the virtual playlist
            // iFramework has release don't use iFramework.FS()
            TInt err = iRfs.Delete( ( *iAbstractMediaArray )[i] );

            PRINT2( _L( "MM MTP <> CMmMtpDpMetadataAccessWrapper::RemoveDummyFile filename = %S, err %d" ),
                &( (*iAbstractMediaArray)[i] ),
                err );
            }
        else
            {
            // leave the Imported playlist in the file system
            PRINT1( _L( "MM MTP <> CMmMtpDpMetadataAccessWrapper::RemoveDummyFile, Don't delete m3u file [%S]" ), &( (*iAbstractMediaArray)[i] ) );
            }
        }
    PRINT( _L( "MM MTP <= CMmMtpDpMetadataAccessWrapper::RemoveDummyFiles" ) );
    }

// ---------------------------------------------------------------------------
// CMmMtpDpMetadataAccessWrapper::UpdateMusicCollectionL
// Update Music collection
// ---------------------------------------------------------------------------
//
EXPORT_C void CMmMtpDpMetadataAccessWrapper::UpdateMusicCollectionL()
    {
    iMmMtpDpMetadataMpxAccess->UpdateMusicCollectionL( );
    }

// end of file
