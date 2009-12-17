/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Defines macro for debug output
*
*/

// Version : %version: 4.1.1 %



#ifndef __MP_DEBUG_H__
#define __MP_DEBUG_H__

#include <e32svr.h>
#include <e32def.h>
#include <flogger.h>

//
//  Forward declaration
//
class AsxParserDebug;

// #ifdef __MARM__
//     #define _ASX_PARSER_FILE_LOGGING_
// #endif

#ifdef _ASX_PARSER_FILE_LOGGING_
    #define ASX_DEBUG AsxParserDebug::FileLog
#else
    #ifdef _DEBUG
        #define ASX_DEBUG RDebug::Print
    #else
         #define ASX_DEBUG AsxParserDebug::NullLog
    #endif
#endif 

class AsxParserDebug
{
    public:
        inline static void NullLog(TRefByValue<const TDesC16> /*aFmt*/,...)
        {
        }	

        inline static void FileLog(TRefByValue<const TDesC16> aFmt,...)
        {
            VA_LIST list;
            VA_START(list,aFmt);
            RFileLogger::WriteFormat(_L("AsxParser"), _L("ap.log"),EFileLoggingModeAppend,aFmt,list);
        }
};

#endif // __MP_DEBUG_H__
