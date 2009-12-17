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
* Description:  Meta data Access Wrapper
*
*/


#ifndef CMMMTPDPMETADATAACCESSWRAPPER_H
#define CMMMTPDPMETADATAACCESSWRAPPER_H

#include <mtp/cmtpobjectmetadata.h>
#include <mtp/mtpprotocolconstants.h>
#include <mpxmediageneraldefs.h>
#include <mtp/mmtpdataproviderframework.h>

// forward declacration
class CMmMtpDpMetadataMpxAccess;
class CMmMtpDpMetadataVideoAccess;

class RFs;
class MMTPType;
class CMTPTypeObjectPropList;
class CMPXMediaArray;
class CMPXMedia;
class MMTPDataProviderFramework;

class CMmMtpDpMetadataAccessWrapper : public CBase
    {
public:
    static CMmMtpDpMetadataAccessWrapper* NewL( RFs& aRfs, 
        MMTPDataProviderFramework& aFramework );

    /**
    * Destructor
    */
    ~CMmMtpDpMetadataAccessWrapper();

public:
    /**
    * Get all playlists from MPX database in the assigned store
    * @param aStoreRoot, specify in which drive playlists are stored
    * @param aPlaylists, return result array
    */
    IMPORT_C void GetAllPlaylistL( const TDesC& aStoreRoot,
        CMPXMediaArray** aPlaylists );

    /**
    * Get all references of specified playlist
    * @param aPlaylist, specify of which reference should be get
    * @param aReferences, return result array which stored handles of all references
    */
    IMPORT_C void GetAllReferenceL( CMPXMedia* aPlaylist,
        CDesCArray& aReferences );

    /**
    * Get an playlist name from CMPXMedia object
    * @param aPlaylist, specify the source
    * @param aPlaylistName, return result
    */
    IMPORT_C void GetPlaylistNameL( CMPXMedia* aPlaylist, TDes& aPlaylistName );

    /**
    * Add object (music, video and playlist) info to DB
    * @param aFullFileName, full file name of file
    * @return void
    */
    void AddObjectL( const TDesC& aFullFileName, TBool aIsVideo = EFalse );

    /**
    * Set playlist to DB
    * @param aPlaylistFileName, full file name of playlist file
    * @param aRefFileArray, a array to store the full file name of media files
    * @return void
    */
    IMPORT_C void SetPlaylistL( const TDesC& aPlaylistFileName,
        CDesCArray& aRefFileArray );

    /**
    * Gets a piece of metadata from the collection
    * @param aPropCode, specify property code of aMediaProp
    * @param aNewData, object property value which will be get from
    *    aObjectMetaData
    * @param aObjectMetaData, owner of the property which should be
    *    inserted or updated into database
    * @return void
    */
    IMPORT_C void GetObjectMetadataValueL( const TUint16 aPropCode,
        MMTPType& aNewData,
        const CMTPObjectMetaData& aObjectMetaData );

    /**
    * Sets a piece of metadata in the collection
    * @param aPropCode, specify property code of aMediaProp
    * @param aNewData, object property value which will be set into
    *     aObjectMetaData
    * @param aObjectMetaData, owner of the property which should be
    *     inserted or updated into database
    * @return void
    */
    void SetObjectMetadataValueL( const TUint16 aPropCode,
        const MMTPType& aNewData,
        const CMTPObjectMetaData& aObjectMetaData );

    /**
    * Renames the file part of a record in the collection database
    * @param aFile, old full file name of Media file
    * @param aNewFileName, new file name need update
    * @return void
    */
    IMPORT_C void RenameObjectL( const TDesC& aOldFileName, const TDesC& aNewFileName );

    /**
    * Deletes metadata information associated with the object
    * @param aFullFileName, full file name of Media file
    * @param aFormatCode, format code
    */
    void DeleteObjectL( const TDesC& aFullFileName, const TUint aFormatCode );

    /**
    * Set current  drive info
    * @param aStorageRoot, current drive info
    */
    void SetStorageRootL( const TDesC& aStorageRoot );

    /*
    * set image specific properties specific to videos
    * @param aFullFileName, the fullFileName of object
    * @param aWidth,  the width of an object in pixels to set
    * @parem aHeight,  the height of an object in pixels to set
    */
    void SetImageObjPropL( const TDesC& aFullFileName,
           const TUint32 aWidth,
           const TUint32 aHeight );

    /*
    * get image specific properties specific to videos
    * @param aFullFileName, the fullFileName of object
    * @param aWidth,  the width of an object in pixels to get
    * @parem aHeight,  the height of an object in pixels to get
    */
    void GetImageObjPropL( const TDesC& aFullFileName,
            TUint32& aWidth,
            TUint32& aHeight );
    
    /**
    * Get Modified object from DB
    * @param aStorageRoot, the root path of storage
    * @param arrayCount, array count
    * @param aRefFileArray, a array to store the full file name of media files
    * @return void
    */
    IMPORT_C void GetModifiedContentL( const TDesC& aStorageRoot, 
        TInt& arrayCount,
        CDesCArray& aRefFileArray );
 
    /**
    * Updated Music DB
    * @return void
    */
    IMPORT_C void UpdateMusicCollectionL(); 
    
    /*
    * Called when the MTP session is initialised
    */
    void OpenSessionL();

    /*
    * clean up db resource especially for video dp
    */
    void CloseSessionL();
    
    /**
    * Cleanup database
    */
    IMPORT_C void CleanupDatabaseL();
    
    /**
    * if the playlsit exist in the MPX DB
    * @param aSuid, the suid of playlsit
    */
    TBool IsExistL( const TDesC& aSuid );
    
    // related to dummy files
    /**
    * Add one dummy file to dummy files array
    */
    IMPORT_C void AddDummyFileL( const TDesC& aDummyFileName );
    
    /**
    * Delete one dummy file from dummy files array
    */    
    IMPORT_C void DeleteDummyFile( const TDesC& aDummyFileName );
    
    /**
    * Create a Dummy File from the virtual playlist URI
    * @param aPlaylistName, specify the filename of the dummy file
    */
    IMPORT_C void CreateDummyFile( const TDesC& aPlaylistName );
    
private:

    CMmMtpDpMetadataAccessWrapper( RFs& aRfs, MMTPDataProviderFramework& aFramework );

    void ConstructL();

    TMPXGeneralCategory Category( const TUint aFormatCode );
    
    /**
    * Remove all dummy file of which format is "pla", and leave the "m3u"
    */
    void RemoveDummyFiles();

private:
    // Data
    RFs& iRfs;
    CMmMtpDpMetadataMpxAccess* iMmMtpDpMetadataMpxAccess;
    CMmMtpDpMetadataVideoAccess* iMmMtpDpMetadataVideoAccess;
    TBool iOpenSession;
    
    MMTPDataProviderFramework& iFramework;  
    
    CDesCArray* iPlaylistArray;
    
    };

#endif // CMMMTPDPMETADATAACCESSWRAPPER_H
