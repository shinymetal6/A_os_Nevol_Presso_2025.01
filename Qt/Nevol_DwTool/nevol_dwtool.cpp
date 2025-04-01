#include "nevol_dwtool.h"
#include "ui_nevol_dwtool.h"
#include <QApplication>
#include <QDebug>
#include <QTextStream>
#include <QDir>
#include <QDateTime>
#include <QFile>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QFile>
#include <QFileDialog>
#include <QCoreApplication>
#include <QTextStream>
#include <QThread>

Nevol_DwTool::Nevol_DwTool(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Nevol_DwTool)
{
    ui->setupUi(this);
    ui->frame->setEnabled(false);
}

int Nevol_DwTool::serial_tx( QByteArray hex_line)
{
QPixmap redled (":/ledred.png");
QPixmap greenled(":/ledgreen.png");
QByteArray reply;

    if ( serial_started == 0 )
        return 1;
    //QThread::msleep(10);
    serial.write(hex_line);
    //QThread::msleep(50);
    return 1;
}

int Nevol_DwTool::serial_rx( void)
{
QPixmap redled (":/ledred.png");
QPixmap greenled(":/ledgreen.png");
QByteArray reply;

    //QThread::msleep(10);
    if(serial.waitForReadyRead(WAIT_REPLY))
    {
        reply = serial.readAll();
        const char *data = reply.data();
        return data[0];
    }
    qDebug()<< "RX timeout";
    return 0x41;
}

Nevol_DwTool::~Nevol_DwTool()
{
    delete ui;
}

void Nevol_DwTool::on_Port_comboBox_currentTextChanged(const QString &arg1)
{
    QPixmap redled (":/ledred.png");
    QPixmap greenled(":/ledgreen.png");

    serial.close();
    qDebug()<< arg1;
    if ( arg1 == "Invalid")
    {
        ui->statusbar->showMessage("Serial port closed");
    }
    serial_started = 0;
    serial.setPortName(arg1);
    if(serial.open(QIODevice::ReadWrite))
    {
        if(!serial.setBaudRate(QSerialPort::Baud115200))
        {
            ui->Comm_label->setPixmap(redled);
            qDebug()<< arg1 << " : " << serial.errorString();
            ui->statusbar->showMessage(arg1+" : "+serial.errorString());
            ui->frame->setEnabled(false);
        }
        else
        {
            ui->Comm_label->setPixmap(greenled);
            serial_started = 1;
            qDebug()<< "Serial port opened";
            ui->statusbar->showMessage(arg1+" : Serial port opened");
            serial.setReadBufferSize (1024);
            ui->frame->setEnabled(true);
        }
    }
    else
    {
        ui->Comm_label->setPixmap(redled);
        qDebug()<< arg1 << " : " << serial.errorString();
        ui->statusbar->showMessage(arg1+" : "+serial.errorString());
        ui->frame->setEnabled(false);
    }
}

void Nevol_DwTool:: create_buf_and_tx(char    *data)
{
#define SLEEP_HERE   10
    QByteArray ba4(QByteArray::fromRawData(data, 132));
    serial.flush();
    serial_tx(ba4);
    //QThread::msleep(SLEEP_HERE);
}

void Nevol_DwTool::download_program(void)
{
    QPixmap redled (":/ledred.png");
    QPixmap greenled(":/ledgreen.png");
    char    data[132];
    QByteArray reply;
    int i,retry=10,rx_data;
    int s_unit;
    int index=0;

    ui->statusbar->showMessage("Downloading "+program_file_name);
    block_number = 1;
    csum = 0;
    ui->FlashingEE_label->setPixmap(redled);
    s_unit = program_file_size/100;
    ui->downloadEE_progressBar->setValue(0);

    qDebug()<< "Awaiting Poll";
    while ( serial_rx() != 0x15 )
    {
        ui->statusbar->showMessage("Retry");
        qDebug()<<"Retry on 0x15";
        retry--;
        if ( retry == 0 )
        {
            ui->statusbar->showMessage(program_file_name+" aborted download");
            qDebug()<<program_file_name<<" aborted download";
            return;
        }
    }

    qDebug()<<"Poll received , downloading";
    index=0;
    while ( index < program_file_size)
    {
        retry=10;
        data[0] = 0x01;
        data[1] = block_number;
        data[2] = 255 - block_number;
        block_number++;
        if ( block_number == 0 )
            block_number = 1;
        csum = 0;
        for(i=0;i<128;i++,index++)
        {
            if ( index < program_file_size )
                data[i+3] = blob[index];
            else
                data[i+3] = 0;
            csum += data[i+3];
        }
        data[131] = csum;

        serial.flush();
        create_buf_and_tx(data);
        serial.flush();
        while ( (rx_data = serial_rx()) != 0x06 )
        {
            ui->statusbar->showMessage("Retry");
            qDebug()<<"Retry on Ack, block_number "<< block_number<<" data "<<rx_data;
            serial.flush();
            create_buf_and_tx(data);
            retry--;
            if ( retry == 0 )
            {
                ui->statusbar->showMessage(program_file_name+" aborted download");
                qDebug()<<program_file_name<<" aborted download";
                return;
            }
        }
        ui->downloadEE_progressBar->setValue(index/s_unit);
    }
    data[0] = 0x04;
    QByteArray ba1(QByteArray::fromRawData(data, 1));
    serial_tx(ba1);
    ui->statusbar->showMessage(program_file_name+" downloaded");
    ui->downloadEE_progressBar->setValue(100);
    qDebug()<<program_file_name<<" downloaded";
    ui->FlashingEE_label->setPixmap(greenled);
}


void Nevol_DwTool::on_SelectEEFile_pushButton_clicked()
{
    QString filters = "CSV files (*.csv)";
    program_filename = QFileDialog::getOpenFileName(this, tr("Open EE File"), "/Devel/Stm32_16.1_A_os_2025.04-rc/Nevol_Presso_Processes/Resources/Programs",filters);

    const QFileInfo info(program_filename);
    const QString program_basename(info.fileName());

    ui->label_FILE->setText(program_basename);
    QFile file(program_filename);
    if (!file.open(QIODevice::ReadOnly))
        qDebug()<<"File not found";
    else
    {
        const QFileInfo info(program_filename);
        const QString ffname(info.fileName());
        program_file_name = ffname;
        qDebug()<<program_file_name;
        program_file_size = file.size();
        blob = file.readAll();
        file.close();
    }
}


void Nevol_DwTool::on_DownloadEEFile_pushButton_clicked()
{
QString cmd;
    serial.flush();
    if ( ui->ProgramNumber_comboBox->currentText() == "Opening")
        cmd = "< PRG 0 >";
    else if ( ui->ProgramNumber_comboBox->currentText() == "Closing")
        cmd = "< PRG 31 >";
    else
        cmd = "< PRG "+ui->ProgramNumber_comboBox->currentText()+" >";
    serial_tx(cmd.toUtf8());
    download_program();
}


void Nevol_DwTool::on_Run_pushButton_clicked()
{
QString cmd;
    if ( ui->Run_pushButton->text() == "HALT")
    {
        serial.flush();
        if ( ui->RunProgramNumber_comboBox->currentText() == "Opening")
            cmd = "< HLT 0 >";
        else if ( ui->RunProgramNumber_comboBox->currentText() == "Closing")
            cmd = "< HLT 31 >";
        else
            cmd = "< HLT "+ui->RunProgramNumber_comboBox->currentText()+" >";
        serial_tx(cmd.toUtf8());
        ui->Run_pushButton->setText("RUN");
    }
    else
    {
        serial.flush();
        if ( ui->RunProgramNumber_comboBox->currentText() == "Opening")
            cmd = "< RUN 0 >";
        if ( ui->RunProgramNumber_comboBox->currentText() == "Closing")
            cmd = "< RUN 31 >";
        else
            cmd = "< RUN "+ui->RunProgramNumber_comboBox->currentText()+" >";

        serial_tx(cmd.toUtf8());
        ui->Run_pushButton->setText("HALT");
    }

}


void Nevol_DwTool::on_SelectProgram_pushButton_clicked()
{

}

void Nevol_DwTool::on_SelectAudioFile_pushButton_clicked()
{
    QString filters = "WAV files (*.wav)";
    audio_filename = QFileDialog::getOpenFileName(this, tr("Open WAV File"), "/Devel/Stm32_16.1_A_os_2025.04-rc/Nevol_Presso_Processes/Resources/Audio",filters);

    const QFileInfo info(audio_filename);
    const QString audio_basename(info.fileName());

    ui->label_AUDIO->setText(audio_basename);

    QFile file(audio_filename);
    if (!file.open(QIODevice::ReadOnly))
        qDebug()<<"File not found";
    else
    {
        const QFileInfo info(audio_filename);
        const QString ffname(info.fileName());
        audio_file_name = ffname;
        qDebug()<<audio_file_name;
        audio_file_size = file.size();
        blob = file.readAll();
        file.close();
    }
}


void Nevol_DwTool::on_DownloadAudioFile_pushButton_clicked()
{
    serial.flush();
    QString cmd = "< WAV "+ui->AudioNumber_comboBox->currentText()+" >";
    serial_tx(cmd.toUtf8());
    download_audio();
}

void Nevol_DwTool::download_audio(void)
{
    QPixmap redled (":/ledred.png");
    QPixmap greenled(":/ledgreen.png");
    char    data[132];
    QByteArray reply;
    int i,retry=10,rx_data;
    int s_unit;
    int index=0;

    ui->statusbar->showMessage("Downloading "+audio_file_name);
    block_number = 1;
    csum = 0;
    ui->FlashingAudio_label->setPixmap(redled);
    s_unit = audio_file_size/100;
    ui->downloadAudio_progressBar->setValue(0);

    qDebug()<< "Awaiting Poll";
    while ( serial_rx() != 0x15 )
    {
        ui->statusbar->showMessage("Retry");
        qDebug()<<"Retry on 0x15";
        retry--;
        if ( retry == 0 )
        {
            ui->statusbar->showMessage(audio_file_name+" aborted download");
            qDebug()<<audio_file_name<<" aborted download";
            return;
        }
    }

    qDebug()<<"Poll received , downloading";
    index=0;
    while ( index < audio_file_size)
    {
        retry=10;
        data[0] = 0x01;
        data[1] = block_number;
        data[2] = 255 - block_number;
        block_number++;
        if ( block_number == 0 )
            block_number = 1;
        csum = 0;
        for(i=0;i<128;i++,index++)
        {
            if ( index < audio_file_size )
                data[i+3] = blob[index];
            else
                data[i+3] = 0;
            csum += data[i+3];
        }
        data[131] = csum;

        serial.flush();
        create_buf_and_tx(data);
        serial.flush();
        while ( (rx_data = serial_rx()) != 0x06 )
        {
            ui->statusbar->showMessage("Retry");
            qDebug()<<"Retry on Ack, block_number "<< block_number<<" data "<<rx_data;
            serial.flush();
            create_buf_and_tx(data);
            retry--;
            if ( retry == 0 )
            {
                ui->statusbar->showMessage(audio_file_name+" aborted download");
                qDebug()<<audio_file_name<<" aborted download";
                return;
            }
        }
        ui->downloadAudio_progressBar->setValue(index/s_unit);
    }
    data[0] = 0x04;
    QByteArray ba1(QByteArray::fromRawData(data, 1));
    serial_tx(ba1);
    int res = serial_rx() != 0x15 ;
    qDebug()<<res<<" received";
    if ( res == 1)
    {
        ui->statusbar->showMessage(audio_file_name+" downloaded");
        ui->FlashingAudio_label->setPixmap(greenled);
    }
    else
    {
        ui->statusbar->showMessage(audio_file_name+" failed to download");
        ui->FlashingAudio_label->setPixmap(redled);
    }

    ui->downloadAudio_progressBar->setValue(100);
    qDebug()<<audio_file_name<<" downloaded";
}



void Nevol_DwTool::on_Play_pushButton_clicked()
{
    //
    if ( ui->Play_pushButton->text() == "STOP")
    {
        serial.flush();
        QString cmd = "< MUT "+ui->PlayAudioNumber_comboBox->currentText()+" >";
        serial_tx(cmd.toUtf8());
        ui->Play_pushButton->setText("PLAY");
    }
    else
    {
        serial.flush();
        QString cmd = "< PLY "+ui->PlayAudioNumber_comboBox->currentText()+" >";
        serial_tx(cmd.toUtf8());
        ui->Play_pushButton->setText("STOP");
    }
}

void Nevol_DwTool::on_PlaySound_pushButton_clicked()
{
    serial.flush();

    QString cmd = "< SND "+ui->PlaySoundNumber_comboBox->currentText()+" >";
    serial_tx(cmd.toUtf8());
}

void Nevol_DwTool::on_Motor_pushButton_clicked()
{
    if ( ui->Motor_pushButton->text() == "Motor ON")
    {
        serial.flush();
        QString cmd = "< TMT 1 >";
        serial_tx(cmd.toUtf8());
        ui->Motor_pushButton->setText("Motor OFF");
    }
    else
    {
        serial.flush();
        QString cmd = "< TMT 0 >";
        serial_tx(cmd.toUtf8());
        ui->Motor_pushButton->setText("Motor ON");
    }
}

void Nevol_DwTool::on_Open_1_pushButton_clicked()
{
    if ( ui->Open_1_pushButton->text() == "Open 1")
    {
        serial.flush();
        QString cmd = "< TOP 1 >";
        serial_tx(cmd.toUtf8());
        ui->Open_1_pushButton->setText("Close 1");
        qDebug()<<audio_file_name<<" Open 1";

    }
    else
    {
        serial.flush();
        QString cmd = "< TOP 0 >";
        serial_tx(cmd.toUtf8());
        ui->Open_1_pushButton->setText("Open 1");
        qDebug()<<audio_file_name<<" Close 1";
    }
}
