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
        }
        else
        {
            ui->Comm_label->setPixmap(greenled);
            serial_started = 1;
            qDebug()<< "Serial port opened";
            ui->statusbar->showMessage(arg1+" : Serial port opened");
            serial.setReadBufferSize (1024);
        }
    }
    else
    {
        ui->Comm_label->setPixmap(redled);
        qDebug()<< arg1 << " : " << serial.errorString();
        ui->statusbar->showMessage(arg1+" : "+serial.errorString());
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

void Nevol_DwTool::download_binary(void)
{
    QPixmap redled (":/ledred.png");
    QPixmap greenled(":/ledgreen.png");
    char    data[132];
    QByteArray reply;
    int i,retry=10,rx_data;
    int s_unit;
    int index=0;

    ui->statusbar->showMessage("Downloading "+file_name);
    block_number = 1;
    csum = 0;
    ui->Flashing_label->setPixmap(redled);
    s_unit = file_size/100;
    ui->download_progressBar->setValue(0);

    qDebug()<< "Awaiting Poll";
    while ( serial_rx() != 0x15 )
    {
        ui->statusbar->showMessage("Retry");
        qDebug()<<"Retry on 0x15";
        retry--;
        if ( retry == 0 )
        {
            ui->statusbar->showMessage(file_name+" aborted download");
            qDebug()<<file_name<<" aborted download";
            return;
        }
    }

    qDebug()<<"Poll received , downloading";
    index=0;
    while ( index < file_size)
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
            if ( index < file_size )
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
                ui->statusbar->showMessage(file_name+" aborted download");
                qDebug()<<file_name<<" aborted download";
                return;
            }
        }
        ui->download_progressBar->setValue(index/s_unit);
    }
    data[0] = 0x04;
    QByteArray ba1(QByteArray::fromRawData(data, 1));
    serial_tx(ba1);
    ui->statusbar->showMessage(file_name+" downloaded");
    ui->download_progressBar->setValue(100);
    qDebug()<<file_name<<" downloaded";
    ui->Flashing_label->setPixmap(greenled);
}


void Nevol_DwTool::on_SelectEEFile_pushButton_clicked()
{
    filename = QFileDialog::getOpenFileName(this, tr("Open EE File"), "/Devel/Stm32_16.1_A_os_2025.01-rc/A_os_Nevol_Presso/PressoMaster");
    ui->label_FILE->setText(filename);
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
        qDebug()<<"File not found";
    else
    {
        const QFileInfo info(filename);
        const QString ffname(info.fileName());
        file_name = ffname;
        qDebug()<<file_name;
        file_size = file.size();
        blob = file.readAll();
        file.close();
    }
}


void Nevol_DwTool::on_DownloadEEFile_pushButton_clicked()
{
    serial.flush();
    QString cmd = "< PRG "+ui->ProgramNumber_comboBox->currentText()+" >";
    serial_tx(cmd.toUtf8());
    download_binary();
}


void Nevol_DwTool::on_Run_pushButton_clicked()
{
    if ( ui->Run_pushButton->text() == "HALT")
    {
        serial.flush();
        QString cmd = "< HLT "+ui->RunProgramNumber_comboBox->currentText()+" >";
        serial_tx(cmd.toUtf8());
        ui->Run_pushButton->setText("RUN");
    }
    else
    {
        serial.flush();
        QString cmd = "< RUN "+ui->RunProgramNumber_comboBox->currentText()+" >";
        serial_tx(cmd.toUtf8());
        ui->Run_pushButton->setText("HALT");
    }

}

