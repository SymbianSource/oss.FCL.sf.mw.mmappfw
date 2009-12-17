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
* Description:  Implement the operation: GetPartialObject
*
*/


#include <mtp/cmtptypefile.h>
#include <mtp/mmtpdataproviderframework.h>
#include <mtp/cmtpobjectmetadata.h>

#include "cgetpartialobject.h"
#include "mmmtpdplogger.h"
#include "tmmmtpdppanic.h"
#include "ttypeflatbuf.h"
#include "mmmtpdpconfig.h"

/**
* Verification data for the GetPartialObject request
*/
const TMTPRequestElementInfo KMTPGetPartialObjectPolicy[] =
    {
        {TMTPTypeRequest::ERequestParameter1, EMTPElementTypeObjectHandle, EMTPElementAttrFile, 0, 0, 0}
    };

// -----------------------------------------------------------------------------
// CGetPartialObject::NewL
// Two-phase construction method
// -----------------------------------------------------------------------------
//
EXPORT_C MMmRequestProcessor* CGetPartialObject::NewL( MMTPDataProviderFramework& aFramework,
    MMTPConnection& aConnection,
    MMmMtpDpConfig& aDpConfig )
    {
    CGetPartialObject* self = new (ELeave) CGetPartialObject( aFramework, aConnection );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CGetPartialObject::~CGetPartialObject()
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CGetPartialObject::~CGetPartialObject()
    {
    delete iBuffer;
    delete iPartialData;
    delete iFileObject;
    }

// -----------------------------------------------------------------------------
// CGetPartialObject::CGetPartialObject
// Standard c++ constructor
// -----------------------------------------------------------------------------
//
EXPORT_C CGetPartialObject::CGetPartialObject( MMTPDataProviderFramework& aFramework,
    MMTPConnection& aConnection ) :
    CRequestProcessor( aFramework,
        aConnection,
        sizeof( KMTPGetPartialObjectPolicy ) / sizeof( TMTPRequestElementInfo ),
        KMTPGetPartialObjectPolicy ),
    iFramework ( aFramework ),
    iFs( iFramework.Fs() ),
    iBufferPtr8( NULL, 0 )
    {
    PRINT( _L( "Operation: GetPartialObject(0x101B)" ) );
    }

// -----------------------------------------------------------------------------
// CGetPartialObject::CheckRequestL
// Check the GetPartialObject reqeust
// -----------------------------------------------------------------------------
//
EXPORT_C TMTPResponseCode CGetPartialObject::CheckRequestL()
    {
    PRINT( _L( "MM MTP => CGetPartialObject::CheckRequestL" ) );
    TMTPResponseCode result = CRequestProcessor::CheckRequestL();
    if( result == EMTPRespCodeOK && !VerifyParametersL() )
        {
        result = EMTPRespCodeInvalidParameter;
        }

    PRINT1( _L( "MM MTP <= CGetPartialObject::CheckRequestL result = ox%x" ), result );
    return result;
    }

// -----------------------------------------------------------------------------
// CGetPartialObject::VerifyParametersL
// Verify if the parameter of the request (i.e. offset) is good.
// -----------------------------------------------------------------------------
//
TBool CGetPartialObject::VerifyParametersL()
    {
    PRINT( _L( "MM MTP => CGetPartialObject::VerifyParametersL" ) );
    __ASSERT_DEBUG( iRequestChecker, Panic( EMmMTPDpRequestCheckNull ) );
    TBool result = EFalse;
    iObjectHandle = Request().Uint32( TMTPTypeRequest::ERequestParameter1 );
    PRINT1( _L( "MM MTP <> CGetPartialObject::VerifyParametersL iObjectHandle = 0x%x" ), iObjectHandle );
    iOffset = Request().Uint32( TMTPTypeRequest::ERequestParameter2 );
    PRINT1( _L( "MM MTP <> CGetPartialObject::VerifyParametersL iOffset = %d" ), iOffset );
    TUint32 maxLength = Request().Uint32( TMTPTypeRequest::ERequestParameter3 );
    PRINT1( _L( "MM MTP <> CGetPartialObject::VerifyParametersL maxLength = %d" ), maxLength );

    //get object info, but do not have the ownship of the object
    CMTPObjectMetaData* objectInfo = iRequestChecker->GetObjectInfo( iObjectHandle );
    __ASSERT_DEBUG( objectInfo, Panic( EMmMTPDpObjectNull ) );

    const TDesC& suid( objectInfo->DesC( CMTPObjectMetaData::ESuid ) );
    PRINT1( _L( "MM MTP <> CGetPartialObject::VerifyParametersL suid = %S" ), &suid );

    if ( objectInfo->Uint( CMTPObjectMetaData::EDataProviderId )
        == iFramework.DataProviderId() )
        result = ETrue;

    TEntry fileEntry;
    User::LeaveIfError( iFs.Entry( suid, fileEntry ) );
    TInt64 fileSize = fileEntry.iSize;
    if( ( iOffset < fileEntry.iSize ) && result )
        {
        if ( maxLength == fileSize )
            {
            iCompleteFile = ETrue;
            }

        if( iOffset + maxLength > fileSize )
            {
            maxLength = fileSize - iOffset;
            }
        iPartialDataLength = maxLength;
        result = ETrue;
        }
    PRINT1( _L( "MM MTP <> CGetPartialObject::VerifyParametersL iPartialDataLength = %d" ), iPartialDataLength );

    PRINT1( _L( "MM MTP <= CGetPartialObject::VerifyParametersL result = %d" ), result );
    return result;
    }

// -----------------------------------------------------------------------------
// CGetPartialObject::ServiceL
// GetPartialObject request handler
// -----------------------------------------------------------------------------
//
EXPORT_C void CGetPartialObject::ServiceL()
    {
    PRINT( _L( "MM MTP => CGetPartialObject::ServiceL" ) );
    // Get file information
    CMTPObjectMetaData* objectInfo = iRequestChecker->GetObjectInfo( iObjectHandle );
    __ASSERT_DEBUG( objectInfo, Panic( EMmMTPDpObjectNull ) );
    iFileSuid.SetLength( 0 );
    iFileSuid.Append( objectInfo->DesC( CMTPObjectMetaData::ESuid ) );

    if ( iCompleteFile )
        {
        // Pass the complete file back to the host
        delete iFileObject;
        iFileObject = NULL;
        iFileObject = CMTPTypeFile::NewL( iFramework.Fs(), iFileSuid, EFileRead );
        SendDataL( *iFileObject );
        }
    else
        {
        // Send partial file fragment back.
        BuildPartialDataL();
        delete iPartialData;
        iPartialData = NULL;
        iPartialData  = new (ELeave) TMTPTypeFlatBuf( iBufferPtr8 );
        SendDataL( *iPartialData );
        }
    PRINT( _L( "MM MTP <= CGetPartialObject::ServiceL" ) );
    }

// -----------------------------------------------------------------------------
// CGetPartialObject::DoHandleResponsePhaseL
// Signal to the initiator how much data has been sent
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CGetPartialObject::DoHandleResponsePhaseL()
    {
    PRINT( _L( "MM MTP => CGetPartialObject::DoHandleResponsePhaseL" ) );
    TUint32 dataLength = iPartialDataLength;
    PRINT1( _L( "MM MTP <> CGetPartialObject::DoHandleResponsePhaseL dataLength = %d" ), dataLength );
    SendResponseL( EMTPRespCodeOK, 1, &dataLength );
    PRINT( _L( "MM MTP <= CGetPartialObject::DoHandleResponsePhaseL" ) );
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CGetPartialObject::ConstructL()
// Second-phase construction
// -----------------------------------------------------------------------------
//
void CGetPartialObject::ConstructL()
    {
    SetPSStatus();
    }

// -----------------------------------------------------------------------------
// CGetPartialObject::BuildPartialDataL()
// Populate the partial data object
// -----------------------------------------------------------------------------
//
void CGetPartialObject::BuildPartialDataL()
    {
    PRINT( _L( "MM MTP => CGetPartialObject::BuildPartialDataL" ) );
    __ASSERT_DEBUG( iRequestChecker, Panic( EMmMTPDpRequestCheckNull ) );

    if ( iBuffer )
        {
        delete iBuffer;
        iBuffer = NULL;
        }

    // We might fail if we have insufficient memory...
    iBuffer = HBufC8::NewL( iPartialDataLength );
    iBuffer->Des().Zero();
    iBufferPtr8.Set( iBuffer->Des() );

    RFile file;
    User::LeaveIfError( file.Open( iFs, iFileSuid, EFileRead ) );
    CleanupClosePushL( file ); // + file
    User::LeaveIfError( file.Read( iOffset, iBufferPtr8, iPartialDataLength ) );
    CleanupStack::PopAndDestroy( &file ); // - file
    PRINT( _L( "MM MTP <= CGetPartialObject::BuildPartialDataL" ) );
    }

// end of file
