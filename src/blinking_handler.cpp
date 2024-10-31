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
    qint64 bytesWritten = serialPort->write(command);

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
  buffer.append(data);

  int newlineIndex;
  QString lastReceivedString;

  while ((newlineIndex = buffer.indexOf('\n')) != -1) {
    QByteArray line = buffer.left(newlineIndex);

    buffer.remove(0, newlineIndex + 1);

    lastReceivedString = QString::fromUtf8(line).trimmed();

    emit commandReceived(lastReceivedString);

    qDebug() << "Received data on SetConnection:" << lastReceivedString;
  }
}

SetConnection *SetConnection::getInstance() {
  static SetConnection instance;
  return &instance;
}

LedHandler::LedHandler(QObject *parent) : QObject(parent) {
  connection = SetConnection::getInstance();
  connect(connection, &SetConnection::commandReceived, this,
          &LedHandler::handleUartCommands);
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
  QString freqString = QString("FREQ%1Z").arg(frequency);
  QByteArray command = freqString.toUtf8();
  connection->sendCommand(command);
}

void LedHandler::setBrightness(const float &brightness) {
  QString brightnessString =
      QString("BRIGHT%1Z").arg(QString::number(brightness, 'f', 1));
  // qDebug() << "Sending command:" << command;
  QByteArray command = brightnessString.toUtf8();
  connection->sendCommand(command);
}

void LedHandler::handleUartCommands(QString &command) {
  if (command == "sw2") {
    emit sw2Pressed();
  }
}
