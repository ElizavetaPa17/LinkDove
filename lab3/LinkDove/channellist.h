#ifndef CHANNELLIST_H
#define CHANNELLIST_H

#include <QWidget>
#include "channelinfo.h"

namespace Ui {
class ChannelList;
}

/**
 * @brief The ChannelList class
 * Отображает список каналов и позволяет создать новый канал.
 */
class ChannelList : public QWidget
{
    Q_OBJECT

public:
    explicit ChannelList(QWidget *parent = nullptr);
    ~ChannelList();

    /**
     * <p> Добавляет информацию о канале в список. </p>
     * @brief addChannel
     * @param channel_info - Структура, содержащая информацию о канале.
     */
    void addChannel(const ChannelInfo &channel_info);

    /**
     * <p> Удаляет все каналы из списка. </p>
     * @brief removeChannels
     */
    void removeChannels();

signals:
    void channelCardClicked();

public slots:
    /**
     * <p> Полностью очищает виджет от объектов ChannelCard. </p>
     * @brief slotClear
     */
    void slotClear();

private slots:
    /**
     * <p> Слот, обратабывающий событие нажатия клавиши Enter. </p>
     * @brief slotsHandleReturnPress
     */
    void slotsHandleReturnPress();

    /**
     * <p> Слот, принимающий от клиента результата поиска данных о канале. </p>
     * @brief slotDisplayFindChannelResult
     * @param result - Результат поиска данных о канале.
     */
    void slotFindChannelResult(int result);

    /**
     * <p> Слот, принимающий события нажатия на виджет результата поиска канала. </p>
     * @brief slotHandleChannelCardClicked
     * @param channel_info
     */
    void slotHandleChannelCardClicked(const ChannelInfo &channel_info);

    /**
     * <p> Слот, обрабатывающий событие нажатие кнопки создания канала. </p>
     * @brief slotCreateChannel
     */
    void slotCreateChannel();

protected:
    // Переопределяем метод отрисовки, т.к. иначе с виджетом
    // не работают css-таблицы (проблема самого  Qt)
    void paintEvent(QPaintEvent *);

private:
    Ui::ChannelList *ui;

    /**
     * <p> Настраивает соединения с классом. </p>
     * @brief setupConnections
     */
    void setupConnection();
};

#endif // CHANNELLIST_H
