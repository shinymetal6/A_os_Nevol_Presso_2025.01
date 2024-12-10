#ifndef NEVOL_DWTOOL_H
#define NEVOL_DWTOOL_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QFile>
#include <QCoreApplication>
#include <QTextStream>

QT_BEGIN_NAMESPACE
namespace Ui { class Nevol_DwTool; }
QT_END_NAMESPACE
#define WAIT_REPLY              5000

class Nevol_DwTool : public QMainWindow
{
    Q_OBJECT

public:
    Nevol_DwTool(QWidget *parent = nullptr);
    ~Nevol_DwTool();

private slots:
    void on_Port_comboBox_currentTextChanged(const QString &arg1);

    void on_SelectEEFile_pushButton_clicked();

    void on_DownloadEEFile_pushButton_clicked();

    void on_Run_pushButton_clicked();

private:
    Ui::Nevol_DwTool *ui;

    int serial_tx( QByteArray hex_line);
    int serial_rx( void);
    void create_buf_and_tx(char    *data);
    void download_binary(void);

    QSerialPort serial;
    int serial_started;
    unsigned char block_number,csum;
    int packets;
    int packets_errors;
    QString filename;
    QString file_name;
    QString fileversion;
    QByteArray blob;
    int file_size;

    int timer0Id;
    int timerint;

};
#endif // NEVOL_DWTOOL_H
