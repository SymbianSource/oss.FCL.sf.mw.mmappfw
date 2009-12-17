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
* Description:
*
*/


#ifndef MMMTPDPCONFIG_H
#define MMMTPDPCONFIG_H

class CMmMtpDpMetadataAccessWrapper;

class MMmMtpDpConfig
    {
public:
    /**
    * Get supported format from special data provider
    * @param aArray         The array which is used to store supported format
    */
    virtual const RArray<TUint>* GetSupportedFormat() const = 0;

    /**
    *
    */
    virtual const RArray<TUint>* GetSupportedPropertiesL( TUint32 aFormatCode ) const = 0;

    /**
    *
    */
    virtual const RArray<TUint>* GetAllSupportedProperties() const = 0;

    /**
    * Get default storage Id from resource file, if not specify a storage Id
    * @return TUint32 Default storage Id which read from resource file
    */
    virtual TUint32 GetDefaultStorageIdL() const = 0;

    /*
    *
    */
    virtual CMmMtpDpMetadataAccessWrapper& GetWrapperL() = 0;
    };

#endif // MMMTPDPCONFIG_H
