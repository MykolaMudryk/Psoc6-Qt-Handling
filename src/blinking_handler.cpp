#include "include/blinking_handler.h"

#include "QtCore/qdebug.h"

SetConnection::SetConnection(QObject *parent) : QObject(parent) {
  serialPort = new QSerialPort(this);
  setupSerialPort();
  connect(serialPort, &QSerialPort::readyRead, this, &SetConnection::readData);
}

SetConnection::~SetConnection() {
  if (serialPort->isOpen()) {
    serialPort->close();
  }
}

void SetConnection::sendCommand(const QByteArray &command) {
  if (serialPort->isOpen()) {
    qint64 bytesWritten = serialPort->write(command + "\r\n");

    qDebug() << "Sending command:" << command;

    if (bytesWritten == -1) {
      qDebug() << "Failed to send command:" << serialPort->errorString();
    }
  }
}

void SetConnection::setupSerialPort() {
  serialPort->setPortName("/dev/tty.usbmodem14203");
  serialPort->setBaudRate(
      QSerialPort::Baud115200);  // або інший значущий baudrate

  if (serialPort->open(QIODevice::ReadWrite)) {
    qDebug() << "Serial port opened successfully";
  } else {
    qDebug() << "Failed to open serial port" << serialPort->errorString();
  }
}

void SetConnection::readData() {
  QByteArray data = serialPort->readLine();
  // qDebug() << "Received data:" << data;

  QString receivedString = QString::fromUtf8(data);

  if (receivedString.trimmed() == "sw2") {
    emit sw2Pressed();
  }
}

SetConnection *SetConnection::getInstance() {
  static SetConnection instance;
  return &instance;
}

LedHandler::LedHandler(QObject *parent) : QObject(parent) {
  connection = SetConnection::getInstance();
}

void LedHandler::setOn() {
  command = "On";
  connection->sendCommand(command.toUtf8());
}

void LedHandler::setOff() {
  command = "Off";
  connection->sendCommand(command.toUtf8());
}

void LedHandler::setBlink() {
  command = "Blink";
  connection->sendCommand(command.toUtf8());
}

void LedHandler::setBlinkFrequency(const int &frequency) {
  QString freqString = QString("FREQ%1Z").arg(frequency, 4, 10, QChar('0'));
  QByteArray command = freqString.toUtf8();
  connection->sendCommand(command);
}
