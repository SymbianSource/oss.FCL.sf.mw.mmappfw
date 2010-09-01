/*
* Copyright (c) 2002 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  mpxplaybackutilitytest blocks implementation for STIF Test Framework TestScripter.
*
*/



// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include <mpxvideoplaybackdefs.h>
#include <mpxplaybackcommanddefs.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxcommand.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxplaybackmessage.h>
#include <mpxmediageneraldefs.h>
#include <mpxlog.h>
#include <mpxcollectionplaylist.h>
#include "mpxplaybackutilitytest.h"
#include "mpxplaybackutilitytestdefs.h"

// ============================ MEMBER FUNCTIONS ===============================
// ---------------------------------------------------------------------------
// From MMPXPlaybackObserver
// Handle playback message.
// ---------------------------------------------------------------------------
//
void Cmpxplaybackutilitytest::HandlePlaybackMessage( CMPXMessage* aMessage, TInt aError )
    {
    iLog->Log(_L("Cmpxharvestertest::HandlePlaybackMessage: Error %d"), aError);
    if ( !aError )
        {
        TMPXMessageId id( *(aMessage->Value<TMPXMessageId>(KMPXMessageGeneralId)) );
        TInt event( *aMessage->Value<TInt>( KMPXMessageGeneralEvent ) );
        TInt type( *aMessage->Value<TInt>( KMPXMessageGeneralType ) );
        TInt data( *aMessage->Value<TInt>( KMPXMessageGeneralData ) );

        if ( KMPXMessageGeneral == id )
            {
            iLog->Log(_L("Cmpxplaybackutilitytest::HandlePlaybackMessage() General event = %d type = %d  value = %d"),
                         event, type, data );
            RemoveExpectedEventL( KGeneralPlaybackMsgOffset + event, type, data );
            }
        else if ( KMPXMediaIdVideoPlayback == id )
            {
            iLog->Log(_L("Cmpxplaybackutilitytest::HandlePlaybackMessage() Video event = %d type = %d  value = %d"),
                         event, type, data );
            RemoveExpectedEventL( KVideoPlaybackMsgOffset + event, type, data );
            }
        else if ( KMPXMessageStif )
            {
            iLog->Log(_L("Cmpxplaybackutilitytest::HandlePlaybackMessage() STIF event = %d type = %d  value = %d"),
                         event, type, data );
            RemoveExpectedEventL( KStifPlaybackMsgOffset + event, type, data );
            }
        }
    else
        {
        if ( !iCallbackError )
            {
            iCallbackError = aError;
            }
        }
    }

// ---------------------------------------------------------------------------
//  Handle playback property.
// ---------------------------------------------------------------------------
//
void Cmpxplaybackutilitytest::HandlePropertyL( TMPXPlaybackProperty aProperty, TInt aValue, TInt aError )
    {
    iLog->Log(_L("Cmpxharvestertest::HandlePropertyL: Property %d, Value %d, Error %d"), 
            aProperty, aValue, aError);   
    }

// ---------------------------------------------------------------------------
//  Method is called continously until aComplete=ETrue, signifying that 
//  it is done and there will be no more callbacks.
//  Only new items are passed each time.
// ---------------------------------------------------------------------------
//
void Cmpxplaybackutilitytest::HandleSubPlayerNamesL( TUid aPlayer, 
                                                     const MDesCArray* aSubPlayers,
                                                     TBool aComplete,
                                                     TInt aError )
    {
    iLog->Log(_L("Cmpxharvestertest::HandleSubPlayerNamesL: Complete %d, Error %d"), aComplete, aError);   
    }

// ---------------------------------------------------------------------------
//  Call back of media request.
// ---------------------------------------------------------------------------
//
void Cmpxplaybackutilitytest::HandleMediaL( const CMPXMedia& aProperties, TInt aError )
    {
    iLog->Log(_L("Cmpxharvestertest::HandleMediaL: Error %d"), aError);   
    }

// ---------------------------------------------------------------------------
//  Handle completion of a asynchronous command.
//  Note: All clients should implement this callback.
// ---------------------------------------------------------------------------
//
void Cmpxplaybackutilitytest::HandlePlaybackCommandComplete( CMPXCommand* aCommandResult, TInt aError )
    {
    iLog->Log(_L("Cmpxharvestertest::HandlePlaybackCommandComplete: Error %d"), aError);   
    }

// -----------------------------------------------------------------------------
// Cmpxplaybackutilitytest::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void Cmpxplaybackutilitytest::Delete()
    {
    iMPXPlaybackUtility->Close();
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
    iFile64.Close();
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
    iFile.Close();
    iFs.Close();
    iExpectedEventArray->ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// Cmpxplaybackutilitytest::CreateL
// Create here all resources needed for test methods. 
// Called from ConstructL(). 
// -----------------------------------------------------------------------------
//
void Cmpxplaybackutilitytest::CreateL()
    {
    iFs.Connect();
    iFs.ShareProtected();

    iExpectedEventArray = new (ELeave) CArrayPtrFlat<TMpxPlaybackTestEvent>( 1 );
    iCallbackError = KErrNone;
    iMPXPlaybackUtility = NULL;
    }

// -----------------------------------------------------------------------------
// Cmpxplaybackutilitytest::AddExpectedEventL
// -----------------------------------------------------------------------------
//
void Cmpxplaybackutilitytest::AddExpectedEventL( TInt aEvent, TInt aType, TInt aData )
    {
    iLog->Log(_L("Cmpxplaybackutilitytest::AddExpectedEventL() event = %d type = %d  value = %d"),
                 aEvent, aType, aData );
    
    TMpxPlaybackTestEvent* event = new (ELeave) TMpxPlaybackTestEvent;
    event->iEvent = aEvent;
    event->iType = aType;
    event->iData = aData;
    iExpectedEventArray->AppendL( event );
    }

// -----------------------------------------------------------------------------
// Cmpxplaybackutilitytest::RemoveExpectedEventL
// -----------------------------------------------------------------------------
//
void Cmpxplaybackutilitytest::RemoveExpectedEventL( TInt aEvent, TInt aType, TInt aData )
    {
    iLog->Log(_L("Cmpxplaybackutilitytest::RemoveExpectedEventL() event = %d type = %d  value = %d"),
                 aEvent, aType, aData );
    
    if ( (iExpectedEventArray->Count() > 0) &&
         ((*iExpectedEventArray)[0]->iEvent == aEvent) )
        {
        iLog->Log(_L("Cmpxplaybackutilitytest::RemoveExpectedEventL() event = %d removed."), aEvent);
        iExpectedEventArray->Delete( 0 );
        }
    }

// -----------------------------------------------------------------------------
// Cmpxplaybackutilitytest::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt Cmpxplaybackutilitytest::RunMethodL( CStifItemParser& aItem ) 
    {
    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 

        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove
        ENTRY( "MMPXPlaybackUtilityNewL", Cmpxplaybackutilitytest::MMPXPlaybackUtilityNewL ),
        ENTRY( "MMPXPlaybackUtilityNewWithCatL", Cmpxplaybackutilitytest::MMPXPlaybackUtilityNewWithCatL ),
        ENTRY( "MMPXPlaybackUtilityUtilityL", Cmpxplaybackutilitytest::MMPXPlaybackUtilityUtilityL ),
        ENTRY( "MMPXPlaybackUtilityUtilityWithCatL", Cmpxplaybackutilitytest::MMPXPlaybackUtilityUtilityWithCatL ),
        ENTRY( "MMPXPlaybackUtilityInitWithPlaylistL", Cmpxplaybackutilitytest::MMPXPlaybackUtilityInitWithPlaylistL ),
        ENTRY( "MMPXPlaybackUtilityInitWithUriL", Cmpxplaybackutilitytest::MMPXPlaybackUtilityInitWithUriL ),
        ENTRY( "MMPXPlaybackUtilityInitWithRFileL", Cmpxplaybackutilitytest::MMPXPlaybackUtilityInitWithRFileL ),
        ENTRY( "MMPXPlaybackUtilityInitStreamingWithUriL", Cmpxplaybackutilitytest::MMPXPlaybackUtilityInitStreamingWithUriL ),
        ENTRY( "MMPXPlaybackUtilityInitStreamingWithRFileL", Cmpxplaybackutilitytest::MMPXPlaybackUtilityInitStreamingWithRFileL ),
        ENTRY( "MMPXPlaybackUtilityInit64L", Cmpxplaybackutilitytest::MMPXPlaybackUtilityInit64L ),
        ENTRY( "MMPXPlaybackUtilityInitStreaming64L", Cmpxplaybackutilitytest::MMPXPlaybackUtilityInitStreaming64L ),
        ENTRY( "MMPXPlaybackUtilityFile64L", Cmpxplaybackutilitytest::MMPXPlaybackUtilityFile64L ),
        ENTRY( "MMPXPlaybackUtilityAddObserverL", Cmpxplaybackutilitytest::MMPXPlaybackUtilityAddObserverL ),
        ENTRY( "MMPXPlaybackUtilityRemoveObserverL", Cmpxplaybackutilitytest::MMPXPlaybackUtilityRemoveObserverL ),
        ENTRY( "MMPXPlaybackUtilityPlayerManagerSelectPlayerL", Cmpxplaybackutilitytest::MMPXPlaybackUtilityPlayerManagerSelectPlayerL ),
        ENTRY( "EndTest", Cmpxplaybackutilitytest::EndTest ),
        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );
   
    return RunInternalL( KFunctions, count, aItem );
    }

// -----------------------------------------------------------------------------
// Cmpxplaybackutilitytest::MMPXPlaybackUtilityNewL
// NewL test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cmpxplaybackutilitytest::MMPXPlaybackUtilityNewL( CStifItemParser& aItem )
	{
	iLog->Log(_L("Cmpxplaybackutilitytest::MMPXPlaybackUtilityNewL"));
	TInt err = KErrNone;
    TInt mode;
   
    if ( aItem.GetNextInt(mode) != KErrNone )
        {
        iLog->Log(_L("MMPXPlaybackUtilityNewL - Missing playback mode."));
        err = KErrArgument;
        return err;
        }

    iLog->Log(_L("Cmpxharvestertest::MMPXPlaybackUtilityNewL: mode %d"), mode);
	TRAP( err,iMPXPlaybackUtility = MMPXPlaybackUtility::NewL(TUid::Uid(mode), this ));
    
	if (err!= KErrNone)
		{
		iLog->Log(_L("Cmpxharvestertest::MMPXPlaybackUtilityNewL: error %d"), err);
		}
	
	return err;
	}

// -----------------------------------------------------------------------------
// Cmpxplaybackutilitytest::MMPXPlaybackUtilityNewWithCatL
// NewL test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cmpxplaybackutilitytest::MMPXPlaybackUtilityNewWithCatL( CStifItemParser& aItem )
    {
    iLog->Log(_L("Cmpxplaybackutilitytest::MMPXPlaybackUtilityNewWithCatL"));
    TInt err = KErrNone;
    TInt mode;
    TInt category;
    if ( aItem.GetNextInt(category) != KErrNone )
        {
        iLog->Log(_L("MMPXPlaybackUtilityNewWithCatL - Missing category."));
        err = KErrArgument;
        return err;
        }

    if ( aItem.GetNextInt(mode) != KErrNone )
        {
        iLog->Log(_L("MMPXPlaybackUtilityNewWithCatL - Missing playback mode."));
        err = KErrArgument;
        return err;
        }

    iLog->Log(_L("Cmpxharvestertest::MMPXPlaybackUtilityNewWithCatL: category %d"), category);
    iLog->Log(_L("Cmpxharvestertest::MMPXPlaybackUtilityNewWithCatL: mode %d"), mode);
    TRAP( err,iMPXPlaybackUtility = MMPXPlaybackUtility::NewL((TMPXCategory)category, TUid::Uid(mode), this ));
    
    if (err!= KErrNone)
        {
        iLog->Log(_L("Cmpxharvestertest::MMPXPlaybackUtilityNewWithCatL: error %d"), err);
        }
    return err;
    }

// -----------------------------------------------------------------------------
// Cmpxplaybackutilitytest::MMPXPlaybackUtilityUtilityL
// UtilityL test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cmpxplaybackutilitytest::MMPXPlaybackUtilityUtilityL( CStifItemParser& aItem )
	{
    iLog->Log(_L("Cmpxplaybackutilitytest::MMPXPlaybackUtilityUtilityL"));
	TInt err = KErrNone;
	TInt mode = 0;
	
	aItem.GetNextInt(mode);
    iLog->Log(_L("Cmpxharvestertest::MMPXPlaybackUtilityUtilityL: mode %d"), mode);
	if ( mode )
	    {
	    TRAP(err,iMPXPlaybackUtility = MMPXPlaybackUtility::UtilityL(TUid::Uid(mode)));
	    }
	else
	    {
        TRAP(err,iMPXPlaybackUtility = MMPXPlaybackUtility::UtilityL());
	    }
    
	iLog->Log(_L("Cmpxharvestertest::MMPXPlaybackUtilityUtilityL: error %d"), err);
	
	return err;
	}

// -----------------------------------------------------------------------------
// Cmpxplaybackutilitytest::MMPXPlaybackUtilityUtilityWithCatL
// UtilityL test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cmpxplaybackutilitytest::MMPXPlaybackUtilityUtilityWithCatL( CStifItemParser& aItem )
    {
    iLog->Log(_L("Cmpxplaybackutilitytest::MMPXPlaybackUtilityUtilityWithCatL"));
    TInt err = KErrNone;
    TInt category;
    TInt mode = 0;
    
    if ( aItem.GetNextInt(category) != KErrNone )
        {
        iLog->Log(_L("MMPXPlaybackUtilityNewWithCatL - Missing category."));
        err = KErrArgument;
        return err;
        }

    aItem.GetNextInt(mode);
    iLog->Log(_L("Cmpxharvestertest::MMPXPlaybackUtilityUtilityWithCatL: category %d"), category);
    iLog->Log(_L("Cmpxharvestertest::MMPXPlaybackUtilityUtilityWithCatL: mode %d"), mode);
    if ( mode )
        {
        TRAP(err,iMPXPlaybackUtility = MMPXPlaybackUtility::UtilityL((TMPXCategory)category, TUid::Uid(mode)));
        }
    else
        {
        TRAP(err,iMPXPlaybackUtility = MMPXPlaybackUtility::UtilityL((TMPXCategory)category));
        }
    
    iLog->Log(_L("Cmpxharvestertest::MMPXPlaybackUtilityUtilityWithCatL: error %d"), err);
    
    return err;
    }

// -----------------------------------------------------------------------------
// Cmpxplaybackutilitytest::MMPXPlaybackUtilityInitWithPlaylistL
// InitL(CMPXCollectionPlaylist) test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cmpxplaybackutilitytest::MMPXPlaybackUtilityInitWithPlaylistL( CStifItemParser& /*aItem*/ )
    {
    iLog->Log(_L("Cmpxplaybackutilitytest::MMPXPlaybackUtilityInitWithPlaylistL"));
    TInt err = KErrNone;
    CMPXCollectionPlaylist* playlist = CMPXCollectionPlaylist::NewL();
    CleanupStack::PushL(playlist);
    TRAP(err,iMPXPlaybackUtility->InitL(*playlist));
    CleanupStack::PopAndDestroy(playlist);
    iLog->Log(_L("Cmpxplaybackutilitytest::MMPXPlaybackUtilityInitWithPlaylistL - error=%d"),err);
    return err;
    }

// -----------------------------------------------------------------------------
// Cmpxplaybackutilitytest::MMPXPlaybackUtilityInitWithUriL
// InitL(Uri) test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cmpxplaybackutilitytest::MMPXPlaybackUtilityInitWithUriL( CStifItemParser& aItem )
    {
    iLog->Log(_L("Cmpxplaybackutilitytest::MMPXPlaybackUtilityInitWithUriL"));
    TInt err = KErrNone;
    TPtrC string;
   
    if( aItem.GetNextString( string ) == KErrNone )
        {
        TBuf<120> uri;
        uri.Append(KmpxplaybackutilityTestFilePath);
        uri.Append(string);
        iLog->Log(_L("Cmpxplaybackutilitytest::MMPXPlaybackUtilityInitWithUriL - uri = %S."), &uri);
        TRAP(err,iMPXPlaybackUtility->InitL(uri));
        
        if ( !err )
            {
            TInt event = KGeneralPlaybackMsgOffset + TMPXPlaybackMessage::EPlayerChanged;
            AddExpectedEventL(event, 0, 0);
            event = KGeneralPlaybackMsgOffset + TMPXPlaybackMessage::EInitializeComplete;
            AddExpectedEventL(event, 0, 0);
            }
        iLog->Log(_L("Cmpxplaybackutilitytest::MMPXPlaybackUtilityInitWithUriL - error=%d"),err);
        }
    else
        {
        iLog->Log(_L("MMPXPlaybackUtilityInitWithUriL - Missing file name."));
        err = KErrArgument;
        }
    return err;
    }

// -----------------------------------------------------------------------------
// Cmpxplaybackutilitytest::MMPXPlaybackUtilityInitWithRFileL
// Init64L test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cmpxplaybackutilitytest::MMPXPlaybackUtilityInitWithRFileL( CStifItemParser& aItem )
    {
    iLog->Log(_L("Cmpxplaybackutilitytest::MMPXPlaybackUtilityInitWithRFileL"));
    TInt err = KErrNone;
    TPtrC string;
   
    if( aItem.GetNextString( string ) == KErrNone )
        {
        TBuf<120> KFrom;
        KFrom.Append(KmpxplaybackutilityTestFilePath);
        KFrom.Append(string);
        if ( iFile.SubSessionHandle() )
            {
            iFile.Close();
            }
        err = iFile.Open(iFs, KFrom, EFileRead | EFileShareReadersOrWriters);
        if ( err == KErrNone )
            {
            iLog->Log(_L("Cmpxplaybackutilitytest::MMPXPlaybackUtilityInitWithRFileL - Open passed."));
            TRAP(err,iMPXPlaybackUtility->InitL(iFile));
            
            if ( !err )
                {
                TInt event = KGeneralPlaybackMsgOffset + TMPXPlaybackMessage::EPlayerChanged;
                AddExpectedEventL(event, 0, 0);
                event = KGeneralPlaybackMsgOffset + TMPXPlaybackMessage::EInitializeComplete;
                AddExpectedEventL(event, 0, 0);
                }
            }
        iLog->Log(_L("Cmpxplaybackutilitytest::MMPXPlaybackUtilityInitWithRFileL - error=%d"),err);
        }
    else
        {
        iLog->Log(_L("MMPXPlaybackUtilityInitWithRFileL - Missing file name."));
        err = KErrArgument;
        }
    return err;
    }

// -----------------------------------------------------------------------------
// Cmpxplaybackutilitytest::MMPXPlaybackUtilityInitStreamingWithUriL
// InitStreamingL(Uri) test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cmpxplaybackutilitytest::MMPXPlaybackUtilityInitStreamingWithUriL( CStifItemParser& aItem )
    {
    iLog->Log(_L("Cmpxplaybackutilitytest::MMPXPlaybackUtilityInitStreamingWithUriL"));
    TInt err = KErrNone;
    TPtrC string;
    TInt accessPoint = 1;
   
    if( aItem.GetNextString( string ) == KErrNone )
        {
        TBuf<120> uri;
        uri.Append(KmpxplaybackutilityTestFilePath);
        uri.Append(string);
        iLog->Log(_L("Cmpxplaybackutilitytest::MMPXPlaybackUtilityInitStreamingWithUriL - uri = %S."), &uri);
        TRAP(err,iMPXPlaybackUtility->InitStreamingL(uri, NULL, accessPoint));
        
        if ( !err )
            {
            TInt event = KGeneralPlaybackMsgOffset + TMPXPlaybackMessage::EPlayerChanged;
            AddExpectedEventL(event, 0, 0);
            event = KGeneralPlaybackMsgOffset + TMPXPlaybackMessage::EInitializeComplete;
            AddExpectedEventL(event, 0, 0);
            }
        iLog->Log(_L("Cmpxplaybackutilitytest::MMPXPlaybackUtilityInitStreamingWithUriL - error=%d"),err);
        }
    else
        {
        iLog->Log(_L("MMPXPlaybackUtilityInitStreamingWithUriL - Missing file name."));
        err = KErrArgument;
        }
    return err;
    }

// -----------------------------------------------------------------------------
// Cmpxplaybackutilitytest::MMPXPlaybackUtilityInitStreamingWithRFileL
// InitStreamingL(RFile) test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cmpxplaybackutilitytest::MMPXPlaybackUtilityInitStreamingWithRFileL( CStifItemParser& aItem )
    {
    iLog->Log(_L("Cmpxplaybackutilitytest::MMPXPlaybackUtilityInitStreamingWithRFileL"));
    TInt err = KErrNone;
    TPtrC string;
    TInt accessPoint = 1;
    
    if( aItem.GetNextString( string ) == KErrNone )
        {
        TBuf<120> KFrom;
        KFrom.Append(KmpxplaybackutilityTestFilePath);
        KFrom.Append(string);
        if ( iFile.SubSessionHandle() )
            {
            iFile.Close();
            }
        err = iFile.Open(iFs, KFrom, EFileRead | EFileShareReadersOrWriters);
        if ( err == KErrNone )
            {
            iLog->Log(_L("Cmpxplaybackutilitytest::MMPXPlaybackUtilityInitStreamingWithRFileL - Open passed."));
            TRAP(err,iMPXPlaybackUtility->InitStreamingL(iFile, accessPoint));

            if ( !err )
                {
                TInt event = KGeneralPlaybackMsgOffset + TMPXPlaybackMessage::EPlayerChanged;
                AddExpectedEventL(event, 0, 0);
                event = KGeneralPlaybackMsgOffset + TMPXPlaybackMessage::EInitializeComplete;
                AddExpectedEventL(event, 0, 0);
                }
            }
        iLog->Log(_L("Cmpxplaybackutilitytest::MMPXPlaybackUtilityInitStreamingWithRFileL - error=%d"),err);
        }
    else
        {
        iLog->Log(_L("MMPXPlaybackUtilityInitStreamingWithRFileL - Missing file name."));
        err = KErrArgument;
        }
    return err;
    }

// -----------------------------------------------------------------------------
// Cmpxplaybackutilitytest::MMPXPlaybackUtilityInit64L
// Init64L test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cmpxplaybackutilitytest::MMPXPlaybackUtilityInit64L( CStifItemParser& aItem )
    {
    iLog->Log(_L("Cmpxplaybackutilitytest::MMPXPlaybackUtilityInit64L"));
    TInt err = KErrNone;
#ifndef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
    err = KErrNotSupported;
    iLog->Log(_L("Cmpxplaybackutilitytest::MMPXPlaybackUtilityInit64L testing Init64L() error=%d"),err);
#else // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
    TPtrC string;
   
    if( aItem.GetNextString( string ) == KErrNone )
        {
        TBuf<120> KFrom;
        KFrom.Append(KmpxplaybackutilityTestFilePath);
        KFrom.Append(string);
        if ( iFile64.SubSessionHandle() )
            {
            iFile64.Close();
            }
        err = iFile64.Open(iFs, KFrom, EFileRead | EFileShareReadersOrWriters);
        if ( err == KErrNone )
            {
            iLog->Log(_L("Cmpxplaybackutilitytest::MMPXPlaybackUtilityInit64L - Open passed."));
            TRAP(err,iMPXPlaybackUtility->Init64L(iFile64));
            
            if ( !err )
                {
                TInt event = KGeneralPlaybackMsgOffset + TMPXPlaybackMessage::EPlayerChanged;
                AddExpectedEventL(event, 0, 0);
                event = KGeneralPlaybackMsgOffset + TMPXPlaybackMessage::EInitializeComplete;
                AddExpectedEventL(event, 0, 0);
                }
            }
        iLog->Log(_L("Cmpxplaybackutilitytest::MMPXPlaybackUtilityInit64L - error=%d"),err);
        }
    else
        {
        iLog->Log(_L("MMPXPlaybackUtilityInit64L - Missing file name."));
        err = KErrArgument;
        }
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
    return err;
    }

// -----------------------------------------------------------------------------
// Cmpxplaybackutilitytest::MMPXPlaybackUtilityInitStreaming64L
// InitStreaming64L test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cmpxplaybackutilitytest::MMPXPlaybackUtilityInitStreaming64L( CStifItemParser& aItem )
    {
    iLog->Log(_L("Cmpxplaybackutilitytest::MMPXPlaybackUtilityInitStreaming64L"));
    TInt err = KErrNone;
#ifndef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
    err = KErrNotSupported;
    iLog->Log(_L("Cmpxplaybackutilitytest::MMPXPlaybackUtilityInitStreaming64L - error=%d"),err);
#else // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
    TPtrC string;
    TInt accessPoint = 1;
    
    if( aItem.GetNextString( string ) == KErrNone )
        {
        TBuf<120> KFrom;
        KFrom.Append(KmpxplaybackutilityTestFilePath);
        KFrom.Append(string);
        if ( iFile64.SubSessionHandle() )
            {
            iFile64.Close();
            }
        err = iFile64.Open(iFs, KFrom, EFileRead | EFileShareReadersOrWriters);
        if ( err == KErrNone )
            {
            iLog->Log(_L("Cmpxplaybackutilitytest::MMPXPlaybackUtilityInitStreaming64L - Open passed."));
            TRAP(err,iMPXPlaybackUtility->InitStreaming64L(iFile64, accessPoint));

            if ( !err )
                {
                TInt event = KGeneralPlaybackMsgOffset + TMPXPlaybackMessage::EPlayerChanged;
                AddExpectedEventL(event, 0, 0);
                event = KGeneralPlaybackMsgOffset + TMPXPlaybackMessage::EInitializeComplete;
                AddExpectedEventL(event, 0, 0);
                }
            }
        iLog->Log(_L("Cmpxplaybackutilitytest::MMPXPlaybackUtilityInitStreaming64L - error=%d"),err);
        }
    else
        {
        iLog->Log(_L("MMPXPlaybackUtilityInitStreaming64L - Missing file name."));
        err = KErrArgument;
        }
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
    return err;
    }

// -----------------------------------------------------------------------------
// Cmpxplaybackutilitytest::MMPXPlaybackUtilityFile64L
// File64L test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cmpxplaybackutilitytest::MMPXPlaybackUtilityFile64L( CStifItemParser& /*aItem*/ )
    {
    iLog->Log(_L("Cmpxplaybackutilitytest::MMPXPlaybackUtilityFile64L"));
    TInt err = KErrNone;
#ifndef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
    err = KErrNotSupported;
    iLog->Log(_L("Cmpxplaybackutilitytest::MMPXPlaybackUtilityFile64L - error=%d"),err);
#else // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
    RFile64* file64Ptr=NULL;
    TRAP(err, file64Ptr=iMPXPlaybackUtility->Source()->File64L());
    if ( !file64Ptr->SubSessionHandle() )
        {
        iLog->Log(_L("MMPXPlaybackUtilityFile64L - file64 = NULL."));
        }
    iLog->Log(_L("Cmpxplaybackutilitytest::MMPXPlaybackUtilityFile64L - error=%d"),err);
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
    return err;
    }

// -----------------------------------------------------------------------------
// Cmpxplaybackutilitytest::MMPXPlaybackUtilityAddObserverL
// AddObserverL test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cmpxplaybackutilitytest::MMPXPlaybackUtilityAddObserverL( CStifItemParser& /*aItem*/ )
    {
    iLog->Log(_L("Cmpxplaybackutilitytest::MMPXPlaybackUtilityAddObserverL"));
    TInt err = KErrNone;
    TRAP(err, iMPXPlaybackUtility->AddObserverL(*this));
    iLog->Log(_L("Cmpxplaybackutilitytest::MMPXPlaybackUtilityAddObserverL - error=%d"),err);
    return err;
    }

// -----------------------------------------------------------------------------
// Cmpxplaybackutilitytest::MMPXPlaybackUtilityRemoveObserverL
// RemoveObserverL test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cmpxplaybackutilitytest::MMPXPlaybackUtilityRemoveObserverL( CStifItemParser& /*aItem*/ )
    {
    iLog->Log(_L("Cmpxplaybackutilitytest::MMPXPlaybackUtilityRemoveObserverL"));
    TInt err = KErrNone;
    TRAP(err, iMPXPlaybackUtility->RemoveObserverL(*this));
    iLog->Log(_L("Cmpxplaybackutilitytest::MMPXPlaybackUtilityRemoveObserverL - error=%d"),err);
    return err;
    }

// -----------------------------------------------------------------------------
// Cmpxplaybackutilitytest::MMPXPlaybackUtilityPlayerManagerSelectPlayerL
// RemoveObserverL test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cmpxplaybackutilitytest::MMPXPlaybackUtilityPlayerManagerSelectPlayerL(CStifItemParser& aItem)
    {
    iLog->Log(_L("Cmpxplaybackutilitytest::MMPXPlaybackUtilityPlayerManagerSelectPlayerL"));
    TInt uidInt;
    TInt err = KErrNone;
    
    // read in UID
    if ( aItem.GetNextInt(uidInt) != KErrNone )
        {
        iLog->Log(_L("MMPXPlaybackUtilityPlayerManagerSelectPlayerL - Missing UID."));
        err = KErrArgument;
        return err;
        }

    iLog->Log(_L("MMPXPlaybackUtilityPlayerManagerSelectPlayerL - UID = 0x%x."), uidInt);
    if ( iMPXPlaybackUtility )
        {
        MMPXPlayerManager& manager = iMPXPlaybackUtility->PlayerManager();
        TRAP( err, manager.SelectPlayerL( TUid::Uid(uidInt) ) );
        iLog->Log(_L("MMPXPlaybackUtilityPlayerManagerSelectPlayerL - SelectPlayer err = %d."), err);
        }
    else
        {
        iLog->Log(_L("MMPXPlaybackUtilityPlayerManagerSelectPlayerL - MPX Playback Utility not created."));
        err = KErrGeneral;
        }
    return err;
    }

// -----------------------------------------------------------------------------
// Cmpxplaybackutilitytest::EndTest
// -----------------------------------------------------------------------------
//
TInt Cmpxplaybackutilitytest::EndTest( CStifItemParser& /*aItem*/ )
    {
    iLog->Log(_L("Cmpxplaybackutilitytest::EndTest"));
    TInt err = iCallbackError;
    
    // check if event queue is empty
    if ( !err && (iExpectedEventArray->Count() > 0 ) ) 
        {
        iLog->Log(_L("Cmpxplaybackutilitytest::EndTest error = KErrTimedOut"));
        err = KErrTimedOut;
        }
    return err;
    }

// end of file
