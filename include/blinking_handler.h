#ifndef BLINKING_HANDLER_H
#define BLINKING_HANDLER_H

#include <QObject>
#include <QSerialPort>
#include <QString>

class SetConnection : public QObject {
  Q_OBJECT

 private:
  QSerialPort *serialPort;

  explicit SetConnection(QObject *parent = nullptr);

 public:
  virtual ~SetConnection();

  void sendCommand(const QByteArray &command);
  void setupSerialPort();
  static SetConnection *getInstance();
  SetConnection(const SetConnection &) = delete;
  SetConnection &operator=(const SetConnection &) = delete;

 private slots:
  void readData();

 signals:
  void sw2Pressed();
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
};

#endif  // BLINKING_HANDLER_H
