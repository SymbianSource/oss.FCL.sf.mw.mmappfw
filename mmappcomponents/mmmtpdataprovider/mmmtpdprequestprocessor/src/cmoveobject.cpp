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
#include "cpropertysettingutility.h"

/**
* Verification data for the MoveObject request
*/
const TMTPRequestElementInfo KMTPMoveObjectPolicy[] =
    {
        {
        TMTPTypeRequest::ERequestParameter1,
        EMTPElementTypeObjectHandle,
        EMTPElementAttrFile | EMTPElementAttrWrite,
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
// CMoveObject::~CMoveObject
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CMoveObject::~CMoveObject()
    {
    Cancel();

    delete iDest;

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
    MMmMtpDpConfig& aDpConfig ) :
        CRequestProcessor( aFramework,
            aConnection,
            sizeof( KMTPMoveObjectPolicy ) / sizeof( TMTPRequestElementInfo ),
            KMTPMoveObjectPolicy ),
        iDpConfig( aDpConfig )
    {
    PRINT( _L( "Operation: MoveObject(0x1019)" ) );
    }

// -----------------------------------------------------------------------------
// CMoveObject::ConstructL
// Second phase constructor
// -----------------------------------------------------------------------------
//
EXPORT_C void CMoveObject::ConstructL()
    {
    iPropertyList = CMTPTypeObjectPropList::NewL();

    // Set the CenRep value of MTP status,
    // also need to do in other processors which related to MPX
    SetPSStatus();
    }

// -----------------------------------------------------------------------------
// CMoveObject::ServiceL
// MoveObject request handler
// -----------------------------------------------------------------------------
//
EXPORT_C void CMoveObject::ServiceL()
    {
    PRINT( _L( "MM MTP => CMoveObject::ServiceL" ) );

    MoveObjectL();

    PRINT( _L( "MM MTP <= CMoveObject::ServiceL" ) );
    }

// -----------------------------------------------------------------------------
// CMoveObject::MoveObjectL
// Move object operation
// -----------------------------------------------------------------------------
//
void CMoveObject::MoveObjectL()
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
    if ( ( newObjectName.Length() + fileNameParser.NameAndExt().Length() )
        <= newObjectName.MaxLength() )
        {
        newObjectName.Append( fileNameParser.NameAndExt() );
        }
    responseCode = CanMoveObjectL( suid, newObjectName );

    if ( responseCode == EMTPRespCodeOK )
        MoveFileL( newObjectName );

    SendResponseL( responseCode );

    CleanupStack::PopAndDestroy( &newObjectName ); // - newObjectName

    PRINT1( _L( "MM MTP <= CMoveObject::MoveObjectL responseCode = 0x%x" ), responseCode );
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
    PRINT3( _L( "MM MTP <> objectHandle = 0x%x, iStorageId = 0x%x, iNewParentHandle = 0x%x" ),
        objectHandle,
        iStorageId,
        iNewParentHandle );

    // not taking owernship
    iObjectInfo = iRequestChecker->GetObjectInfo( objectHandle );
    __ASSERT_DEBUG( iObjectInfo, Panic( EMmMTPDpObjectNull ) );

    if ( iNewParentHandle == KMTPHandleNone )
        {
        SetDefaultParentObjectL();
        }
    else
        {
        CMTPObjectMetaData* parentObject = iRequestChecker->GetObjectInfo( iNewParentHandle );
        __ASSERT_DEBUG( parentObject, Panic( EMmMTPDpObjectNull ) );

        delete iDest;
        iDest = NULL;
        iDest = parentObject->DesC( CMTPObjectMetaData::ESuid ).AllocL();
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
    iDest = iFramework.StorageMgr().StorageL( iStorageId ).DesC( CMTPStorageMetaData::EStorageSuid ).AllocL();
    iNewParentHandle = KMTPHandleNoParent;

    PRINT1( _L( "MM MTP <= CMoveObject::SetDefaultParentObjectL, iDest = %S" ), iDest );
    }

// -----------------------------------------------------------------------------
// CMoveObject::CanMoveObjectL
// Check if we can move the file to the new location
// -----------------------------------------------------------------------------
//
TMTPResponseCode CMoveObject::CanMoveObjectL( const TDesC& aOldName,
    const TDesC& aNewName ) const
    {
    PRINT2( _L( "MM MTP => CMoveObject::CanMoveObjectL aOldName = %S, aNewName = %S" ),
        &aOldName,
        &aNewName );
    TMTPResponseCode result = EMTPRespCodeOK;

    TEntry fileEntry;
    User::LeaveIfError( iFramework.Fs().Entry( aOldName, fileEntry ) );
    TInt drive = iFramework.StorageMgr().DriveNumber( iStorageId );
    User::LeaveIfError( drive );
    TVolumeInfo volumeInfo;
    User::LeaveIfError( iFramework.Fs().Volume( volumeInfo, drive ) );

    if ( volumeInfo.iFree < fileEntry.FileSize() )
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
// CMoveObject::MoveFileL
// A helper function of MoveObjectL
// -----------------------------------------------------------------------------
//
void CMoveObject::MoveFileL( const TDesC& aNewFileName )
    {
    TFileName oldFileName = iObjectInfo->DesC( CMTPObjectMetaData::ESuid );
    PRINT2( _L( "MM MTP => CMoveObject::MoveFileL old name = %S, aNewFileName = %S" ),
        &oldFileName,
        &aNewFileName );

    if ( iStorageId == iObjectInfo->Uint( CMTPObjectMetaData::EStorageId ) )
        iSameStorage = ETrue;
    else
        iSameStorage = EFalse;
    GetPreviousPropertiesL( *iObjectInfo );
    SetPropertiesL( oldFileName, aNewFileName );

    CFileMan* fileMan = CFileMan::NewL( iFramework.Fs() );
    CleanupStack::PushL( fileMan );
    User::LeaveIfError( fileMan->Move( oldFileName, aNewFileName ) );
    CleanupStack::PopAndDestroy( fileMan );

    User::LeaveIfError( iFramework.Fs().SetModified( aNewFileName,
        iPreviousModifiedTime ) );

    PRINT( _L( "MM MTP <= CMoveObject::MoveFileL" ) );
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
    TInt err = KErrNone;
    TUint16 propCode;
    TUint32 handle = aObject.Uint( CMTPObjectMetaData::EHandle );

    if ( iPropertyElement != NULL )
        {
        delete iPropertyElement;
        iPropertyElement = NULL;
        }

    for ( TInt i = 0; i < count; i++ )
        {
        propCode = ( *properties )[i];

        switch ( propCode )
            {
            case EMTPObjectPropCodeStorageID:
            case EMTPObjectPropCodeObjectFormat:
            case EMTPObjectPropCodeProtectionStatus:
            case EMTPObjectPropCodeObjectSize:
            case EMTPObjectPropCodeObjectFileName:
            case EMTPObjectPropCodeParentObject:
            case EMTPObjectPropCodePersistentUniqueObjectIdentifier:
            case EMTPObjectPropCodeDateCreated:
            case EMTPObjectPropCodeDateModified:
                break;

            case EMTPObjectPropCodeNonConsumable:
                iPropertyElement = &( iPropertyList->ReservePropElemL( handle, propCode ) );
                iPropertyElement->SetUint8L( CMTPTypeObjectPropListElement::EValue,
                    aObject.Uint( CMTPObjectMetaData::ENonConsumable ) );
                break;

            case EMTPObjectPropCodeName:
            case EMTPObjectPropCodeDateAdded:
                if ( ( propCode == EMTPObjectPropCodeName )
                    || ( !MmMtpDpUtility::IsVideoL( aObject.DesC( CMTPObjectMetaData::ESuid ), iFramework )
                        && ( propCode == EMTPObjectPropCodeDateAdded ) ) )
                    {
                    CMTPTypeString* textData = CMTPTypeString::NewLC(); // + textData

                    TRAP( err, iDpConfig.GetWrapperL().GetObjectMetadataValueL( propCode,
                        *textData,
                        aObject ) );

                    PRINT1( _L( "MM MTP <> CMoveObject::GetPreviousPropertiesL err = %d" ), err );

                    if ( err == KErrNone )
                        {
                        iPropertyElement = &( iPropertyList->ReservePropElemL( handle, propCode ) );
                        iPropertyElement->SetStringL( CMTPTypeObjectPropListElement::EValue,
                            textData->StringChars() );
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

        if ( iPropertyElement != NULL )
            {
            iPropertyList->CommitPropElemL( *iPropertyElement );
            iPropertyElement = NULL;
            }
        } // end of for loop

    PRINT1( _L( "MM MTP <= CMoveObject::GetPreviousPropertiesL err = %d" ), err );
    }

// -----------------------------------------------------------------------------
// CMoveObject::SetPreviousPropertiesL
// Set the object properties after doing the move
// -----------------------------------------------------------------------------
//
void CMoveObject::SetPreviousPropertiesL()
    {
    PRINT( _L( "MM MTP => CMoveObject::SetPreviousPropertiesL" ) );

    TMTPResponseCode respcode = EMTPRespCodeOK;

    iPropertyList->ResetCursor();
    const TInt count = iPropertyList->NumberOfElements();
    for ( TInt i = 0; i < count; i++ )
        {
        CMTPTypeObjectPropListElement& element = iPropertyList->GetNextElementL();

        TUint32 handle = element.Uint32L( CMTPTypeObjectPropListElement::EObjectHandle );
        TUint16 propertyCode = element.Uint16L( CMTPTypeObjectPropListElement::EPropertyCode );
        TUint16 dataType = element.Uint16L( CMTPTypeObjectPropListElement::EDatatype );
        PRINT3( _L( "MM MTP <> CCopyObject::SetPreviousPropertiesL = 0x%x, propertyCode = 0x%x, dataType = 0x%x" ),
            handle,
            propertyCode,
            dataType );

        switch ( propertyCode )
            {
            case EMTPObjectPropCodeStorageID:
            case EMTPObjectPropCodeObjectFormat:
            case EMTPObjectPropCodeProtectionStatus:
            case EMTPObjectPropCodeObjectSize:
            case EMTPObjectPropCodeObjectFileName:
            case EMTPObjectPropCodeParentObject:
            case EMTPObjectPropCodePersistentUniqueObjectIdentifier:
            case EMTPObjectPropCodeDateCreated:
            case EMTPObjectPropCodeDateModified:
            case EMTPObjectPropCodeDateAdded:
                break;

            case EMTPObjectPropCodeNonConsumable:
                iObjectInfo->SetUint( CMTPObjectMetaData::ENonConsumable,
                    element.Uint8L( CMTPTypeObjectPropListElement::EValue ) );
                break;

            case EMTPObjectPropCodeName:
                {
                CMTPTypeString *stringData = CMTPTypeString::NewLC( element.StringL( CMTPTypeObjectPropListElement::EValue ) ); // + stringData

                respcode = iDpConfig.PropSettingUtility()->SetMetaDataToWrapperL( iDpConfig,
                    propertyCode,
                    *stringData,
                    *iObjectInfo );

                CleanupStack::PopAndDestroy( stringData ); // - stringData
                }
                break;

            default:
                {
                respcode = iDpConfig.PropSettingUtility()->SetSpecificObjectPropertyL( iDpConfig,
                    propertyCode,
                    *iObjectInfo,
                    element );
                }
                break;
            }
        } // end of for loop

    // ignore errors

    PRINT1( _L( "MM MTP <= CMoveObject::SetPreviousPropertiesL respcode = 0x%x" ), respcode );
    }

// -----------------------------------------------------------------------------
// CMoveObject::SetPropertiesL
// Set the object properties in the object property store.
// -----------------------------------------------------------------------------
//
void CMoveObject::SetPropertiesL( const TDesC& aOldFileName,
    const TDesC& aNewFileName )
    {
    PRINT2( _L( "MM MTP => CMoveObject::SetPropertiesL aOldFileName = %S, aNewFileName = %S" ),
        &aOldFileName, 
        &aNewFileName );
		
    iObjectInfo->SetDesCL( CMTPObjectMetaData::ESuid, aNewFileName );
    iObjectInfo->SetUint( CMTPObjectMetaData::EStorageId, iStorageId );
    iObjectInfo->SetUint( CMTPObjectMetaData::EParentHandle, iNewParentHandle );

    TUint32 formatCode = iObjectInfo->Uint( CMTPObjectMetaData::EFormatCode );
    if ( formatCode == EMTPFormatCodeAbstractAudioVideoPlaylist )
        {
        // This is used to keep the same behavior in mass storage and device file manager.
        PRINT( _L( "MM MTP <> CMoveObject::SetPropertiesL Playlist file do not update the MPX DB" ) );
        }
    else
        {
        if ( iSameStorage )
            iDpConfig.GetWrapperL().RenameObjectL( aOldFileName, aNewFileName );
        // if the two object in different storage, we should delete the old one and insert new one
        else
            {
            iDpConfig.GetWrapperL().DeleteObjectL( aOldFileName, formatCode );

            TUint32 subFormatCode = iObjectInfo->Uint( CMTPObjectMetaData::EFormatSubCode );
            iDpConfig.GetWrapperL().AddObjectL( aNewFileName,
                formatCode,
                subFormatCode );

            SetPreviousPropertiesL();
            }
        }

    iFramework.ObjectMgr().ModifyObjectL( *iObjectInfo );

    // It's not necessary to change references of playlists since Reference DB is used PUID

    PRINT( _L( "MM MTP <= CMoveObject::SetPropertiesL" ) );
    }

// end of file
