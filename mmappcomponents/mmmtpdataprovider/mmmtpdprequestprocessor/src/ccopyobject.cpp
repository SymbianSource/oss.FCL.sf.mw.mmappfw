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
* Description:  Implement the operation: CopyObject
*
*/


#include <bautils.h>

#include <mtp/mmtpdataproviderframework.h>
#include <mtp/mmtpobjectmgr.h>
#include <mtp/mmtpreferencemgr.h>
#include <mtp/mmtpstoragemgr.h>
#include <mtp/cmtpobjectmetadata.h>
#include <mtp/cmtptypestring.h>
#include <mtp/cmtptypearray.h>
#include <mtp/cmtptypeobjectproplist.h>

#include "ccopyobject.h"
#include "mmmtpdplogger.h"
#include "tmmmtpdppanic.h"
#include "mmmtpdputility.h"
#include "mmmtpdpconfig.h"
#include "cmmmtpdpmetadataaccesswrapper.h"

/**
* Verification data for the CopyObject request
*/
const TMTPRequestElementInfo KMTPCopyObjectPolicy[] =
    {
        {
        TMTPTypeRequest::ERequestParameter1,
        EMTPElementTypeObjectHandle,
        EMTPElementAttrFileOrDir,
        0,
        0,
        0
        },
        {
        TMTPTypeRequest::ERequestParameter2,
        EMTPElementTypeStorageId,
        EMTPElementAttrWrite,
        0,
        0,
        0
        },
        {
        TMTPTypeRequest::ERequestParameter3,
        EMTPElementTypeObjectHandle,
        EMTPElementAttrDir | EMTPElementAttrWrite,
        1,
        0,
        0
        }
    };

// -----------------------------------------------------------------------------
// CCopyObject::~CCopyObject
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CCopyObject::~CCopyObject()
    {
    Cancel();

    delete iDest;
    delete iFileMan;
    iObjectHandles.Close();
    if ( iPropertyElement )
        delete iPropertyElement;
    delete iPropertyList;
    delete iPathToCopy;
    delete iNewRootFolder;
    }

// -----------------------------------------------------------------------------
// CCopyObject::CCopyObject
// Standard c++ constructor
// -----------------------------------------------------------------------------
//
EXPORT_C CCopyObject::CCopyObject( MMTPDataProviderFramework& aFramework,
    MMTPConnection& aConnection,
    MMmMtpDpConfig& aDpConfig ) :
    CRequestProcessor( aFramework,
        aConnection,
        sizeof ( KMTPCopyObjectPolicy ) / sizeof(TMTPRequestElementInfo),
        KMTPCopyObjectPolicy ),
    iDpConfig( aDpConfig ),
    iObjectHandles( KMmMtpRArrayGranularity )
    {
    PRINT( _L( "Operation: CopyObject(0x101A)" ) );
    }

// -----------------------------------------------------------------------------
// CCopyObject::ServiceL
// CopyObject request handler
// -----------------------------------------------------------------------------
//
EXPORT_C void CCopyObject::ServiceL()
    {
    PRINT( _L( "MM MTP => CCopyObject::ServiceL" ) );
    iHandle = KMTPHandleNone;

    CopyObjectL( iHandle );

    PRINT( _L( "MM MTP <= CCopyObject::ServiceL" ) );
    }

// -----------------------------------------------------------------------------
// CCopyObject::ConstructL
// Second phase constructor
// -----------------------------------------------------------------------------
//
EXPORT_C void CCopyObject::ConstructL()
    {
    CActiveScheduler::Add( this );

    iPropertyList = CMTPTypeObjectPropList::NewL();

    // Set the CenRep value of MTP status,
    // also need to do in other processors which related to MPX
    SetPSStatus();
    }

// -----------------------------------------------------------------------------
// CCopyObject::RunL
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CCopyObject::RunL()
    {
    PRINT( _L( "MM MTP => CCopyObject::RunL" ) );

    if ( iCopyObjectIndex < iNumberOfObjects )
        {
        CopyAndUpdateL( iObjectHandles[iCopyObjectIndex++] );

        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, iStatus.Int() );
        SetActive();
        }
    else
        {
        PRINT1( _L( "MM MTP <> CCopyObject::RunL iHandle = 0x%x" ), iHandle );
        SendResponseL( EMTPRespCodeOK, 1, &iHandle );
        }

    PRINT( _L( "MM MTP <= CCopyObject::RunL" ) );
    }

// -----------------------------------------------------------------------------
// CCopyObject::CopyFileL
// A helper function of CopyObjectL
// -----------------------------------------------------------------------------
//
TUint32 CCopyObject::CopyFileL( const TDesC& aNewFileName )
    {
    const TDesC& suid( iObjectInfo->DesC( CMTPObjectMetaData::ESuid ) );
    PRINT2( _L( "MM MTP => CCopyObject::CopyFileL old name = %S, aNewFileName = %S" ),
        &suid,
        &aNewFileName );

    GetPreviousPropertiesL( *iObjectInfo );
    User::LeaveIfError( iFileMan->Copy( suid, aNewFileName ) ); // iDest just folder
    User::LeaveIfError( iFramework.Fs().SetModified( aNewFileName, iPreviousModifiedTime ) );
    TUint32 handle = UpdateObjectInfoL( suid, aNewFileName );

    PRINT1( _L( "MM MTP <= CCopyObject::CopyFileL handle = 0x%x" ), handle );

    return handle;
    }

// -----------------------------------------------------------------------------
// CCopyObject::GenerateObjectHandleListL
// Generate the list of handles that need to be copied to the new location
// -----------------------------------------------------------------------------
//
void CCopyObject::GenerateObjectHandleListL( TUint32 aParentHandle )
    {
    PRINT1( _L( "MM MTP => CCopyObject::GenerateObjectHandleListL aParentHandle = 0x%x" ),
        aParentHandle );
    RMTPObjectMgrQueryContext context;
    RArray<TUint> handles;
    CleanupClosePushL( context ); // + context
    CleanupClosePushL( handles ); // + handles

    TMTPObjectMgrQueryParams params( KMTPStorageAll, KMTPFormatsAll,
            aParentHandle );
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
    CleanupStack::PopAndDestroy( &context ); // - context
    PRINT( _L( "MM MTP <= CCopyObject::GenerateObjectHandleListL" ) );
    }

// -----------------------------------------------------------------------------
// CCopyObject::CopyFolderL
// A helper function of CopyObjectL
// -----------------------------------------------------------------------------
//
TUint32 CCopyObject::CopyFolderL( const TDesC& aNewFolderName )
    {
    PRINT1( _L( "MM MTP => CCopyObject::CopyFolderL aNewFolderName = %S" ), &aNewFolderName );
    TUint32 handle = iFramework.ObjectMgr().HandleL( aNewFolderName ); // just get it

    GenerateObjectHandleListL( iObjectInfo->Uint( CMTPObjectMetaData::EHandle ) );
    iCopyObjectIndex = 0;
    iNumberOfObjects = iObjectHandles.Count();
    PRINT1( _L( "MM MTP <> CCopyObject::CopyFolderL iNumberOfObjects = %d" ), iNumberOfObjects );

    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, iStatus.Int() );
    SetActive();

    PRINT1( _L( "MM MTP <= CCopyObject::CopyFolderL handle = 0x%x" ), handle );
    return handle;
    }

// -----------------------------------------------------------------------------
// CCopyObject::CopyObjectL
// Copy object operation
// -----------------------------------------------------------------------------
//
void CCopyObject::CopyObjectL( TUint32& aNewHandle )
    {
    PRINT( _L( "MM MTP => CCopyObject::CopyObjectL" ) );
    TMTPResponseCode responseCode = EMTPRespCodeOK;
    aNewHandle = KMTPHandleNone;

    GetParametersL();

    RBuf newObjectName;
    newObjectName.CleanupClosePushL(); // + newObjectName
    newObjectName.CreateL( KMaxFileName );
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
        responseCode = CanCopyObjectL( suid, newObjectName );
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

    delete iNewRootFolder;
    iNewRootFolder = NULL;
    iNewRootFolder = newObjectName.AllocL();

    if ( responseCode == EMTPRespCodeOK )
        {
        delete iFileMan;
        iFileMan = NULL;
        iFileMan = CFileMan::NewL( iFramework.Fs() );

        if ( !isFolder ) // It is a file.
            {
            aNewHandle = CopyFileL( newObjectName );
//            if ( responseCode == EMTPRespCodeOK  )
            SendResponseL( EMTPRespCodeOK, 1, &aNewHandle );
//            else
//                SendResponseL( responseCode );
            }
        else // It is a folder.
            {
            delete iPathToCopy;
            iPathToCopy = NULL;
            iPathToCopy = suid.AllocL();
            PRINT1( _L( "MM MTP <> CCopyObject::CopyObjectL iPathToCopy = %S" ), iPathToCopy );
            aNewHandle = CopyFolderL( newObjectName );
            }
        }
    else
        SendResponseL( responseCode );

    CleanupStack::PopAndDestroy( &newObjectName ); // - newObjectName
    PRINT2( _L( "MM MTP <= CCopyObject::CopyObjectL responseCode = 0x%x, aNewHandle = 0x%x" ),
            responseCode, aNewHandle );
    }

// -----------------------------------------------------------------------------
// CCopyObject::GetParametersL
// Retrieve the parameters of the request
// -----------------------------------------------------------------------------
//
void CCopyObject::GetParametersL()
    {
    PRINT( _L( "MM MTP => CCopyObject::GetParametersL" ) );
    __ASSERT_DEBUG( iRequestChecker, Panic( EMmMTPDpRequestCheckNull ) );

    TUint32 objectHandle = Request().Uint32( TMTPTypeRequest::ERequestParameter1 );
    iStorageId = Request().Uint32( TMTPTypeRequest::ERequestParameter2 );
    TUint32 parentObjectHandle = Request().Uint32( TMTPTypeRequest::ERequestParameter3 );
    PRINT3( _L( "MM MTP <> CCopyObject::GetParametersL Object Hanlde = 0x%x, StorageId = 0x%x, Parent Handle = 0x%x" ),
            objectHandle, iStorageId, parentObjectHandle );

    // not taking owernship
    iObjectInfo = iRequestChecker->GetObjectInfo( objectHandle );
    __ASSERT_DEBUG( iObjectInfo, Panic( EMmMTPDpObjectNull ) );

    if ( parentObjectHandle == 0 )
        {
        SetDefaultParentObjectL();
        }
    else
        {
        CMTPObjectMetaData* parentObjectInfo = iRequestChecker->GetObjectInfo( parentObjectHandle );
        __ASSERT_DEBUG( parentObjectInfo, Panic( EMmMTPDpObjectNull ) );
        delete iDest;
        iDest = NULL;
        iDest = parentObjectInfo->DesC( CMTPObjectMetaData::ESuid ).AllocL();
        iNewParentHandle = parentObjectHandle;
        }
    PRINT( _L( "MM MTP <= CCopyObject::GetParametersL" ) );
    }

// -----------------------------------------------------------------------------
// CCopyObject::SetDefaultParentObjectL
// Get a default parent object, ff the request does not specify a parent object,
// -----------------------------------------------------------------------------
//
void CCopyObject::SetDefaultParentObjectL()
    {
    PRINT( _L( "MM MTP => CCopyObject::SetDefaultParentObjectL" ) );

    delete iDest;
    iDest = NULL;
    iDest = ( iFramework.StorageMgr().StorageL( iStorageId ).DesC(
                CMTPStorageMetaData::EStorageSuid ) ).AllocL();
    PRINT1( _L( "MM MTP <> CCopyObject::SetDefaultParentObjectL Destination location is %S" ), iDest );
    iNewParentHandle = KMTPHandleNoParent;
    PRINT( _L( "MM MTP <= CCopyObject::SetDefaultParentObjectL" ) );
    }

// -----------------------------------------------------------------------------
// CCopyObject::CanCopyObjectL
// Check if we can copy the file to the new location
// -----------------------------------------------------------------------------
//
TMTPResponseCode CCopyObject::CanCopyObjectL( const TDesC& aOldName,
    const TDesC& aNewName ) const
    {
    PRINT2( _L( "MM MTP => CCopyObject::CanCopyObjectL aOldName = %S, aNewName = %S" ),
            &aOldName, &aNewName );
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
        PRINT1( _L( "MM MTP <> CCopyObject::CanCopyObjectL delErr = %d" ), delErr );
        // delete from the metadata DB
        TRAPD( err, iFramework.ObjectMgr().RemoveObjectL( aNewName ) );
        PRINT1( _L( "MM MTP <> CCopyObject::CanCopyObjectL err = %d" ), err );
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
    // This is used to keep the same behavior in mass storage and device file manager.
    else if ( iObjectInfo->Uint( CMTPObjectMetaData::EFormatCode )
        == EMTPFormatCodeAbstractAudioVideoPlaylist )
        {
        PRINT( _L( "MM MTP <> CCopyObject::CanCopyObjectL playlist file can't copy" ) );
        result = EMTPRespCodeAccessDenied;
        }

    PRINT1( _L( "MM MTP <= CCopyObject::CanCopyObjectL result = 0x%x" ), result );
    return result;
    }

// -----------------------------------------------------------------------------
// CCopyObject::GetPreviousPropertiesL
// Save the object properties before doing the copy
// -----------------------------------------------------------------------------
//
void CCopyObject::GetPreviousPropertiesL( const CMTPObjectMetaData& aObject )
    {
    PRINT( _L( "MM MTP => CCopyObject::GetPreviousPropertiesL" ) );

    const TDesC& suid( aObject.DesC( CMTPObjectMetaData::ESuid ) );

    User::LeaveIfError( iFramework.Fs().Modified( suid, iPreviousModifiedTime ) );

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

                    PRINT1( _L( "MM MTP <> CCopyObject::GetPreviousPropertiesL::ServiceSpecificObjectPropertyL err = %d" ), err );

                    if ( err == KErrNone )
                        {
                        iPropertyElement = &(iPropertyList->ReservePropElemL(handle, propCode));
                        iPropertyElement->SetStringL(CMTPTypeObjectPropListElement::EValue, textData->StringChars());
//                        iPropertyElement = CMTPTypeObjectPropListElement::NewL(
//                                handle, propCode, *textData );
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

    PRINT1( _L( "MM MTP <= CCopyObject::GetPreviousPropertiesL err = %d" ), err );
    }

// -----------------------------------------------------------------------------
// CCopyObject::ServiceMetaDataToWrapper
//
// -----------------------------------------------------------------------------
//
EXPORT_C TMTPResponseCode CCopyObject::ServiceMetaDataToWrapper(
    const TUint16 aPropCode,
    MMTPType& aNewData,
    const CMTPObjectMetaData& aObject )
    {
    TMTPResponseCode resCode = EMTPRespCodeOK;

    TRAPD( err, iDpConfig.GetWrapperL().SetObjectMetadataValueL( aPropCode,
        aNewData,
        aObject ) );

    PRINT1( _L("MM MTP <> CCopyObject::ServiceMetaDataToWrapper err = %d"), err);

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

    PRINT1( _L("MM MTP <= CCopyObject::ServiceMetaDataToWrapper resCode = 0x%x"), resCode);

    return resCode;
    }

// -----------------------------------------------------------------------------
// CCopyObject::SetPreviousPropertiesL
// Set the object properties after doing the copy
// -----------------------------------------------------------------------------
//
void CCopyObject::SetPreviousPropertiesL( const CMTPObjectMetaData& aObject )
    {
    PRINT( _L( "MM MTP => CCopyObject::SetPreviousPropertiesL" ) );
    const TInt count( iPropertyList->NumberOfElements() );
    PRINT1( _L( "MM MTP <> CCopyObject::SetPreviousPropertiesL count = %d" ), count );
    TMTPResponseCode respcode = EMTPRespCodeOK;
    CMTPTypeString* stringData = NULL;
    iPropertyList->ResetCursor();
    for ( TInt i = 0; i < count; i++ )
        {
        CMTPTypeObjectPropListElement& element = iPropertyList->GetNextElementL();

        TUint32 handle = element.Uint32L(
                CMTPTypeObjectPropListElement::EObjectHandle );
        TUint16 propertyCode = element.Uint16L(
                CMTPTypeObjectPropListElement::EPropertyCode );
        TUint16 dataType = element.Uint16L(
                CMTPTypeObjectPropListElement::EDatatype );
        PRINT3( _L( "MM MTP <> CCopyObject::SetPreviousPropertiesL = 0x%x, propertyCode = 0x%x, dataType = 0x%x" ),
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
                    element.StringL(
                    CMTPTypeObjectPropListElement::EValue)); // + stringData

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

    PRINT1( _L( "MM MTP <= CCopyObject::SetPreviousPropertiesL respcode = 0x%x" ), respcode );
    }

// -----------------------------------------------------------------------------
// CCopyObject::UpdateObjectInfoL
// Update object info in the database
// -----------------------------------------------------------------------------
//
TUint32 CCopyObject::UpdateObjectInfoL( const TDesC& aOldObjectName, const TDesC& aNewObjectName )
    {
    PRINT2( _L( "MM MTP => CCopyObject::UpdateObjectInfoL aOldObjectName = %S, aNewObjectName = %S" ),
            &aOldObjectName, &aNewObjectName );
    // We should not modify this object's handle, so just get a "copy".
    CMTPObjectMetaData* objectInfo = CMTPObjectMetaData::NewLC(); // + objectInfo
    const TMTPTypeUint32 objectHandle( iObjectInfo->Uint( CMTPObjectMetaData::EHandle ) );
    if ( iFramework.ObjectMgr().ObjectL( objectHandle, *objectInfo) )
        {
        objectInfo->SetDesCL( CMTPObjectMetaData::ESuid, aNewObjectName );
        objectInfo->SetUint( CMTPObjectMetaData::EParentHandle,
                iNewParentHandle );
        // Modify storage Id.
        objectInfo->SetUint( CMTPObjectMetaData::EStorageId, iStorageId );
        iFramework.ObjectMgr().InsertObjectL( *objectInfo );
        }
    else
        {
        User::Leave( KErrCorrupt );
        }

    TUint32 handle = objectInfo->Uint( CMTPObjectMetaData::EHandle );
    PRINT1( _L( "MM MTP <> CCopyObject::UpdateObjectInfoL handle = 0x%x" ), handle );
    SetPropertiesL( aOldObjectName, aNewObjectName, *objectInfo );
    CleanupStack::PopAndDestroy( objectInfo ); // - objectInfo
    PRINT( _L( "MM MTP <= CCopyObject::UpdateObjectInfoL" ) );
    return handle;
    }

// -----------------------------------------------------------------------------
// CCopyObject::CopyAndUpdateL
// Move a single object and update the database
// -----------------------------------------------------------------------------
//
void CCopyObject::CopyAndUpdateL( TUint32 aObjectHandle )
    {
    PRINT1( _L( "MM MTP => CopyObject::CopyAndUpdateL aObjectHanlde = 0x%x" ), aObjectHandle );
    CMTPObjectMetaData* objectInfo = CMTPObjectMetaData::NewLC(); // + objectInfo

    if ( iFramework.ObjectMgr().ObjectL( aObjectHandle, *objectInfo ) )
        {
        // This is used to keep the same behavior in mass storage and device file manager.
        if ( objectInfo->Uint( CMTPObjectMetaData::EFormatCode )
            == EMTPFormatCodeAbstractAudioVideoPlaylist )
            {
            PRINT( _L( "MM MTP <> CopyObject::CopyAndUpdateL Playlist file don't to be copieds" ) );
            CleanupStack::PopAndDestroy( objectInfo ); // - objectInfo
            return;
            }

        RBuf fileName; // This is the source object name.
        fileName.CleanupClosePushL(); // + fileName
        fileName.CreateL( KMaxFileName );

        RBuf oldFileName;
        oldFileName.CleanupClosePushL(); // + oldFileName
        oldFileName.CreateL( KMaxFileName );

        RBuf rightPartName;
        rightPartName.CleanupClosePushL(); // + rightPartName
        rightPartName.CreateL( KMaxFileName );

        fileName = objectInfo->DesC( CMTPObjectMetaData::ESuid );
        oldFileName = fileName;

        rightPartName = fileName.Right( fileName.Length()
            - iPathToCopy->Length() );

        if ( ( iNewRootFolder->Length() + rightPartName.Length() ) > fileName.MaxLength() )
            {
            User::Leave( KErrCorrupt );
            }

        fileName.Zero();
        fileName.Append( *iNewRootFolder );
        fileName.Append( rightPartName );
        PRINT1( _L( "MM MTP <> CopyAndUpdateL fileName = %S" ), &fileName );

        if ( objectInfo->Uint( CMTPObjectMetaData::EDataProviderId )
            == iFramework.DataProviderId() )
            {
            // should copy before the set metadata DB
            GetPreviousPropertiesL( *objectInfo );
            TInt err = iFileMan->Copy( oldFileName, fileName );
            PRINT1( _L( "MM MTP <> CCopyObject::CopyAndUpdateL err = %d" ), err );
            User::LeaveIfError( err );
            User::LeaveIfError( iFramework.Fs().SetModified( fileName,
                iPreviousModifiedTime ) );

            // Modify Suid
            objectInfo->SetDesCL( CMTPObjectMetaData::ESuid, fileName );

            // Modify parentHandle
            TParsePtrC parentSuid( fileName );
            PRINT1( _L( "MM MTP <> CCopyObject::CopyAndUpdateL parentSuid = %S" ), &(parentSuid.DriveAndPath()) );

            TUint32 parentHandle = iFramework.ObjectMgr().HandleL( parentSuid.DriveAndPath() );
            objectInfo->SetUint( CMTPObjectMetaData::EParentHandle, parentHandle );
            PRINT1( _L( "MM MTP <> CCopyObject::CopyAndUpdateL parentHandle = 0x%x" ), parentHandle );

            // Modify storage Id.
            objectInfo->SetUint( CMTPObjectMetaData::EStorageId, iStorageId );
            TRAP( err, iFramework.ObjectMgr().InsertObjectL( *objectInfo ) );
            if ( err != KErrNone )
                PRINT1( _L( "MM MTP <> CCopyObject::CopyAndUpdateL err = %d" ), err );
            // Set the properties of the new object
            SetPropertiesL( oldFileName, fileName, *objectInfo );
            }
        // Else this is not the owner of this object, so don't update the object store.

        CleanupStack::PopAndDestroy( &rightPartName ); // - rightPartName
        CleanupStack::PopAndDestroy( &oldFileName ); // - oldFileName
        CleanupStack::PopAndDestroy( &fileName ); // - fileName
        }
    else
        {
        User::Leave( KErrCorrupt );
        }

    CleanupStack::PopAndDestroy( objectInfo ); // - objectInfo

    PRINT( _L( "MM MTP <= CopyObject::CopyAndUpdateL" ) );
    }

// -----------------------------------------------------------------------------
// CCopyObject::SetPropertiesL
// Set the object properties in the object property store
// -----------------------------------------------------------------------------
//
void CCopyObject::SetPropertiesL( const TDesC& aOldFileName,
        const TDesC& aNewFileName,
        const CMTPObjectMetaData& aObject )
    {
    PRINT( _L( "MM MTP => CCopyObject::SetPropertiesL" ) );
    // won't leave with KErrAlreadyExist
    iDpConfig.GetWrapperL().AddObjectL( aNewFileName );

    TUint formatCode = aObject.Uint( CMTPObjectMetaData::EFormatCode );
    if ( formatCode == EMTPFormatCodeM3UPlaylist )
        {
        MMTPReferenceMgr& referenceMgr = iFramework.ReferenceMgr();
        CDesCArray* references = referenceMgr.ReferencesLC( aOldFileName ); // + references
        referenceMgr.SetReferencesL( aNewFileName, *references );
        CleanupStack::PopAndDestroy( references ); // - references
        }

    SetPreviousPropertiesL( aObject );
    PRINT( _L( "MM MTP <= CCopyObject::SetPropertiesL" ) );
    }

// -----------------------------------------------------------------------------
// CCopyObject::RunError
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CCopyObject::RunError( TInt aError )
    {
    if ( aError != KErrNone )
        PRINT1( _L( "MM MTP <> CCopyObject::RunError aError = %d" ), aError );

    TRAP_IGNORE( SendResponseL( EMTPRespCodeGeneralError ) );
    return KErrNone;
    }

// end of file
