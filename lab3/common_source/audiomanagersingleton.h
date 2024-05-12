#ifndef AUDIOMANAGERSINGLETON_H
#define AUDIOMANAGERSINGLETON_H

#include <memory>

#include "audiomanager.h"

class AudioManagerSingleton
{
public:
    AudioManagerSingleton(const AudioManagerSingleton&) = delete;
    AudioManagerSingleton& operator=(const AudioManagerSingleton&) = delete;

    static AudioManager* get_manager() {
        static AudioManagerSingleton instance_;
        return instance_.audio_manager_.get();
    }

private:
    AudioManagerSingleton() {}

    std::shared_ptr<AudioManager> audio_manager_ = std::make_shared<AudioManager>();
};

#endif // AUDIOMANAGERSINGLETON_H
