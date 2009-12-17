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
#include "mpxplaybackutilitytest.h"
// ============================ MEMBER FUNCTIONS ===============================
// ---------------------------------------------------------------------------
// From MMPXPlaybackObserver
// Handle playback message.
// ---------------------------------------------------------------------------
//
void Cmpxplaybackutilitytest::HandlePlaybackMessage( 
    CMPXMessage* /*aMessage*/, TInt aError )
    {
    iLog->Log(_L("Cmpxharvestertest::HandlePlaybackMessage: %d"), aError);   
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
    iFs.Close();
    }

// -----------------------------------------------------------------------------
// Cmpxplaybackutilitytest::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt Cmpxplaybackutilitytest::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        		
        ENTRY( "MMPXPlaybackUtilityNewL", Cmpxplaybackutilitytest::MMPXPlaybackUtilityNewL ),
        ENTRY( "MMPXPlaybackUtilityUtilityL", Cmpxplaybackutilitytest::MMPXPlaybackUtilityUtilityL ),
        ENTRY( "MMPXPlaybackUtilityInit64L", Cmpxplaybackutilitytest::MMPXPlaybackUtilityInit64L ),
        ENTRY( "MMPXPlaybackUtilityInitStreaming64L", Cmpxplaybackutilitytest::MMPXPlaybackUtilityInitStreaming64L ),
        ENTRY( "MMPXPlaybackUtilityFile64L", Cmpxplaybackutilitytest::MMPXPlaybackUtilityFile64L ),
       
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

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
TInt Cmpxplaybackutilitytest::MMPXPlaybackUtilityNewL(CStifItemParser& /*aItem*/)
	{
	TInt err = KErrNone;
	
	TRAP( err,iMPXPlaybackUtility = MMPXPlaybackUtility::NewL(KPbModeDefault,this ));
    
	if (err!= KErrNone)
		{
		iLog->Log(_L("Cmpxharvestertest::MMPXPlaybackUtilityNewL: %d"), err);
		}
	
	return err;
	}

// -----------------------------------------------------------------------------
// Cmpxplaybackutilitytest::MMPXPlaybackUtilityUtilityL
// UtilityL test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cmpxplaybackutilitytest::MMPXPlaybackUtilityUtilityL(CStifItemParser& /*aItem*/)
	{
	TInt err = KErrNone;
	
	TRAP(err,iMPXPlaybackUtility = MMPXPlaybackUtility::UtilityL());
    
	if (err!= KErrNone)
		{
		iLog->Log(_L("Cmpxharvestertest::MMPXPlaybackUtilityUtilityL: %d"), err);
		}
	
	return err;
	}

// -----------------------------------------------------------------------------
// Cmpxplaybackutilitytest::MMPXPlaybackUtilityInit64L
// Init64L test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cmpxplaybackutilitytest::MMPXPlaybackUtilityInit64L(CStifItemParser& aItem)
    {
    iLog->Log(_L("Cmpxplaybackutilitytest::MMPXPlaybackUtilityInit64L testing Init64L() begin"));
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
            iLog->Log(_L("Cmpxplaybackutilitytest::MMPXPlaybackUtilityInit64L testing Init64L() Open passed."));
            TRAP(err,iMPXPlaybackUtility->Init64L(iFile64));
            }
        iLog->Log(_L("Cmpxplaybackutilitytest::MMPXPlaybackUtilityInit64L testing Init64L() error=%d"),err);
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
TInt Cmpxplaybackutilitytest::MMPXPlaybackUtilityInitStreaming64L(CStifItemParser& aItem)
    {
    iLog->Log(_L("Cmpxplaybackutilitytest::MMPXPlaybackUtilityInitStreaming64L testing InitStreaming64L() begin"));
    TInt err = KErrNone;
#ifndef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
    err = KErrNotSupported;
    iLog->Log(_L("Cmpxplaybackutilitytest::MMPXPlaybackUtilityInitStreaming64L testing InitStreaming64L() error=%d"),err);
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
            iLog->Log(_L("Cmpxplaybackutilitytest::MMPXPlaybackUtilityInitStreaming64L testing InitStreaming64L() Open passed."));
            TRAP(err,iMPXPlaybackUtility->InitStreaming64L(iFile64, accessPoint));
            }
        iLog->Log(_L("Cmpxplaybackutilitytest::MMPXPlaybackUtilityInitStreaming64L testing InitStreaming64L() error=%d"),err);
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
TInt Cmpxplaybackutilitytest::MMPXPlaybackUtilityFile64L(CStifItemParser& /*aItem*/)
    {
    iLog->Log(_L("Cmpxplaybackutilitytest::MMPXPlaybackUtilityFile64L testing File64L() begin"));
    TInt err = KErrNone;
#ifndef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
    err = KErrNotSupported;
    iLog->Log(_L("Cmpxplaybackutilitytest::MMPXPlaybackUtilityFile64L testing File64L() error=%d"),err);
#else // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
    RFile64* file64Ptr;
    TRAP(err, file64Ptr=iMPXPlaybackUtility->Source()->File64L());
    if ( !file64Ptr->SubSessionHandle() )
        {
        iLog->Log(_L("MMPXPlaybackUtilityFile64L - file64 = NULL."));
        }
    iLog->Log(_L("Cmpxplaybackutilitytest::MMPXPlaybackUtilityFile64L testing File64L() error=%d"),err);
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
    return err;
    }

