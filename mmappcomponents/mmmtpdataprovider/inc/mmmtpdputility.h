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
* Description:  Definition for MTP Utility File
*
*/


#ifndef MMMTPDPUTILITY_H
#define MMMTPDPUTILITY_H

#include <mtp/mtpprotocolconstants.h>

#include "mmmtpdpfiledefs.h"

// forward declacration
class CMTPObjectMetaData;
class MMTPDataProviderFramework;

class MmMtpDpUtility
    {
public:
    /**
    * Utility function to get the type of an object from the filename
    * @param aFullFileName, full path of file name
    * @return TMTPFormatCode for the passed file name
    */
    IMPORT_C static TMTPFormatCode FormatFromFilename( const TDesC& aFullFileName );

    /**
    * Utility function to decide if has metadata
    * @param aObjFormatCode, object format code
    * @return TBool for decide if has metadata
    */
    static TBool HasMetadata( TUint16 aObjFormatCode );

    /**
    * Utility function to decide if the file is video.
    * @param aFullFileName, the full file name
    * @return TBool for decide if the file is video
    */
    IMPORT_C static TBool IsVideoL( const TDesC& aFullFileName );

    /**
    * Utility function to decide if the file is video.
    * For internal use, this is fast version by querying framework DB
    * @param aFullFileName, the full file name
    * @param aFramework, the mtp framework instance pointer, default is null
    * @return TBool for decide if the file is video
    */
    static TBool IsVideoL( const TDesC& aFullFileName, const MMTPDataProviderFramework& aFramework );

    /**
    * Check the filename length to see if it exceeds Symbian 256 limit.
    * @param aPathName, the path name of the file
    * @param aFileName, the file to be check
    * @return TBool for decide if the file name validate
    */
    static TBool ValidateFilename( const TDesC& aPathName,
        const TDesC& aFileName );

    /**
    * Check the file size, if file doesn't exist, leave
    * @param aFs, handle of file server session
    * @param aFileName, fine name
    * @return TUint64 size for specified file
    */
    static TUint64 GetObjectSizeL( RFs& aFs, const TDesC& aFileName );

    /**
    * Get file date modified info
    * @param aFs , handle of file server session
    * @param aFullFileName, fine name
    * @return TTime for specified file
    */
    static TTime GetObjectDateModifiedL( RFs& aFs, const TDesC& aFullFileName );

    /**
    * Check if file is read-only, if file doesn't exist, leave
    * @param aFs , handle of file server session
    * @param aFullFileName, the file to be check
    */
    static TUint16 GetProtectionStatusL( RFs& aFs, const TDesC& aFullFileName );

    /**
    * Check if prop type and data type match
    * @param aPropertyCode, the proptery code
    * @param aDataType, the data type
    * @param TMTPResponseCode, decide if match
    */
    static TMTPResponseCode CheckPropType( TUint16 aPropertyCode,
            TUint16 aDataType );

    /**
    * Update object file name
    * @param aFs
    * @param aPathName, the path name of the file
    * @param aFullFileName, the file to be check
    */
    static TInt UpdateObjectFileName( RFs& aFs,
        const TDesC& aFullFileName,
        TDes& aNewName );

    /**
    * Get the mime type of file, support mp4/3gp now
    * @param aFullPath, full path of file
    * @return mime type string, if return NULL, no mime or failed
    */
    static HBufC8* ContainerMimeType( const TDesC& aFullPath );

    /**
    * Get the Mime from Mp4 file
    * @param aFullFileName, full path of file
    * @return mime type string, if return NULL, no mime or failed
    */
    static HBufC8* Mp4MimeTypeL( const TDesC& aFullFileName );

    /**
    * Get the Mime from Odf file
    * @param aFullFileName, full path of file
    * @return mime type string, if return NULL, no mime or failed
    */
    static HBufC8* OdfMimeTypeL( const TDesC& aFullFileName );

    /**
    * Get the Mime from Asf file
    * @param aFullFileName, full path of file
    * @return mime type string, if return NULL, no mime or failed
    */
    static HBufC8* AsfMimeTypeL( const TDesC& aFullFileName );

    /**
    * get the sub format code by mime
    * @param aMimeType, mime type
    * @param aSubFormatCode, output the sub format code
    * @return if success, return KErrNone, otherwise, error code
    */
    static TInt SubFormatCodeFromMime( const TDesC8& aMimeType, TMmMtpSubFormatCode& aSubFormatCode );

    /**
    * Get DRM status
    * @param aFullFileName, full file name
    * @return return DRM status code
    */
    IMPORT_C static TInt GetDrmStatus( const TDesC& aFullFileName );

    };

#endif // MMMTPDPUTILITY_H
