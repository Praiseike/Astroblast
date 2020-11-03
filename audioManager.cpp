#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H
#include <map>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <thread>

// This class handles loading and playing of audio files.
// For some weird reason SDL2_mixer can't play ogg files
// so for now we're working with wav format instead

class AudioManager
{
    public:
        AudioManager();
        ~AudioManager();
        void registerMusic(const char * filename,const char * id);
        void registerSound(const char * filename,const char * id,int volume);
        void playSound(const char *id);
        void playMusic(const char *id);
        void _playSound(const char *id);
        void _playMusic(const char *id);


        // complain
        void error(void)
        {
            fprintf(stderr,"Error: %s\n",Mix_GetError());
            exit(1);
        }
    protected:
        std::map<const char *,Mix_Chunk *>soundStore;   // Storage for sound effects
        std::map<const char *,Mix_Music *>musicStore;   // Storage for game music
};



AudioManager::AudioManager()
{
    // Check if the SDL_Mixer was initialized
    // if not, do the initialization
    int ret = SDL_WasInit(SDL_INIT_AUDIO);
    if(!(ret&SDL_INIT_AUDIO))
    {
        printf("Audio has been instantiated\n");
        SDL_Init(SDL_INIT_AUDIO);
    }
    if(Mix_Init(MIX_INIT_OGG) < 0)
        error();

    // Open a connection to the audio device
    if(Mix_OpenAudio(22050,MIX_DEFAULT_FORMAT,2,4096) == -1)
        error();

}

// Loads the audio file from the given path in filename
// and stores with a unique id to make reference to in future

void AudioManager::registerMusic(const char * filename,const char * id)
{
    Mix_Music * music = Mix_LoadMUS(filename);
    if(!music)
        error();
    musicStore[id] = music;
}

// Loads the audio file from the given path in filename
// and stores with a unique id to make reference to in future.
// supports volume control
void AudioManager::registerSound(const char * filename,const char *id,int volume = 30)
{
    Mix_Chunk * sound = Mix_LoadWAV(filename);
    if(!sound)
        error();

    Mix_VolumeChunk(sound,volume);
    soundStore[id] = sound;
}


void AudioManager::_playMusic(const char * id)
{
    // try to play the music with the unique id
    // if the id is not found it means the music file wasn't registered

    if(Mix_PlayMusic(musicStore[id],1000000) < 0)
    {
        printf("Unregistered music id detected: %s\n",id);
        exit(1);
    }
}

void AudioManager::_playSound(const char * id)
{
    // try to play the sound effect with the unique id
    // if the id is not found it means the sound effect file wasn't registered

    if(Mix_PlayChannel(2,soundStore[id],0) < 0)
    {
        printf("Unregistered sound effect id detected: %s\n",id);
        exit(1);
    }
}

void AudioManager::playMusic(const char * id)
{
    // Finally. My first multithreaded program
    std::thread music_thread(&AudioManager::_playMusic,this,id);
    //this->_playMusic(id);
    music_thread.join();
}
void AudioManager::playSound(const char * id)
{
    // here too
    std::thread sound_thread(&AudioManager::_playSound,this,id);
    //this->_playSound(id);
    sound_thread.join();
}


AudioManager::~AudioManager()
{
    // Empty music store
    for(std::map<const char *,Mix_Music *>::iterator it = musicStore.begin();it!=musicStore.end();it++)
    {
        Mix_FreeMusic((*it).second);
    }

    // Empty sound store
    for(std::map<const char *,Mix_Chunk *>::iterator it = soundStore.begin();it!=soundStore.end();it++)
    {
        Mix_FreeChunk((*it).second);
    }

    // Finally close the link to the audio devices
    Mix_CloseAudio();
}
#endif // AUDIO_MANAGER_H
