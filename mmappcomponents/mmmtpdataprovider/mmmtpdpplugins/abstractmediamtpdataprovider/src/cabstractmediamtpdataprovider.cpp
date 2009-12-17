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
* Description:  AbstractMedia data provider
*
*/


#include <mtp/mmtpconnection.h>
#include <mtp/mtpprotocolconstants.h>
#include <mtp/mmtpdataproviderframework.h>
#include <mtp/mmtpstoragemgr.h>
#include <mtp/tmtptyperequest.h>
#include <mtp/cmtpobjectmetadata.h>
#include <mtp/mmtpobjectmgr.h>
#include <barsc.h>
#include <barsread.h>
#include <driveinfo.h>

#include "cabstractmediamtpdataprovider.h"
#include "crequestprocessor.h"
#include "abstractmediamtpdataproviderconst.h"
#include "tmmmtpdppanic.h"
#include "cabstractmediamtpdataproviderenumerator.h"
#include "mmmtpdplogger.h"
#include "cmmmtpdpaccesssingleton.h"
#include "mmmtpdputility.h"
#include "cabstractmediamtpdataproviderrenameobject.h"
#include "mmmtpdpdefs.h"
#include "mmmtpdpfiledefs.h"

// Class constants.
// Defines the number of MTP Active Processors allowed
static const TInt KMTPAbstractMediaDpSessionGranularity = 3;
static const TInt KAbstractMediaMtpDpArrayGranularity = 2;

static const TInt KActiveEnumeration = 0;

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProvider::NewL
// Two Phase Construction
// -----------------------------------------------------------------------------
//
TAny* CAbstractMediaMtpDataProvider::NewL( TAny* aParams )
    {
    CAbstractMediaMtpDataProvider* self = new ( ELeave ) CAbstractMediaMtpDataProvider( aParams );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProvider::CAbstractMediaMtpDataProvider
// Standard C++ constructor
// -----------------------------------------------------------------------------
//
CAbstractMediaMtpDataProvider::CAbstractMediaMtpDataProvider( TAny* aParams ) :
    CMTPDataProviderPlugin( aParams ),
    iActiveProcessors( KMTPAbstractMediaDpSessionGranularity ),
    iAbstractMediaEnumerator( NULL ),
    iPendingEnumerations ( KAbstractMediaMtpDpArrayGranularity ),
    iActiveProcessor( -1 ),
    iRenameObject( NULL ),
    iSupportedFormat( KAbstractMediaMtpDpArrayGranularity ),
    iSupportedProperties( KAbstractMediaMtpDpArrayGranularity )
    {
    }

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProvider::~CAbstractMediaMtpDataProvider
// Destructor
// -----------------------------------------------------------------------------
//
CAbstractMediaMtpDataProvider::~CAbstractMediaMtpDataProvider()
    {
    PRINT( _L( "MM MTP => CAbstractMediaMtpDataProvider::~CAbstractMediaMtpDataProvider" ) );

    iPendingEnumerations.Close();
    TInt count = iActiveProcessors.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        iActiveProcessors[i]->Release();
        }
    iActiveProcessors.Close();

    delete iAbstractMediaEnumerator;
    CMmMtpDpAccessSingleton::Release();

    if ( iRenameObject )
        delete iRenameObject;

    iSupportedFormat.Close();
    iSupportedProperties.Close();

    PRINT( _L( "MM MTP => CAbstractMediaMtpDataProvider::~CAbstractMediaMtpDataProvider" ) );
    }

void CAbstractMediaMtpDataProvider::Cancel()
    {
    iAbstractMediaEnumerator->Cancel();
    }

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProvider::ConstructL
// Second-phase construction
// -----------------------------------------------------------------------------
//
void CAbstractMediaMtpDataProvider::ConstructL()
    {
    PRINT( _L( "MM MTP => CAbstractMediaMtpDataProvider::ConstructL" ) );

    iAbstractMediaEnumerator = CAbstractMediaMtpDataProviderEnumerator::NewL( Framework(), *this );

    CMmMtpDpAccessSingleton::CreateL( Framework().Fs(), Framework() );

    GetSupportedFormatL();
    GetSupportedPropertiesL();

    PRINT( _L( "MM MTP <= CAbstractMediaMtpDataProvider::ConstructL" ) );
    }

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProvider::ProcessEventL
// Process event from initiator
// -----------------------------------------------------------------------------
//
void CAbstractMediaMtpDataProvider::ProcessEventL( const TMTPTypeEvent& aEvent,
    MMTPConnection& aConnection )
    {
    TInt index = LocateRequestProcessorL( aEvent, aConnection );
    if ( index != KErrNotFound )
        {
        iActiveProcessors[index]->HandleEventL( aEvent );
        }
    }

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProvider::ProcessNotificationL
// Process notification from initiator
// -----------------------------------------------------------------------------
//
void CAbstractMediaMtpDataProvider::ProcessNotificationL( TMTPNotification aNotification,
    const TAny* aParams )
    {
    switch ( aNotification )
        {
        case EMTPSessionClosed:
            PRINT( _L( "MM MTP <> CAbstractMediaMtpDataProvider::ProcessNotificationL EMTPSessionClosed event recvd" ) );

            SessionClosedL( *reinterpret_cast<const TMTPNotificationParamsSessionChange*> ( aParams ) );
            break;

        case EMTPSessionOpened:
            PRINT( _L( "MM MTP <> CAbstractMediaMtpDataProvider::ProcessNotificationL EMTPSessionOpened event recvd" ) );

            SessionOpenedL( *reinterpret_cast<const TMTPNotificationParamsSessionChange*> ( aParams ) );
            break;

        case EMTPStorageAdded:
            break;

        case EMTPStorageRemoved:
            break;

        case EMTPRenameObject:
            PRINT( _L( "MM MTP <> CAbstractMediaMtpDataProvider::ProcessNotificationL EMTPRenameObject event recvd" ) );
            RenameObjectL( *reinterpret_cast<const TMTPNotificationParamsHandle*> ( aParams ) );
            break;

        default:
            PRINT( _L( "MM MTP <> CAbstractMediaMtpDataProvider::ProcessNotificationL Ignore all other notifications" ) );
            // Ignore all other notifications.
            break;
        }
    }

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProvider::ProcessRequestPhaseL
// Process the request from initiator
// -----------------------------------------------------------------------------
//
void CAbstractMediaMtpDataProvider::ProcessRequestPhaseL( TMTPTransactionPhase aPhase,
    const TMTPTypeRequest& aRequest,
    MMTPConnection& aConnection )
    {
    TInt index = LocateRequestProcessorL( aRequest, aConnection );

    __ASSERT_DEBUG( index != KErrNotFound, Panic( EMmMTPDpNoMatchingProcessor ) );

    MMmRequestProcessor* processor = iActiveProcessors[index];
    iActiveProcessor = index;
    // iActiveProcessorRemoved = EFalse;
    TBool result = processor->HandleRequestL( aRequest, aPhase );
    if( !iIsSessionOpen )
        {
        processor->Release();
        }
    // iActiveProcessorRemoved will be set to ETrue in the above function
    // HandleRequestL(),such as SessionClose()
    // else if ( iActiveProcessorRemoved )
    //     {
    //     processor->Release(); // destroy the processor
    //     }
    else if ( result ) // destroy the processor
        {
        processor->Release();
        iActiveProcessors.Remove( index );
        }
    iActiveProcessor = -1;
    }

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProvider::SessionClosedL
// Notify the data provider that the session has been closed
// -----------------------------------------------------------------------------
//
void CAbstractMediaMtpDataProvider::SessionClosedL( const TMTPNotificationParamsSessionChange& aSession )
    {
    iIsSessionOpen = EFalse;
    TInt count = iActiveProcessors.Count();
    PRINT1( _L( "MM MTP => CAbstractMediaMtpDataProvider::SessionClosedL, total processor count = %d" ), count );
    for( TInt i = 0; i < count; i++ )
        {
        MMmRequestProcessor* processor = iActiveProcessors[i];

        // replaced for the Request() is invalid sometimes
        // TUint32 sessionId( processor->Request().Uint32( TMTPTypeRequest::ERequestSessionID ) );
        TUint32 sessionId = processor->SessionId();

        if ( ( sessionId == aSession.iMTPId )
                && ( processor->Connection().ConnectionId()
                == aSession.iConnection.ConnectionId() ) )
            {
            processor->UsbDisconnect(); // Rollback

            iActiveProcessors.Remove( i );
            // if ( i == iActiveProcessor )
            //     {
            //     iActiveProcessorRemoved = ETrue;
            //     }
            //     else
                {
                processor->Release();
                }
            }
        }

    // TODO: Compare to Media Dp, missing enumerator's SessionClose
    CMmMtpDpAccessSingleton::CloseSessionL();
    }

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProvider::SessionOpenedL
// Open Session operation
// -----------------------------------------------------------------------------
//
void CAbstractMediaMtpDataProvider::SessionOpenedL( const TMTPNotificationParamsSessionChange& /*aSession*/ )
    {
    CMmMtpDpAccessSingleton::OpenSessionL();
    iIsSessionOpen = ETrue;
    }

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProvider::RenameObjectL
// Process the changed folder
// -----------------------------------------------------------------------------
//
void CAbstractMediaMtpDataProvider::RenameObjectL( const TMTPNotificationParamsHandle& aObject )
    {
    PRINT( _L( "MM MTP => CAbstractMediaMtpDataProvider::RenameObjectL" ) );

    if ( !iRenameObject )
        {
        iRenameObject = CAbstractMediaMtpDataProviderRenameObject::NewL( Framework(),
            GetWrapperL() );
        }

    iRenameObject->StartL( aObject.iHandleId, aObject.iFileName );

    PRINT( _L( "MM MTP <= CAbstractMediaMtpDataProvider::RenameObjectL" ) );
    }

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProvider::StartObjectEnumerationL
// Start object enumeration
// -----------------------------------------------------------------------------
//
void CAbstractMediaMtpDataProvider::StartObjectEnumerationL( TUint32 aStorageId )
    {
    iPendingEnumerations.AppendL( aStorageId );

    if ( iPendingEnumerations.Count() > 0 )
        {
        iAbstractMediaEnumerator->StartL( iPendingEnumerations[KActiveEnumeration] );
        }
    }

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProvider::StartStorageEnumerationL
// Start Storage enumeration
// -----------------------------------------------------------------------------
//
void CAbstractMediaMtpDataProvider::StartStorageEnumerationL()
    {
    Framework().StorageEnumerationCompleteL();
    }

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProvider::Supported
// Defines the supported operations and formats of the data provider
// -----------------------------------------------------------------------------
//
void CAbstractMediaMtpDataProvider::Supported( TMTPSupportCategory aCategory,
    RArray<TUint>& aArray ) const
    {
    switch ( aCategory )
        {
        case EEvents:
            break;

        case EObjectCaptureFormats:
        case EObjectPlaybackFormats:
            {
            TInt count = sizeof( KAbstractMediaMtpDataProviderSupportedFormats ) / sizeof( TUint16 );
            for ( TInt i = 0; i < count; i++ )
                {
                aArray.Append( KAbstractMediaMtpDataProviderSupportedFormats[i] );
                }
            }
            break;

        case EOperations:
            {
            TInt count = sizeof( KAbstractMediaMtpDataProviderSupportedOperations ) / sizeof( TUint16 );

            for ( TInt i = 0; i < count; i++ )
                {
                aArray.Append( KAbstractMediaMtpDataProviderSupportedOperations[i] );
                }
            }
            break;

        case EObjectProperties:
            {
            TInt count = iSupportedProperties.Count();

            for ( TInt i = 0; i < count; i++ )
                {
                aArray.Append( iSupportedProperties[i] );
                }
            PRINT1( _L( "MM MTP <> CAbstractMediaMtpDataProvider::Supported properties count = %d" ), aArray.Count() );
            }
            break;

        case EStorageSystemTypes:
            {
            aArray.Append( CMTPStorageMetaData::ESystemTypeDefaultFileSystem );
            }
            break;

        default:
            // Unrecognised category, leave aArray unmodified.
            break;
        }
    }

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProvider::SupportedL
// Defines the supported operations and formats of the data provider
// -----------------------------------------------------------------------------
//
void CAbstractMediaMtpDataProvider::SupportedL( TMTPSupportCategory aCategory,
    CDesCArray& aStrings ) const
    {
    if( aCategory == EFormatExtensionSets )
        {
        //EMTPFormatCodeM3U,
        aStrings.AppendL(KFormatExtensionM3U);
        aStrings.AppendL(KFormatExtensionPLA);
        aStrings.AppendL(KFormatExtensionVIR);
        }
    }

TAny* CAbstractMediaMtpDataProvider::GetExtendedInterface( TUid /*aInterfaceUid*/ )
    {
    return NULL;
    }

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProvider::NotifyEnumerationCompleteL
// enumeration completed
// -----------------------------------------------------------------------------
//
void CAbstractMediaMtpDataProvider::NotifyEnumerationCompleteL( TUint32 /*aStorageId*/,
    TInt /*aError*/ )
    {
    Framework().ObjectEnumerationCompleteL( iPendingEnumerations[KActiveEnumeration] );
    iPendingEnumerations.Remove( KActiveEnumeration );
    if ( iPendingEnumerations.Count() )
        {
        iAbstractMediaEnumerator->StartL( iPendingEnumerations[KActiveEnumeration] );
        }
    }

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProvider::LocateRequestProcessorL
// Find or create a request processor that can process the request
// -----------------------------------------------------------------------------
//
TInt CAbstractMediaMtpDataProvider::LocateRequestProcessorL( const TMTPTypeRequest& aRequest,
    MMTPConnection& aConnection )
    {
    TInt index = KErrNotFound;
    TInt count = iActiveProcessors.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        if ( iActiveProcessors[i]->Match( aRequest, aConnection ) )
            {
            index = i;
            break;
            }
        }
    if ( index == KErrNotFound )
        {
        MMmRequestProcessor* processor =
            AbstractMediaMtpDataProviderProcessor::CreateL( Framework(),
                aRequest,
                aConnection,
                *this );

        CleanupReleasePushL( *processor );
        iActiveProcessors.AppendL( processor );
        CleanupStack::Pop( processor );
        index = count;
        }

    return index;
    }

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProvider::LocateRequestProcessorL
// Find or create a request processor that can process the event
// -----------------------------------------------------------------------------
//
TInt CAbstractMediaMtpDataProvider::LocateRequestProcessorL( const TMTPTypeEvent& aEvent,
    MMTPConnection& aConnection )
    {
    TInt index = KErrNotFound;
    TInt count = iActiveProcessors.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        if ( iActiveProcessors[i]->Match( aEvent, aConnection ) )
            {
            index = i;
            break;
            }
        }
    return index;
    }

// -----------------------------------------------------------------------------
// CAbstractMediaMtpDataProvider::GetWrapper
// return the reference of CMmMtpDpMetadataAccessWrapper to enumerator
// -----------------------------------------------------------------------------
//
CMmMtpDpMetadataAccessWrapper& CAbstractMediaMtpDataProvider::GetWrapperL()
    {
    return CMmMtpDpAccessSingleton::GetAccessWrapperL();
    }

// ---------------------------------------------------------------------------
// CAbstractMediaMtpDataProvider::GetSupportedFormat
//
// ---------------------------------------------------------------------------
//
const RArray<TUint>* CAbstractMediaMtpDataProvider::GetSupportedFormat() const
    {
    return &iSupportedFormat;
    }

void CAbstractMediaMtpDataProvider::GetSupportedFormatL()
    {
    iSupportedFormat.Reset();

    TInt count = sizeof ( KAbstractMediaMtpDataProviderSupportedFormats ) / sizeof( TUint16 );
    for ( TInt i = 0; i < count; i++ )
        {
        InsertL( iSupportedFormat, KAbstractMediaMtpDataProviderSupportedFormats[i] );
        }
    }

const RArray<TUint>* CAbstractMediaMtpDataProvider::GetSupportedPropertiesL( TUint32 aFormatCode ) const
    {
    return &iSupportedProperties;
    }

// ---------------------------------------------------------------------------
// CAbstractMediaMtpDataProvider::GetSupportedPropertiesL
//
// ---------------------------------------------------------------------------
//
void CAbstractMediaMtpDataProvider::GetSupportedPropertiesL()
    {
    iSupportedProperties.Reset();

    TInt count = 0, i = 0;
    count = sizeof( KMmMtpDpSupportedPropMandatoryAll ) / sizeof( TUint16 );
    for ( i = 0; i < count; i++ )
        {
        InsertL( iSupportedProperties, KMmMtpDpSupportedPropMandatoryAll[i] );
        }

    count = sizeof( KMmMtpDpSupportedPropAdditionalAll ) / sizeof( TUint16 );
    for ( i = 0; i < count; i++ )
        {
        InsertL( iSupportedProperties, KMmMtpDpSupportedPropAdditionalAll[i] );
        }
    }

// ---------------------------------------------------------------------------
// CAbstractMediaMtpDataProvider::GetAllSupportedProperties
//
// ---------------------------------------------------------------------------
//
const RArray<TUint>* CAbstractMediaMtpDataProvider::GetAllSupportedProperties() const
    {
    return &iSupportedProperties;
    }

// ---------------------------------------------------------------------------
// CMediaMtpDataProvider::GetDefaultStorageIdL
//
// ---------------------------------------------------------------------------
//
TUint32 CAbstractMediaMtpDataProvider::GetDefaultStorageIdL() const
    {
    TInt driveNum = -1;
    TInt err = DriveInfo::GetDefaultDrive( DriveInfo::EDefaultMassStorage, driveNum );
    PRINT2( _L( "MM MTP <> GetDefaultDrive, driveNum = %d, err = %d" ), driveNum, err );

    TDriveInfo driveInfo;
    User::LeaveIfError( Framework().Fs().Drive( driveInfo, driveNum ) );
    if( driveInfo.iType == EMediaNotPresent || driveInfo.iType == EMediaUnknown )
        {
        err = DriveInfo::GetDefaultDrive( DriveInfo::EDefaultPhoneMemory, driveNum );
        PRINT( _L( "MM MTP <> Memory card doesn't exist, set PhoneMemory to default" ) );
        }

    return Framework().StorageMgr().FrameworkStorageId( TDriveNumber( driveNum ) );
    }

// ---------------------------------------------------------------------------
// CAbstractMediaMtpDataProvider::Insert
//
// ---------------------------------------------------------------------------
//
void CAbstractMediaMtpDataProvider::InsertL( RArray<TUint>& aArray, const TUint aProperCode ) const
    {
    TInt err = KErrNone;
    err = aArray.Find( aProperCode );
    if ( err == KErrNotFound )
        err = aArray.Append( aProperCode );

    User::LeaveIfError( err );
    }

// end of file
