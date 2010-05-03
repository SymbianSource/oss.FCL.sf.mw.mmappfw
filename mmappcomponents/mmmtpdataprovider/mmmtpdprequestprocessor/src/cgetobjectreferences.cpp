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
* Description:  Implement the operation: GetObjectReferences
*
*/


#include <mtp/cmtptypearray.h>
#include <mtp/mmtpreferencemgr.h>
#ifdef _DEBUG
#include <mtp/mmtpobjectmgr.h>
#include <mtp/tmtptypeuint128.h>
#endif

#include "cgetobjectreferences.h"
#include "mmmtpdplogger.h"

// -----------------------------------------------------------------------------
// Verification data for the GetReferences request
// -----------------------------------------------------------------------------
//
const TMTPRequestElementInfo KMTPGetObjectReferencesPolicy[] =
    {
        {
        TMTPTypeRequest::ERequestParameter1,
        EMTPElementTypeObjectHandle,
        EMTPElementAttrNone,
        0,
        0,
        0
        }
    };

// -----------------------------------------------------------------------------
// CGetObjectReferences::NewL
// Two-phase construction method
// -----------------------------------------------------------------------------
//
EXPORT_C MMmRequestProcessor* CGetObjectReferences::NewL( MMTPDataProviderFramework& aFramework,
    MMTPConnection& aConnection,
    MMmMtpDpConfig& /*aDpConfig*/ )
    {
    CGetObjectReferences* self = new ( ELeave ) CGetObjectReferences( aFramework,
        aConnection );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CGetObjectReferences::ConstructL
// Two-phase construction method
// -----------------------------------------------------------------------------
//
void CGetObjectReferences::ConstructL()
    {
    SetPSStatus();
    }

// -----------------------------------------------------------------------------
// CGetObjectReferences::~CGetObjectReferences
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CGetObjectReferences::~CGetObjectReferences()
    {
    delete iReferences;
    }

// -----------------------------------------------------------------------------
// CGetObjectReferences::CGetObjectReferences
// Standard c++ constructor
// -----------------------------------------------------------------------------
//
CGetObjectReferences::CGetObjectReferences( MMTPDataProviderFramework& aFramework,
    MMTPConnection& aConnection ) :
    CRequestProcessor( aFramework,
        aConnection,
        sizeof ( KMTPGetObjectReferencesPolicy ) / sizeof( TMTPRequestElementInfo ),
        KMTPGetObjectReferencesPolicy )
    {
    PRINT( _L( "Operation: GetObjectReferences(0x9810)" ) );
    }

// -----------------------------------------------------------------------------
// CGetObjectReferences::ServiceL
// GetObjectInfo request handler
// -----------------------------------------------------------------------------
//
void CGetObjectReferences::ServiceL()
    {
    PRINT( _L( "MM MTP => CGetObjectReferences::ServiceL" ) );

    TUint32 objectHandle = Request().Uint32( TMTPTypeRequest::ERequestParameter1 );
    PRINT1( _L( "MM MTP <> CGetObjectReferences::ServiceL objectHandle = 0x%x" ),
            objectHandle );
    MMTPReferenceMgr& referenceMgr = iFramework.ReferenceMgr();
    delete iReferences;
    iReferences = NULL;
    iReferences = referenceMgr.ReferencesLC( TMTPTypeUint32( objectHandle ) );
    CleanupStack::Pop( iReferences );
    SendDataL( *iReferences );

#if _DEBUG
    TUint32 handle = 0;
    TUint count = iReferences->NumElements();
    PRINT1( _L( "MM MTP <> CGetObjectReferences::ServiceL count = %d" ), count );
    CMTPObjectMetaData* object = NULL;
    for( TUint i = 0; i < count; i++ )
        {
        object = CMTPObjectMetaData::NewLC(); // + object
        handle = iReferences->ElementUint( i );
        PRINT1( _L( "MM MTP <> CGetObjectReferences::ServiceL handle = 0x%x" ), handle );
        iFramework.ObjectMgr().ObjectL( handle, *object);
        PRINT1( _L( "MM MTP <> CGetObjectReferences::ServiceL object file name is %S" ), &object->DesC( CMTPObjectMetaData::ESuid ) );
        CleanupStack::PopAndDestroy( object ); // - object
        TMTPTypeUint128 puoid;
        puoid.Set( 0, 0 );
        puoid = iFramework.ObjectMgr().PuidL( handle );
        PRINT1( _L( "MM MTP <> CGetObjectReferences::ServiceL higher value of puoid = 0x%04Lx" ), puoid.UpperValue() );
        PRINT1( _L( "MM MTP <> CGetObjectReferences::ServiceL lower value of puoid = 0x%04Lx" ), puoid.LowerValue() );
        }
#endif

    PRINT( _L( "MM MTP <= CGetObjectReferences::ServiceL" ) );
    }

// end of file
