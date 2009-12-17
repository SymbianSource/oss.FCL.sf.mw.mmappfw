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
* Description:  Implement the operation: MoveObject
*
*/


#include <bautils.h>
#include <mtp/mmtpdataproviderframework.h>
#include <mtp/mmtpobjectmgr.h>
#include <mtp/mmtpstoragemgr.h>
#include <mtp/mmtpreferencemgr.h>
#include <mtp/cmtpobjectmetadata.h>
#include <mtp/cmtptypearray.h>
#include <mtp/cmtptypestring.h>
#include <mtp/cmtptypeobjectproplist.h>

#include "cmoveobject.h"
#include "mmmtpdplogger.h"
#include "tmmmtpdppanic.h"
#include "mmmtpdputility.h"
#include "cmmmtpdpmetadataaccesswrapper.h"
#include "mmmtpdpconfig.h"

/**
* Verification data for the MoveObject request
*/
const TMTPRequestElementInfo KMTPMoveObjectPolicy[] =
    {
        {
        TMTPTypeRequest::ERequestParameter1, EMTPElementTypeObjectHandle,
                EMTPElementAttrFileOrDir | EMTPElementAttrWrite, 0, 0, 0
        },
        {
        TMTPTypeRequest::ERequestParameter2, EMTPElementTypeStorageId,
                EMTPElementAttrWrite, 0, 0, 0
        },
        {
        TMTPTypeRequest::ERequestParameter3, EMTPElementTypeObjectHandle,
                EMTPElementAttrDir | EMTPElementAttrWrite, 1, 0, 0
        }
    };

// -----------------------------------------------------------------------------
// CMoveObject::NewL
// Two-phase construction method
// -----------------------------------------------------------------------------
//
//EXPORT_C MMmRequestProcessor* CMoveObject::NewL( MMTPDataProviderFramework& aFramework,
//        MMTPConnection& aConnection,
//        CMmMtpDpMetadataAccessWrapper& aWrapper )
//    {
//    CMoveObject* self = new (ELeave) CMoveObject( aFramework, aConnection, aWrapper );
//    CleanupStack::PushL( self );
//    self->ConstructL();
//    CleanupStack::Pop( self );
//
//    return self;
//    }

// -----------------------------------------------------------------------------
// CMoveObject::~CMoveObject
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CMoveObject::~CMoveObject()
    {
    Cancel();

    delete iDest;
    delete iFileMan;
    delete iPathToMove;
    delete iNewRootFolder;
    iObjectHandles.Close();
    if ( iPropertyElement )
        delete iPropertyElement;
    delete iPropertyList;
    }

// -----------------------------------------------------------------------------
// CMoveObject::CMoveObject
// Standard c++ constructor
// -----------------------------------------------------------------------------
//
EXPORT_C CMoveObject::CMoveObject( MMTPDataProviderFramework& aFramework,
        MMTPConnection& aConnection,
        MMmMtpDpConfig& aDpConfig) :
    CRequestProcessor( aFramework, aConnection, sizeof( KMTPMoveObjectPolicy )
            /sizeof( TMTPRequestElementInfo ), KMTPMoveObjectPolicy),
    iDpConfig( aDpConfig ),
    iObjectHandles( KMmMtpRArrayGranularity ),
    iMoveObjectIndex( 0 )
    {
    PRINT( _L( "Operation: MoveObject(0x1019)" ) );
    }

// -----------------------------------------------------------------------------
// CMoveObject::ServiceL
// MoveObject request handler
// -----------------------------------------------------------------------------
//
EXPORT_C void CMoveObject::ServiceL()
    {
    PRINT( _L( "MM MTP => CMoveObject::ServiceL" ) );
    TMTPResponseCode ret = MoveObjectL();
    PRINT1( _L( "MM MTP <> CMoveObject::ServiceL ret = 0x%x" ), ret );
    if ( EMTPRespCodeOK != ret )
        {
        SendResponseL( ret );
        }
    PRINT( _L( "MM MTP <= CMoveObject::ServiceL" ) );
    }

// -----------------------------------------------------------------------------
// CMoveObject::ConstructL
// Second phase constructor
// -----------------------------------------------------------------------------
//
EXPORT_C void CMoveObject::ConstructL()
    {
    CActiveScheduler::Add( this );

    iPropertyList = CMTPTypeObjectPropList::NewL();
    SetPSStatus();
    }

// -----------------------------------------------------------------------------
// CMoveObject::RunL
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CMoveObject::RunL()
    {
    PRINT( _L( "MM MTP => CMoveObject::RunL" ) );
    if ( MoveOwnedObjectsL() )
        {
        // Reset iMoveObjectIndex count since move completed
        iMoveObjectIndex = 0;

        TMTPResponseCode ret = EMTPRespCodeOK;
        // Delete the original folders in the device dp.
        if ( iObjectInfo->Uint( CMTPObjectMetaData::EDataProviderId )
                == iFramework.DataProviderId() )
            {
            ret = FinalPhaseMove();
            }
        PRINT1( _L("MM MTP <> CMoveObject::RunL ret = 0x%x"), ret );
        SendResponseL( ret );
        }
    PRINT( _L( "MM MTP <= CMoveObject::RunL" ) );
    }

// -----------------------------------------------------------------------------
// CMoveObject::RunError
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMoveObject::RunError( TInt aError )
    {
    if ( aError != KErrNone )
        PRINT1( _L( "MM MTP <> CMoveObject::RunError aError = %d" ), aError );
    TRAP_IGNORE( SendResponseL( EMTPRespCodeGeneralError ) );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMoveObject::MoveFileL
// A helper function of MoveObjectL
// -----------------------------------------------------------------------------
//
void CMoveObject::MoveFileL( const TDesC& aNewFileName )
    {
    PRINT1( _L( "MM MTP => CMoveObject::MoveFileL aNewFileName = %S" ), &aNewFileName );
    const TDesC& suid( iObjectInfo->DesC( CMTPObjectMetaData::ESuid ) );
    TFileName oldFileName( suid ); // save the old file name, the suid will be modified
    PRINT1( _L( "MM MTP <> CMoveObject::MoveFileL oldFileName = %S" ), &suid );

    if ( iStorageId == iObjectInfo->Uint( CMTPObjectMetaData::EStorageId ) )
        iSameStorage = ETrue;
    else
        iSameStorage = EFalse;
    GetPreviousPropertiesL( *iObjectInfo );
    User::LeaveIfError( iFileMan->Move( suid, aNewFileName ) ); // iDest just Folder
    User::LeaveIfError( iFramework.Fs().SetModified( aNewFileName, iPreviousModifiedTime ) );

    iObjectInfo->SetDesCL( CMTPObjectMetaData::ESuid, aNewFileName );
    iObjectInfo->SetUint( CMTPObjectMetaData::EStorageId, iStorageId );
    iObjectInfo->SetUint( CMTPObjectMetaData::EParentHandle, iNewParentHandle );
    iFramework.ObjectMgr().ModifyObjectL( *iObjectInfo );
    SetPropertiesL( oldFileName, aNewFileName, *iObjectInfo );
    PRINT( _L( "MM MTP <= CMoveObject::MoveFileL" ) );
    }

// -----------------------------------------------------------------------------
// CMoveObject::MoveOwnedObjectsL
// Move the objects through iterations of RunL
// -----------------------------------------------------------------------------
//
TBool CMoveObject::MoveOwnedObjectsL()
    {
    PRINT( _L( "MM MTP => CMoveObject::MoveOwnedObjectsL" ) );
    TBool ret = EFalse;

    if ( iMoveObjectIndex < iNumberOfObjects )
        {
        MoveAndUpdateL( iObjectHandles[iMoveObjectIndex++] );

        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, iStatus.Int() );
        SetActive();
        }
    else
        {
        ret = ETrue;
        }

    PRINT1( _L( "MM MTP <= CMoveObject::MoveOwnedObjectsL ret = %d" ), ret );
    return ret;
    }

// -----------------------------------------------------------------------------
// CMoveObject::MoveFolderL
// A helper function of MoveObjectL
// -----------------------------------------------------------------------------
//
void CMoveObject::MoveFolderL()
    {
    PRINT( _L( "MM MTP => CMoveObject::MoveFolderL" ) );
    RBuf oldFolderName;
    oldFolderName.CreateL( KMaxFileName );
    oldFolderName.CleanupClosePushL(); // + oldFileName
    oldFolderName = iObjectInfo->DesC( CMTPObjectMetaData::ESuid );
    PRINT1( _L( "MM MTP <> CMoveObject::MoveFolderL oldFolderName = %S" ), &oldFolderName );
    iPathToMove = oldFolderName.AllocL();
    CleanupStack::PopAndDestroy( &oldFolderName ); // - oldFolderName

    GenerateObjectHandleListL( iObjectInfo->Uint( CMTPObjectMetaData::EHandle ) );

    iNumberOfObjects = iObjectHandles.Count();
    PRINT1( _L( "MM MTP <> CMoveObject::MoveFolderL iNumberOfObjects = %d" ), iNumberOfObjects );

    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, iStatus.Int() );
    SetActive();

    PRINT( _L( "MM MTP <= CMoveObject::MoveFolderL" ) );
    }

// -----------------------------------------------------------------------------
// CMoveObject::MoveObjectL
// move object operations
// -----------------------------------------------------------------------------
//
TMTPResponseCode CMoveObject::MoveObjectL()
    {
    PRINT( _L( "MM MTP => CMoveObject::MoveObjectL" ) );
    TMTPResponseCode responseCode = EMTPRespCodeOK;

    GetParametersL();

    RBuf newObjectName;
    newObjectName.CreateL( KMaxFileName );
    newObjectName.CleanupClosePushL(); // + newObjectName
    newObjectName = *iDest;

    const TDesC& suid( iObjectInfo->DesC( CMTPObjectMetaData::ESuid ) );
    TParsePtrC fileNameParser( suid );

    // Check if the object is a folder or a file.
    TBool isFolder = EFalse;
    User::LeaveIfError( BaflUtils::IsFolder( iFramework.Fs(), suid, isFolder ) );

    if ( !isFolder )
        {
        if ( ( newObjectName.Length() + fileNameParser.NameAndExt().Length() ) <= newObjectName.MaxLength() )
            {
            newObjectName.Append( fileNameParser.NameAndExt() );
            }
        responseCode = CanMoveObjectL( suid, newObjectName );
        }
    else // It is a folder.
        {
        TFileName rightMostFolderName;
        User::LeaveIfError( BaflUtils::MostSignificantPartOfFullName( suid,
                rightMostFolderName ) );
        if ( ( newObjectName.Length() + rightMostFolderName.Length() + 1 ) <= newObjectName.MaxLength() )
            {
            newObjectName.Append( rightMostFolderName );
            // Add backslash.
            _LIT( KBackSlash, "\\" );
            newObjectName.Append( KBackSlash );
            }
        }

    iNewRootFolder = newObjectName.AllocL();

    if ( responseCode == EMTPRespCodeOK )
        {
        delete iFileMan;
        iFileMan = NULL;
        iFileMan = CFileMan::NewL( iFramework.Fs() );

        if ( !isFolder )
            {
            MoveFileL( newObjectName );
            SendResponseL( responseCode );
            }
        else
            {
            MoveFolderL();
            }
        }
    CleanupStack::PopAndDestroy( &newObjectName ); // - newObjectName.

    PRINT1( _L( "MM MTP <= CMoveObject::MoveObjectL responseCode = 0x%x" ), responseCode );
    return responseCode;
    }

// -----------------------------------------------------------------------------
// CMoveObject::GetParametersL
// Retrieve the parameters of the request
// -----------------------------------------------------------------------------
//
void CMoveObject::GetParametersL()
    {
    PRINT( _L( "MM MTP => CMoveObject::GetParametersL" ) );
    __ASSERT_DEBUG( iRequestChecker, Panic( EMmMTPDpRequestCheckNull ) );

    TUint32 objectHandle = Request().Uint32( TMTPTypeRequest::ERequestParameter1 );
    iStorageId = Request().Uint32( TMTPTypeRequest::ERequestParameter2 );
    iNewParentHandle = Request().Uint32( TMTPTypeRequest::ERequestParameter3 );
    PRINT3( _L( "MM MTP <> objectHandle = 0x%x, iStoargeId = 0x%x, iNewParentHandle = 0x%x" ),
            objectHandle, iStorageId, iNewParentHandle );

    // not taking owernship
    iObjectInfo = iRequestChecker->GetObjectInfo( objectHandle );
    __ASSERT_DEBUG( iObjectInfo, Panic( EMmMTPDpObjectNull ) );

    if ( iNewParentHandle == 0 )
        {
        SetDefaultParentObjectL();
        }
    else
        {
        CMTPObjectMetaData* parentObjectInfo =
                iRequestChecker->GetObjectInfo( iNewParentHandle );
        __ASSERT_DEBUG( parentObjectInfo, Panic( EMmMTPDpObjectNull ) );
        delete iDest;
        iDest = NULL;
        iDest = parentObjectInfo->DesC( CMTPObjectMetaData::ESuid ).AllocL();
        PRINT1( _L( "MM MTP <> CMoveObject::GetParametersL iDest = %S" ), iDest );
        }
    PRINT( _L( "MM MTP <= CMoveObject::GetParametersL" ) );
    }

// -----------------------------------------------------------------------------
// CMoveObject::SetDefaultParentObjectL
// Get a default parent object, ff the request does not specify a parent object,
// -----------------------------------------------------------------------------
//
void CMoveObject::SetDefaultParentObjectL()
    {
    PRINT( _L( "MM MTP => CMoveObject::SetDefaultParentObjectL" ) );

    delete iDest;
    iDest = NULL;
    iDest = ( iFramework.StorageMgr().StorageL( iStorageId ).DesC(
                CMTPStorageMetaData::EStorageSuid ) ).AllocL();
    PRINT1( _L( "MM MTP <> CMoveObject::SetDefaultParentObjectL iDest = %S" ), iDest );
    iNewParentHandle = KMTPHandleNoParent;
    PRINT( _L( "MM MTP <= CMoveObject::SetDefaultParentObjectL" ) );
    }

// -----------------------------------------------------------------------------
// CMoveObject::CanMoveObjectL
// Check if we can move the file to the new location
// -----------------------------------------------------------------------------
//
TMTPResponseCode CMoveObject::CanMoveObjectL( const TDesC& aOldName,
        const TDesC& aNewName ) const
    {
    PRINT( _L( "MM MTP => CMoveObject::CanMoveObjectL" ) );
    TMTPResponseCode result = EMTPRespCodeOK;

    TEntry fileEntry;
    User::LeaveIfError( iFramework.Fs().Entry( aOldName, fileEntry ) );
    TDriveNumber drive( static_cast<TDriveNumber>( iFramework.StorageMgr().DriveNumber( iStorageId ) ) );
    User::LeaveIfError( drive );
    TVolumeInfo volumeInfo;
    User::LeaveIfError( iFramework.Fs().Volume( volumeInfo, drive ) );

    if ( volumeInfo.iFree < fileEntry.iSize )
        {
        result = EMTPRespCodeStoreFull;
        }
    else if ( BaflUtils::FileExists( iFramework.Fs(), aNewName ) )
        {
#ifdef MMMTPDP_REPLACE_EXIST_FILE
        // delete the old one and replace
        TInt delErr = iFramework.Fs().Delete( aNewName );
        PRINT1( _L( "MM MTP <> CMoveObject::CanMoveObjectL delErr = %d" ), delErr );
        // delete from the metadata DB
        TRAPD( err, iFramework.ObjectMgr().RemoveObjectL( aNewName ) );
        PRINT1( _L( "MM MTP <> CMoveObject::CanMoveObjectL err = %d" ), err );
        // delete from video/mpx DB
        CMTPObjectMetaData* objectInfo = CMTPObjectMetaData::NewLC(); // + objectInfo
        if ( iFramework.ObjectMgr().ObjectL( aNewName, *objectInfo ) )
            {
            TRAP( err, iDpConfig.GetWrapperL().DeleteObjectL( aNewName,
                objectInfo->Uint( CMTPObjectMetaData::EFormatCode ) ) );
            }
        CleanupStack::PopAndDestroy( objectInfo ); // - objectInfo
        if ( err )
            {
            // do nothing
            }
#else
        result = EMTPRespCodeInvalidParentObject;
#endif
        }

    PRINT1( _L( "MM MTP <= CMoveObject::CanMoveObjectL result = 0x%x" ), result );
    return result;
    }

// -----------------------------------------------------------------------------
// CMoveObject::GetPreviousPropertiesL
// Save the object properties before doing the move
// -----------------------------------------------------------------------------
//
void CMoveObject::GetPreviousPropertiesL( const CMTPObjectMetaData& aObject )
    {
    PRINT( _L( "MM MTP => CMoveObject::GetPreviousPropertiesL" ) );

    const TDesC& suid( aObject.DesC( CMTPObjectMetaData::ESuid ) );

    User::LeaveIfError( iFramework.Fs().Modified( suid, iPreviousModifiedTime ) );

    // same storage, not necessary to get the properties
    if ( iSameStorage )
        return;

    TUint formatCode = aObject.Uint( CMTPObjectMetaData::EFormatCode );
    const RArray<TUint>* properties = iDpConfig.GetSupportedPropertiesL( formatCode );
    TInt count = properties->Count();
    CMTPTypeString* textData = NULL;
    TInt err = KErrNone;
    TUint16 propCode;
    TUint32 handle = aObject.Uint( CMTPObjectMetaData::EHandle ) ;

    if ( iPropertyElement )
        {
        delete iPropertyElement;
        iPropertyElement = NULL;
        }

    for ( TInt i = 0; i < count; i++ )
        {
        propCode = (*properties)[i];
        switch( propCode )
            {
            case EMTPObjectPropCodeStorageID:
            case EMTPObjectPropCodeObjectFormat:
            case EMTPObjectPropCodeProtectionStatus:
            case EMTPObjectPropCodeObjectSize:
            case EMTPObjectPropCodeObjectFileName:
            case EMTPObjectPropCodeParentObject:
            case EMTPObjectPropCodePersistentUniqueObjectIdentifier:
            case EMTPObjectPropCodeNonConsumable:
            case EMTPObjectPropCodeDateCreated:
            case EMTPObjectPropCodeDateModified:
                break;

            case EMTPObjectPropCodeName:
            case EMTPObjectPropCodeDateAdded:
                if ( ( propCode == EMTPObjectPropCodeName )
                   || ( ( !MmMtpDpUtility::IsVideoL( aObject.DesC( CMTPObjectMetaData::ESuid ), iFramework ) )
                        && ( propCode == EMTPObjectPropCodeDateAdded ) ) )
                    {
                    textData = CMTPTypeString::NewLC(); // + textData

                    TRAP( err, iDpConfig.GetWrapperL().GetObjectMetadataValueL( propCode,
                        *textData,
                        aObject ) );

                    PRINT1( _L( "MM MTP <> CMoveObject::GetPreviousPropertiesL::ServiceSpecificObjectPropertyL err = %d" ), err );

                    if ( err == KErrNone )
                        {
                        iPropertyElement = &(iPropertyList->ReservePropElemL(handle, propCode));
                        iPropertyElement->SetStringL(CMTPTypeObjectPropListElement::EValue, textData->StringChars());
//                        iPropertyElement = CMTPTypeObjectPropListElement::NewL(
//                            handle, propCode, *textData );
                        }
                    else if ( err == KErrNotFound )
                        {
                        iPropertyElement = NULL;
                        }
                    else
                       {
                       User::Leave( err );
                       }

                    CleanupStack::PopAndDestroy( textData ); // - textData
                    }
                break;

            default:
                {
                ServiceGetSpecificObjectPropertyL( propCode, handle, aObject );
                }
                break;
            }

        if ( iPropertyElement )
            {
            iPropertyList->CommitPropElemL( *iPropertyElement );

            iPropertyElement = NULL;
            }

        } // end of for loop

    PRINT1( _L( "MM MTP <= CMoveObject::GetPreviousPropertiesL err = %d" ), err );
    }

// -----------------------------------------------------------------------------
// CMoveObject::ServiceMetaDataToWrapper
//
// -----------------------------------------------------------------------------
//
EXPORT_C TMTPResponseCode CMoveObject::ServiceMetaDataToWrapper(
    const TUint16 aPropCode,
    MMTPType& aNewData,
    const CMTPObjectMetaData& aObject )
    {
    TMTPResponseCode resCode = EMTPRespCodeOK;

    TRAPD( err, iDpConfig.GetWrapperL().SetObjectMetadataValueL( aPropCode,
        aNewData,
        aObject ) );

    PRINT1( _L("MM MTP <> CMoveObject::ServiceMetaDataToWrapper err = %d"), err);

    if ( err == KErrNone )
        {
        resCode = EMTPRespCodeOK;
        }
    else if ( err == KErrTooBig )
        // according to the codes of S60
        {
        resCode = EMTPRespCodeInvalidDataset;
        }
    else if ( err == KErrPermissionDenied )
        {
        resCode = EMTPRespCodeAccessDenied;
        }
    else if ( err == KErrNotFound )
        {
        if ( MmMtpDpUtility::HasMetadata( aObject.Uint( CMTPObjectMetaData::EFormatCode ) ) )
            resCode = EMTPRespCodeAccessDenied;
        else
            resCode = EMTPRespCodeOK;
        }
    else
        {
        resCode = EMTPRespCodeGeneralError;
        }

    PRINT1( _L("MM MTP <= CMoveObject::ServiceMetaDataToWrapper resCode = 0x%x"), resCode);

    return resCode;
    }

// -----------------------------------------------------------------------------
// CMoveObject::SetPreviousPropertiesL
// Set the object properties after doing the move
// -----------------------------------------------------------------------------
//
void CMoveObject::SetPreviousPropertiesL( const CMTPObjectMetaData& aObject )
    {
    PRINT( _L( "MM MTP => CMoveObject::SetPreviousPropertiesL" ) );
    const TInt count( iPropertyList->NumberOfElements() );
    PRINT1( _L( "MM MTP <> CMoveObject::SetPreviousPropertiesL count = %d" ), count );
    TMTPResponseCode respcode = EMTPRespCodeOK;
    CMTPTypeString* stringData = NULL;
    iPropertyList->ResetCursor();
    
    for ( TInt i = 0; i < count; i++ )
        {
        CMTPTypeObjectPropListElement& element = iPropertyList->GetNextElementL( );

        TUint32 handle = element.Uint32L(
                CMTPTypeObjectPropListElement::EObjectHandle );
        TUint16 propertyCode = element.Uint16L(
                CMTPTypeObjectPropListElement::EPropertyCode );
        TUint16 dataType = element.Uint16L(
                CMTPTypeObjectPropListElement::EDatatype );
        PRINT3( _L( "MM MTP <> CMoveObject::SetPreviousPropertiesL = 0x%x, propertyCode = 0x%x, dataType = 0x%x" ),
          handle, propertyCode, dataType );

        switch ( propertyCode )
            {
            case EMTPObjectPropCodeStorageID:
            case EMTPObjectPropCodeObjectFormat:
            case EMTPObjectPropCodeProtectionStatus:
            case EMTPObjectPropCodeObjectSize:
            case EMTPObjectPropCodeObjectFileName:
            case EMTPObjectPropCodeParentObject:
            case EMTPObjectPropCodePersistentUniqueObjectIdentifier:
            case EMTPObjectPropCodeNonConsumable:
            case EMTPObjectPropCodeDateCreated:
            case EMTPObjectPropCodeDateModified:
            case EMTPObjectPropCodeDateAdded:
                break;

            case EMTPObjectPropCodeName:
                {
                stringData = CMTPTypeString::NewLC(
                    element.StringL(CMTPTypeObjectPropListElement::EValue)); // + stringData

                respcode = ServiceMetaDataToWrapper( propertyCode,
                    *stringData,
                    aObject );

                CleanupStack::PopAndDestroy( stringData ); // - stringData
                }
                break;

            default:
                {
                respcode = ServiceSetSpecificObjectPropertyL( propertyCode,
                        aObject,
                        element );
                }
                break;
            }
        } // end of for loop

    if( respcode == EMTPRespCodeOK )
        {
        // do nothing, ignore warning
        }

    PRINT1( _L( "MM MTP <= CMoveObject::SetPreviousPropertiesL respcode = 0x%x" ), respcode );
    }

// -----------------------------------------------------------------------------
// CMoveObject::SetPropertiesL
// Set the object properties in the object property store.
// -----------------------------------------------------------------------------
//
void CMoveObject::SetPropertiesL( const TDesC& aOldFileName, const TDesC& aNewFileName,
        const CMTPObjectMetaData& aNewObject )
    {
    PRINT2( _L( "MM MTP => CMoveObject::SetPropertiesL aOldFileName = %S, aNewFileName = %S" ),
            &aOldFileName, &aNewFileName );

    TUint32 formatCode = aNewObject.Uint( CMTPObjectMetaData::EFormatCode );
    // This is used to keep the same behavior in mass storage and device file manager.
    if ( formatCode == EMTPFormatCodeAbstractAudioVideoPlaylist )
        {
        PRINT( _L( "MM MTP <> CMoveObject::SetPropertiesL Playlist file do not update the MPX DB" ) );
        }
    else
        {
        // if the two object in different stoarge, we should delete the old one and insert new one
        if ( iSameStorage )
            iDpConfig.GetWrapperL().RenameObjectL( aOldFileName, aNewFileName );
        else
            {
            iDpConfig.GetWrapperL().DeleteObjectL( aOldFileName, formatCode );
            iDpConfig.GetWrapperL().AddObjectL( aNewFileName );
            SetPreviousPropertiesL( aNewObject );
            }
        }

      // Reference DB is used PUID
//    if ( formatCode == EMTPFormatCodeAbstractAudioVideoPlaylist
//            || formatCode == EMTPFormatCodeM3UPlaylist )
//        {
//        MMTPReferenceMgr& referenceMgr = iFramework.ReferenceMgr();
//        CDesCArray* references = referenceMgr.ReferencesLC( aOldFileName ); // + references
//        referenceMgr.SetReferencesL( aNewFileName, *references );
//        CleanupStack::PopAndDestroy( references ); // - references
//        // delete the old references
//        referenceMgr.RemoveReferencesL( aOldFileName );
//        }


    PRINT( _L( "MM MTP <= CMoveObject::SetPropertiesL" ) );
    }

// -----------------------------------------------------------------------------
// CMoveObject::FinalPhaseMove
// This function will actually delete the orginal folders from the file system
// -----------------------------------------------------------------------------
//
TMTPResponseCode CMoveObject::FinalPhaseMove()
    {
    PRINT( _L( "MM MTP => CMoveObject::FinalPhaseMove" ) );
    TMTPResponseCode ret = EMTPRespCodeOK;

    TInt rel = iFileMan->RmDir( *iPathToMove );
    PRINT1( _L( "MM MTP <> CMoveObject::FinalPhaseMove rel = %d" ), rel );

    if ( rel != KErrNone )
        {
        ret = EMTPRespCodeGeneralError;
        }

    PRINT1( _L( "MM MTP <= CMoveObject::FinalPhaseMove ret = 0x%x" ), ret );
    return ret;
    }

// -----------------------------------------------------------------------------
// CMoveObject::GenerateObjectHandleListL
// Generate the list of handles that need to be moved to the new location
// -----------------------------------------------------------------------------
//
void CMoveObject::GenerateObjectHandleListL( TUint32 aParentHandle )
    {
    PRINT1( _L( "MM MTP => CMoveObject::GenerateObjectHandleListL aParentHandle = 0x%x" ), aParentHandle );
    RMTPObjectMgrQueryContext context;
    RArray<TUint> handles;
    TMTPObjectMgrQueryParams params( KMTPStorageAll, KMTPFormatsAll,
            aParentHandle );
    CleanupClosePushL( context ); // + context
    CleanupClosePushL( handles ); // - handles

    do
        {
        iFramework.ObjectMgr().GetObjectHandlesL( params, context, handles );

        TInt numberOfObjects = handles.Count();
        for ( TInt i = 0; i < numberOfObjects; i++ )
            {
            if ( iFramework.ObjectMgr().ObjectOwnerId( handles[i] ) == iFramework.DataProviderId() )
                {
                iObjectHandles.AppendL( handles[i] );
                continue;
                }

            // Folder
            if ( iFramework.ObjectMgr().ObjectOwnerId( handles[i] ) == 0 ) // We know that the device dp id is always 0, otherwise the whole MTP won't work.
                {
                GenerateObjectHandleListL( handles[i] );
                }
            }
        }
    while ( !context.QueryComplete() );

    CleanupStack::PopAndDestroy( &handles ); // - handles
    CleanupStack::PopAndDestroy( &context ); // - contect

    PRINT( _L( "MM MTP <= CMoveObject::GenerateObjectHandleListL" ) );
    }

// -----------------------------------------------------------------------------
// CMoveObject::MoveAndUpdateL
// Move a single object and update the database
// -----------------------------------------------------------------------------
//
void CMoveObject::MoveAndUpdateL( TUint32 aObjectHandle )
    {
    PRINT1( _L( "MM MTP => CMoveObject::MoveAndUpdateL aObjectHanlde = 0x%x" ), aObjectHandle );

    CMTPObjectMetaData* objectInfo( CMTPObjectMetaData::NewLC() ); // + objectInfo

    RBuf fileName;
    fileName.CreateL( KMaxFileName );
    fileName.CleanupClosePushL(); // + fileName

    RBuf rightPartName;
    rightPartName.CreateL( KMaxFileName );
    rightPartName.CleanupClosePushL(); // + rightPartName

    RBuf oldName;
    oldName.CreateL( KMaxFileName );
    oldName.CleanupClosePushL(); // + oldName

    if ( iFramework.ObjectMgr().ObjectL( TMTPTypeUint32( aObjectHandle ), *objectInfo ) )
        {
        fileName = objectInfo->DesC( CMTPObjectMetaData::ESuid );
        oldName = fileName;

        if ( objectInfo->Uint( CMTPObjectMetaData::EDataProviderId )
                == iFramework.DataProviderId() )
            {
            rightPartName = fileName.Right( fileName.Length() - iPathToMove->Length() );

            if ( ( iNewRootFolder->Length() + rightPartName.Length() ) > fileName.MaxLength() )
                {
                User::Leave( KErrCorrupt );
                }

            fileName.Zero();
            fileName.Append( *iNewRootFolder );
            fileName.Append( rightPartName );
            PRINT1( _L( "MM MTP <> MoveAndUpdateL fileName = %S" ), &fileName );

            if ( iStorageId == objectInfo->Uint( CMTPObjectMetaData::EStorageId ) )
                iSameStorage = ETrue;
            else
                iSameStorage = EFalse;
            GetPreviousPropertiesL( *objectInfo );
            TInt err = iFileMan->Move( oldName, fileName );
            PRINT1( _L( "MM MTP <> CMoveObject::MoveAndUpdateL Move error code = %d" ), err );
            User::LeaveIfError( err );
            User::LeaveIfError( iFramework.Fs().SetModified( fileName, iPreviousModifiedTime ) );

            objectInfo->SetDesCL( CMTPObjectMetaData::ESuid, fileName );
            objectInfo->SetUint( CMTPObjectMetaData::EStorageId, iStorageId );
            TParsePtrC parentSuid( fileName );
            TUint32 parentHandle = iFramework.ObjectMgr().HandleL( parentSuid.DriveAndPath() );
            objectInfo->SetUint( CMTPObjectMetaData::EParentHandle, parentHandle );

            //TUint32 parentHandle = iFramework.ObjectMgr().HandleL( parentSuid.DriveAndPath() );
            PRINT1( _L( "MM MTP <> CMoveObject::MoveAndUpdateL parentHandle = 0x%x" ), parentHandle );

            iFramework.ObjectMgr().ModifyObjectL( *objectInfo );

            SetPropertiesL( oldName, fileName, *objectInfo );
            }
        }
    else
        {
        User::Leave( KErrCorrupt );
        }

    CleanupStack::PopAndDestroy( &oldName ); // - oldName
    CleanupStack::PopAndDestroy( &rightPartName ); // - rightPartName
    CleanupStack::PopAndDestroy( &fileName ); // - fileName
    CleanupStack::PopAndDestroy( objectInfo ); // - objectInfo
    PRINT( _L( "MM MTP <= CMoveObject::MoveAndUpdateL" ) );
    }

// end of file
