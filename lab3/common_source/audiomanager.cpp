#include "audiomanager.h"

#include <iostream>

AudioManager::AudioManager() {
    setup();
}

void AudioManager::start_recording(const QString &output) {
    recorder_.setOutputLocation(QUrl::fromLocalFile(output));
    recorder_.record();
}

void AudioManager::stop_recording() {
    recorder_.stop();
}

void AudioManager::play(const QString &input) {
    player_.setSource(QUrl::fromLocalFile(input));
    player_.play();
}

void AudioManager::setup() {
    session_.setAudioInput(&audio_input_);
    session_.setRecorder(&recorder_);

    audio_output_.setVolume(50);
    player_.setAudioOutput(&audio_output_);

    recorder_.setQuality(QMediaRecorder::HighQuality);
}
