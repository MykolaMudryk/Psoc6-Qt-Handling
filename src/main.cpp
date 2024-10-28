#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "include/blinking_handler.h"

int main(int argc, char *argv[]) {
  QGuiApplication app(argc, argv);
  QQmlApplicationEngine engine;

  LedHandler *ledHandler = new LedHandler();
  engine.rootContext()->setContextProperty("ledHandler", ledHandler);

  const QUrl url(QStringLiteral("qrc:qml/Main.qml"));

  QObject::connect(
      &engine, &QQmlApplicationEngine::objectCreationFailed, &app,
      []() { QCoreApplication::exit(-1); }, Qt::QueuedConnection);
  engine.load(url);

  return app.exec();
}
