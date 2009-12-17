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
* Description:  Debug print macros
*
*/

// Version : %version: 4 %




#ifndef PLAYBACKHELPER_LOG_H
#define PLAYBACKHELPER_LOG_H

// INCLUDES
#include <e32svr.h>
#include <e32def.h>
#include <flogger.h>



// #define _PLAYBACKHELPER_FILE_LOGGING_

#ifdef _DEBUG
    #define PLAYBACKHELPER_DEBUG RDebug::Print
#else
    #ifdef _PLAYBACKHELPER_FILE_LOGGING_
        #define PLAYBACKHELPER_DEBUG PlaybackHelperDebug::FileLog
    #else
        #define PLAYBACKHELPER_DEBUG RDebug::Print
    #endif
#endif 


class PlaybackHelperDebug
{
    public:
        inline static void NullLog( TRefByValue<const TDesC16> /*aFmt*/, ... )
        {
        }

        inline static void FileLog( TRefByValue<const TDesC16> aFmt, ... )
        {
            VA_LIST list;
            VA_START(list,aFmt);
            RFileLogger::WriteFormat( _L("PlaybackHelper"), 
                                      _L("playbackhelper.log"),
                                      EFileLoggingModeAppend,
                                      aFmt,
                                      list );
        }
};

#endif  // PLAYBACKHELPER_LOG_H

// End of File
