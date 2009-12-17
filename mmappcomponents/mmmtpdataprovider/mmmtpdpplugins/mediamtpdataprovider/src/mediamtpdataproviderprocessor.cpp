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
* Description:  media mtp data provider processor handle
*
*/


#include <mtp/tmtptyperequest.h>

//Include the standard request processor headers
#include "crequestunknown.h"

#include "mediamtpdataproviderprocessor.h"

//Include the required header for carrying out the specified request
#include "mmmtpdplogger.h"

#include "cgetobjectpropssupported.h"
#include "cmediamtpdataprovidergetobjectpropdesc.h"
#include "cmediamtpdataprovidergetinterdependentpropdesc.h"
#include "cgetobject.h"
#include "cgetobjectinfo.h"
#include "cmediamtpdataprovidergetobjectpropvalue.h"
#include "cmediamtpdataprovidergetobjectproplist.h"
#include "cmediamtpdataprovidersendobject.h"
#include "cmediamtpdataprovidersetobjectpropvalue.h"
#include "cmediamtpdataprovidersetobjectproplist.h"
#include "cgetobjectreferences.h"
#include "csetobjectreferences.h"
#include "cdeleteobject.h"
#include "cmediamtpdataprovidercopyobject.h"
#include "cmediamtpdataprovidermoveobject.h"
#include "cgetpartialobject.h"
#include "cmediamtpdataprovidergetformatcapabilities.h"

/** A mapping table from the operation code to the request processor factory method */
const TMediaMtpDataProviderRequestProcessorEntry KKMediaMtpDataProviderRequestProcessorTable[] =
    {
    {EMTPOpCodeGetObjectPropsSupported, CGetObjectPropsSupported::NewL}, //Get Supported Props
    {EMTPOpCodeGetObjectPropDesc, CMediaMtpDataProviderGetObjectPropDesc::NewL}, //Get Props Description
    {EMTPOpCodeGetInterdependentPropDesc, CMediaMtpDataProviderGetInterDependentPropDesc::NewL},//Get Interdependent PropDesc
    {EMTPOpCodeGetObjectInfo, CGetObjectInfo::NewL}, //GetObjectInfo
    {EMTPOpCodeGetObject, CGetObject::NewL}, //GetObject
    {EMTPOpCodeGetObjectPropValue, CMediaMtpDataProviderGetObjectPropValue::NewL}, //Get Props Value
    {EMTPOpCodeGetObjectPropList, CMediaMtpDataProviderGetObjectPropList::NewL}, //GetObjectPropList
    {EMTPOpCodeSendObjectInfo, CMediaMtpDataProviderSendObject::NewL}, //SendObjectInfo (routed to SendObject)
    {EMTPOpCodeSendObject, CMediaMtpDataProviderSendObject::NewL}, //SendObject
    {EMTPOpCodeSendObjectPropList, CMediaMtpDataProviderSendObject::NewL}, // SendobjectPropList (routed to SendObject)
    {EMTPOpCodeSetObjectPropValue, CMediaMtpDataProviderSetObjectPropValue::NewL}, //Set Props Value
    {EMTPOpCodeSetObjectPropList, CMediaMtpDataProviderSetObjectPropList::NewL}, //SetObjectPropList
    {EMTPOpCodeDeleteObject, CDeleteObject::NewL}, //DeleteObject
    {EMTPOpCodeCopyObject, CMediaMtpDataProviderCopyObject::NewL}, // Copy Object
    {EMTPOpCodeMoveObject, CMediaMtpDataProviderMoveObject::NewL}, // Move Object
    {EMTPOpCodeGetPartialObject, CGetPartialObject::NewL}, // GetPartialObject
    {EMTPOpCodeSetObjectReferences, CSetObjectReferences::NewL}, //Set Object References
    {EMTPOpCodeGetObjectReferences, CGetObjectReferences::NewL},  //Get Object References
    {EMTPOpCodeGetFormatCapabilities, CMediaMtpDataProviderGetFormatCapabilities::NewL} // GetFormatCapabilities
    };

// -----------------------------------------------------------------------------
// MediaMtpDataProviderProcessor::CreateL
// Create a request processor that matches the request
// -----------------------------------------------------------------------------
//
MMmRequestProcessor* MediaMtpDataProviderProcessor::CreateL( MMTPDataProviderFramework& aFramework,
    const TMTPTypeRequest& aRequest,
    MMTPConnection& aConnection,
    MMmMtpDpConfig& aDpConfig )
    {
    TMediaMtpDataProviderRequestProcessorCreateFunc createFunc = NULL;
    TUint16 operationCode = aRequest.Uint16(
            TMTPTypeRequest::ERequestOperationCode );

    TInt count = sizeof ( KKMediaMtpDataProviderRequestProcessorTable )
        / sizeof( TMediaMtpDataProviderRequestProcessorEntry );

    for ( TInt i = 0; i < count; i++ )
        {
        if ( KKMediaMtpDataProviderRequestProcessorTable[i].iOperationCode
            == operationCode )
            {
            PRINT1( _L( "MM MTP <> MediaMtpDataProviderProcessor::CreateL, operation code = 0x%x" ),
                operationCode );
            createFunc = KKMediaMtpDataProviderRequestProcessorTable[i].iCreateFunc;
            break;
            }
        }

    if ( !createFunc )
        {
        PRINT( _L( "MM MTP <> MediaMtpDataProviderProcessor::CreateL, Can't find processor!!!" ) );
        return CRequestUnknown::NewL( aFramework, aConnection );
        }

    return ( *createFunc )( aFramework, aConnection, aDpConfig );
    }

// end of file
