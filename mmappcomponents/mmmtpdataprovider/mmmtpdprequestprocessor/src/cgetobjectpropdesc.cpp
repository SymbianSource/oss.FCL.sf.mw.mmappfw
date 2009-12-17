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
* Description:  Implement operation: GetObjectPropDesc
*
*/


#include <mtp/cmtptypeobjectpropdesc.h>
#include <mtp/cmtptypestring.h>

#include "cgetobjectpropdesc.h"
#include "tmmmtpdppanic.h"
#include "mmmtpdplogger.h"
#include "tobjectdescription.h"
#include "mmmtpdpconfig.h"

_LIT( KMtpObjDescObjFileName, "[a-zA-Z!#\\$%&'\\(\\)\\-0-9@\\^_\\`\\{\\}\\~][a-zA-Z!#\\$%&'\\(\\)\\-0-9@\\^_\\`\\{\\}\\~ ]{0, 7}\\.[[a-zA-Z!#\\$%&'\\(\\)\\-0-9@\\^_\\`\\{\\}\\~][a-zA-Z!#\\$%&'\\(\\)\\-0-9@\\^_\\`\\{\\}\\~ ]{0, 2}]?" );
// RegEx is [a-zA-Z!#\$%&'\(\)\-0-9@\^_\`\{\}\~][a-zA-Z!#\$%&'\(\)\-0-9@\^_\`\{\}\~ ]{0, 7}\.[[a-zA-Z!#\$%&'\(\)\-0-9@\^_\`\{\}\~][a-zA-Z!#\$%&'\(\)\-0-9@\^_\`\{\}\~ ]{0, 2}]?

// -----------------------------------------------------------------------------
// CGetObjectPropDesc::~CGetObjectPropDesc
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CGetObjectPropDesc::~CGetObjectPropDesc()
    {
    delete iObjectProperty;
    }

// -----------------------------------------------------------------------------
// CGetObjectPropDesc::CGetObjectPropDesc
// Standard c++ constructor
// -----------------------------------------------------------------------------
//
EXPORT_C CGetObjectPropDesc::CGetObjectPropDesc( MMTPDataProviderFramework& aFramework,
    MMTPConnection& aConnection,
    MMmMtpDpConfig& aDpConfig ) :
    CRequestProcessor( aFramework, aConnection, 0, NULL ),
    iObjectProperty( NULL ),
    iDpConfig( aDpConfig )
    {
    PRINT( _L( "Operation: GetObjectPropDesc(0x9802)" ) );
    }

// -----------------------------------------------------------------------------
// CGetObjectPropDesc::ConstructL
// Second phase constructor
// -----------------------------------------------------------------------------
//
EXPORT_C void CGetObjectPropDesc::ConstructL()
    {

    }

// -----------------------------------------------------------------------------
// CGetObjectPropList::CheckRequestL
// Verify the reqeust and returns it
// -----------------------------------------------------------------------------
//
EXPORT_C TMTPResponseCode CGetObjectPropDesc::CheckRequestL()
    {
    TMTPResponseCode response = CRequestProcessor::CheckRequestL();

    if ( response == EMTPRespCodeOK )
        {
        response = CheckFormatL();
        }

    if ( response == EMTPRespCodeOK )
        {
        response = CheckPropCodeL();
        }

    PRINT1( _L("MM MTP <= CGetObjectPropDesc::CheckRequestL response = 0x%x"), response );
    return response;
    }

// -----------------------------------------------------------------------------
// CGetObjectPropDesc::ServiceL
// GetObjectPropDesc request handler
// -----------------------------------------------------------------------------
//
EXPORT_C void CGetObjectPropDesc::ServiceL()
    {
    PRINT( _L( "MM MTP => CGetObjectPropDesc::ServiceL" ) );
    delete iObjectProperty;
    iObjectProperty = NULL;

    TUint32 propCode = Request().Uint32( TMTPTypeRequest::ERequestParameter1 );
    iFormatCode = Request().Uint32( TMTPTypeRequest::ERequestParameter2 );
    PRINT2( _L( "MM MTP <> CGetObjectPropDesc::ServiceL propCode = 0x%x, iFormatCode = 0x%x" ),
            propCode,
            iFormatCode );

    CMTPTypeObjectPropDesc::TPropertyInfo propertyInfo;

    /* Create new PropDesc object to return to device
     m - signals only limited supported values - hence these requests require the expected form
     variable to be passed into the NewL contstuctor as well */
    switch ( propCode )
        {
        case EMTPObjectPropCodeStorageID: // Storage ID
        case EMTPObjectPropCodeObjectFormat: // Format Code
        case EMTPObjectPropCodeObjectSize: // Object Size
        case EMTPObjectPropCodeParentObject: // Parent Object
        case EMTPObjectPropCodePersistentUniqueObjectIdentifier: // Unique Object Identifier
        case EMTPObjectPropCodeName: // Name
        case EMTPObjectPropCodeDateAdded: // Date Added
            iObjectProperty = CMTPTypeObjectPropDesc::NewL( propCode );
            break;

            // Protection Status (m)
        case EMTPObjectPropCodeProtectionStatus:
            ServiceProtectionStatusL();
            break;

            // FileName
        case EMTPObjectPropCodeObjectFileName:
            ServiceFileNameL();
            break;

            // Consumable (m)
        case EMTPObjectPropCodeNonConsumable:
            ServiceNonConsumableL();
            break;

        case EMTPObjectPropCodeDateModified: // Date Modified
        case EMTPObjectPropCodeDateCreated: // Date Created
            propertyInfo.iDataType     = EMTPTypeString;
            propertyInfo.iFormFlag     = CMTPTypeObjectPropDesc::EDateTimeForm;
            propertyInfo.iGetSet       = CMTPTypeObjectPropDesc::EReadOnly;
            iObjectProperty = CMTPTypeObjectPropDesc::NewL( propCode, propertyInfo, NULL);
            break;

            // Error - Should be caught by CheckRequestL
        default:
            ServiceSpecificObjectPropertyL( propCode );
            break;
        }

    // Set group code
    TUint32 groupCode = GetGroupCode( propCode );
    PRINT1( _L("MM MTP <> CGetObjectPropDesc::ServiceL, groupCode = 0x%x"), groupCode );
    iObjectProperty->SetUint32L( CMTPTypeObjectPropDesc::EGroupCode, groupCode );

    __ASSERT_DEBUG( iObjectProperty, Panic( EMmMTPDpObjectPropertyNull ) );

    SendDataL( *iObjectProperty );

    PRINT( _L( "MM MTP <= CGetObjectPropDesc::ServiceL" ) );
    }

// -----------------------------------------------------------------------------
// CGetObjectPropDesc::ServiceProtectionStatusL
// Create list of possible protection status and create new ObjectPropDesc
// -----------------------------------------------------------------------------
//
void CGetObjectPropDesc::ServiceProtectionStatusL()
    {
    CMTPTypeObjectPropDescEnumerationForm* expectedForm =
        CMTPTypeObjectPropDescEnumerationForm::NewL( EMTPTypeUINT16 );
    CleanupStack::PushL( expectedForm ); // + expectedForm

    TUint16 values[] =
        {
        EMTPProtectionNoProtection,
        EMTPProtectionReadOnly
        };

    TInt numValues = sizeof ( values ) / sizeof ( values[0] ) ;
    for ( TInt i = 0; i < numValues; i++ )
        {
        TMTPTypeUint16 data( values[i] );
        expectedForm->AppendSupportedValueL( data );
        }

    // Althrough iObjectProperty is released in ServiceL(),
    // release it here maybe a more safer way :)
    if ( iObjectProperty != NULL )
        {
        delete iObjectProperty;
        iObjectProperty = NULL;
        }

    iObjectProperty = CMTPTypeObjectPropDesc::NewL( EMTPObjectPropCodeProtectionStatus, *expectedForm );
    CleanupStack::PopAndDestroy( expectedForm ); // - expectedForm
    }

// -----------------------------------------------------------------------------
// CGetObjectPropDesc::ServiceFileNameL()
// Create Regular expression for a file name and create new ObjectPropDesc
// -----------------------------------------------------------------------------
//
void CGetObjectPropDesc::ServiceFileNameL()
    {
    CMTPTypeString* form = CMTPTypeString::NewLC( KMtpObjDescObjFileName ); // + form

    // Althrough iObjectProperty is released in ServiceL(),
    // release it here maybe a more safer way
    if ( iObjectProperty != NULL )
        {
        delete iObjectProperty;
        iObjectProperty = NULL;
        }

    iObjectProperty = CMTPTypeObjectPropDesc::NewL( EMTPObjectPropCodeObjectFileName,
        CMTPTypeObjectPropDesc::ERegularExpressionForm,
        form );

    CleanupStack::PopAndDestroy( form ); // - form
    }

// -----------------------------------------------------------------------------
// CGetObjectPropDesc::ServiceNonConsumableL
// Create list of possible nonConsumable values and create new ObjectPropDesc
// -----------------------------------------------------------------------------
//
void CGetObjectPropDesc::ServiceNonConsumableL()
    {
    CMTPTypeObjectPropDescEnumerationForm* expectedForm =
        CMTPTypeObjectPropDescEnumerationForm::NewL( EMTPTypeUINT8 );
    CleanupStack::PushL( expectedForm ); // + expectedForm
    TUint8 values[] =
        {
        EMTPConsumable,
        EMTPNonConsumable
        };

    TInt numValues = sizeof ( values ) / sizeof ( values[0] );
    for ( TInt i = 0; i < numValues; i++ )
        {
        TMTPTypeUint8 data( values[i] );
        expectedForm->AppendSupportedValueL( data );
        }

    // Althrough iObjectProperty is released in ServiceL(),
    // release it here maybe a more safer way :)
    if ( iObjectProperty != NULL )
        {
        delete iObjectProperty;
        iObjectProperty = NULL;
        }

    CMTPTypeObjectPropDesc::TPropertyInfo propertyInfo;
    propertyInfo.iDataType = EMTPTypeUINT8;
    propertyInfo.iFormFlag = CMTPTypeObjectPropDesc::EEnumerationForm;
    propertyInfo.iGetSet = CMTPTypeObjectPropDesc::EReadOnly;
    iObjectProperty = CMTPTypeObjectPropDesc::NewL( EMTPObjectPropCodeNonConsumable,
        propertyInfo,
        expectedForm );
    CleanupStack::PopAndDestroy( expectedForm ); // - expectedForm
    }

// -----------------------------------------------------------------------------
// CGetObjectPropList::CheckFormatL
// Ensures the object format operation parameter is valid
// -----------------------------------------------------------------------------
//
TMTPResponseCode CGetObjectPropDesc::CheckFormatL() const
    {
    TMTPResponseCode response = EMTPRespCodeOK;

    TUint32 formatCode = Request().Uint32( TMTPTypeRequest::ERequestParameter2 );
    PRINT1( _L( "MM MTP <> CGetObjectPropDesc::CheckFormatL formatCode = 0x%x" ), formatCode );

    if( formatCode != 0)
        {
        response = EMTPRespCodeInvalidObjectFormatCode;

        const RArray<TUint>* format = iDpConfig.GetSupportedFormat();
        TInt count = format->Count();

        for ( TInt i = 0; i < count; i++ )
            {
            if ( formatCode == (*format)[i] )
                {
                response = EMTPRespCodeOK;
                break;
                }
            }
        }

    return response;
    }

// -----------------------------------------------------------------------------
// CGetObjectPropDesc::CheckPropCodeL
// Ensures the object prop code operation parameter is valid
// -----------------------------------------------------------------------------
//
TMTPResponseCode CGetObjectPropDesc::CheckPropCodeL() const
    {
    TMTPResponseCode response = EMTPRespCodeOK;

    TUint32 propCode = Request().Uint32( TMTPTypeRequest::ERequestParameter1 );
    PRINT1( _L( "MM MTP <> CGetObjectPropDesc::CheckPropCode, propCode = 0x%x" ), propCode );

    if ( propCode == 0 )
        {
        // A propCode of 0 means specification by group (which is not supported)
        response = EMTPRespCodeSpecificationByGroupUnsupported;
        }
    else
        {
        TUint32 formatCode  = Request().Uint32( TMTPTypeRequest::ERequestParameter2 );

        const RArray<TUint>* properties = NULL;
        if ( formatCode == KMTPFormatsAll )
            properties= iDpConfig.GetAllSupportedProperties();
        else
            properties = iDpConfig.GetSupportedPropertiesL( formatCode );

        const TInt count = properties->Count();
        TInt i = 0;
        for( i = 0; i < count; i++ )
            {
            if ( (*properties)[i] == propCode )
                {
                break;
                }
            }

        if ( i == count )
            {
            response = EMTPRespCodeInvalidObjectPropCode;
            }
        }

    return response;
    }

TUint32 CGetObjectPropDesc::GetGroupCode( TUint16 aPropCode )
    {
    TInt count = sizeof( KPropGroupMapTable );
    // TODO: if need to refine the search approach to improve performance
    for( TInt i = 0; i < count; i++ )
        {
        if ( aPropCode == KPropGroupMapTable[i].iPropCode )
            return KPropGroupMapTable[i].iGroupCode;
        }
    return EGroupCodeNotDefined;
    }

// end of file
