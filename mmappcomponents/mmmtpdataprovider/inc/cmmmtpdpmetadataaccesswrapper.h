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
    * Get all abstract medias from MPX database in the assigned store
    * @param aStoreRoot, specify in which drive abstract medias are stored
    * @param aAbstractMedias, return result array
    * @param aCategory, indicate the category of abstract medias
    */
    IMPORT_C void GetAllAbstractMediaL( const TDesC& aStoreRoot,
        CMPXMediaArray** aAbstractMedias,
        TMPXGeneralCategory aCategory );

    /**
    * Get all references of specified abstract medias
    * @param aAbstractMedia, specify of which reference should be get
    * @param aReferences, return result array which stored handles of all references
    */
    IMPORT_C void GetAllReferenceL( CMPXMedia* aAbstractMedia,
        CDesCArray& aReferences );

    /**
    * Get an abstract media name from CMPXMedia object
    * @param aAbstractMedia, specify the source
    * @param aCategory, specify the category
    * @return, name of the source, ownership transferred
    */
    IMPORT_C HBufC* GetAbstractMediaNameL( CMPXMedia* aAbstractMedia,
        TMPXGeneralCategory aCategory );
    /**
    * Add object (music, video, playlist and abstract album) info to DB
    * @param aFullFileName, full file name of file
    * @return void
    */
    void AddObjectL( const TDesC& aFullFileName, TUint aFormatCode, TUint aSubFormatCode );

    /**
    * Set abstract media to DB
    * @param aAbstractMediaFileName, full file name of abstract media file
    * @param aRefFileArray, a array to store the full file name of media files
    * @param aCategory, the category of abstract media
    * @return void
    */
    IMPORT_C void SetAbstractMediaL( const TDesC& aAbstractMediaFileName,
        CDesCArray& aRefFileArray,
        TMPXGeneralCategory aCategory );

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

    CDesCArray* iAbstractMediaArray;

    };

#endif // CMMMTPDPMETADATAACCESSWRAPPER_H
