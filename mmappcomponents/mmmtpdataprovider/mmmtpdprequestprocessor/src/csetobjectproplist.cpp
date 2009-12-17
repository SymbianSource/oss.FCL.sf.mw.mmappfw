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
* Description:  Implement the operation: SetObjectPropList
*
*/


#include <mtp/mmtpdataproviderframework.h>
#include <mtp/cmtptypeobjectproplist.h>
#include <mtp/mmtpobjectmgr.h>
#include <mtp/cmtptypestring.h>
#include <mtp/cmtptypearray.h>

#include "csetobjectproplist.h"
#include "cmmmtpdpmetadataaccesswrapper.h"
#include "mmmtpdputility.h"
#include "mmmtpdplogger.h"
#include "mmmtpdpconfig.h"

// -----------------------------------------------------------------------------
// CSetObjectPropList::NewL
// Two-phase construction method
// -----------------------------------------------------------------------------
//
//EXPORT_C MMmRequestProcessor* CSetObjectPropList::NewL( MMTPDataProviderFramework& aFramework,
//    MMTPConnection& aConnection,
//    CMmMtpDpMetadataAccessWrapper& aWrapper )
//    {
//    CSetObjectPropList* self = new ( ELeave ) CSetObjectPropList( aFramework,
//        aConnection,
//        aWrapper );
//    CleanupStack::PushL( self );
//    self->ConstructL();
//    CleanupStack::Pop( self );
//    return self;
//    }

// -----------------------------------------------------------------------------
// CSetObjectPropList::CSetObjectPropList
// Standard c++ constructor
// -----------------------------------------------------------------------------
//
EXPORT_C CSetObjectPropList::CSetObjectPropList( MMTPDataProviderFramework& aFramework,
    MMTPConnection& aConnection,
    MMmMtpDpConfig& aDpConfig ) :
    CRequestProcessor( aFramework, aConnection, 0, NULL),
    iObjectMgr( aFramework.ObjectMgr() ),
    iFs( aFramework.Fs() ),
    iDpConfig( aDpConfig ),
    iUnprocessedIndex ( 0 )
    {
    PRINT( _L( "Operation: SetObjectPropList(0x9806)" ) );
    }

// -----------------------------------------------------------------------------
// CSetObjectPropList::ConstructL
// 2nd Phase Constructor
// -----------------------------------------------------------------------------
//
EXPORT_C void CSetObjectPropList::ConstructL()
    {
    CActiveScheduler::Add( this );

    iPropertyList = CMTPTypeObjectPropList::NewL();
    SetPSStatus();
    }

// -----------------------------------------------------------------------------
// CSetObjectPropList::~CSetObjectPropList
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CSetObjectPropList::~CSetObjectPropList()
    {
    Cancel();
    delete iPropertyList;
    }

// -----------------------------------------------------------------------------
// CSetObjectPropList::ServiceL
// SetObjectPropList request handler
// -----------------------------------------------------------------------------
//
EXPORT_C void CSetObjectPropList::ServiceL()
    {
    PRINT( _L( "MM MTP => CSetObjectPropList::ServiceL" ) );
    // Recieve the data from the property list
    ReceiveDataL( *iPropertyList );
    PRINT( _L( "MM MTP <= CSetObjectPropList::ServiceL" ) );
    }

// -----------------------------------------------------------------------------
// CSetObjectPropList::DoHandleResponsePhaseL
// Completing phase for the request handler
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CSetObjectPropList::DoHandleResponsePhaseL()
    {
    PRINT( _L( "MM MTP => CSetObjectPropList::DoHandleResponsePhaseL" ) );

    iElementCount = iPropertyList->NumberOfElements();
    iPropertyList->ResetCursor();
    if( iElementCount > 0 )
        {
        iUnprocessedIndex = 0;
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, iStatus.Int() );
        SetActive();
        }
    else
        {
        SendResponseL( EMTPRespCodeOK );
        }

    PRINT( _L( "MM MTP <= CSetObjectPropList::DoHandleResponsePhaseL" ) );
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CSetObjectPropList::SetObjectPropListL
// Set object proplist
// -----------------------------------------------------------------------------
//
TMTPResponseCode CSetObjectPropList::SetObjectPropListL(
        const CMTPTypeObjectPropListElement& aPropListElement )
    {
    PRINT( _L( "MM MTP => CSetObjectPropList::SetObjectPropListL" ) );

    TMTPTypeUint16 protectionStatus( EMTPProtectionNoProtection );
    TMTPResponseCode responseCode( EMTPRespCodeOK );

    TUint32 handle = aPropListElement.Uint32L( CMTPTypeObjectPropListElement::EObjectHandle );
    TUint16 propertyCode = aPropListElement.Uint16L( CMTPTypeObjectPropListElement::EPropertyCode );
    TUint16 dataType = aPropListElement.Uint16L( CMTPTypeObjectPropListElement::EDatatype );
    PRINT3( _L( "MM MTP <> handle = 0x%x, propertyCode = 0x%x, dataType = 0x%x" ),
        handle, propertyCode, dataType );

    responseCode = MmMtpDpUtility::CheckPropType( propertyCode, dataType );
    PRINT1( _L( "MM MTP <> CheckPropType response code is 0x%x" ), responseCode );
    if( responseCode != EMTPRespCodeOK )
        return responseCode;

    if ( iFramework.ObjectMgr().ObjectOwnerId( handle )
        == iFramework.DataProviderId() )
        {
        PRINT( _L( "MM MTP => CSetObjectPropList::SetObjectPropListL enter" ) );
        CMTPObjectMetaData* object = CMTPObjectMetaData::NewLC(); // + object
        iFramework.ObjectMgr().ObjectL( handle, *object );

        if ( protectionStatus.Value() != EMTPProtectionNoProtection )
            {
            //for some reason, P4S expects Access Denied response instead of write protected
            return EMTPRespCodeAccessDenied; // EMTPRespCodeObjectWriteProtected;
            }

        switch ( propertyCode )
            {
            case EMTPObjectPropCodeStorageID:
            case EMTPObjectPropCodeObjectFormat:
            case EMTPObjectPropCodeProtectionStatus:
            case EMTPObjectPropCodeObjectSize:
            case EMTPObjectPropCodeParentObject:
            case EMTPObjectPropCodePersistentUniqueObjectIdentifier:
            case EMTPObjectPropCodeNonConsumable:
            case EMTPObjectPropCodeDateAdded:
            case EMTPObjectPropCodeDateCreated:
            case EMTPObjectPropCodeDateModified:
                {
                responseCode = EMTPRespCodeAccessDenied;
                }
                break;

            case EMTPObjectPropCodeObjectFileName:
                {
                TPtrC suid( object->DesC( CMTPObjectMetaData::ESuid ) );
                TBuf<KMaxFileName> newSuid( aPropListElement.StringL(
                    CMTPTypeObjectPropListElement::EValue ) );
                TInt err = KErrNone;
                err = MmMtpDpUtility::UpdateObjectFileName( iFramework.Fs(), suid, newSuid );
                PRINT1( _L( "MM MTP <> Update object file name err = %d" ), err );
                if ( KErrOverflow == err ) // full path name is too long
                    {
                    responseCode = EMTPRespCodeInvalidDataset;
                    }
                else if ( ( KErrNone == err ) || ( KErrAlreadyExists == err ) )
                    {
                    TRAP( err, iDpConfig.GetWrapperL().RenameObjectL( suid, newSuid ) ); //Update MPX DB
                    PRINT1( _L( "MM MTP <> Rename Object err = %d" ), err );
                    // it is ok if file is not found in DB, following S60 solution
                    if ( KErrNotFound == err )
                        {
                        TRAP( err, iDpConfig.GetWrapperL().AddObjectL( newSuid ) );
                        PRINT1( _L( "MM MTP <> Add Object err = %d" ), err );
                        }

                    if ( KErrNone == err )
                        {
                        object->SetDesCL( CMTPObjectMetaData::ESuid, newSuid );
                        iFramework.ObjectMgr().ModifyObjectL( *object );
                        }
                    else
                        {
                        responseCode = EMTPRespCodeGeneralError;
                        }
                    }
                }
                break;

            case EMTPObjectPropCodeName:
                {
                CMTPTypeString* stringData = CMTPTypeString::NewLC(
                    aPropListElement.StringL(
                        CMTPTypeObjectPropListElement::EValue ) );// + stringData

                responseCode = ServiceMetaDataToWrapperL( propertyCode,
                    *stringData, *object );

                CleanupStack::PopAndDestroy( stringData );// - stringData
                }
                break;

            default:
                {
                /*// trap and handle with response code here, so correct fail index should report
                TRAPD( err, responseCode = ServiceSpecificObjectPropertyL( propertyCode, *object, aPropListElement ) );
                PRINT1( _L("MM MTP <> CSetObjectPropList::SetObjectPropListL, ServiceSpecificObjectPropertyL, err = %d"), err );

                if ( err == KErrNotSupported )
                    {
                    responseCode = EMTPRespCodeAccessDenied;
                    }*/
                responseCode = ServiceSpecificObjectPropertyL( propertyCode, *object,
                    aPropListElement );
                }
                break;
            }

        CleanupStack::PopAndDestroy( object ); // - object
        }

    PRINT1( _L( "MM MTP <= CSetObjectPropList::SetObjectPropListL responseCode = 0x%x" ), responseCode );
    return responseCode;
    }

// -----------------------------------------------------------------------------
// CSetObjectPropList::HasDataphase
// Decide if has data phase
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CSetObjectPropList::HasDataphase() const
    {
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CSetObjectPropList::ServiceMetaDataToWrapperL
//
// -----------------------------------------------------------------------------
//
EXPORT_C TMTPResponseCode CSetObjectPropList::ServiceMetaDataToWrapperL(
    const TUint16 aPropCode,
    MMTPType& aNewData,
    const CMTPObjectMetaData& aObjectMetaData )
    {
    TMTPResponseCode resCode = EMTPRespCodeOK;

    TRAPD( err, iDpConfig.GetWrapperL().SetObjectMetadataValueL( aPropCode,
        aNewData,
        aObjectMetaData ) );

    PRINT1( _L("MM MTP <> CSetObjectPropList::ServiceMetaDataToWrapperL err = %d"), err);

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
        if( MmMtpDpUtility::HasMetadata( aObjectMetaData.Uint( CMTPObjectMetaData::EFormatCode ) ) )
            SendResponseL( EMTPRespCodeAccessDenied );
        }
    else
        {
        // add new virtual call to see if the above condition can be handle probably
        err = HandleSpecificWrapperError( err, aObjectMetaData );

        if ( err != KErrNone )
            resCode = EMTPRespCodeGeneralError;
        }

    PRINT1( _L( "MM MTP <= CSetObjectPropList::ServiceMetaDataToWrapperL resCode = 0x%x" ), resCode );

    return resCode;
    }

// -----------------------------------------------------------------------------
// CSetObjectPropList::RunL
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CSetObjectPropList::RunL()
    {
    if ( iUnprocessedIndex <  iElementCount )
        {
        TMTPResponseCode responseCode = SetObjectPropListL( iPropertyList->GetNextElementL());

        if ( responseCode != EMTPRespCodeOK )
            {
            SendResponseL( responseCode, 1, &iUnprocessedIndex );
            }
        else
            {
            // Complete ourselves with current TRequestStatus
            // Increase index to process next handle on next round
            iUnprocessedIndex++;
            TRequestStatus* status = &iStatus;
            User::RequestComplete( status, iStatus.Int() );
            SetActive();
            }
        }
    else // all handles processed, can send data
        {
        SendResponseL( EMTPRespCodeOK );
        }
    }

// -----------------------------------------------------------------------------
// CSetObjectPropList::RunError
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSetObjectPropList::RunError( TInt aError )
    {
    PRINT1( _L( "MM MTP <> CSetObjectPropList::RunError with error %d" ), aError );

    // Reschedule ourselves
    // TODO: go to next index or increase?
    // iUnprocessedIndex++
//    TRequestStatus* status = &iStatus;
//    User::RequestComplete( status, aError );
//    SetActive();
    PRINT1( _L( "MM MTP <> CGetObjectPropList::RunError aError = %d" ), aError );
    TRAP_IGNORE( SendResponseL( EMTPRespCodeGeneralError ) );

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSetObjectPropList::DoCancel()
// Cancel the process
// -----------------------------------------------------------------------------
//
EXPORT_C void CSetObjectPropList::DoCancel()
    {

    }

// end of file
