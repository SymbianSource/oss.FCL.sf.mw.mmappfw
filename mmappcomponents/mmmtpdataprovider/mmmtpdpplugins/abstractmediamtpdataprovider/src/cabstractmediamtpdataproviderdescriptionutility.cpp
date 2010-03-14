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
* Description:  Media dp specific property description construction utility.
*              Used by all request processors which related to property description
*
*/

#include <mtp/cmtptypeobjectpropdesc.h>
#include <mtp/cmtptypestring.h>

#include "cabstractmediamtpdataproviderdescriptionutility.h"
#include "mmmtpdpconfig.h"
#include "mmmtpdplogger.h"

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProviderDescriptionUtility::NewL
//
// -----------------------------------------------------------------------------
//
CAbstractMediaMtpDataProviderDescriptionUtility* CAbstractMediaMtpDataProviderDescriptionUtility::NewL()
    {
    CAbstractMediaMtpDataProviderDescriptionUtility* self =
        new( ELeave ) CAbstractMediaMtpDataProviderDescriptionUtility();

    return self;
    }

CAbstractMediaMtpDataProviderDescriptionUtility::CAbstractMediaMtpDataProviderDescriptionUtility()
    {
    // Do nothing
    }

CAbstractMediaMtpDataProviderDescriptionUtility::~CAbstractMediaMtpDataProviderDescriptionUtility()
    {
    // Do nothing
    }

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProviderDescriptionUtility::NewInterdepentPropDescL
// Construct media dp specific interdependent property description.
// -----------------------------------------------------------------------------
//
CMTPTypeInterdependentPropDesc* CAbstractMediaMtpDataProviderDescriptionUtility::NewInterdepentPropDescL( MMmMtpDpConfig& /*aDpConfig*/,
    TUint /*aFormatCode*/ )
    {
    PRINT( _L( "MM MTP => CAbstractMediaMtpDataProviderDescriptionUtility::NewInterdepentPropDescL" ) );

    CMTPTypeInterdependentPropDesc* interdependentPropDesc = CMTPTypeInterdependentPropDesc::NewL();

    PRINT( _L( "MM MTP <= CAbstractMediaMtpDataProviderDescriptionUtility::NewInterdepentPropDescL" ) );
    return interdependentPropDesc;
    }

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProviderDescriptionUtility::NewSpecificPropDescL
// Construct media dp specific property description.
// -----------------------------------------------------------------------------
//
CMTPTypeObjectPropDesc* CAbstractMediaMtpDataProviderDescriptionUtility::NewSpecificPropDescL( TUint /*aFormatCode*/,
    TUint16 aPropCode )
    {
    CMTPTypeObjectPropDesc* propertyDesc = NULL;

    switch ( aPropCode )
        {
        // format specific
        // May need add implementation here for further extension.
        default:
            // Do nothing now.
            break;
        }
    return propertyDesc;
    }

// end of file
