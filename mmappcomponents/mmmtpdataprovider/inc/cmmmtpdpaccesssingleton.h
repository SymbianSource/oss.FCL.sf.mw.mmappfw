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
* Description:  Meta data Access Singleton
*
*/


#ifndef CMMMTPDPACCESSSINGLETON_H
#define CMMMTPDPACCESSSINGLETON_H

/**
* Implements the MM MTP Data Priovider access singletons reference manager.
*/

class RFs;
class CMmMtpDpMetadataAccessWrapper;
class MMTPDataProviderFramework;

class CMmMtpDpAccessSingleton : public CObject
    {
public:
    /**
     * Create Singleton instance
     * need Release(), if no use
     * @param aRFs RFs reference
     * @param aFramework framework reference
     */
    IMPORT_C static void CreateL( RFs& aRfs, 
        MMTPDataProviderFramework& aFramework );
    
    /**
     * release singleton instance
     */
    IMPORT_C static void Release();
    
    /**
     * get metadata access wrapper reference
     */
    IMPORT_C static CMmMtpDpMetadataAccessWrapper& GetAccessWrapperL();
    
    /**
     * do some special process with assess DBs when receives opensession command
     */
    IMPORT_C static void OpenSessionL();
    
    /**
     * do some special process with assess DBs when receives closesession command
     */
    IMPORT_C static void CloseSessionL();
    
private:
    /*
     * get singleton instance, for internal use
     */
    static CMmMtpDpAccessSingleton* Instance();
    
    /*
     * 2-phase construction
     */
    static CMmMtpDpAccessSingleton* NewL( RFs& aRfs, 
        MMTPDataProviderFramework& aFramework );
    
    /*
     * 2-phase construction
     */
    void ConstructL( RFs& aRfs, 
        MMTPDataProviderFramework& aFramework );
    /*
     * destruction
     */
    ~CMmMtpDpAccessSingleton();
    
private:
    CMmMtpDpMetadataAccessWrapper* iWrapper;

    };

#endif // CMMMTPDPACCESSSINGLETON_H
