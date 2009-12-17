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
* Description:  Debug print macros
*
*/

// Version : %version: 2 %




#ifndef VIDEOPLAYLISTUTILITY_LOG_H
#define VIDEOPLAYLISTUTILITY_LOG_H

// INCLUDES
#include <e32std.h>
#include <e32svr.h>
#include <e32des16.h>
#include <e32property.h>
#include <flogger.h>
#include <e32def.h>



// #define _VPU_FILE_LOGGING_

#ifdef _DEBUG
    #define VPU_DEBUG RDebug::Print
#else
    #ifdef _VPU_FILE_LOGGING_
        #define VPU_DEBUG VideoPlaylistUtilityDebug::FileLog
    #else
        #define VPU_DEBUG RDebug::Print
    #endif
#endif 


class VideoPlaylistUtilityDebug
{
    public:
        inline static void NullLog( TRefByValue<const TDesC16> /*aFmt*/, ... )
        {
        }

        inline static void FileLog( TRefByValue<const TDesC16> aFmt, ... )
        {
            VA_LIST list;
            VA_START(list,aFmt);
            RFileLogger::WriteFormat( _L("VideoPlaylistUtility"), 
                                      _L("videoplaylistutility.log"),
                                      EFileLoggingModeAppend,
                                      aFmt,
                                      list );
        }
};



// MACROS
#define VIDEOPLAYLISTUTILITY_DEBUG             TVideoPlaylistUtilityLog::VideoPlaylistUtilityLog
#define VIDEOPLAYLISTUTILITY_ENTER_EXIT        TEnterExitLog _s

class TVideoPlaylistUtilityLog : public TDes16Overflow
{
    public:
        
        inline static void VideoPlaylistUtilityLog( TRefByValue<const TDesC16> aFmt, ... )
        {
            TBuf< 512 > buffer;
            
            VA_LIST list;
            VA_START( list, aFmt );
            buffer.AppendFormatList( aFmt, list );
            VA_END(list);
            
            VPU_DEBUG(_L("#VideoPlaylistUtility# %S"), &buffer );
        }
};

class TEnterExitLog : public TDes16Overflow
{
    public:
        
        void Overflow(TDes16& /*aDes*/)
        {
            VPU_DEBUG(_L("%S Logging Overflow"), &iFunctionName);
        }

        TEnterExitLog( TRefByValue<const TDesC> aFunctionName,
                       TRefByValue<const TDesC> aFmt, ... )
        {
            iFunctionName = HBufC::New( TDesC(aFunctionName).Length() );
            
            if ( iFunctionName )
            {
                iFunctionName->Des().Copy(aFunctionName);
            }
            
            TBuf< 512 > buffer;
            
            VA_LIST list;
            VA_START( list, aFmt );
            buffer.AppendFormatList( aFmt, list, this );
            VA_END(list);
            
            VPU_DEBUG(_L("#VideoPlaylistUtility# --> %S %S"), iFunctionName, &buffer );
        }
        
        TEnterExitLog( TRefByValue<const TDesC> aFunctionName )
        {
            iFunctionName = HBufC::New( TDesC(aFunctionName).Length() );
            
            if ( iFunctionName )
            {
                iFunctionName->Des().Copy(aFunctionName);
            }
            
            VPU_DEBUG(_L("#VideoPlaylistUtility# --> %S"), iFunctionName );
        }
        
        ~TEnterExitLog()
        {
            VPU_DEBUG(_L("#VideoPlaylistUtility# <-- %S"), iFunctionName );
            delete iFunctionName;
        }
        
    private:
        HBufC*    iFunctionName;
};

_LIT(_KVPUErrorInfo, "#VideoPlaylistUtility# Error : error %d file %s line %d");

#define VIDEOPLAYLISTUTILITY_S(a) _S(a)

#define VIDEOPLAYLISTUTILITY_ERROR_LOG(aErr) \
    {\
        if (aErr) \
            VIDEOPLAYLISTUTILITY_DEBUG(_KVPUErrorInfo, aErr, VIDEOPLAYLISTUTILITY_S(__FILE__), __LINE__);\
    }

#define VIDEOPLAYLISTUTILITY_TRAP(_r, _s) TRAP(_r,_s);VIDEOPLAYLISTUTILITY_ERROR_LOG(_r);
#define VIDEOPLAYLISTUTILITY_TRAPD(_r, _s) TRAPD(_r,_s);VIDEOPLAYLISTUTILITY_ERROR_LOG(_r);

#endif  // VIDEOPLAYLISTUTILITY_LOG_H

// End of File
