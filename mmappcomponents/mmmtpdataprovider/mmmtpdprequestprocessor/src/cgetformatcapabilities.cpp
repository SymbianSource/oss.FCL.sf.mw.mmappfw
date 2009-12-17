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

#include <mtp/cmtptypeinterdependentpropdesc.h>
#include <mtp/cmtptypeobjectpropdesc.h>
#include <mtp/mtpprotocolconstants.h>
#include <mtp/mmtpdataproviderframework.h>
#include <mtp/cmtptypestring.h>


#include "cgetformatcapabilities.h"
#include "mmmtpdplogger.h"
#include "mmmtpdpconfig.h"
#include "tobjectdescription.h"

_LIT( KMtpObjDescObjFileName, "[a-zA-Z!#\\$%&'\\(\\)\\-0-9@\\^_\\`\\{\\}\\~][a-zA-Z!#\\$%&'\\(\\)\\-0-9@\\^_\\`\\{\\}\\~ ]{0, 7}\\.[[a-zA-Z!#\\$%&'\\(\\)\\-0-9@\\^_\\`\\{\\}\\~][a-zA-Z!#\\$%&'\\(\\)\\-0-9@\\^_\\`\\{\\}\\~ ]{0, 2}]?" );

// -----------------------------------------------------------------------------
// CGetFormatCapabilities::CGetFormatCapabilities
// Standard c++ constructor
// -----------------------------------------------------------------------------
//
EXPORT_C CGetFormatCapabilities::CGetFormatCapabilities( MMTPDataProviderFramework& aFramework,
        MMTPConnection& aConnection,
        MMmMtpDpConfig& aDpConfig ) :
    CRequestProcessor( aFramework, aConnection, 0, NULL ),
    iDpConfig( aDpConfig )
    {
    PRINT( _L( "Operation: CGetFormatCapabilities(0x930A)" ) );
    }

// -----------------------------------------------------------------------------
// CGetFormatCapabilities::ConstructL
// Second phase constructor
// -----------------------------------------------------------------------------
//
EXPORT_C void CGetFormatCapabilities::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CGetFormatCapabilities::~CGetFormatCapabilities
// Second phase constructor
// -----------------------------------------------------------------------------
//
EXPORT_C CGetFormatCapabilities::~CGetFormatCapabilities()
    {
    delete iCapabilityList;
    iCapabilityList = NULL;
    }

// -----------------------------------------------------------------------------
// CGetFormatCapabilities::CheckRequestL
// Ensures the object prop code operation parameter is valid
// -----------------------------------------------------------------------------
//
EXPORT_C TMTPResponseCode CGetFormatCapabilities::CheckRequestL()
    {
    PRINT( _L( "MM MTP => CGetFormatCapabilities::CheckRequestL" ) );
    TMTPResponseCode response = EMTPRespCodeOK;
    iFormatCode = Request().Uint32( TMTPTypeRequest::ERequestParameter1 );

    if ( iFormatCode != KMTPFormatsAll )
        {
        response = EMTPRespCodeInvalidObjectFormatCode;

        const RArray<TUint>* format = iDpConfig.GetSupportedFormat();
        TInt count = format->Count();

        for ( TInt i = 0; i < count; i++ )
            {
            if ( iFormatCode == (*format)[i] )
                {
                response = EMTPRespCodeOK;
                break;
                }
            }
        }

    PRINT1( _L( "MM MTP <= CGetFormatCapabilities::CheckRequestL, response = 0x%X" ), response );
    return response;
    }

// -----------------------------------------------------------------------------
// CGetFormatCapabilities::ServiceL
// GetFormatCapabilities request handler
// -----------------------------------------------------------------------------
//
EXPORT_C void CGetFormatCapabilities::ServiceL()
    {
    PRINT( _L( "MM MTP => CGetFormatCapabilities::ServiceL" ) );

    delete iCapabilityList;
    iCapabilityList = NULL;

    iCapabilityList = CMTPTypeFormatCapabilityList::NewL();
    iFormatCode = Request().Uint32( TMTPTypeRequest::ERequestParameter1 );

    if ( KMTPFormatsAll == iFormatCode )
        {
        const RArray<TUint>* formatArray = iDpConfig.GetSupportedFormat();
        TInt count = formatArray->Count();
        for( TInt i = 0; i < count; i++ )
            {
            PRINT( _L( "MM MTP <> CGetFormatCapabilities::ServiceL, KMTPFormatsAll!!!" ) );
            iFormatCode = ( *formatArray )[i];
            ServiceInterdepentPropDescL();
            CMTPTypeFormatCapability* frmCap = CMTPTypeFormatCapability::NewL( iFormatCode, iInterdependentPropDesc );
            const RArray<TUint>* propertiesArray = iDpConfig.GetSupportedPropertiesL( iFormatCode );
            TInt num = propertiesArray->Count();
            for( TInt j = 0;j < num; j++ )
                {
                CMTPTypeObjectPropDesc* propertyDesc = ServicePropDescL( (*propertiesArray)[j] );
                if ( propertyDesc != NULL )
                    {
                    frmCap->AppendL( propertyDesc );
                    }
                }//end for j
            iCapabilityList->AppendL( frmCap );
            }//end for i
        }//end if
    else
        {
        ServiceInterdepentPropDescL();
        PRINT1( _L( "MM MTP <> CGetFormatCapabilities::ServiceL, ONLY one Format!!! formatCode = 0x%X" ), iFormatCode );

        CMTPTypeFormatCapability* frmCap = CMTPTypeFormatCapability::NewL( iFormatCode, iInterdependentPropDesc );
        const RArray<TUint>* propertiesArray = iDpConfig.GetSupportedPropertiesL( iFormatCode );
        TInt num = propertiesArray->Count();
        for( TInt j = 0;j < num; j++ )
            {
            CMTPTypeObjectPropDesc* propertyDesc = ServicePropDescL( ( *propertiesArray )[j] );
            if ( propertyDesc != NULL )
                {
                frmCap->AppendL( propertyDesc );
                }
            }//end for j
        iCapabilityList->AppendL( frmCap );
        }

    SendDataL( *iCapabilityList );
    PRINT( _L( "MM MTP <= CGetFormatCapabilities::ServiceL" ) );
    }

// -----------------------------------------------------------------------------
// CGetFormatCapabilities::ServicePropDescL
// Get MTPTypeObjectPropDesc according to property code
// -----------------------------------------------------------------------------
//
CMTPTypeObjectPropDesc* CGetFormatCapabilities::ServicePropDescL( TUint16 aPropCode )
    {
    PRINT1( _L( "MM MTP => CGetFormatCapabilities::ServicePropDescL, aPropCode = 0x%X" ), aPropCode );
    // implement just like what we did in CGetpropertyDescc::ServiceL
    CMTPTypeObjectPropDesc* propertyDesc;
    CMTPTypeObjectPropDesc::TPropertyInfo propertyInfo;

    /* Create new PropDesc object to return to device
     m - signals only limited supported values - hence these requests require the expected form
     variable to be passed into the NewL contstuctor as well */
    switch( aPropCode )
        {
        case EMTPObjectPropCodeStorageID: // Storage ID
        case EMTPObjectPropCodeObjectFormat: // Format Code
        case EMTPObjectPropCodeObjectSize: // Object Size
        case EMTPObjectPropCodeParentObject: // Parent Object
        case EMTPObjectPropCodePersistentUniqueObjectIdentifier: // Unique Object Identifier
        case EMTPObjectPropCodeName: // Name
        case EMTPObjectPropCodeDateAdded: // Date Added
            propertyDesc = CMTPTypeObjectPropDesc::NewL( aPropCode );
            break;

            // Protection Status (m)
        case EMTPObjectPropCodeProtectionStatus:
            propertyDesc = ServiceProtectionStatusL();
            break;

            // FileName
        case EMTPObjectPropCodeObjectFileName:
            propertyDesc = ServiceFileNameL();
            break;

            // Consumable (m)
        case EMTPObjectPropCodeNonConsumable:
            propertyDesc = ServiceNonConsumableL();
            break;

        case EMTPObjectPropCodeDateModified: // Date Modified
        case EMTPObjectPropCodeDateCreated: // Date Created
            propertyInfo.iDataType     = EMTPTypeString;
            propertyInfo.iFormFlag     = CMTPTypeObjectPropDesc::EDateTimeForm;
            propertyInfo.iGetSet       = CMTPTypeObjectPropDesc::EReadOnly;
            propertyDesc = CMTPTypeObjectPropDesc::NewL( aPropCode, propertyInfo, NULL);
            break;

        default:
            propertyDesc = ServiceSpecificPropertyDescL( aPropCode );
            break;
        }
    if ( propertyDesc != NULL )
        {
        TUint32 groupCode = GetGroupCode( aPropCode );
        propertyDesc->SetUint32L( CMTPTypeObjectPropDesc::EGroupCode, groupCode );
        }

    PRINT( _L( "MM MTP <= CGetFormatCapabilities::ServicePropDescL" ) );

    return propertyDesc;
    }

// -----------------------------------------------------------------------------
// CGetFormatCapabilities::GetGroupCode
// Get MTPTypeObjectPropDesc according to property code
// -----------------------------------------------------------------------------
//
TUint32 CGetFormatCapabilities::GetGroupCode( TUint32 aPropCode )
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

// -----------------------------------------------------------------------------
// CGetFormatCapabilities::ServiceProtectionStatusL
// Get MTPTypeObjectPropDesc protection status
// -----------------------------------------------------------------------------
//
CMTPTypeObjectPropDesc* CGetFormatCapabilities::ServiceProtectionStatusL()
    {
    PRINT( _L( "MM MTP => CGetFormatCapabilities::ServiceProtectionStatusL" ) );

    CMTPTypeObjectPropDescEnumerationForm* expectedForm =
        CMTPTypeObjectPropDescEnumerationForm::NewLC( EMTPTypeUINT16 ); // + expectedForm

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

    CMTPTypeObjectPropDesc* propertyDesc  = CMTPTypeObjectPropDesc::NewL( EMTPObjectPropCodeProtectionStatus, *expectedForm );
    CleanupStack::PopAndDestroy( expectedForm ); // - expectedForm

    PRINT( _L( "MM MTP <= CGetFormatCapabilities::ServiceProtectionStatusL" ) );
    return propertyDesc;
    }

// -----------------------------------------------------------------------------
// CGetFormatCapabilities::ServiceFileNameL
// Get MTPTypeObjectPropDesc service file name
// -----------------------------------------------------------------------------
//
CMTPTypeObjectPropDesc* CGetFormatCapabilities::ServiceFileNameL()
    {
    PRINT( _L( "MM MTP => CGetFormatCapabilities::ServiceFileNameL" ) );
    CMTPTypeString* form = CMTPTypeString::NewLC( KMtpObjDescObjFileName ); // + form

    CMTPTypeObjectPropDesc* ret = CMTPTypeObjectPropDesc::NewL( EMTPObjectPropCodeObjectFileName,
        CMTPTypeObjectPropDesc::ERegularExpressionForm,
        form );

    CleanupStack::PopAndDestroy( form ); // - form
    PRINT( _L( "MM MTP <= CGetFormatCapabilities::ServiceFileNameL" ) );
    return ret;
    }

// -----------------------------------------------------------------------------
// CGetFormatCapabilities::ServiceFileNameL
// Get MTPTypeObjectPropDesc service file name
// -----------------------------------------------------------------------------
//
CMTPTypeObjectPropDesc* CGetFormatCapabilities::ServiceNonConsumableL()
    {
    PRINT( _L( "MM MTP => CGetFormatCapabilities::ServiceNonConsumableL" ) );
    CMTPTypeObjectPropDescEnumerationForm* expectedForm =
        CMTPTypeObjectPropDescEnumerationForm::NewLC( EMTPTypeUINT8 ); // + expectedForm
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

    CMTPTypeObjectPropDesc::TPropertyInfo propertyInfo;
    propertyInfo.iDataType = EMTPTypeUINT8;
    propertyInfo.iFormFlag = CMTPTypeObjectPropDesc::EEnumerationForm;
    propertyInfo.iGetSet = CMTPTypeObjectPropDesc::EReadOnly;
    CMTPTypeObjectPropDesc* ret = CMTPTypeObjectPropDesc::NewL( EMTPObjectPropCodeNonConsumable,
        propertyInfo,
        expectedForm );
    CleanupStack::PopAndDestroy( expectedForm ); // - expectedForm

    PRINT( _L( "MM MTP <= CGetFormatCapabilities::ServiceNonConsumableL" ) );
    return ret;
    }

// end of file
