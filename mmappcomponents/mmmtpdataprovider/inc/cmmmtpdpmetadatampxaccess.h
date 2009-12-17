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
* Description:  Meta data Mpx access
*
*/


#ifndef CMMMTPDPMETADATAMPXACCESS_H
#define CMMMTPDPMETADATAMPXACCESS_H

#include <badesca.h>

#include "cmmmtpdpperflog.h"

// keep here to avoid warning on urel
_LIT( KMpxCollectionNewL, "MpxCollectionNewL" );
_LIT( KMpxCollectionAddL, "MpxCollectionAddL" );
_LIT( KMpxCollectionGetL, "MpxCollectionGetL" );
_LIT( KMpxCollectionSetL, "MpxCollectionSetL" );
_LIT( KMpxCollectionGetPlaylist, "MpxCollectionGetPlaylist" );
_LIT( KMpxCollectionGetReference, "MpxCollectionGetReference" );
_LIT( KMpxCollectionFindAllLValidate, "MpxCollectionValidate" );
_LIT( KMpxCollectionFindAllLBeforeAdd, "KMpxCollectionFindAllLBeforeAdd" );

// forward declacration
class CMPXMedia;
class CMTPObjectMetaData;
class MMPXCollectionHelper;
class MMTPDataProviderFramework;
class MMTPType;
class TMPXAttributeData;

const TInt KStorageRootMaxLength = 10;

typedef struct
    {
    TMTPObjectPropertyCode iPropCode;
    TMPXAttributeData iMpxAttrib;
    } TMetadataTable;

/**
* Controls access to the music MPX store database.  Allows access to the
* information about any of the music stored in any of the MPX db's on any
* of the drives. Also supports deletions and insertions into the db.
*/
class CMmMtpDpMetadataMpxAccess : public CBase
    {
public:

    static CMmMtpDpMetadataMpxAccess* NewL( RFs& aRfs, MMTPDataProviderFramework& aFramework );

    /**
    * Destructor
    */
    ~CMmMtpDpMetadataMpxAccess();

public:
    /**
    * Get all playlists from MPX database in the assigned store
    * @param aStoreRoot, specify in which drive playlists are stored
    * @param aPlaylists, return result array
    */
    void GetAllPlaylistL( const TDesC& aStoreRoot, CMPXMediaArray** aPlaylists );

    /**
    * Get all references of specified playlist
    * @param aPlaylist, specify of which reference should be get
    * @param aReferences, return result array which stored handles of all references
    */
    void GetAllReferenceL( CMPXMedia* aPlaylist, CDesCArray& aReferences );

    /**
    * Get an playlist name from CMPXMedia object
    * @param aPlaylist, specify the source
    * @param aPlaylistName, return result
    */
    void GetPlaylistNameL( CMPXMedia* aPlaylist, TDes& aPlaylistName );

    /**
    * Adds Songs info to Mpx DB
    * @param aFullFileName, full file name of Media file
    */
    void AddSongL( const TDesC& aFullFileName );

    /**
    * Adds Playlist to Mpx DB
    * @param aFullFileName, full file name of Playlist file
    */
    void AddPlaylistL( const TDesC& aFullFileName );

    /**
    * Set playlist to DB
    * @param aPlaylistFileName, full file name of playlist file
    * @param aRefFileArray, a array to store the full file name of media files
    */
    void SetPlaylistL( const TDesC& aPlaylistFileName,
            CDesCArray& aRefFileArray );

    void GetObjectMetadataValueL( const TUint16 aPropCode,
            MMTPType& aNewData,
            const CMTPObjectMetaData& aObjectMetaData );

    /**
    * Set object property and updated object in DB according property code,
    * only for update, not for creation
    * @param aPropCode, property code of aObjectMetaData
    * @param aNewData, object property value which will be set into
    *     aObjectMetaData
    * @param aObjectMetaData, owner of the property which should be
    *     updated into database
    */
    void SetObjectMetadataValueL( const TUint16 aPropCode,
            const MMTPType& aNewData,
            const CMTPObjectMetaData& aObjectMetaData );

    /**
    * Set object property and updated object in DB according property code,
    * only for update, not for creation
    * @param aPropCode, property code of aObjectMetaData
    * @param aNewData, object property value which will be set into
    *     aObjectMetaData
    * @param aSuid, full file name of object of which properties need to set
    */
    void SetObjectMetadataValueL( const TUint16 aPropCode,
            const MMTPType& aNewData,
            const TDesC& aSuid );

    CMPXMedia* FindWMPMediaLC( TMPXAttributeData aWMPMediaID, TBool aFlag );

    /**
    * Update the Sync flag for those not synchronized, Update the Modified
    * flag for those have been modified, and delete the stale records for
    * files that have been deleted.
    */
    void UpdateMusicCollectionL();

    /**
    * Remove the file from MPX DB
    * @param aFullFileName, the file to remove
    * @param aCategory, the category of the file
    */
    void DeleteObjectL( const TDesC& aFullFileName, TMPXGeneralCategory aCategory );

    /**
    * Rename the filename onto MPX DB
    * @param aOldFileName, the old file to rename
    * @param aNewFileName, the new file name
    * @param aFormatCode, the format of object
    */
    void RenameObjectL( const TDesC& aOldFileName, const TDesC& aNewFileName,
            TUint aFormatCode );

    /**
    * Set current  drive info
    * @param aStorageRoot, current drive info
    */
    void SetStorageRootL( const TDesC& aStorageRoot );

    /**
    * Get modified content
    * @param arrayCount, count of modified content
    * @param aModifiedcontent, modified files name list
    */
    void GetModifiedContentL( TInt& arrayCount, CDesCArray& aModifiedcontent );
    
    /**
    * if the playlsit exist in the MPX DB
    * @param aSuid, the suid of playlsit
    */
    TBool IsExistL( const TDesC& aSuid );

private:
    CMmMtpDpMetadataMpxAccess( RFs& aFs, MMTPDataProviderFramework& aFramework );

    void ConstructL();

    void SetDefaultL( CMPXMedia& aMediaProp );

    /**
    * Set property value into MPX object according to property code,
    * only used by member functions of this class.
    * @param aPropCode, specify property code of aMediaProp
    * @param aNewData, object property value which will be set into
    *     aObjectMetaData
    * @param aObjectMetaData, owner of the property which should be
    *     inserted or updated into database
    */
    void SetMetadataValueL( const TUint16 aPropCode,
            const MMTPType& aNewData,
            CMPXMedia& aMediaProp );

    TMPXAttributeData MpxAttribFromPropL( const CMPXMedia& aMedia,
            const TUint16 aPropCode );

    MMPXCollectionHelper* CollectionHelperL();

private:
    // File server to allow file access
    RFs& iRfs;

    TBuf<KStorageRootMaxLength> iStoreRoot;

    MMPXCollectionHelper* iCollectionHelper;
    
    MMTPDataProviderFramework& iFramework;  

#if defined(_DEBUG) || defined(MMMTPDP_PERFLOG)
    CMmMtpDpPerfLog* iPerfLog;
#endif

    };

#endif // CMMMTPDPMETADATAMPXACCESS_H
