#include "audiomanager.h"

#include <iostream>

AudioManager::AudioManager()
    : QObject()
{
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
    stop();
    player_.setSource(QUrl::fromLocalFile(input));
    player_.setPosition(0);
    player_.play();
}

void AudioManager::stop() {
    player_.stop();
}

bool AudioManager::is_playing() {
    return player_.isPlaying();
}

void AudioManager::setup() {
    session_.setAudioInput(&audio_input_);
    session_.setRecorder(&recorder_);

    audio_output_.setVolume(50);
    player_.setAudioOutput(&audio_output_);

    recorder_.setQuality(QMediaRecorder::HighQuality);

    connect(&player_, &QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status) {
                                                                        std::cerr << status << '\n';
                                                                        std::cerr << "ERR: " << player_.errorString().toStdString() << '\n';
                                                                        if (status == QMediaPlayer::EndOfMedia) {
                                                                            emit playing_stopped();
                                                                        }
                                                                       });
}
