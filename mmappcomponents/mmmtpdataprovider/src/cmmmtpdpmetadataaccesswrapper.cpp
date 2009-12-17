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


#include <bautils.h>
#include <e32math.h>
#include <mtp/mmtpdataproviderframework.h>
#include <e32property.h>
#include <MtpPrivatePSKeys.h>

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
    
    iPlaylistArray = new ( ELeave ) CDesCArrayFlat( KMMMTPDummyFileArrayGranularity );
    
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

    delete iPlaylistArray;

    delete iMmMtpDpMetadataVideoAccess;
    delete iMmMtpDpMetadataMpxAccess;
    
    // unblock MPX
    RProperty::Set( KMtpPSUid, 
                    KMtpPSStatus, 
                    EMtpPSStatusUninitialized);
    PRINT( _L( "MM MTP <= CMmMtpDpMetadataAccessWrapper::~CMmMtpDpMetadataAccessWrapper" ) );
    }

// -----------------------------------------------------------------------------
// CMmMtpDpMetadataAccessWrapper::SetPlaylist
// Set playlist to DB
// -----------------------------------------------------------------------------
//
EXPORT_C void CMmMtpDpMetadataAccessWrapper::SetPlaylistL( const TDesC& aPlaylistFileName, CDesCArray& aRefFileArray )
    {
    PRINT1( _L( "MM MTP => CMmMtpDpMetadataAccessWrapper::SetPlaylistL aPlaylistFileName = %S" ), &aPlaylistFileName );

    if ( !MmMtpDpUtility::IsVideoL( aPlaylistFileName, iFramework ) )
        {
        iMmMtpDpMetadataMpxAccess->SetPlaylistL( aPlaylistFileName, aRefFileArray );
        }

    PRINT( _L( "MM MTP <= CMmMtpDpMetadataAccessWrapper::SetPlaylistL" ) );
    }

// ---------------------------------------------------------------------------
// CMmMtpDpMetadataAccessWrapper::AddMediaL
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

    TPtrC fullFileName( aObjectMetaData.DesC( CMTPObjectMetaData::ESuid ) );
    if ( MmMtpDpUtility::IsVideoL( fullFileName, iFramework ) )
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
    
   TMTPFormatCode formatCode = MmMtpDpUtility::FormatFromFilename( aOldFileName );
    if ( formatCode == EMTPFormatCodeWMV )
        {
        iMmMtpDpMetadataVideoAccess->RenameRecordL( aOldFileName, aNewFileName );
        }
    else
        {
        if ( !MmMtpDpUtility::IsVideoL( aNewFileName , iFramework ) )
            {
            iMmMtpDpMetadataMpxAccess->RenameObjectL( aOldFileName, aNewFileName, formatCode );
            }
        else
            {
            iMmMtpDpMetadataVideoAccess->RenameRecordL( aOldFileName, aNewFileName );
            }
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
// CMmMtpDpMetadataAccessWrapper::GetAllPlaylistL
// 
// ---------------------------------------------------------------------------
EXPORT_C void CMmMtpDpMetadataAccessWrapper::GetAllPlaylistL( const TDesC& aStoreRoot, CMPXMediaArray** aPlaylists )
    {
    iMmMtpDpMetadataMpxAccess->GetAllPlaylistL( aStoreRoot, aPlaylists );
    }

// ---------------------------------------------------------------------------
// CMmMtpDpMetadataAccessWrapper::GetAllReferenceL
// 
// ---------------------------------------------------------------------------
//s
EXPORT_C void CMmMtpDpMetadataAccessWrapper::GetAllReferenceL( CMPXMedia* aPlaylist, CDesCArray& aReferences )
    {
    iMmMtpDpMetadataMpxAccess->GetAllReferenceL( aPlaylist, aReferences );
    }

// ---------------------------------------------------------------------------
// CMmMtpDpMetadataAccessWrapper::GetPlaylistNameL
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CMmMtpDpMetadataAccessWrapper::GetPlaylistNameL( CMPXMedia* aPlaylist, TDes& aPlaylistName )
    {
    iMmMtpDpMetadataMpxAccess->GetPlaylistNameL( aPlaylist, aPlaylistName );
    }

// ----------------------------------------------------------------------------- 
// CMmMtpDpMetadataAccessWrapper::AddObjectL
// Add object (music, video and playlist) info to DB
// -----------------------------------------------------------------------------
//
void CMmMtpDpMetadataAccessWrapper::AddObjectL( const TDesC& aFullFileName, TBool aIsVideo /*= EFalse */ )
    {
    PRINT1( _L( "MM MTP => CMmMtpDpMetadataAccessWrapper::AddObjectL aFullFileName = %S" ), &aFullFileName );

    if ( aFullFileName.Length() <= 0)
        {
        User::Leave( KErrArgument );
        }
    if ( aIsVideo )
        {
        PRINT( _L( "MM MTP => CMmMtpDpMetadataAccessWrapper::AddObjectL Addvideo" ) );
        iMmMtpDpMetadataVideoAccess->AddVideoL( aFullFileName );
        }
    else
        {
        if ( MmMtpDpUtility::IsVideoL( aFullFileName, iFramework ) )
            {
            PRINT( _L( "MM MTP => CMmMtpDpMetadataAccessWrapper::AddObjectL Addvideo" ) );
            iMmMtpDpMetadataVideoAccess->AddVideoL( aFullFileName );
            }
        else
            {
            TMTPFormatCode formatCode = MmMtpDpUtility::FormatFromFilename( aFullFileName );

            if ( formatCode == EMTPFormatCodeM3UPlaylist
                || formatCode == EMTPFormatCodeMPLPlaylist
                || formatCode == EMTPFormatCodeAbstractAudioVideoPlaylist
                || formatCode == EMTPFormatCodeAbstractAudioPlaylist
                || formatCode == EMTPFormatCodeAbstractVideoPlaylist
                || formatCode == EMTPFormatCodeASXPlaylist
                || formatCode == EMTPFormatCodePLSPlaylist )
                {
                PRINT( _L( "MM MTP => CMmMtpDpMetadataAccessWrapper::AddObjectL AddPlaylist" ) );
                iMmMtpDpMetadataMpxAccess->AddPlaylistL( aFullFileName );
                }
            else
                {
                PRINT( _L( "MM MTP => CMmMtpDpMetadataAccessWrapper::AddObjectL AddSong" ) );
                iMmMtpDpMetadataMpxAccess->AddSongL( aFullFileName );
                }
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
    iPlaylistArray->AppendL( aDummyFileName );
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
    if ( iPlaylistArray->Count() > 0 )
        {
        if ( 0 == iPlaylistArray->Find( aDummyFileName, pos ) )
            {
            PRINT1( _L( "MM MTP <> CMmMtpDpMetadataAccessWrapper::DeleteDummyFile pos = %d" ), pos );
            iPlaylistArray->Delete( pos );
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
    
    TInt count = iPlaylistArray->Count();
    // Check if playlist file is a dummy file or an imported file
    for ( TInt i = 0; i < count; i++ )
        {
        if ( MmMtpDpUtility::FormatFromFilename( (*iPlaylistArray)[i] ) !=
            EMTPFormatCodeM3UPlaylist )
            {
            // delete the virtual playlist
            // iFramework has release don't use iFramework.FS()
            TInt err = iRfs.Delete( (*iPlaylistArray)[i] );

            PRINT2( _L( "MM MTP <> CMmMtpDpMetadataAccessWrapper::RemoveDummyFile filename = %S, err %d" ),
                &( (*iPlaylistArray)[i] ),
                err );
            }
        else
            {
            // leave the Imported playlist in the file system
            PRINT1( _L( "MM MTP <> CMmMtpDpMetadataAccessWrapper::RemoveDummyFile, Don't delete m3u file [%S]" ), &( (*iPlaylistArray)[i] ) );
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
