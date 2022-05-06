/*==============================================================================
Play Sound Example
Copyright (c), Firelight Technologies Pty, Ltd 2004-2021.

This example shows how to simply load and play multiple sounds, the simplest 
usage of FMOD. By default FMOD will decode the entire file into memory when it
loads. If the sounds are big and possibly take up a lot of RAM it would be
better to use the FMOD_CREATESTREAM flag, this will stream the file in realtime
as it plays.
==============================================================================*/

#include "common.h"
#include <string>
#include <fmod.hpp>

int FMOD_Main()
{
    FMOD::System     *system;
    FMOD::Sound      *sound1;
    FMOD::Channel    *channel = 0;
    FMOD_RESULT       result;
    unsigned int      version;
    void             *extradriverdata = 0;
    
    Common_Init(&extradriverdata);

    /*
        Create a System object and initialize
    */

    result = FMOD::System_Create(&system);
    ERRCHECK(result);

    result = system->getVersion(&version);
    ERRCHECK(result);

    if (version < FMOD_VERSION)
    {
        Common_Fatal("FMOD lib version %08x doesn't match header version %08x", version, FMOD_VERSION);
    }

    result = system->init(32, FMOD_INIT_NORMAL, extradriverdata);
    ERRCHECK(result);

    result = system->createSound("D:/GameEngines/ShardEngine/bin/Shard3D/x64/Debug/sigma.mp3", FMOD_DEFAULT, 0, &sound1);
    ERRCHECK(result);


       Common_Update();

        if (Common_BtnPress(BTN_ACTION1))
        {
            result = system->playSound(sound1, 0, false, &channel);
            ERRCHECK(result);
        }   

        result = system->update();
        ERRCHECK(result);

        unsigned int ms = 0;
        unsigned int lenms = 0;
        bool         playing = 0;
        bool         paused = 0;
        int          channelsplaying = 0;

         FMOD::Sound* currentsound = 0;

            result = channel->isPlaying(&playing);
            if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
            {
                ERRCHECK(result);
            }

            result = channel->getPaused(&paused);
            if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
            {
                ERRCHECK(result);
            }

            result = channel->getPosition(&ms, FMOD_TIMEUNIT_MS);
            if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
            {
                ERRCHECK(result);
            }

            channel->getCurrentSound(&currentsound);
            if (currentsound)
            {
                result = currentsound->getLength(&lenms, FMOD_TIMEUNIT_MS);
                if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
                {
                    ERRCHECK(result);
                }
            }
        

        system->getChannelsPlaying(&channelsplaying, NULL);

     

    /*
        Shut down
    */

    result = sound1->release();
    ERRCHECK(result);
    result = system->close();
    ERRCHECK(result);
    result = system->release();
    ERRCHECK(result);

    Common_Close();

    return 0;
}


void FMOD_PlaySound(const char* soundFile)
{
    FMOD::System* system;
    FMOD::Sound* sound1;
    FMOD::Channel* channel = 0;
    FMOD_RESULT       result;
    unsigned int      version;
    void* extradriverdata = 0;

    Common_Init(&extradriverdata);

    /*
        Create a System object and initialize
    */

    result = FMOD::System_Create(&system);
    ERRCHECK(result);

    result = system->getVersion(&version);
    ERRCHECK(result);

    if (version < FMOD_VERSION)
    {
        Common_Fatal("FMOD lib version %08x doesn't match header version %08x", version, FMOD_VERSION);
    }

    result = system->init(32, FMOD_INIT_NORMAL, extradriverdata);
    ERRCHECK(result);

    result = system->createSound(soundFile, FMOD_DEFAULT, 0, &sound1);
    ERRCHECK(result);


    Common_Update();

    if (Common_BtnPress(BTN_ACTION1))
    {
        result = system->playSound(sound1, 0, false, &channel);
        ERRCHECK(result);
    }

    result = system->update();
    ERRCHECK(result);

    unsigned int ms = 0;
    unsigned int lenms = 0;
    bool         playing = 0;
    bool         paused = 0;
    int          channelsplaying = 0;

    FMOD::Sound* currentsound = 0;

    result = channel->isPlaying(&playing);
    if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
    {
        ERRCHECK(result);
    }

    result = channel->getPaused(&paused);
    if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
    {
        ERRCHECK(result);
    }

    result = channel->getPosition(&ms, FMOD_TIMEUNIT_MS);
    if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
    {
        ERRCHECK(result);
    }

    channel->getCurrentSound(&currentsound);
    if (currentsound)
    {
        result = currentsound->getLength(&lenms, FMOD_TIMEUNIT_MS);
        if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
        {
            ERRCHECK(result);
        }
    }


    system->getChannelsPlaying(&channelsplaying, NULL);



    /*
        Shut down
    */

    result = sound1->release();
    ERRCHECK(result);
    result = system->close();
    ERRCHECK(result);
    result = system->release();
    ERRCHECK(result);

    Common_Close();
}
