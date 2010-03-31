/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Extracts metadata from a file
*  Version     : %version: da1mmcf#38.1.4.2.6.1.7 % << Don't touch! Updated by Synergy at check-out.
*
*/


#include <e32base.h>
#include <badesca.h>
#include <apgcli.h>
#include <MetaDataUtility.h>
#include <MetaDataFieldContainer.h>

#ifdef RD_MPX_TNM_INTEGRATION
#include <hash.h>
#include <f32file.h>
#include <sysutil.h>
#include <thumbnailmanager.h>
#endif //RD_MPX_TNM_INTEGRATION


#include <mpxlog.h>
#include <mpxmedia.h>
#include <mpxcollectionpath.h>
#include <mpxcollectiontype.h>
#include <mpxdrmmediautility.h>

#include <mpxmediamusicdefs.h>
#include <mpxmediageneraldefs.h>
#include <mpxmediaaudiodefs.h>
#include <mpxmediadrmdefs.h>
#include <mpxmediamtpdefs.h>

#include "mpxmetadataextractor.h"
#include "mpxfileinfoutility.h"
#ifdef RD_MPX_TNM_INTEGRATION
_LIT( KImageFileType, "image/jpeg" );
const TInt KMPXTimeoutTimer = 3000000; // 3 seconds
const TInt KMPXMaxThumbnailRequest = 5; 
#endif //RD_MPX_TNM_INTEGRATION

#ifdef ABSTRACTAUDIOALBUM_INCLUDED
_LIT( KNonEmbeddedArtExt, ".alb" );
#endif
//Helper functions
static void FindAndReplaceForbiddenChars(TDes& aString, TInt aLen)
    {
    MPX_ASSERT(aLen == aString.Length());

    for (TInt i = 0; i < aLen; ++i)
        {
        if (aString[i] == TText('\t'))
            {
            aString[i] = TText(' ');
            }
        }
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CMPXMetadataExtractor::CMPXMetadataExtractor(
                                  RFs& aFs,
                                  RApaLsSession& aAppArc,
                                  RPointerArray<CMPXCollectionType>& aTypes  )
                                  : iFs( aFs ),
                                    iAppArc( aAppArc ),
                                    iSupportedTypes( aTypes ),
                                    iOutstandingThumbnailRequest(0),
                                    iTNMBlockCount(0)
    {

    }


// ---------------------------------------------------------------------------
// 2nd Phase Constructor
// ---------------------------------------------------------------------------
//
void CMPXMetadataExtractor::ConstructL()
    {
    iMetadataUtility = CMetaDataUtility::NewL();
    iDrmMediaUtility = CMPXDrmMediaUtility::NewL();
    iFileInfoUtil    = CMPXFileInfoUtility::NewL();

#ifdef RD_MPX_TNM_INTEGRATION
    // Create Thumbnail Manager instance. This object is the observer.
    iTNManager = CThumbnailManager::NewL( *this );
    iTNManager->SetFlagsL( CThumbnailManager::EDefaultFlags );
    iTNManager->SetQualityPreferenceL( CThumbnailManager::EOptimizeForQuality);
    // create wait loop
    iTNSyncWait = new (ELeave) CActiveSchedulerWait;
    iTimer = CPeriodic::NewL( CActive::EPriorityIdle );
#endif //RD_MPX_TNM_INTEGRATION
    }

// ---------------------------------------------------------------------------
// Two-Phased Constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CMPXMetadataExtractor* CMPXMetadataExtractor::NewL(
                                  RFs& aFs,
                                  RApaLsSession& aAppArc,
                                  RPointerArray<CMPXCollectionType>& aTypes  )
    {
    CMPXMetadataExtractor* self = new( ELeave ) CMPXMetadataExtractor( aFs,
                                                                       aAppArc,
                                                                       aTypes );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMPXMetadataExtractor::~CMPXMetadataExtractor()
    {
    delete iMetadataUtility;
    delete iFileInfoUtil;
    delete iDrmMediaUtility;
#ifdef RD_MPX_TNM_INTEGRATION
    delete iTNManager;
    if (iTNSyncWait && iTNSyncWait->IsStarted() )
        {
        iTNSyncWait->AsyncStop();
        }
    delete iTNSyncWait;
    delete iTimer;
#endif //RD_MPX_TNM_INTEGRATION
    
    MPX_DEBUG2("CMPXMetadataExtractor: TNM Block Count: %d ", iTNMBlockCount );
    }

// ---------------------------------------------------------------------------
// Constructs a media properties object
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXMetadataExtractor::CreateMediaL( const TDesC& aFile,
                                                   CMPXMedia*& aNewProperty,
                                                   TBool aMetadataOnly )
    {
    // make a copy of aFile
    HBufC* fileName = HBufC::NewL(KMaxFileName);
    CleanupStack::PushL( fileName );
    fileName->Des().Append( aFile );
    MPX_DEBUG2("CMPXMetadataExtractor::CreateMediaL %S <---", fileName );

    RArray<TInt> contentIDs;
    contentIDs.AppendL( KMPXMediaIdGeneral );
    contentIDs.AppendL( KMPXMediaIdAudio );
    contentIDs.AppendL( KMPXMediaIdMusic );
    contentIDs.AppendL( KMPXMediaIdDrm );
    contentIDs.AppendL( KMPXMediaIdMTP );
    aNewProperty = NULL;
    CMPXMedia* media = CMPXMedia::NewL( contentIDs.Array() );
    CleanupStack::PushL( media );
    contentIDs.Close();

    // CMPXMedia default types

    media->SetTObjectValueL<TMPXGeneralType>( KMPXMediaGeneralType,
                                              EMPXItem );
    media->SetTObjectValueL<TMPXGeneralCategory>( KMPXMediaGeneralCategory,
                                                  EMPXSong );

    TParsePtrC parse( *fileName );

    // Title, default is file name
    media->SetTextValueL( KMPXMediaGeneralTitle,
                          parse.Name() );

    // Default album track
    media->SetTextValueL( KMPXMediaMusicAlbumTrack,
                          KNullDesC );

    // Set the Mime Type and collection UID
    //
    if( !aMetadataOnly )
        {
        TInt index(KErrNotFound);
        TInt count( iSupportedTypes.Count() );
        for (TInt i=0; i <count; ++i)
            {
            TInt index2(KErrNotFound);
            const CDesCArray& exts = iSupportedTypes[i]->Extensions();
            const TDesC& ext = parse.Ext();
            if (!exts.FindIsq(ext, index2))
                { // found
                index = i;
                break;
                }
            }
        if( KErrNotFound != index )
            {
            MPX_DEBUG1("CMPXMetadataExtractor::CreateMediaPropertiesL apparc <---" );
            TInt mimeIndex = SupportedContainerTypeL( *fileName, index );
            User::LeaveIfError( mimeIndex );
            MPX_DEBUG1("CMPXMetadataExtractor::CreateMediaPropertiesL apparc --->" );

            media->SetTextValueL( KMPXMediaGeneralMimeType,
                                  iSupportedTypes[index]->Mimetypes()[mimeIndex] );

            media->SetTObjectValueL( KMPXMediaGeneralCollectionId,
                                     iSupportedTypes[index]->Uid() );
            }
        else
            {
            User::Leave(KErrNotSupported);
            }
        }
    else // other case use apparc to fetch and set mimetype
        {
        TDataType dataType;
        TUid dummyUid(KNullUid);
        iAppArc.AppForDocument(*fileName, dummyUid, dataType);
        media->SetTextValueL( KMPXMediaGeneralMimeType,dataType.Des() );
        }
        
    // Use file handle here
    //
    RFile file;
    TInt err = file.Open( iFs, *fileName, EFileRead | EFileShareReadersOrWriters );
    CleanupClosePushL(file);
    
    // Metadata related
    //
    if( err == KErrNone )
        {
        const TDesC& mimeType = media->ValueText( KMPXMediaGeneralMimeType );
        HBufC8* mimeType8 = HBufC8::NewLC( mimeType.Length() );
        mimeType8->Des().Append( mimeType );
        TRAPD( metadataerror, iMetadataUtility->OpenFileL( file, *mimeType8 ) );
        CleanupStack::PopAndDestroy( mimeType8 );

        // No problem
        if( KErrNone == metadataerror )
            {
            // Add TRAPD to capture exception KErrNoMemory.
            //If album art size is too large, trap this exception and SetDefaultL.
            //Fix EYLU-7ESE5L
            TRAPD( err, SetMediaPropertiesL( *media, *fileName ) );
            if ( KErrNoMemory == err )
                {
                SetDefaultL( *media );
                }
            }
        else  // Error, Set defaults
            {
            SetDefaultL( *media );
            }

        // Reset the utility
        iMetadataUtility->ResetL();
        }
    else // Error, Set defaults
        {
        SetDefaultL( *media );
        }
    
    // Common properties that we can extract
    //
    SetExtMediaPropertiesL( *media, *fileName, aMetadataOnly, file, err );
    CleanupStack::PopAndDestroy(&file);

    // Set the pointers now that the object is ready
    //
    CleanupStack::Pop( media );
    aNewProperty = media;

    CleanupStack::PopAndDestroy( fileName );
    MPX_DEBUG1("CMPXMetadataExtractor::CreateMediaPropertiesL --->");
    }

// ---------------------------------------------------------------------------
// Sets all of the default media properties
// ---------------------------------------------------------------------------
//
void CMPXMetadataExtractor::SetDefaultL( CMPXMedia& aMediaProp )
    {
    // Comment
    aMediaProp.SetTextValueL( KMPXMediaGeneralComment,
                              KNullDesC );
    // Artist
    aMediaProp.SetTextValueL( KMPXMediaMusicArtist,
                              KNullDesC );
    // Album
    aMediaProp.SetTextValueL( KMPXMediaMusicAlbum,
                              KNullDesC );
    // Year
    aMediaProp.SetTObjectValueL<TInt64>( KMPXMediaMusicYear,
                                         (TInt64) 0 );
    // Track
    aMediaProp.SetTextValueL( KMPXMediaMusicAlbumTrack,
                              KNullDesC );
    // Genre
    aMediaProp.SetTextValueL( KMPXMediaMusicGenre,
                              KNullDesC );
    // Composer
    aMediaProp.SetTextValueL( KMPXMediaMusicComposer,
                              KNullDesC );
    // Album artFilename
    aMediaProp.SetTextValueL( KMPXMediaMusicAlbumArtFileName,
                              KNullDesC );
    // URL
    aMediaProp.SetTextValueL( KMPXMediaMusicURL,
                              KNullDesC );
    }

// ---------------------------------------------------------------------------
// Sets media object attributes from metadata utilities
// ---------------------------------------------------------------------------
//
void CMPXMetadataExtractor::SetMediaPropertiesL( CMPXMedia& aMedia,
                                                 const TDesC& aFile )
    {
    MPX_DEBUG1("CMPXMetadataExtractor::SetMediaPropertiesL <---" );

    const CMetaDataFieldContainer& metaCont =
                                          iMetadataUtility->MetaDataFieldsL();
    TInt count( metaCont.Count() );
    for( TInt i=0; i<count; ++i )
        {
        TMetaDataFieldId fieldType;

        HBufC* value = NULL;
        metaCont.FieldIdAt( i, fieldType );  // get the field type
        
        // get the value, except for album art
        if ( fieldType != EMetaDataJpeg )
           {
           TRAPD( err, value = metaCont.At( i, fieldType ).AllocL() );
           CleanupStack::PushL( value );
           if ( KErrNone != err )
               {
               MPX_DEBUG2("CMPXMetadataExtractor::SetMediaPropertiesL - error = %i", err);           
               CleanupStack::PopAndDestroy( value );
               continue;
               }     
           }
        
        switch( fieldType )
            {
            case EMetaDataSongTitle:
                {
                TPtr valptr = value->Des();
                valptr.Trim();
                TInt vallen = value->Length();
                if (vallen>0)
                    {
                    FindAndReplaceForbiddenChars(valptr, vallen);
                    aMedia.SetTextValueL(KMPXMediaGeneralTitle, *value);
                    }
                break;
                }
            case EMetaDataArtist:
                {
                TPtr valptr = value->Des();
                valptr.Trim();
                TInt vallen = value->Length();
                if (vallen>0)
                    {
                    FindAndReplaceForbiddenChars(valptr, vallen);
                    aMedia.SetTextValueL(KMPXMediaMusicArtist, *value);
                    }
                break;
                }
            case EMetaDataAlbum:
                {
                TPtr valptr = value->Des();
                valptr.Trim();
                TInt vallen = value->Length();
                if (vallen>0)
                    {
                    FindAndReplaceForbiddenChars(valptr, vallen);
                    aMedia.SetTextValueL(KMPXMediaMusicAlbum, *value );
                    }
                break;
                }
            case EMetaDataYear:
                {
                // Perform the date time conversion
                TLex lexer( *value );
                TInt year( 0 );
                lexer.Val( year );

                // Convert from TInt -> TDateTime -> TTime -> TInt64
                TDateTime dt;
                dt.SetYear( year );
                TTime time( dt );
                aMedia.SetTObjectValueL<TInt64>( KMPXMediaMusicYear,
                                                 time.Int64() );
                break;
                }
            case EMetaDataComment:
                {
                aMedia.SetTextValueL( KMPXMediaGeneralComment,
                                      *value );
                break;
                }
            case EMetaDataAlbumTrack:
                {
                aMedia.SetTextValueL( KMPXMediaMusicAlbumTrack,
                                      *value );
                break;
                }
            case EMetaDataGenre:
                {
                TPtr valptr = value->Des();
                valptr.Trim();
                TInt vallen = value->Length();
                if (vallen>0)
                    {
                    FindAndReplaceForbiddenChars(valptr, vallen);
                    aMedia.SetTextValueL(KMPXMediaMusicGenre, *value);
                    }
                break;
                }
            case EMetaDataComposer:
                {
                TPtr valptr = value->Des();
                valptr.Trim();
                TInt vallen = value->Length();
                if (vallen>0)
                    {
                    FindAndReplaceForbiddenChars(valptr, vallen);
                    aMedia.SetTextValueL(KMPXMediaMusicComposer, *value);
                    }
                break;
                }
            case EMetaDataUrl:
            case EMetaDataUserUrl:  // fall through
                {
                aMedia.SetTextValueL( KMPXMediaMusicURL,
                                      *value );
                break;
                }
            case EMetaDataJpeg:
                {
#ifdef RD_MPX_TNM_INTEGRATION
                MPX_PERF_START(CMPXMetadataExtractor_SetMediaPropertiesL_JPEG_TNM);
                TPtrC8 ptr8 = metaCont.Field8( EMetaDataJpeg );
                HBufC8* value8; 
                TRAPD( err, value8 = ptr8.AllocL() );
                if ( KErrNone != err )
                    {
                    MPX_DEBUG2("CMPXMetadataExtractor::SetMediaPropertiesL - error jpeg = %i", err);           
                    User::Leave( err );  
                    }                 
                CleanupStack::PushL( value8 );
                AddMediaAlbumArtL( aMedia, aFile, *value8 );
                CleanupStack::Pop( value8 );
                MPX_PERF_END(CMPXMetadataExtractor_SetMediaPropertiesL_JPEG_TNM);
#else //RD_MPX_TNM_INTEGRATION
                aMedia.SetTextValueL( KMPXMediaMusicAlbumArtFileName,
                                      aFile );
#endif //RD_MPX_TNM_INTEGRATION
                break;
                }
            case EMetaDataCopyright:
                {
                aMedia.SetTextValueL( KMPXMediaGeneralCopyright,
                                      *value );
                break;
                }
            case EMetaDataOriginalArtist:  // fall through
            case EMetaDataVendor:          // fall through
            case EMetaDataRating:          // fall through
            case EMetaDataUniqueFileIdentifier:  // fall through
            case EMetaDataDuration:        // fall through
            case EMetaDataDate:            // fall through
                {
                // not used
                break;
                }
            default:
                {
                //ASSERT(0);
                break;
                }
            }
        if (fieldType != EMetaDataJpeg)
            {
            CleanupStack::PopAndDestroy( value );       
            }
        }

    MPX_DEBUG1("CMPXMetadataExtractor::SetMediaPropertiesL --->" );
    }

// ---------------------------------------------------------------------------
// Sets extra media properties not returned by metadata utilities
// ---------------------------------------------------------------------------
//
void CMPXMetadataExtractor::SetExtMediaPropertiesL( CMPXMedia& aProp, 
                                                    const TDesC& aFile,
                                                    TBool aMetadataOnly,
                                                    RFile& aFileHandle,
                                                    TInt aFileErr )
    {
    MPX_DEBUG1("CMPXMetadataExtractor::SetExtMediaPropertiesL <---");

    // DB Flags to set
    //
    TUint dbFlags(KMPXMediaGeneralFlagsSetOrUnsetBit);

    // File Path
    //
    TParsePtrC parse( aFile );
    aProp.SetTextValueL( KMPXMediaGeneralUri,
                         aFile );
    aProp.SetTextValueL( KMPXMediaGeneralDrive,
                         parse.Drive() );

    // DRM Rights
    //
    CMPXMedia* drm = NULL;
    TRAPD( drmError, iDrmMediaUtility->InitL( aFile );
                     drm = CMPXMedia::NewL( *iDrmMediaUtility->GetMediaL( KMPXMediaDrmProtected.iAttributeId |
                                                                          KMPXMediaDrmRightsStatus.iAttributeId ) );
         );

    TBool prot(EFalse);
    if( drm )
        {
        CleanupStack::PushL( drm );
        MPX_DEBUG1("CMPXMetadataExtractor::SetExtMediaPropertiesL -- is a drm file");
        if( drm->IsSupported( KMPXMediaDrmProtected ) )
            {
            prot = drm->ValueTObjectL<TBool>( KMPXMediaDrmProtected );
            MPX_DEBUG2("CMPXMetadataExtractor::SetExtMediaPropertiesL protected %i", prot);
            }

        TMPXMediaDrmRightsStatus status(EMPXDrmRightsFull);
        if( drm->IsSupported( KMPXMediaDrmRightsStatus ) )
            {
            status = drm->ValueTObjectL<TMPXMediaDrmRightsStatus>(KMPXMediaDrmRightsStatus);
            aProp.SetTObjectValueL<TInt>(KMPXMediaDrmRightsStatus, status );
            MPX_DEBUG2("CMPXMetadataExtractor::SetExtMediaPropertiesL -- status %i", status);
            }

        // Set DB flags
        dbFlags |= KMPXMediaGeneralFlagsIsDrmProtected;
        if( status != EMPXDrmRightsFull && status != EMPXDrmRightsRestricted && status != EMPXDrmRightsPreview )
            {
            dbFlags |= KMPXMediaGeneralFlagsIsDrmLicenceInvalid;
            }
        CleanupStack::PopAndDestroy( drm );
        }

    // Trapped PV DRM error. If -46, we know the file has no rights
    //
    if( drmError == KErrPermissionDenied )
        {
        dbFlags |= KMPXMediaGeneralFlagsIsDrmLicenceInvalid;
        }
    else
        {
        User::LeaveIfError( drmError );
        }

    aProp.SetTObjectValueL<TBool>( KMPXMediaDrmProtected, prot );
    aProp.SetTObjectValueL<TUint16>( KMPXMediaMTPDrmStatus, (TUint16)prot );
    
    iDrmMediaUtility->Close();
    
    // File Size
    //
    TInt size( 0 );
    if( aFileErr == KErrNone )
        {
        aFileHandle.Size( size );
        aProp.SetTObjectValueL<TInt>( KMPXMediaGeneralSize,
                                      size );

        // Duration, bitrate, samplerate, etc
        //
        if( !aMetadataOnly )
            {
            TRAPD(err2, iFileInfoUtil->OpenFileL(
                          aFileHandle, 
                          aProp.ValueText(KMPXMediaGeneralMimeType)));
            MPX_DEBUG2("CMPXMetadataExtractor::SetExtMediaPropertiesL, file info util error %i", err2);
            if( KErrNone == err2 )
                {
                aProp.SetTObjectValueL<TUint>( KMPXMediaAudioBitrate,
                                               iFileInfoUtil->BitRate() );
                aProp.SetTObjectValueL<TUint>( KMPXMediaAudioSamplerate,
                                               iFileInfoUtil->SampleRate() );
                TInt64 duration = (TInt64) iFileInfoUtil->Duration().Int64() / 1000; // ms
                aProp.SetTObjectValueL<TInt32>( KMPXMediaGeneralDuration,
                                              duration );
                MPX_DEBUG2("CMPXMetadataExtractor::SetExtMediaPropertiesL -- duration %i", duration);
                }
            }
        }
    else if( aFileErr == KErrNotFound || aFileErr == KErrPathNotFound )
        {
        dbFlags |= KMPXMediaGeneralFlagsIsInvalid;
        }
    // Finally set the db flag
    //
    aProp.SetTObjectValueL( KMPXMediaGeneralFlags,
                            dbFlags );

    iFileInfoUtil->Reset();
    
    MPX_DEBUG1("CMPXMetadataExtractor::SetExtMediaPropertiesL --->");
    }

// ---------------------------------------------------------------------------
// Check to see if this file is a supported container
// ---------------------------------------------------------------------------
//
TInt CMPXMetadataExtractor::SupportedContainerTypeL( const TDesC& aFile,
                                                     TInt aIndex )
    {
    TInt index(KErrNotFound);

    TDataType dataType;
    TUid dummyUid(KNullUid);
    iAppArc.AppForDocument(aFile, dummyUid, dataType);

    TInt index2(KErrNotFound);
    const CDesCArray& mimeTypes = iSupportedTypes[aIndex]->Mimetypes();
    if (!mimeTypes.FindIsq(dataType.Des(), index2))
        { // found
        index = index2;
        }

    return ( index == KErrNotFound ? KErrNotSupported : index );
    }


// ---------------------------------------------------------------------------
// CMPXMetadataExtractor::ThumbnailPreviewReady
// Callback but not used here
// ---------------------------------------------------------------------------
void CMPXMetadataExtractor::ThumbnailPreviewReady( 
        MThumbnailData& /*aThumbnail*/, TThumbnailRequestId /*aId*/ )
    {
    MPX_FUNC("CMPXMetadataExtractor::ThumbnailPreviewReady()");
    }
        

// ---------------------------------------------------------------------------
// CMPXMetadataExtractor::ThumbnailReady
// Callback but not used here
// ---------------------------------------------------------------------------
void CMPXMetadataExtractor::ThumbnailReady( TInt /*aError*/, 
        MThumbnailData& /*aThumbnail*/, TThumbnailRequestId /*aId*/ )
    {
    MPX_FUNC("CMPXMetadataExtractor::ThumbnailReady()");
    iOutstandingThumbnailRequest--;
    if ( iOutstandingThumbnailRequest <= KMPXMaxThumbnailRequest )
        {
        StopWaitLoop();
        }
    }

// ----------------------------------------------------------------------------
// Callback for timer.
// ----------------------------------------------------------------------------
TInt CMPXMetadataExtractor::TimeoutTimerCallback(TAny* aPtr)
    {
    MPX_FUNC("CMPXMetadataExtractor::TimeoutTimerCallback()");

    CMPXMetadataExtractor* ptr =
        static_cast<CMPXMetadataExtractor*>(aPtr);
        
    // Timer expired before thumbnail callback occurred. Stop wait loop to unblock. 
    ptr->StopWaitLoop();
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// Stop the wait loop.
// ----------------------------------------------------------------------------
void CMPXMetadataExtractor::StopWaitLoop()
    {
    MPX_FUNC("CMPXMetadataExtractor::StopWaitLoop()");
    // Cancel timer
    CancelTimeoutTimer();
    
    // Stop wait loop to unblock.
    if ( iTNSyncWait->IsStarted() )
        {
        MPX_DEBUG1("CMPXMetadataExtractor::StopWaitLoop(): Stopping the wait loop.");
        iTNSyncWait->AsyncStop();
        }
    }

// ----------------------------------------------------------------------------
// Cancel timer. 
// ----------------------------------------------------------------------------
void CMPXMetadataExtractor::CancelTimeoutTimer()
    {
    MPX_FUNC("CMPXMetadataExtractor::CancelTimeoutTimer()");
    
    // Cancel timer.
    if ( iTimer && iTimer->IsActive() )
        {
        MPX_DEBUG1("CMPXMetadataExtractor::CancelTimeoutTimer(): Timer active, cancelling");
        iTimer->Cancel();
        }
    }

// ----------------------------------------------------------------------------
// Extract album art from a file and add to thumbnail manager.
// ----------------------------------------------------------------------------
EXPORT_C TInt CMPXMetadataExtractor::ExtractAlbumArtL( CMPXMedia* aMedia )
    {
    MPX_FUNC("CMPXMetadataExtractor::ExtractAlbumArtL()");
    TInt err = KErrNone; 
    
    if ( !aMedia->IsSupported(KMPXMediaGeneralUri) )
        {
        return KErrArgument;
        }
    
    // Get full file name.
    const TDesC& path = aMedia->ValueText(KMPXMediaGeneralUri);
    MPX_DEBUG2("CMPXMetadataExtractor::ExtractAlbumArtL Filename:%S ", &path );
#ifdef ABSTRACTAUDIOALBUM_INCLUDED
    TParsePtrC parse( path );
    TPtrC ext( parse.Ext() );
    if (ext.CompareF(KNonEmbeddedArtExt)== 0)
        {
		#ifdef RD_MPX_TNM_INTEGRATION

        //check if can send TN request, If thumbnail creation is ongoing, wait til it is done
        CheckBeforeSendRequest();

        CThumbnailObjectSource* source = CThumbnailObjectSource::NewLC(
           path, KImageFileType  );
          
       

        iTNManager->CreateThumbnails( *source );
        
        iOutstandingThumbnailRequest++;
        CleanupStack::PopAndDestroy( source );

        #endif
        }
    else
        {
#endif
    // create wanted fields array
    RArray<TMetaDataFieldId> wantedFields;
    CleanupClosePushL( wantedFields );
    wantedFields.Append(EMetaDataJpeg);
    
    // Open file
    if ( aMedia->IsSupported(KMPXMediaGeneralMimeType) )
        {
        const TDesC& mimeType = aMedia->ValueText( KMPXMediaGeneralMimeType );
        MPX_DEBUG2("CMPXMetadataExtractor::ExtractAlbumArtL MimeType:%S ", &mimeType );
        HBufC8* mimeType8 = HBufC8::NewLC( mimeType.Length() );
        mimeType8->Des().Append( mimeType );
        TRAP( err, iMetadataUtility->OpenFileL( path, wantedFields, *mimeType8 ) );
        CleanupStack::PopAndDestroy( mimeType8 );
        }
    else
        {
        TRAP( err, iMetadataUtility->OpenFileL( path, wantedFields ) );
        }
    CleanupStack::PopAndDestroy( &wantedFields );
    
    if ( !err )
        {
        TRAP( err, GetMediaAlbumArtL( *aMedia, path ));
        }

    // Reset the utility
    iMetadataUtility->ResetL();
#ifdef ABSTRACTAUDIOALBUM_INCLUDED
      }
#endif
    return err;
    }

// ----------------------------------------------------------------------------
// Set album art.
// ----------------------------------------------------------------------------
TInt CMPXMetadataExtractor::GetMediaAlbumArtL( CMPXMedia& aMedia, 
                                               const TDesC& aFile )
    {
    MPX_FUNC("CMPXMetadataExtractor::GetMediaAlbumArtL()");
    TInt err = KErrNone;
    // get metadata container.
    const CMetaDataFieldContainer& metaCont = iMetadataUtility->MetaDataFieldsL();

    TPtrC8 data8 = metaCont.Field8( EMetaDataJpeg );
    
    if ( data8.Length() )
        {
        MPX_DEBUG1("CMPXMetadataExtractor::GetMediaAlbumArtL(): Album art exist.");

#ifdef RD_MPX_TNM_INTEGRATION
        HBufC8* value8; 
        TRAPD( err, value8 = data8.AllocL() );
        if ( KErrNone != err )
            {
            MPX_DEBUG2("CMPXMetadataExtractor::GetMediaAlbumArtL - error jpeg = %i", err);           
            User::Leave( err );  
            }              
        CleanupStack::PushL( value8 );
        AddMediaAlbumArtL( aMedia, aFile, *value8 );
        CleanupStack::Pop( value8 );
#else // RD_MPX_TNM_INTEGRATION
        aMedia.SetTextValueL( KMPXMediaMusicAlbumArtFileName, aFile );
#endif // RD_MPX_TNM_INTEGRATION          
        }
    else
        {
        err = KErrNotFound;
        }
    
    return err;
    }

// ----------------------------------------------------------------------------
// Add album art to media object.
// ----------------------------------------------------------------------------
void CMPXMetadataExtractor::AddMediaAlbumArtL( CMPXMedia& aMedia, 
                                               const TDesC& aFile,
                                               TDesC8& aValue )
    {
    MPX_FUNC("CMPXMetadataExtractor::AddMediaAlbumArtL()");
#ifdef RD_MPX_TNM_INTEGRATION
    
    //check if can send TN request, If thumbnail creation is ongoing, wait til it is done
    CheckBeforeSendRequest();

    aMedia.SetTextValueL( KMPXMediaMusicAlbumArtFileName, aFile );
    
    TBuf<256> mimeType;
    mimeType.Copy( KImageFileType );
    CThumbnailObjectSource* source = CThumbnailObjectSource::NewLC(
        &aValue, mimeType, aFile );
    iTNManager->CreateThumbnails( *source );
    CleanupStack::PopAndDestroy( source );
    aMedia.SetTextValueL( KMPXMediaMusicOriginalAlbumArtFileName, aFile );
    iOutstandingThumbnailRequest++;
    
#endif // RD_MPX_TNM_INTEGRATION          
    }

void CMPXMetadataExtractor::CheckBeforeSendRequest()
     {
     MPX_FUNC("CMPXMetadataExtractor::CheckBeforeSendRequest()");
#ifdef RD_MPX_TNM_INTEGRATION
	// If thumbnail creation is ongoing, wait til it is done
    if ( iOutstandingThumbnailRequest > KMPXMaxThumbnailRequest )
        {
        MPX_DEBUG1("CMPXMetadataExtractor::CheckBeforeSendRequest(): Thumbnail creation ongoing!");
        iTNMBlockCount++;
        // Cancel timer.
        CancelTimeoutTimer();
        // Start timer in case there is no callback from ThumbNail Manager.
        iTimer->Start(
            KMPXTimeoutTimer,
            KMPXTimeoutTimer,
            TCallBack(TimeoutTimerCallback, this ));

        // Start wait loop until we get a callback from ThumbNail Manager.
        if ( !iTNSyncWait->IsStarted() )
            {
            iTNSyncWait->Start();
            }
        }
#endif // RD_MPX_TNM_INTEGRATION
     }
