/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class is responsible for reading file info
*  Version     : %version: da1mmcf#5.1.3.1.6 % << Don't touch! Updated by Synergy at check-out.
*
*  Copyright © 2005 Nokia. All rights reserved.
*/


// INCLUDE FILES
#include <mmf/common/mmfstandardcustomcommands.h>
#include <mmf/server/mmffile.h>
#include <mmf/common/mmcaf.h>
#include <mpxlog.h>
#include "mpxfileinfoutility.h"
using namespace ContentAccess;

// CONSTANTS
const TInt KMCExpandSize = 100;

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CMPXFileInfoUtility* CMPXFileInfoUtility::NewL()
    {
    CMPXFileInfoUtility* self = NewLC();
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// Two-phased constructor, leave object pointer in the cleanup stack
// ----------------------------------------------------------------------------
//
CMPXFileInfoUtility* CMPXFileInfoUtility::NewLC()
    {
    CMPXFileInfoUtility* self = new (ELeave) CMPXFileInfoUtility();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that might leave
// ----------------------------------------------------------------------------
//
CMPXFileInfoUtility::CMPXFileInfoUtility(): iCurrentControllerUid(NULL)
    {
    }

// Destructor
CMPXFileInfoUtility::~CMPXFileInfoUtility()
    {
    iAudioControllers.ResetAndDestroy();
    //Reset();
    // Close all MMF controllers in the cache
    TPtrHashMapIter<TUint32, RMMFController> iter(iMMFControllers);
    RMMFController* controller;
    
    while((controller = const_cast<RMMFController*>(iter.NextValue())) != NULL)
    	{
    	controller->Close();
    	}
    
    iMMFControllers.ResetAndDestroy();
    iMMFControllers.Close();
    
    // delete non cached controller
    if(iController)
        {
        iController->Close();
        delete iController;
        iController = NULL;
        }
    
    REComSession::FinalClose();
    }

// ----------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CMPXFileInfoUtility::ConstructL()
    {
    CreateAudioFormatsArrayL();
    }

// ----------------------------------------------------------------------------
// Open a file using audio controller
// ----------------------------------------------------------------------------
//
void CMPXFileInfoUtility::OpenFileL(RFile& aFile,
                                    const TDesC& aMimeType /*= KNullDesC*/)
    {
    MPX_DEBUG1("CMPXFileInfoUtility::OpenFileL()");
    Reset();

    HBufC* fileNameBuf = HBufC::NewLC(KMaxFileName);
    TPtr fileName = fileNameBuf->Des();
    aFile.FullName(fileName);

    TUid pluginUid;
    FindController(fileName, aMimeType, pluginUid);
    CleanupStack::PopAndDestroy(fileNameBuf);

    // Open a new controller
    OpenControllerL(pluginUid);
    TMMFMessageDestination dataSource;

    TPtrC defaultID(KDefaultContentObject);
    TBool enableUI = EFalse;

    //Code taken from TMMFileHandleSource


    //{//build custom mmf message packet
     //based on CMMFileSourceSink::DoCreateFileHandleSourceConfigDataL()
    CBufFlat* buf = CBufFlat::NewL(KMCExpandSize);
    CleanupStack::PushL(buf);
    RBufWriteStream stream;
    stream.Open(*buf);
    CleanupClosePushL(stream);

    TPckgBuf<RFile*> fileptr(&aFile);
    stream.WriteInt32L(KMMFileHandleSourceUid.iUid);
    stream.WriteL(fileptr);

    //Code taken from TMMFileHandleSource

    TInt length = 0;
    //if (defaultID != NULL)
    length = defaultID.Length();
    stream.WriteInt32L(length);
    //if (length>0)
    stream.WriteL(defaultID);

    stream.WriteInt32L(enableUI);

    stream.CommitL();
    CleanupStack::PopAndDestroy(&stream);
    //iSourceSinkData = buf->Ptr(0).AllocL();

    //CleanupStack::PopAndDestroy(buf);
    //}

    // User::LeaveIfError(iController->Reset());
    
   
   // Add new data source
    User::LeaveIfError(iController->AddDataSource(KUidMmfFileSource,
                                                 buf->Ptr(0),
                                                 dataSource));
    
    User::LeaveIfError(iController->AddDataSink(KUidMmfAudioOutput,
                                               KNullDesC8));
    CleanupStack::PopAndDestroy(buf);
    }

// ----------------------------------------------------------------------------
// Reset file info utility
// ----------------------------------------------------------------------------
//
void CMPXFileInfoUtility::Reset()
    {
    // Reset the controller
    if(iController)
        {
        if(iCurrentControllerUid == 0x101FAFB1 || iCurrentControllerUid == 0x10283351
            || iCurrentControllerUid == 0x10207B65 )
            {
            iController->Close();
            delete iController;
            iController = NULL;
            }
        else
            {
           	iController->Reset();
            }
    	}
    
    }

// ----------------------------------------------------------------------------
// Get the duration of a song
// ----------------------------------------------------------------------------
//
TTimeIntervalMicroSeconds CMPXFileInfoUtility::Duration()
    {
    MPX_DEBUG1("CMPXFileInfoUtility::Duration()");
    TTimeIntervalMicroSeconds duration;
    TInt err = iController->GetDuration(duration);
    if(err != KErrNone)
        {
        duration = TInt64(0);
        }
    return duration;
    }

// ----------------------------------------------------------------------------
// Get the bit rate of a song
// ----------------------------------------------------------------------------
//
TUint CMPXFileInfoUtility::BitRate()
    {
    RMMFAudioControllerCustomCommands customCommands(*iController);

    TUint bitRate(0);
    //Ignore return value, bitRate remain 0 if error
    customCommands.GetSourceBitRate(bitRate);
    return bitRate;
    }

// ----------------------------------------------------------------------------
// Get the sample rate of a song
// ----------------------------------------------------------------------------
//
TUint CMPXFileInfoUtility::SampleRate()
    {
    RMMFAudioControllerCustomCommands  customCommands(*iController);

    TUint sampleRate(0) ;
    //Ignore return value, sampleRate remain 0 if error
    customCommands.GetSourceSampleRate(sampleRate);
    return sampleRate;
    }

// ----------------------------------------------------------------------------
// Find a controller based on mime type, then file name
// ----------------------------------------------------------------------------
void CMPXFileInfoUtility::FindController(const TDesC& aFileName,
                                         const TDesC& aMimeType,
                                         TUid& aUid)
    {
    MPX_DEBUG1("CMPXFileInfoUtility::FindControllerL()");
    TBool found(EFalse);
    TInt i(0);
    TInt j(0);
    TInt count = iAudioControllers.Count();

    // try with MIME type
    if (aMimeType.Length() > 0)
        {
        TBuf8<KMaxDataTypeLength> mimeType;
        mimeType.Copy(aMimeType);
        for (i = 0 ; !found && i < count; i++)
            {
            RMMFFormatImplInfoArray formats =
                        iAudioControllers[i]->PlayFormats();
                for (j = 0; j < formats.Count() ; j++)
                    {
                    if (formats[j]->SupportsMimeType(mimeType))
                        {
                        aUid = iAudioControllers[i]->Uid();
                        found = ETrue;
                        break;
                        }
                    }
            }
        }
    // try file extension
    if (!found)
        {
        TParsePtrC parser(aFileName);
        TBuf8<KMaxFileName> fileExtension;
        fileExtension.Copy(parser.Ext());
        for (i = 0 ; !found && i < count; i++)
            {
            RMMFFormatImplInfoArray formats =
                    iAudioControllers[i]->PlayFormats();
            for (j = 0; j < formats.Count() ; j++)
                {
                if (formats[j]->SupportsFileExtension(fileExtension))
                    {
                    aUid = iAudioControllers[i]->Uid();
                    found = ETrue;
                    break;
                    }
                }
            }
        }
    }

// ----------------------------------------------------------------------------
// Open a controller based on file name
// The Controller Framework APIs are made of three distinct areas: choosing a
// controller plugin, loading and controlling a controller plugin, and custom
// commands.
// ----------------------------------------------------------------------------
void CMPXFileInfoUtility::OpenControllerL(const TUid& aUid)
    {
    MPX_DEBUG1("CMPXFileInfoUtility::OpenControllerL()");
    
    // 3gp and helix (wma) do not allow controller caching
    if(aUid.iUid == 0x101FAFB1 || aUid.iUid == 0x10283351
        || aUid.iUid == 0x10207B65 )
        {
         iCurrentControllerUid = aUid.iUid;
         TMMFPrioritySettings prioritySettings;
         prioritySettings.iPriority = EMdaPriorityNormal;
         prioritySettings.iPref = EMdaPriorityPreferenceTimeAndQuality;

         iController = new (ELeave) RMMFController;

         // Controller loading
         User::LeaveIfError(iController->Open(aUid, prioritySettings));  

         iCurrentControllerUid = aUid.iUid;
         return;
        }
    
    // check if we already have controller open for this UID in the cache
     RMMFController* controller = const_cast<RMMFController*>(iMMFControllers.Find(aUid.iUid));

    if(!controller)
	    {
	    // Controller not found from the cache
	   	
    	// Allocate and open new controller for this UID
    	controller = new (ELeave) RMMFController;
    	CleanupStack::PushL(controller);
    	
    	TUint32* key = new (ELeave) TUint32(aUid.iUid);
    	CleanupStack::PushL(key);
    // Now instantiate the first controller in the array
    TMMFPrioritySettings prioritySettings;
    prioritySettings.iPriority = EMdaPriorityNormal;
    prioritySettings.iPref = EMdaPriorityPreferenceTimeAndQuality;

	    // Try to open controller
	    User::LeaveIfError(controller->Open(aUid,
	                       prioritySettings));
	    
	    iMMFControllers.InsertL(key, controller);
	    
	    // iMMFController owns the objects now
	    CleanupStack::Pop(2);
	    
	    }
	
	iController = controller;
	iCurrentControllerUid = aUid.iUid;
                       
    }

// -----------------------------------------------------------------------------
// CMPXFileInfoUtility::CreateAudioFormatsArrayL
// -----------------------------------------------------------------------------
//
void CMPXFileInfoUtility::CreateAudioFormatsArrayL()
    {
    CMMFControllerPluginSelectionParameters* cSelect =
            CMMFControllerPluginSelectionParameters::NewLC();
    CMMFFormatSelectionParameters* fSelect =
            CMMFFormatSelectionParameters::NewLC();
    // Set the play and record format selection parameters to be blank.
    // - format support is only retrieved if requested.
    cSelect->SetRequiredPlayFormatSupportL(*fSelect);
    // Set the media ids
    RArray<TUid> mediaIds;
    CleanupClosePushL(mediaIds);
    User::LeaveIfError(mediaIds.Append(KUidMediaTypeAudio));
    // Get plugins that supports audio only
    cSelect->SetMediaIdsL( mediaIds,
            CMMFPluginSelectionParameters::EAllowOnlySuppliedMediaIds );
    // iAudioControllers contains now all audio plugins that
    // support at least audio.
    cSelect->ListImplementationsL(iAudioControllers);
    // Clean up
    CleanupStack::PopAndDestroy(3); //fSelect, cSelect, mediaIds
    }

// End of File
