#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <QMediaRecorder>
#include <QAudioInput>
#include <QAudioOutput>
#include <QMediaCaptureSession>
#include <QMediaPlayer>
#include <QObject>

class AudioManager : public QObject
{
    Q_OBJECT

public:
    AudioManager();

    /**
     * <p> Начинает запись аудио с микрофона и устанавливает путь к сохраняемому аудио. </p>
     * @brief start_recording
     * @param output - Путь, куда сохранится аудио. ОТНОСИТЕЛЬНО ЛОКАЛЬНОГО ХРАНИЛИЩА АУДИО! В ЛИНУКС ЭТО ПАПКА ~/Music! (по-другому QUrl файлы не сохраняет)
     */
    void start_recording(const QString &output);

    /**
     * <p> Останавливает запись аудио с микрофона и сохраняет .m4a файл по пути, переданному ранее. </p>
     * @brief stop_recording
     */
    void stop_recording();

    /**
     * <p> Проигрывает аудио по заданному пути. </p>
     * @brief play
     * @param input - Путь к аудио.
     */
    void play(const QString &input);

    /**
     * <p> Останавливает воспроизведение аудио. </p>
     * @brief stop
     */
    void stop();

    /**
     * <p> Возвращает статус проигрывания аудио. </p>
     * @brief is_playing
     * @return - Возвращает true, если аудио проигрывается, иначе - false.
     */
    bool is_playing();

signals:
    /**
     * <p> Генерирует сигнал, когда текущее аудио было полностью проиграно. </p>
     * @brief playing_stopped
     */
    void playing_stopped();

private:
    QMediaCaptureSession session_;
    QAudioInput audio_input_;
    QAudioOutput audio_output_;
    QMediaRecorder recorder_;
    QMediaPlayer player_;

    /**
     * <p> Настраивает работу с аудио. </p>
     * @brief setup
     */
    void setup();
};

#endif // AUDIOMANAGER_H
