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
* Description:  Implementation for MTP Common Function
*
*/


#include <bautils.h>
#include <e32math.h>
#include <f32file.h>
#include <mtp/mtpprotocolconstants.h>
#include <mtp/mtpdatatypeconstants.h>
#include <3gplibrary/mp4lib.h>
#include <caf/content.h>
#include <mtp/cmtpobjectmetadata.h>
#include <mtp/mmtpdataproviderframework.h>
#include <mtp/mmtpobjectmgr.h>
#include <mtp/tmtptypeuint32.h>

// for asf mimetype parsing
#ifdef __WINDOWS_MEDIA
#include <hxmetadatakeys.h>
#include <hxmetadatautil.h>
#endif

#include "mmmtpdputility.h"
#include "mmmtpdpfiledefs.h"
#include "mmmtpdplogger.h"

using namespace ContentAccess;

// local to this file, non standard mimetype used for ASF parsing with helix
_LIT( KHxMimeTypeWma, "audio/x-hx-wma" );
_LIT( KHxMimeTypeWmv, "video/x-hx-wmv" );

// -----------------------------------------------------------------------------
// MmMtpDpUtility::FormatFromFilename
// Utility function to get the type of an object from the filename
// The recommended way from the MS implementation
// These should be kept in sync with the object formats supported
// -----------------------------------------------------------------------------
//
EXPORT_C TMTPFormatCode MmMtpDpUtility::FormatFromFilename( const TDesC& aFullFileName )
    {
    if ( aFullFileName.Right( 1 ).CompareF( KTxtBackSlash ) == 0 ) // We have a directory name
        {
        return EMTPFormatCodeAssociation;
        }

    TParsePtrC file( aFullFileName );

    // need to do it in popularity of format, to optmize performance
    if ( file.Ext().CompareF( KTxtExtensionMP3 ) == 0 )
            return EMTPFormatCodeMP3;

#ifdef __WINDOWS_MEDIA
    if ( file.Ext().CompareF( KTxtExtensionWMA ) == 0 )
            return EMTPFormatCodeWMA;
#endif // __WINDOWS_MEDIA
    
    if ( ( file.Ext().CompareF( KTxtExtensionMP4 ) == 0 ) || ( file.Ext().CompareF( KTxtExtensionM4A ) == 0 ) )
        return EMTPFormatCodeMP4Container;

    if ( ( file.Ext().CompareF( KTxtExtension3GP ) == 0 ) || ( file.Ext().CompareF( KTxtExtensionO4A ) == 0 ) || ( file.Ext().CompareF( KTxtExtensionO4V ) == 0 ) )
        return EMTPFormatCode3GPContainer;

    if ( file.Ext().CompareF( KTxtExtensionAAC ) == 0 )
        return EMTPFormatCodeAAC;
    
    if ( file.Ext().CompareF( KTxtExtensionWAV ) == 0 )
        return EMTPFormatCodeWAV;
    
#ifdef __WINDOWS_MEDIA
    if ( file.Ext().CompareF( KTxtExtensionWMV ) == 0 )
        return EMTPFormatCodeWMV;
    
    if ( file.Ext().CompareF( KTxtExtensionASF ) == 0 )
        return EMTPFormatCodeASF;

#endif // __WINDOWS_MEDIA

    if ( file.Ext().CompareF( KTxtExtensionODF ) == 0 )
        {
        HBufC8* mime = MmMtpDpUtility::ContainerMimeType( file.FullName() );
        if ( mime != NULL )
            {
            // 3GP
            if ( mime->CompareF( KMimeTypeAudio3gpp ) == 0
                || mime->CompareF( KMimeTypeVideo3gpp ) == 0 )
                {
                delete mime;
                mime = NULL;
                return EMTPFormatCode3GPContainer;
                }
            }

        if ( mime != NULL )
            {
            delete mime;
            mime = NULL;
            }
        }

    if (( file.Ext().CompareF( KTxtExtensionPLA ) == 0 ) || ( file.Ext().CompareF( KTxtExtensionVIR ) == 0 ) )
        return EMTPFormatCodeAbstractAudioVideoPlaylist;

    if ( file.Ext().CompareF( KTxtExtensionM3U ) == 0 )
        return EMTPFormatCodeM3UPlaylist;

    return EMTPFormatCodeUndefined;
    }

// -----------------------------------------------------------------------------
// MmMtpDpUtility::HasMetadata
// Utility function to determine whether a format has metadata support
// -----------------------------------------------------------------------------
//
TBool MmMtpDpUtility::HasMetadata( TUint16 aObjFormatCode )
    {
    if ( aObjFormatCode == EMTPFormatCodeMP3
        || aObjFormatCode == EMTPFormatCodeWMA
        || aObjFormatCode == EMTPFormatCodeAAC
        || aObjFormatCode == EMTPFormatCodeWAV
        || aObjFormatCode == EMTPFormatCodeMP4Container
        || aObjFormatCode == EMTPFormatCode3GPContainer
        || aObjFormatCode == EMTPFormatCodeWMV
        || aObjFormatCode == EMTPFormatCodeASF )
        {
        return ETrue;
        }

    return EFalse;
    }

// -----------------------------------------------------------------------------
// MmMtpDpUtility::IsVideoL (Slow Version)
// Utility function to determine whether a format is Video or not
// -----------------------------------------------------------------------------
//
EXPORT_C TBool MmMtpDpUtility::IsVideoL( const TDesC& aFullFileName )
    {
    PRINT1( _L( "MM MTP => MmMtpDpUtility::IsVideoL (Slow Version) aFullFileName = %S" ), &aFullFileName );

    TParse pathParser;
    User::LeaveIfError( pathParser.Set( aFullFileName, NULL, NULL ) );

    TPtrC ext( pathParser.Ext() );

    if ( ext.Length() <= 0 )
        {
        PRINT( _L( "MM MTP <> MmMtpDpUtility::IsVideoL ext len <= 0, return false" ) );
        return EFalse;
        }

    // move WMV early to optmize comparison
    if ( ext.CompareF( KTxtExtensionWMV ) == 0 )
        {
        return ETrue;
        }
    else if ( ext.CompareF( KTxtExtensionMP4 ) == 0
        || ext.CompareF( KTxtExtension3GP ) == 0
        || ext.CompareF( KTxtExtensionODF ) == 0 
        || ext.CompareF( KTxtExtensionASF ) == 0 )
            {
            HBufC8* mimetype = ContainerMimeType( aFullFileName );
            User::LeaveIfNull( mimetype );

            CleanupStack::PushL( mimetype ); // + mimetype

            TMmMtpSubFormatCode subFormatCode;

            User::LeaveIfError( SubFormatCodeFromMime( *mimetype, subFormatCode ) );
            CleanupStack::PopAndDestroy( mimetype ); // - mimetype

            if ( subFormatCode == EMTPSubFormatCodeVideo )
                {
                return ETrue;
                }
            else
                {
                return EFalse;
                }
            }
    else if ( ext.CompareF( KTxtExtensionO4V ) == 0 )
        return ETrue;

    // other format, as audio
    return EFalse;
    }

// -----------------------------------------------------------------------------
// MmMtpDpUtility::IsVideoL (Fast Version)
// Utility function to determine whether a format is Video or not
// -----------------------------------------------------------------------------
//
TBool MmMtpDpUtility::IsVideoL( const TDesC& aFullFileName, 
    const MMTPDataProviderFramework& aFramework )
    {
    PRINT1( _L( "MM MTP => MmMtpDpUtility::IsVideoL (Fast Version) aFullFileName = %S" ), &aFullFileName );
    
    CMTPObjectMetaData* info = CMTPObjectMetaData::NewLC(); // + info
    aFramework.ObjectMgr().ObjectL( aFullFileName, *info );

    TUint formatCode = info->Uint( CMTPObjectMetaData::EFormatCode );
    TUint subFormatCode = info->Uint( CMTPObjectMetaData::EFormatSubCode );

    CleanupStack::PopAndDestroy( info ); // - info
    
    if ( formatCode == EMTPFormatCodeWMV )
        {
        return ETrue;
        }
    else if ( ( formatCode == EMTPFormatCodeMP4Container )
        || ( formatCode == EMTPFormatCode3GPContainer )
        || ( formatCode == EMTPFormatCodeASF ) )
        {
        if ( subFormatCode == EMTPSubFormatCodeAudio )
            {
            return EFalse;
            }
        else if ( subFormatCode == EMTPSubFormatCodeVideo )
            {
            return ETrue;
            }
        }

    PRINT( _L( "MM MTP <= MmMtpDpUtility::IsVideoL (Fast Version)" ) );

    // other format, as audio
    return EFalse;
    }

// -----------------------------------------------------------------------------
// MmMtpDpUtility::ValidateFilename
// Check the filename to see if it exceeds Symbian 256 limit.
// -----------------------------------------------------------------------------
//
TBool MmMtpDpUtility::ValidateFilename( const TDesC& aPathName,
    const TDesC& aFileName )
    {
    TBool result = ETrue;

    if ( ( aPathName.Length() + aFileName.Length() ) > KMaxFileName )
        {
        result = EFalse;
        }

    return result;
    }

// -----------------------------------------------------------------------------
// MmMtpDpUtility::GetObjectSizeL
// Get the filesize.
// -----------------------------------------------------------------------------
//
TUint64 MmMtpDpUtility::GetObjectSizeL( RFs& aFs, const TDesC& aFileName )
    {
    TEntry fileInfo;
    // Shouldn't leave
    User::LeaveIfError( aFs.Entry( aFileName, fileInfo ) );
    return fileInfo.iSize;
    }

// -----------------------------------------------------------------------------
// MmMtpDpUtility::GetDateModifiedL
// Get the file date modified.
// -----------------------------------------------------------------------------
//
TTime MmMtpDpUtility::GetObjectDateModifiedL( RFs& aFs, const TDesC& aFullFileName )
    {
    TTime dataModified;
    TEntry fileInfo;
    // Shouldn't leave
    User::LeaveIfError( aFs.Entry( aFullFileName, fileInfo ) );

    dataModified = fileInfo.iModified;

    return dataModified;
    }

// -----------------------------------------------------------------------------
// MmMtpDpUtility::GetProtectionStatusL
// Get the file protection status.
// -----------------------------------------------------------------------------
//
TUint16 MmMtpDpUtility::GetProtectionStatusL( RFs& aFs, const TDesC& aFullFileName )
    {
    TUint16 protectionStatus = EMTPProtectionNoProtection;

    TEntry fileInfo;
    // Shouldn't leave
    User::LeaveIfError( aFs.Entry( aFullFileName, fileInfo ) );
    if ( fileInfo.IsReadOnly() )
        {
        protectionStatus = EMTPProtectionReadOnly;
        }

    return protectionStatus;
    }

// -----------------------------------------------------------------------------
// MmMtpDpUtility::CheckPropType
// Check if property type match datatype.
// -----------------------------------------------------------------------------
//
TMTPResponseCode MmMtpDpUtility::CheckPropType(TUint16 aPropertyCode, TUint16 aDataType)
    {
    PRINT2( _L( "MM MTP => MmMtpDpUtility::CheckPropCode aPropertyCode = 0x%x, aDataType = 0x%x" ), 
        aPropertyCode, 
        aDataType );

    TMTPResponseCode responseCode = EMTPRespCodeOK;
    switch ( aPropertyCode )
        {
        //Access denied Properties
        case EMTPObjectPropCodeStorageID:
        case EMTPObjectPropCodeObjectFormat:
        case EMTPObjectPropCodeProtectionStatus:
        case EMTPObjectPropCodeObjectSize:
        case EMTPObjectPropCodeParentObject:
        case EMTPObjectPropCodePersistentUniqueObjectIdentifier:
        case EMTPObjectPropCodeDateCreated:
        case EMTPObjectPropCodeDateModified:
        case EMTPObjectPropCodeDateAdded:
        case EMTPObjectPropCodeNonConsumable:
        case EMTPObjectPropCodeVideoBitRate:
            {
            responseCode = EMTPRespCodeAccessDenied;
            }
            break;

        //String properties
        case EMTPObjectPropCodeObjectFileName: // 0xDC07
        case EMTPObjectPropCodeName: // 0xDC44
        case EMTPObjectPropCodeArtist: // 0xDC46
        case EMTPObjectPropCodeGenre: // 0xDC8C
        case EMTPObjectPropCodeParentalRating: // 0xDC94
        case EMTPObjectPropCodeComposer: // 0xDC96
        case EMTPObjectPropCodeOriginalReleaseDate: // 0xDC99
        case EMTPObjectPropCodeAlbumName: // 0xDC9A
        case EMTPObjectPropCodeEncodingProfile: // 0xDEA1
            {
            if ( aDataType != EMTPTypeString )
                {
                responseCode = EMTPRespCodeInvalidObjectPropFormat;
                }
            }
            break;

        case EMTPObjectPropCodeDescription:
            {
            if ( aDataType != EMTPTypeAUINT16 )
                {
                responseCode = EMTPRespCodeInvalidObjectPropFormat;
                }
            }
            break;

        //Uint32 properties
        case EMTPObjectPropCodeWidth: // 0xDC87
        case EMTPObjectPropCodeHeight: // 0xDC88
        case EMTPObjectPropCodeDuration: // 0xDC89
        case EMTPObjectPropCodeUseCount: // 0xDC91
        case EMTPObjectPropCodeSampleRate: // 0xDE93
        case EMTPObjectPropCodeAudioWAVECodec: // 0xDE99
        case EMTPObjectPropCodeAudioBitRate: // 0xDE9A
        case EMTPObjectPropCodeVideoFourCCCodec: // 0xDE9B
        case EMTPObjectPropCodeFramesPerThousandSeconds: // 0xDE9D
        case EMTPObjectPropCodeKeyFrameDistance: // 0xDE9E
            {
            if ( aDataType != EMTPTypeUINT32 )
                {
                responseCode = EMTPRespCodeInvalidObjectPropFormat;
                }
            }
            break;

        //Uint16 properties
        case EMTPObjectPropCodeTrack: // 0xDC8B
        case EMTPObjectPropCodeDRMStatus: // 0xDC9D
        case EMTPObjectPropCodeNumberOfChannels: // 0xDE94
        case EMTPObjectPropCodeScanType: // 0xDE97
            {
            if ( aDataType != EMTPTypeUINT16 )
                {
                responseCode = EMTPRespCodeInvalidObjectPropFormat;
                }
            }
            break;

        default:
            {
            responseCode = EMTPRespCodeInvalidObjectPropCode;
            }
            break;
        }
    PRINT1( _L( "MM MTP <= MmMtpDpUtility::CheckPropCode responseCode = 0x%x" ), 
        responseCode );
    return responseCode;
    }

// -----------------------------------------------------------------------------
// MmMtpDpUtility::UpdateObjectFileName
// Update object file name.
// -----------------------------------------------------------------------------
//
TInt MmMtpDpUtility::UpdateObjectFileName( RFs& aFs,
    const TDesC& aFullFileName,
    TDes& aNewName )
    {
    TInt bufSize = aNewName.MaxLength() < KMaxFileName ? aNewName.MaxLength() : KMaxFileName;

    TParsePtrC parser( aFullFileName );
    TPtrC ptr( parser.DriveAndPath() );

    if ( aNewName.Length() + ptr.Length() > bufSize )
        {
        return KErrOverflow;
        }
    else
        {
        aNewName.Insert( 0, ptr );
        }

    return aFs.Rename( aFullFileName, aNewName );
    }
    
// -----------------------------------------------------------------------------
// MetadataAccessWrapper::ContainerMimeType
// Get mime type from file
// -----------------------------------------------------------------------------
//
HBufC8* MmMtpDpUtility::ContainerMimeType( const TDesC& aFullPath )
    {
    PRINT( _L( "MM MTP => MmMtpDpUtility::ContainerMimeType" ) );

    // parse the file path
    TParse pathParser;
    TInt retCode = pathParser.Set( aFullPath, NULL, NULL );
    if ( retCode != KErrNone )
        {
        PRINT( _L( "MM MTP <> MmMtpDpUtility::ContainerMimeType parse path failed" ) );
        return NULL;
        }

    // get the extension of file
    TPtrC ext( pathParser.Ext() );
    if ( ext.Length() <= 0 )
        {
        PRINT( _L( "MM MTP <> MmMtpDpUtility::ContainerMimeType file ext len == 0" ) );
        return NULL;
        }

    HBufC8* mimebuf = NULL;
    TInt err = KErrNone;

    // MP4/3GP
    if ( ext.CompareF( KTxtExtensionMP4 ) == 0
        || ext.CompareF( KTxtExtension3GP ) == 0 )
        {
        TRAP( err, mimebuf = Mp4MimeTypeL( aFullPath ) );
        PRINT1( _L("MM MTP <> MmMtpDpUtility::ContainerMimeType, Mp4MimeTypeL err = %d"), err );
        }
    else if ( ext.CompareF( KTxtExtensionODF ) == 0 )
        {
        TRAP( err, mimebuf = Mp4MimeTypeL( aFullPath ) );
        PRINT1( _L("MM MTP <> MmMtpDpUtility::ContainerMimeType, OdfMimeTypeL err = %d"), err );
        }
#ifdef __WINDOWS_MEDIA
    else if ( ext.CompareF( KTxtExtensionASF ) == 0 )
        {
        TRAP( err, mimebuf = AsfMimeTypeL( aFullPath ) );
        PRINT1( _L("MM MTP <> MmMtpDpUtility::ContainerMimeType, AsfMimeTypeL err = %d"), err );
        }
#endif
    
    PRINT( _L( "MM MTP <= MmMtpDpUtility::ContainerMimeType" ) );
    return mimebuf;
    }

// -----------------------------------------------------------------------------
// MetadataAccessWrapper::Mp4MimeTypeL
// Get mime type from mp4 file
// -----------------------------------------------------------------------------
//
HBufC8* MmMtpDpUtility::Mp4MimeTypeL( const TDesC& aFullPath )
    {
    PRINT( _L( "MM MTP => MmMtpDpUtility::Mp4MimeTypeL" ) );
    HBufC8* mimebuf = NULL;
    TParsePtrC file( aFullPath );
    
    if ( file.Ext().CompareF( KTxtExtensionMP4 ) == 0
        || file.Ext().CompareF( KTxtExtension3GP ) == 0 )
        {
        // get mime from file
        MP4Handle mp4Handle = NULL;

        RBuf buf;
        TInt err = buf.Create( aFullPath.Length() + 1 );
        User::LeaveIfError( err );
        buf = aFullPath;

        // open mp4 file
        MP4Err mp4err = MP4ParseOpen( &mp4Handle, (MP4FileName) buf.PtrZ() );

        if ( mp4err == MP4_OK )
            {

            mp4_u32 videoLength, videoType, videoWidth, videoHeight, timeScale;
            mp4_double frameRate;

            // get video description
            mp4err = MP4ParseRequestVideoDescription( mp4Handle, &videoLength,
                &frameRate, &videoType, &videoWidth, &videoHeight, &timeScale );

            // not the video file
            if ( mp4err == MP4_NO_VIDEO )
                {
                mp4_u32 audioLength, audioType, timeScale, averateBitRate;
                mp4_u8 framesPerSample;

                // get audio description
                mp4err = MP4ParseRequestAudioDescription( mp4Handle,
                    &audioLength, &audioType, &framesPerSample, &timeScale,
                    &averateBitRate );

                if ( mp4err == MP4_OK )
                    {
                    if ( file.Ext().CompareF( KTxtExtension3GP ) == 0 )
                        {
                        mimebuf = KMimeTypeAudio3gpp().Alloc();
                        }
                    else
                        {
                        mimebuf = KMimeTypeAudioMp4().Alloc();
                        }
                    }
                }
            // is video file
            else if ( mp4err == MP4_OK )
                {
                if ( file.Ext().CompareF( KTxtExtension3GP ) == 0 )
                    {
                    mimebuf = KMimeTypeVideo3gpp().Alloc();
                    }
                else
                    {
                    mimebuf = KMimeTypeVideoMp4().Alloc();
                    }
                }

            // Close mp4 parser
            if ( mp4Handle != NULL )
                {
                MP4ParseClose( mp4Handle );
                }
            }

        buf.Close();
        }
    else
        {
        User::Leave( KErrNotSupported );
        }
    
    if ( mimebuf == NULL )
        {
        User::Leave( KErrNotFound );
        }
    PRINT( _L( "MM MTP <= MmMtpDpUtility::Mp4MimeTypeL" ) );
    return mimebuf;
    }

// -----------------------------------------------------------------------------
// MetadataAccessWrapper::OdfMimeTypeL
// Get mime type from odf file
// -----------------------------------------------------------------------------
//
HBufC8* MmMtpDpUtility::OdfMimeTypeL( const TDesC& aFullPath )
    {
    PRINT( _L( "MM MTP => MmMtpDpUtility::OdfMimeTypeL" ) );
    HBufC8* mimebuf = NULL;
    
    TParsePtrC file( aFullPath );
        
    if ( file.Ext().CompareF( KTxtExtensionODF ) == 0 )
        {    
        CContent* content = CContent::NewL( aFullPath );
        CleanupStack::PushL( content ); // + content
        
        HBufC* buffer = HBufC::NewL( KMimeTypeMaxLength );
        CleanupStack::PushL( buffer ); // + buffer
        
        TPtr data = buffer->Des();
        TInt err = content->GetStringAttribute( EMimeType, data );
                
        if ( err == KErrNone )
            {
            mimebuf = HBufC8::New( buffer->Length() );
    
            if (mimebuf == NULL)
                {
                User::LeaveIfError( KErrNotFound );
                }
            
            mimebuf->Des().Copy( *buffer );
            }
        
        // leave if NULL
        if ( mimebuf == NULL )
            {
            User::Leave( KErrNotFound );
            }
        
        CleanupStack::PopAndDestroy( buffer ); // - buffer
        CleanupStack::PopAndDestroy( content ); // - content
        }
    else
        {
        User::Leave( KErrNotSupported );
        }
    
    PRINT( _L( "MM MTP <= MmMtpDpUtility::OdfMimeTypeL" ) );
    return mimebuf;
    }

// -----------------------------------------------------------------------------
// MetadataAccessWrapper::AsfMimeTypeL
// Get mime type from asf file
// -----------------------------------------------------------------------------
//
HBufC8* MmMtpDpUtility::AsfMimeTypeL( const TDesC& aFullPath )
    {
    PRINT( _L( "MM MTP => MmMtpDpUtility::AsfMimeTypeL" ) );
    
    HBufC8* mimebuf = NULL;
    
#ifdef __WINDOWS_MEDIA
    TParsePtrC file( aFullPath );
    
    if ( file.Ext().CompareF( KTxtExtensionASF ) == 0 )
        {
        CHXMetaDataUtility *hxUtility = CHXMetaDataUtility::NewL();
        CleanupStack::PushL( hxUtility );
    
        hxUtility->OpenFileL( aFullPath );
        
        HXMetaDataKeys::EHXMetaDataId id;
        TUint count = 0;
        TBool isAudio = EFalse;
        hxUtility->GetMetaDataCount( count );
        for ( TUint i = 0; i < count; i++ )
            {    
            HBufC* buf = NULL;
            hxUtility->GetMetaDataAt( i, id, buf ); 

            if ( id == HXMetaDataKeys::EHXMimeType )
                {
                TPtr des = buf->Des();
                
                if ( des.Find( KHxMimeTypeWma() ) != KErrNotFound )
                    {
                    isAudio = ETrue;
                    }
                else if ( des.Find( KHxMimeTypeWmv() ) != KErrNotFound )
                    {
                    PRINT( _L( "MM MTP <> MmMtpDpUtility::AsfMimeTypeL, video" ) );
                    mimebuf = KMimeTypeVideoWm().Alloc();
                    break;
                    }
                }
            else if ( i == count - 1 )
                {
                if ( isAudio )
                    {
                    PRINT( _L( "MM MTP <> MmMtpDpUtility::AsfMimeTypeL, audio" ) );
                    mimebuf = KMimeTypeAudioWm().Alloc();
                    }
                else
                    {
                    User::Leave( KErrNotFound );
                    }
                }
            }

        hxUtility->ResetL();
        CleanupStack::PopAndDestroy( hxUtility );        
        }
    else
        {
        User::Leave( KErrNotSupported );
        }

#else
    User::Leave( KErrNotSupported );
#endif
    
    PRINT( _L( "MM MTP <= MmMtpDpUtility::AsfMimeTypeL" ) );
    return mimebuf;
    }

// -----------------------------------------------------------------------------
// MetadataAccessWrapper::SubFormatCodeFromMime
// Get subformat code from mime string
// -----------------------------------------------------------------------------
//
TInt MmMtpDpUtility::SubFormatCodeFromMime( const TDesC8& aMimeType, 
    TMmMtpSubFormatCode& aSubFormatCode )
    {
    PRINT( _L( "MM MTP => MmMtpDpUtility::SubFormatCodeFromMime" ) );

    if ( aMimeType.CompareF( KMimeTypeVideoMp4 ) == 0
        || aMimeType.CompareF( KMimeTypeVideo3gpp ) == 0
#ifdef __WINDOWS_MEDIA
        || aMimeType.CompareF( KMimeTypeVideoWm ) == 0
#endif
        )
        {
        aSubFormatCode = EMTPSubFormatCodeVideo;
        }
    else if ( aMimeType.CompareF( KMimeTypeAudioMp4 ) == 0
        || aMimeType.CompareF( KMimeTypeAudio3gpp ) == 0
#ifdef __WINDOWS_MEDIA
        || aMimeType.CompareF( KMimeTypeAudioWm ) == 0
#endif
        )
        {
        aSubFormatCode = EMTPSubFormatCodeAudio;
        }
    else
        {
        PRINT( _L( "MM MTP <= MmMtpDpUtility::SubFormatCodeFromMime format not supported" ) );
        return KErrNotFound;
        }

    PRINT( _L( "MM MTP <= MmMtpDpUtility::SubFormatCodeFromMime" ) );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// MetadataAccessWrapper::GetDrmStatus
// Get drm status code from file
// -----------------------------------------------------------------------------
//
EXPORT_C TInt MmMtpDpUtility::GetDrmStatus( const TDesC& aFullFileName )
    {
    PRINT1( _L( "MM MTP => MmMtpDpUtility::GetDrmStatus, full file name: %S" ), &aFullFileName );
    TParsePtrC file( aFullFileName );
    TInt drmStatus = EMTPDrmStatusUnknown;

    if ( ( file.Ext().CompareF( KTxtExtensionODF ) == 0 ) ||
         ( file.Ext().CompareF( KTxtExtensionO4A ) == 0 ) ||
         ( file.Ext().CompareF( KTxtExtensionO4V ) == 0 ) )
        {
        CContent* content = NULL;

        TRAPD( err, content = CContent::NewL( aFullFileName ) );

        if ( err == KErrNone )
            {
            TInt value = 0;

            content->GetAttribute( EIsProtected, value );

            if ( value != 0 )
                {
                drmStatus = EMTPDrmStatusProtected;
                }
            else
                {
                drmStatus = EMTPDrmStatusNotProtected;
                }
            }

        if ( content )
            {
            delete content;
            content = NULL;
            }
        }

    PRINT1( _L( "MM MTP <= MmMtpDpUtility::GetDrmStatus, drmStatus: %d" ), drmStatus );
    
    return drmStatus;
    }

//end of file
