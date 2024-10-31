#ifndef BLINKING_HANDLER_H
#define BLINKING_HANDLER_H

#include <QObject>
#include <QSerialPort>
#include <QString>

class LedHandler;

class SetConnection : public QObject {
  Q_OBJECT

 private:
  QSerialPort *serialPort;
  LedHandler *ledHandler;

  QByteArray buffer;

  explicit SetConnection(QObject *parent = nullptr);

 public:
  ~SetConnection();

  void sendCommand(const QByteArray &command);
  void setupSerialPort();

  static SetConnection *getInstance();
  SetConnection(const SetConnection &) = delete;
  SetConnection &operator=(const SetConnection &) = delete;

 public slots:
  void readData();

 signals:
  void commandReceived(QString &receivedComamnd);
};

class LedHandler : public QObject {
  Q_OBJECT
 private:
  SetConnection *connection;
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

#endif  // BLINKING_HANDLER_H
