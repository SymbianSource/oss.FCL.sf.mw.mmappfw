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
* Description:   CCollectionHelperTestClass block implementaion for STIF Test Framework TestScripter.
*
*/


// INCLUDE FILES
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "CollectionHelperTestClass.h"
#include <mpxmessagegeneraldefs.h> // KMPXMessageGeneralEvent, KMPXMessageGeneralType
#include <mpxmediageneraldefs.h>
#include <mpxmediaaudiodefs.h>
#include <mpxmediamusicdefs.h>
#include <mpxmediadrmdefs.h>
#include <mpxmediaarray.h>
#include <mpxmediacontainerdefs.h>
#include <mpxattribute.h>
#include "debug.h"

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def

// LOCAL CONSTANTS AND MACROS
//const ?type ?constant_var = ?constant;
//#define ?macro_name ?macro_def

// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

// LOCAL FUNCTION PROTOTYPES
//?type ?function_name( ?arg_type, ?arg_type );

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ?function_name ?description.
// ?description
// Returns: ?value_1: ?description
//          ?value_n: ?description_line1
//                    ?description_line2
// -----------------------------------------------------------------------------
//
/*
?type ?function_name(
    ?arg_type arg,  // ?description
    ?arg_type arg)  // ?description
    {

    ?code  // ?comment

    // ?comment
    ?code
    }
*/

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCollectionHelperTestClass::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CCollectionHelperTestClass::Delete() 
    {
    FTRACE(FPrint(_L("CTunerUtilityTestClass::Delete")));
    
    iExpectedEvents.Close();
    iOcurredEvents.Close();
    
    iLog->Log(_L("Deleting test class..."));
	iLog->Log(_L(""));
	iLog->Log(_L(""));
    
    if(iUIHelper)
   		{
        //delete iUIHelper;
        iUIHelper = NULL;
 	  	}
    if(iHelper)
   		{
        //delete iHelper;
        iHelper = NULL;
   		}
    if(iCachedHelper)
   		{
        //delete iCachedHelper;
        iCachedHelper = NULL;
   		}
    }

// -----------------------------------------------------------------------------
// CCollectionHelperTestClass::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CCollectionHelperTestClass::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {        
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "OpenCollectionUtility", CCollectionHelperTestClass::OpenCollectionUtilityL ),
        ENTRY( "NewUiHelper", CCollectionHelperTestClass::NewUiHelperL ),
        ENTRY( "NewHelper", CCollectionHelperTestClass::NewHelperL ),
        ENTRY( "NewCachedHelper", CCollectionHelperTestClass::NewCachedHelperL ),
        ENTRY( "CloseUiHelper", CCollectionHelperTestClass::CloseUiHelperL ),
        ENTRY( "CloseHelper", CCollectionHelperTestClass::CloseHelperL ),
        ENTRY( "CloseCachedHelper", CCollectionHelperTestClass::CloseCachedHelperL ),        
        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );
    }


// -----------------------------------------------------------------------------
// CCollectionHelperTestClass::EventName
// Return descriptor with the notification description
// -----------------------------------------------------------------------------
TPtrC CCollectionHelperTestClass::EventName( TInt aKey )
{
	static TText* const badKeyword = (TText*)L"BadKeyword";
	static TText* const keywords[] =
	{
		(TText*)L"EHandleCollectionMessage",
		(TText*)L"EHandleOpen",
		(TText*)L"EHandleAddFileComplete",
		(TText*)L"EHandleRemove",
		(TText*)L"EHandleFindAllComplete"
	};

	if( (TUint)aKey >= (sizeof( keywords )/sizeof(TText*)) )
	{
		iLog->Log(_L("Keyword out of bounds"));
		TPtrC keyword( badKeyword );
		return keyword;
	}
	else
	{
		TPtrC keyword( keywords[aKey] );
		return keyword;
	}
}

// -----------------------------------------------------------------------------
// CCollectionHelperTestClass::AddExpectedEvent
// Add an event to the expected events' list
// -----------------------------------------------------------------------------
void CCollectionHelperTestClass::AddExpectedEvent(TCollectionHelperExpectedEvent event, TInt ms)
{
	FTRACE(FPrint(_L("CCollectionHelperTestClass::AddExpectedEvent")));
	iExpectedEvents.Append(event);
	TPtrC eventName = EventName(event);
	iLog->Log(_L("Adding expected event:(0x%02x)%S Total=%d"), event, &eventName, iExpectedEvents.Count() );

	if ( iTimeoutController && !iTimeoutController->IsActive() )
	{
		if (ms > 0)
		{
			iTimeoutController->Start( TTimeIntervalMicroSeconds(ms * 1000) );
		}
		else
		{
			iLog->Log(_L("Timeout with default value (1s)"));
			iTimeoutController->Start( TTimeIntervalMicroSeconds(1000000) );
		}
	}
}

// -----------------------------------------------------------------------------
// CCollectionHelperTestClass::RemoveExpectedEvent
// Remove the indicated event from the expected events' list
// Returns: ETrue: Event found.
//          EFalse: Event not found.
// -----------------------------------------------------------------------------
TBool CCollectionHelperTestClass::RemoveExpectedEvent(TCollectionHelperExpectedEvent aEvent)
{
	FTRACE(FPrint(_L("CCollectionHelperTestClass::RemoveExpectedEvent")));
	TBool match = EFalse;
	for (TUint i=0; i < iExpectedEvents.Count() ; i++)
	{
		if (iExpectedEvents[i] == aEvent)
		{
			iExpectedEvents.Remove(i);
			match = ETrue;
			break;
		}
	}

	return match;
}


// -----------------------------------------------------------------------------
// CCollectionHelperTestClass::RemoveAllExpectedEvents
// Remove the indicated event from the expected events' list
// -----------------------------------------------------------------------------
void CCollectionHelperTestClass::RemoveAllExpectedEvents()
{
	FTRACE(FPrint(_L("CCollectionHelperTestClass::RemoveAllExpectedEvents")));
	iLog->Log(_L("Removing all expected events"));

	iExpectedEvents.Reset();
	iOcurredEvents.Reset();
}


// -----------------------------------------------------------------------------
// CCollectionHelperTestClass::ProcessEvent
// Process events.
// -----------------------------------------------------------------------------
void CCollectionHelperTestClass::ProcessEvent(TCollectionHelperExpectedEvent aEvent, TInt aError)
{
	FTRACE(FPrint(_L("CCollectionHelperTestClass::ProcessExpectedEvent")));
	TPtrC nameEvent = EventName(aEvent);

	// Check for error
	if (aError == KErrNone)
	{
		// Remove the event
		if (RemoveExpectedEvent(aEvent))
		{
			iLog->Log(_L("Expected Event: (0x%02x)%S has ocurred Total=%d"), aEvent, &nameEvent,iExpectedEvents.Count());
		}
		else
		{
			iLog->Log(_L("Event: (0x%02x)%S has ocurred"), aEvent, &nameEvent);
			return;
		}

		// All expected events have ocurred
		if (iExpectedEvents.Count() == 0 )
		{
			Signal();
			iTimeoutController->Cancel();
		}
	}
	else
	{
		iLog->Log(_L("[Error] Event: (0x%02x)%S return with error code=%d"), aEvent, &nameEvent, aError);
		if (iExpectedEvents.Count() != 0 )
		{
			RemoveExpectedEvent(aEvent);
		}
		iTimeoutController->Cancel();
		Signal(KErrCallbackErrorCode);
	}
}

// -----------------------------------------------------------------------------
// CCollectionHelperTestClass::HandleTimeout
// Review if all the expected events have ocurred once the time is over
// -----------------------------------------------------------------------------
void CCollectionHelperTestClass::HandleTimeout(TInt aError)
{
	FTRACE(FPrint(_L("CCollectionHelperTestClass::HandleTimeout")));
	// All expected events have ocurred
	if (aError != KErrNone)
	{
		if (iExpectedEvents.Count() == 0 )
		{
			iLog->Log(_L("Timing out but events have ocurred"));
			Signal();
		}
		else
		{
			RemoveAllExpectedEvents();
			iLog->Log(_L("Timing out and events still pending"));
			Signal(KErrEventPending);
		}
	}
	else
	{
		iLog->Log(_L("Timing out return a error %d"), aError);
		Signal(aError);
	}
}

// -----------------------------------------------------------------------------
// CCollectionHelperTestClass::SetTimeout
// Create a timer and set a timeout
// When the timeout is reached the test case is marked as failed
// It's used rather than the "timeout" keyword in the configuration file
// because in this way the log continues
// -----------------------------------------------------------------------------
TInt CCollectionHelperTestClass::SetTimeout( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CCollectionHelperTestClass::SetTimeout")));
	TInt timeout=0;
	TInt error = aItem.GetNextInt(timeout) ;
	if ( iTimeoutController )
	{
		if ( timeout > 0 )
		{
			iTimeoutController->Start( TTimeIntervalMicroSeconds(timeout*1000) );
		}
		else
		{
			iTimeoutController->Start( TTimeIntervalMicroSeconds(1000000) );
		}
	}
	else
	{
		iLog->Log(_L("Timeout Controller doesn't exist"));
		error = KErrTimeoutController;
	}
	return error;
}


// -----------------------------------------------------------------------------
// Uses the TestModuleBase API to allow a panic as exit reason for a test case
// Returns: Symbian OS errors.
// -----------------------------------------------------------------------------
TInt CCollectionHelperTestClass::SetAllowedPanic( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CCollectionHelperTestClass::SetAllowedPanic")));
	TInt error = KErrNone;
	TInt panicCode;
	TPtrC panicType;
	if (  ( KErrNone == aItem.GetNextString(panicType) ) &&
	( KErrNone == aItem.GetNextInt(panicCode) )  )
	{
		iLog->Log(_L("Allowing panic: %S %d"), &panicType, panicCode);
		iTestModuleIf.SetExitReason( CTestModuleIf::EPanic, panicCode );
		iNormalExitReason = EFalse;
	}
	else
	{
		iLog->Log(KMsgBadTestParameters);
		error = KErrBadTestParameter;
	}
	return error;
}

// -----------------------------------------------------------------------------
// CCollectionHelperTestClass::SetExpectedEvents()
// Returns: Symbian OS errors.
// -----------------------------------------------------------------------------
TInt CCollectionHelperTestClass::SetExpectedEvents( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CCollectionHelperTestClass::SetExpectedEvents")));
	TInt error = KErrNone;
	TInt event=0;
	while ( KErrNone == aItem.GetNextInt(event))
	{
		AddExpectedEvent(static_cast<TCollectionHelperExpectedEvent>(event), 0); // Default timeout value
	}
	return error;
}
// -----------------------------------------------------------------------------
// CCollectionHelperTestClass::CreateTestMediaL()
// Returns: no return value.
// -----------------------------------------------------------------------------
void CCollectionHelperTestClass::CreateTestMediaL( CMPXMedia*& aNewProperty )
    {
    RArray<TInt> contentIDs;
    contentIDs.AppendL( KMPXMediaIdGeneral );
    contentIDs.AppendL( KMPXMediaIdAudio );
    contentIDs.AppendL( KMPXMediaIdMusic );
    contentIDs.AppendL( KMPXMediaIdDrm ); 
            
    aNewProperty = NULL;
    CMPXMedia* media = CMPXMedia::NewL( contentIDs.Array() );
    CleanupStack::PushL( media );
    contentIDs.Close();
    
    // CMPXMedia default types    
    media->SetTObjectValueL<TMPXGeneralType>( KMPXMediaGeneralType, 
                                              EMPXItem );
    media->SetTObjectValueL<TMPXGeneralCategory>( KMPXMediaGeneralCategory, 
                                                  EMPXSong );
    media->SetTextValueL( KMPXMediaGeneralUri, KTestFile() );
    
    // Title, default is file name
    media->SetTextValueL( KMPXMediaGeneralTitle, KTestFile() );
    
    // Default album track
    media->SetTextValueL( KMPXMediaMusicAlbumTrack, KNullDesC );
    
    // Set the Mime Type and collection UID
    media->SetTextValueL( KMPXMediaGeneralMimeType, KTestFileMimeType() );
    //media->SetTObjectValueL( KMPXMediaGeneralCollectionId, );

    // Set the pointers now that the object is ready
    CleanupStack::Pop( media );
    aNewProperty = media;
    }

// -----------------------------------------------------------------------------
// CCollectionHelperTestClass::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCollectionHelperTestClass::ExampleL( CStifItemParser& aItem )
    {
    // Print to UI
    _LIT( KCollectionHelperTestClass, "CollectionHelperTestClass" );
    _LIT( KExample, "In Example" );
    TestModuleIf().Printf( 0, KCollectionHelperTestClass, KExample );
    // Print to log file
    iLog->Log( KExample );

    TInt i = 0;
    TPtrC string;
    _LIT( KParam, "Param[%i]: %S" );
    while ( aItem.GetNextString ( string ) == KErrNone )
        {
        TestModuleIf().Printf( i, KCollectionHelperTestClass, 
                                KParam, i, &string );
        i++;
        }

    return KErrNone;
    }     
// -----------------------------------------------------------------------------
// CCollectionHelperTestClass::OpenCollectionUtilityL()
// Returns: Symbian OS errors.
// -----------------------------------------------------------------------------
    
TInt CCollectionHelperTestClass::OpenCollectionUtilityL(CStifItemParser& /*aItem*/)
	{    
	FTRACE(FPrint(_L("CCollectionHelperTest::OpenCollectionUtilityL")));
	iLog->Log(_L("CCollectionHelperTestClass::OpenCollectionUtilityL"));
		
    TRAPD( result, iCollectionUtility->Collection().OpenL() );
    
	if( KErrNone == result )
		{		
		AddExpectedEvent( EHandleCollectionMessage, KMediumTimeout );
		AddExpectedEvent( EHandleOpen, KMediumTimeout );
		}
	
	return result;
    }    
	    	
// -----------------------------------------------------------------------------
// CCollectionHelperTestClass::NewUiHelperL()
// Returns: Symbian OS errors.
// -----------------------------------------------------------------------------
	    	
TInt CCollectionHelperTestClass::NewUiHelperL(CStifItemParser& /*aItem*/)
	{
	FTRACE(FPrint(_L("CCollectionHelperTest::NewUiHelperL")));
	iLog->Log(_L("CCollectionHelperTestClass::NewUiHelperL"));

	TRAPD( result, iUIHelper = CMPXCollectionHelperFactory::NewCollectionUiHelperL() ); 

	return result;
    }    

// -----------------------------------------------------------------------------
// CCollectionHelperTestClass::NewHelperL()
// Returns: Symbian OS errors.
// -----------------------------------------------------------------------------
		
TInt CCollectionHelperTestClass::NewHelperL(CStifItemParser& /*aItem*/)
	{
	FTRACE(FPrint(_L("CCollectionHelperTest::NewHelperL")));
	iLog->Log(_L("CCollectionHelperTestClass::NewHelperL"));

	TRAPD( result, iHelper = CMPXCollectionHelperFactory::NewCollectionHelperL() ); 

	return result;
	}
// -----------------------------------------------------------------------------
// CCollectionHelperTestClass::NewCachedHelperL()
// Returns: Symbian OS errors.
// -----------------------------------------------------------------------------

TInt CCollectionHelperTestClass::NewCachedHelperL(CStifItemParser& /*aItem*/)
	{
	FTRACE(FPrint(_L("CCollectionHelperTest::NewCachedHelperL")));
	iLog->Log(_L("CCollectionHelperTestClass::NewCachedHelperL"));

	TRAPD( result, iCachedHelper = CMPXCollectionHelperFactory::NewCollectionCachedHelperL() ); 

	return result;
	}
// -----------------------------------------------------------------------------
// CCollectionHelperTestClass::CloseUiHelperL()
// Returns: Symbian OS errors.
// -----------------------------------------------------------------------------

TInt CCollectionHelperTestClass::CloseUiHelperL(CStifItemParser& /*aItem*/)
	{
	FTRACE(FPrint(_L("CCollectionHelperTest::CloseUiHelperL")));
	iLog->Log(_L("CCollectionHelperTestClass::CloseUiHelperL"));

	TRAPD( result, iUIHelper->Close() ); 

	return result;
	}
// -----------------------------------------------------------------------------
// CCollectionHelperTestClass::CloseHelperL()
// Returns: Symbian OS errors.
// -----------------------------------------------------------------------------
	
TInt CCollectionHelperTestClass::CloseHelperL(CStifItemParser& /*aItem*/)
	{
	FTRACE(FPrint(_L("CCollectionHelperTest::CloseHelperL")));
	iLog->Log(_L("CCollectionHelperTestClass::CloseHelperL"));
	
	TRAPD( result, iHelper->Close() ); 

	return result;
	}
// -----------------------------------------------------------------------------
// CCollectionHelperTestClass::CloseCachedHelperL()
// Returns: Symbian OS errors.
// -----------------------------------------------------------------------------
	
TInt CCollectionHelperTestClass::CloseCachedHelperL(CStifItemParser& /*aItem*/)
	{
	FTRACE(FPrint(_L("CCollectionHelperTest::CloseCachedHelperL")));
	iLog->Log(_L("CCollectionHelperTestClass::CloseCachedHelperL"));

	TRAPD( result, iCachedHelper->Close() ); 

	return result;
	}

// ---------------------------------------------------------------------------
// Handle collection message
// ---------------------------------------------------------------------------
//
void CCollectionHelperTestClass::HandleCollectionMessageL(const CMPXMessage& aMsg)
    {
	FTRACE(FPrint(_L("CCollectionHelperTestClass::HandleCollectionMessageL")));
	iLog->Log(_L("CCollectionHelperTestClass::HandleCollectionMessageL"));
	
	if( aMsg.IsSupported(KMPXMessageGeneralEvent) &&
        aMsg.IsSupported(KMPXMessageGeneralType) )         
        {
        TMPXCollectionMessage::TEvent event = 
        *aMsg.Value<TMPXCollectionMessage::TEvent>( KMPXMessageGeneralEvent );
        TInt type = *aMsg.Value<TInt>( KMPXMessageGeneralType );
        TInt data = *aMsg.Value<TInt>( KMPXMessageGeneralData );
        
        TMPXCollectionMessage message(event, type, data);
	    if(message.Event() == TMPXCollectionMessage::EPathChanged && 
	       message.Type() == EMcPathChangedByOpen)
        	{
        	iCollectionUtility->Collection().OpenL();
        	
        	this->ProcessEvent(EHandleCollectionMessage, KErrNone);
        	}	
        }
    }

// ---------------------------------------------------------------------------
//  Handles the collection entries being opened. Typically called
//  when client has Open()'d a folder
// ---------------------------------------------------------------------------
//
void CCollectionHelperTestClass::HandleOpenL(const CMPXMedia& /*aEntries*/,
                                  TInt /*aIndex*/,TBool /*aComplete*/,TInt aError)
    {
	FTRACE(FPrint(_L("CCollectionHelperTest::HandleOpenL")));
	iLog->Log(_L("CCollectionHelperTestClass::HandleOpenL"));
	
	this->ProcessEvent(EHandleOpen, aError);
    }
                     
// ---------------------------------------------------------------------------
// Handles the item being opened. Typically called
// when client has Open()'d an item. Client typically responds by
// 'playing' the item via the playlist
// ---------------------------------------------------------------------------
void CCollectionHelperTestClass::HandleOpenL(const CMPXCollectionPlaylist& /*aPlaylist*/,TInt /*aError*/)
    {
	FTRACE(FPrint(_L("CCollectionHelperTest::HandleOpenL")));
	iLog->Log(_L("CCollectionHelperTestClass::HandleOpenL"));
    }

// ---------------------------------------------------------------------------
// Handle completion of a asynchronous command
// ---------------------------------------------------------------------------
void CCollectionHelperTestClass::HandleCommandComplete(CMPXCommand* /*aCommandResult*/, TInt /*aError*/)
    {
	FTRACE(FPrint(_L("CCollectionHelperTest::HandleCommandComplete")));
	iLog->Log(_L("CCollectionHelperTestClass::HandleCommandComplete"));
	
	Signal();
	iTimeoutController->Cancel();
    }
    
// ---------------------------------------------------------------------------
//  Handle extended media properties
// ---------------------------------------------------------------------------
//
void CCollectionHelperTestClass::HandleCollectionMediaL(const CMPXMedia& /*aMedia*/, 
                                             TInt /*aError*/)
    {
	FTRACE(FPrint(_L("CCollectionHelperTest::HandleCollectionMediaL")));
	iLog->Log(_L("CCollectionHelperTestClass::HandleCollectionMediaL"));
	
	Signal();
	iTimeoutController->Cancel();
    }

// ---------------------------------------------------------------------------
//  Handles removing a collection path
// ---------------------------------------------------------------------------
//
void CCollectionHelperTestClass::HandleRemoveL(MDesCArray& /*aUriArray*/, TInt aError)
    {
	FTRACE(FPrint(_L("CCollectionHelperTest::HandleRemoveL")));
	iLog->Log(_L("CCollectionHelperTestClass::HandleRemoveL"));
	
	this->ProcessEvent(EHandleRemove, aError);
    }

// ---------------------------------------------------------------------------
//  Handle callback for "find" operation
// ---------------------------------------------------------------------------
//
void CCollectionHelperTestClass::HandleFindAllL(const CMPXMedia& /*aResults*/, TBool aComplete,
                                     TInt aError)
    {
	FTRACE(FPrint(_L("CCollectionHelperTest::HandleFindAllL")));
	iLog->Log(_L("CCollectionHelperTestClass::HandleFindAllL"));
	
	if(aComplete)
		{
		this->ProcessEvent(EHandleFindAllComplete, aError);
		}
		else 
		{
		this->ProcessEvent(EHandleFindAllComplete, KErrGeneral );
		}	
    }
   
// ---------------------------------------------------------------------------
//  Handle callback for "AddFileComplete" operation
// ---------------------------------------------------------------------------
//
void CCollectionHelperTestClass::HandleAddFileCompleteL( TInt aErr )
	{
	FTRACE(FPrint(_L("CCollectionHelperTest::HandleAddFileCompleteL")));
	iLog->Log(_L("CCollectionHelperTestClass::HandleAddFileCompleteL"));
	
	this->ProcessEvent(EHandleAddFileComplete, aErr);
	}
// ---------------------------------------------------------------------------
//  Handle callback for "OperationComplete" operation
// ---------------------------------------------------------------------------
//
void CCollectionHelperTestClass::HandleOperationCompleteL( 
		TCHelperOperation /*aOperation*/, TInt /*aErr*/, void* /*aArgument*/ )
    {
	FTRACE(FPrint(_L("CCollectionHelperTest::HandleOperationCompleteL")));
	iLog->Log(_L("CCollectionHelperTestClass::HandleOperationCompleteL"));
	
	Signal();
	iTimeoutController->Cancel();
	}      
// ---------------------------------------------------------------------------
//  Handle callback for "EmbeddedOpen" operation
// ---------------------------------------------------------------------------
//	                                 
void CCollectionHelperTestClass::HandleEmbeddedOpenL( 
		TInt /*aErr*/, TMPXGeneralCategory /*aCategory*/ )
    {
	FTRACE(FPrint(_L("CCollectionHelperTest::HandleEmbeddedOpenL")));
	iLog->Log(_L("CCollectionHelperTestClass::HandleEmbeddedOpenL"));
	
	Signal();
	iTimeoutController->Cancel();
	}
//add new function

