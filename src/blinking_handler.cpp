#include "include/blinking_handler.h"

#include "QtCore/qdebug.h"

ConnectionHandler::ConnectionHandler(QObject *parent) : QObject(parent) {
  serialPort = new QSerialPort(this);

  connect(serialPort, &QSerialPort::readyRead, this,
          &ConnectionHandler::readData);
}

ConnectionHandler::~ConnectionHandler() {
  if (serialPort->isOpen()) {
    serialPort->close();
  }
}

void ConnectionHandler::sendCommand(const QByteArray &command) {
  if (serialPort->isOpen()) {
    qint64 bytesWritten = serialPort->write(command);

    qDebug() << "Sending command:" << command;

    if (bytesWritten == -1) {
      qDebug() << "Failed to send command:" << serialPort->errorString();
    }
  }
}

void ConnectionHandler::setupSerialPort(QString port) {
  serialPort->setPortName(port);
  serialPort->setBaudRate(QSerialPort::Baud115200);

  if (serialPort->open(QIODevice::ReadWrite)) {
    qDebug() << "Serial port opened successfully";
  } else {
    qDebug() << "Failed to open serial port" << serialPort->errorString();
  }
}

QStringList ConnectionHandler::getAvailablePorts() {
  QStringList portList;
  QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();

  for (QSerialPortInfo &port : ports) {
    portList.append(port.portName());
  }
  return portList;
}

void ConnectionHandler::readData() {
  QByteArray data = serialPort->readLine();
  buffer.append(data);

  int newlineIndex;
  QString lastReceivedString;

  while ((newlineIndex = buffer.indexOf('\n')) != -1) {
    QByteArray line = buffer.left(newlineIndex);

    buffer.remove(0, newlineIndex + 1);

    lastReceivedString = QString::fromUtf8(line).trimmed();

    emit commandReceived(lastReceivedString);

    qDebug() << "Received data on ConnectionHandler:" << lastReceivedString;
  }
}

ConnectionHandler &ConnectionHandler::getInstance() {
  static ConnectionHandler instance;
  return instance;
}

LedHandler::LedHandler(QObject *parent)
    : QObject(parent), connection(ConnectionHandler::getInstance()) {
  connect(&connection, &ConnectionHandler::commandReceived, this,
          &LedHandler::handleUartCommands);
}

void LedHandler::setOn() {
  command = "On";
  connection.sendCommand(command.toUtf8());
}

void LedHandler::setOff() {
  command = "Off";
  connection.sendCommand(command.toUtf8());
}

void LedHandler::setBlink() {
  command = "Blink";
  connection.sendCommand(command.toUtf8());
}

void LedHandler::setBlinkFrequency(const int &frequency) {
  QString freqString = QString("FREQ%1Z").arg(frequency);
  QByteArray command = freqString.toUtf8();
  connection.sendCommand(command);
}

void LedHandler::setBrightness(const float &brightness) {
  QString brightnessString =
      QString("BRIGHT%1Z").arg(QString::number(brightness, 'f', 1));
  // qDebug() << "Sending command:" << command;
  QByteArray command = brightnessString.toUtf8();
  connection.sendCommand(command);
}

void LedHandler::handleUartCommands(QString &command) {
  if (command == "sw2") {
    emit sw2Pressed();
  }
}
