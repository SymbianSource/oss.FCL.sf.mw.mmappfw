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
* Description: 
*
*/


#include <ecom/implementationproxy.h>

#include "cabstractmediamtpdataprovider.h"

// -----------------------------------------------------------------------------
// Define the interface UIDs
// -----------------------------------------------------------------------------
//
static const TImplementationProxy ImplementationTable[] =
    {
    { { 0x10207C53 }, ( TProxyNewLPtr ) ( CAbstractMediaMtpDataProvider::NewL ) }
    };

// -----------------------------------------------------------------------------
//  ECOM entry point
// -----------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    aTableCount = sizeof( ImplementationTable ) / sizeof( TImplementationProxy );
    return ImplementationTable;
    }

// end of file
