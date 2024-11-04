#pragma once

#include <QObject>
#include <QSerialPort>
#include <QString>
#include <QtSerialPort/QSerialPortInfo>

#include "blinking_handler.h"
class LedHandler;

class ConnectionHandler : public QObject {
  Q_OBJECT

 private:
  QSerialPort *serialPort;
  LedHandler *ledHandler;

  QByteArray buffer;

  explicit ConnectionHandler(QObject *parent = nullptr);

 public:
  ~ConnectionHandler();

  void sendCommand(const QByteArray &command);
  Q_INVOKABLE void setupSerialPort(QString port);
  Q_INVOKABLE QStringList getAvailablePorts();

  static ConnectionHandler &getInstance();
  ConnectionHandler(const ConnectionHandler &) = delete;
  ConnectionHandler &operator=(const ConnectionHandler &) = delete;

 public slots:
  void readData();

 signals:
  void commandReceived(QString &receivedComamnd);
};

class LedHandler : public QObject {
  Q_OBJECT
 private:
  ConnectionHandler &connection;
  QString command;

 public:
  explicit LedHandler(QObject *parent = nullptr);

  Q_INVOKABLE void setOn();
  Q_INVOKABLE void setOff();
  Q_INVOKABLE void setBlink();
  Q_INVOKABLE void setBlinkFrequency(const int &frequency);
  Q_INVOKABLE void setBrightness(const float &brightness);

 public slots:
  void handleUartCommands(QString &command);

 signals:
  void sw2Pressed();
};
