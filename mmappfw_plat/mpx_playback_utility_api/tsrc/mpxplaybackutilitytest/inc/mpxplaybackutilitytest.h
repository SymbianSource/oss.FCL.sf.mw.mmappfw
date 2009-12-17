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
* Description:  mpxplaybackutilitytest header for STIF Test Framework TestScripter.
*
*/



#ifndef MPXPLAYBACKUTILITYTEST_H
#define MPXPLAYBACKUTILITYTEST_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <TestclassAssert.h>
#include <mpxplaybackutility.h>
#include <mpxplaybackobserver.h>
// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def
#define TEST_CLASS_VERSION_MAJOR 0
#define TEST_CLASS_VERSION_MINOR 0
#define TEST_CLASS_VERSION_BUILD 0

// Logging path
#ifdef __WINSCW__
_LIT( KmpxplaybackutilitytestLogPath, "C:\\logs\\testframework\\mpxplaybackutilitytest\\" ); 
// Log file
_LIT( KmpxplaybackutilitytestLogFile, "mpxplaybackutilitytest.txt" ); 
_LIT( KmpxplaybackutilitytestLogFileWithTitle, "mpxplaybackutilitytest_[%S].txt" );
// data file
_LIT( KmpxplaybackutilityTestFilePath, "C:\\testing\\data\\" );
#else
_LIT( KmpxplaybackutilitytestLogPath, "\\logs\\testframework\\mpxplaybackutilitytest\\" ); 
// Log file
_LIT( KmpxplaybackutilitytestLogFile, "mpxplaybackutilitytest.txt" ); 
_LIT( KmpxplaybackutilitytestLogFileWithTitle, "mpxplaybackutilitytest_[%S].txt" );
// data file
_LIT( KmpxplaybackutilityTestFilePath, "e:\\testing\\data\\" );
#endif
// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class Cmpxplaybackutilitytest;

class MMPXPlaybackUtility;
class MMPXPlaybackObserver;
// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// CLASS DECLARATION

/**
*  Cmpxplaybackutilitytest test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(Cmpxplaybackutilitytest) : public CScriptBase,
                                             public MMPXPlaybackObserver   
                                            
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static Cmpxplaybackutilitytest* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~Cmpxplaybackutilitytest();


    public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * @since ?Series60_version
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );
      
        // From base class MMPXPlaybackObserver
        /**
         * Handle playback message
         *
         * @since 3.1
         * @param aMessage playback message
         * @param aErr system error code.
         */
        void HandlePlaybackMessage(
            CMPXMessage* aMessage, TInt aError );
    private:

        /**
        * C++ default constructor.
        */
        Cmpxplaybackutilitytest( CTestModuleIf& aTestModuleIf );
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        // Prohibit copy constructor if not deriving from CBase.
        // ?classname( const ?classname& );
        // Prohibit assigment operator if not deriving from CBase.
        // ?classname& operator=( const ?classname& );
        /**
        * Frees all resources allocated from test methods.
        * @since ?Series60_version
        */
        void Delete();
        /**
        * Test methods are listed below. 
        */
        TInt MMPXPlaybackUtilityNewL(CStifItemParser& /*aItem*/);
        TInt MMPXPlaybackUtilityUtilityL(CStifItemParser& /*aItem*/);
        TInt MMPXPlaybackUtilityInit64L(CStifItemParser& /*aItem*/);
        TInt MMPXPlaybackUtilityInitStreaming64L(CStifItemParser& /*aItem*/);
        TInt MMPXPlaybackUtilityFile64L(CStifItemParser& /*aItem*/);
       
        /**
         * Method used to log version of test class
         */
        void SendTestClassVersion();
        //ADD NEW METHOD DEC HERE
        //[TestMethods] - Do not remove 
    private:    // Friend classes
        //From  mpxplaybackutility.h
    	MMPXPlaybackUtility* iMPXPlaybackUtility;
        RFs iFs;
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
        RFile64 iFile64;
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
    };

#endif      // MPXPLAYBACKUTILITYTEST_H

// End of File
