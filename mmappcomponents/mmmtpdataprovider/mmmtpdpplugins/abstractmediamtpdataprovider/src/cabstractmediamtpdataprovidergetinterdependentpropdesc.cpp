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
* Description:  Implement the operation: GetInterdenpendentPropDesc
*
*/


#include <mtp/tmtptyperequest.h>
#include <mtp/mtpdatatypeconstants.h>
#include <mtp/mmtpdataproviderframework.h>
#include <mtp/mmtpobjectmgr.h>
#include <mtp/cmtpobjectmetadata.h>
#include <mtp/cmtptypestring.h>
#include <mtp/cmtptypeobjectpropdesc.h>
#include <mtp/cmtptypeinterdependentpropdesc.h>

#include "cabstractmediamtpdataprovidergetinterdependentpropdesc.h"
#include "cmmmtpdpmetadataaccesswrapper.h"
#include "tmmmtpdppanic.h"
#include "abstractmediamtpdataproviderconst.h"
#include "mmmtpdplogger.h"
#include "mmmtpdpconfig.h"

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProviderGetInterdependentPropDesc::NewL
// Two-phase construction method
// -----------------------------------------------------------------------------
//
MMmRequestProcessor* CAbstractMediaMtpDataProviderGetInterdependentPropDesc::NewL( MMTPDataProviderFramework& aFramework,
    MMTPConnection& aConnection,
    MMmMtpDpConfig& /*aDpConfig*/ )
    {
    CAbstractMediaMtpDataProviderGetInterdependentPropDesc* self =
        new ( ELeave ) CAbstractMediaMtpDataProviderGetInterdependentPropDesc( aFramework,
            aConnection );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProviderGetInterdependentPropDesc::ConstructL
// Two-phase construction method
// -----------------------------------------------------------------------------
//
void CAbstractMediaMtpDataProviderGetInterdependentPropDesc::ConstructL()
    {

    }

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProviderGetInterdependentPropDesc::~CAbstractMediaMtpDataProviderGetInterdependentPropDesc
// Destructor
// -----------------------------------------------------------------------------
//
CAbstractMediaMtpDataProviderGetInterdependentPropDesc::~CAbstractMediaMtpDataProviderGetInterdependentPropDesc()
    {
    delete iDataset;
    }

// -----------------------------------------------------------------------------
// CMTPGetInterdependentPropDesc::CMTPGetInterdependentPropDesc
// Standard c++ constructor
// -----------------------------------------------------------------------------
//
CAbstractMediaMtpDataProviderGetInterdependentPropDesc::CAbstractMediaMtpDataProviderGetInterdependentPropDesc( MMTPDataProviderFramework& aFramework,
    MMTPConnection& aConnection ) :
    CRequestProcessor( aFramework, aConnection, 0, NULL )
    {
    PRINT( _L( "Operation: GetInterdependentPropDesc(0x9807)" ) );
    }

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProviderGetInterdependentPropDesc::CheckRequestL
// Verify the reqeust and returns it
// -----------------------------------------------------------------------------
//
TMTPResponseCode CAbstractMediaMtpDataProviderGetInterdependentPropDesc::CheckRequestL()
    {
    TMTPResponseCode responseCode = CRequestProcessor::CheckRequestL();
    iFormatCode = Request().Uint32( TMTPTypeRequest::ERequestParameter1 );

    if ( EMTPRespCodeOK == responseCode )
        {
        TInt count = sizeof( KAbstractMediaMtpDataProviderSupportedFormats ) / sizeof( TUint16 );

        responseCode = EMTPRespCodeInvalidObjectFormatCode;

        for ( TInt i = 0; i < count; i++ )
            {
            if ( iFormatCode == KAbstractMediaMtpDataProviderSupportedFormats[i] )
                {
                responseCode = EMTPRespCodeOK;
                break;
                }
            }
        }

    return responseCode;
    }

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProviderGetInterdependentPropDesc::ServiceL
// service a request at request phase
// -----------------------------------------------------------------------------
//
void CAbstractMediaMtpDataProviderGetInterdependentPropDesc::ServiceL()
    {
    PRINT( _L( "MM MTP => CAbstractMediaMtpDataProviderGetInterdependentPropDesc::ServiceL" ) );

    // Clear the data set.
    delete iDataset;
    iDataset = NULL;
    iDataset = CMTPTypeInterdependentPropDesc::NewL();

    PRINT( _L( "MM MTP <> CAbstractMediaMtpDataProviderGetInterdependentPropDesc::ServiceL don't have interdependent properties!" ) );

    // Send the dataset.
    SendDataL( *iDataset );
    PRINT( _L( "MM MTP <= CAbstractMediaMtpDataProviderGetInterdependentPropDesc::ServiceL" ) );
    }

// end of file
