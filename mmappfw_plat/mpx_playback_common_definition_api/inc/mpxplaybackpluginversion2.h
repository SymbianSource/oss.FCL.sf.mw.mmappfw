/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Playback Init Access Point interface
*
*/

 
#ifndef CMPXPLAYBACKPLUGINVERSION2_H
#define CMPXPLAYBACKPLUGINVERSION2_H

// INCLUDES

#include <mpxplaybackplugin.h>

/**
*  Interface for receiving property values asynchronously.
*
*  @lib mpxcommon.lib
*/
NONSHARABLE_CLASS(CMPXPlaybackPluginVersion2): public CMPXPlaybackPlugin
    {
public:
    
    /**
    * Initializes a file for playback.
    *
    * @since S60 9.2
    * @param aUri URI of the item
    * @param aType the mime type of the item
    * @param aAccessPoint the access point
    */
    virtual void InitStreamingL(const TDesC& aUri, const TDesC8& aType, TInt aAccessPoint) = 0;

    /**
    * Initializes a file handle for playback.
    *
    * @since S60 9.2
    * @param aFile file handle of a file
    * @param aAccessPoint the access point
    */
    virtual void InitStreamingL(RFile& aFile, TInt aAccessPoint) = 0;
    
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
    /**
    * Initializes a file handle for playback.
    *
    * @since S60 9.2
    * @param aFile 64 bit file handle of a file
    * @param aAccessPoint the access point
    */
    virtual void InitStreaming64L(RFile64& aFile, TInt aAccessPoint) = 0;

    /**
    * Initializes a song for playback.
    *
    * @since S60 9.2
    * @param aFile 64 bit file handle of a song
    */
    virtual void Initialise64L(RFile64& aSong) = 0;
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
    };
    
#endif // CMPXPLAYBACKPLUGINVERSION2_H
