#include <QDebug>
#include <QtCore/QCoreApplication>
#include <QNetworkConfiguration>
#include <QNetworkConfigurationManager>
#include <QTimer>
#include <QList>

// Mobility stuff
//#include <QSystemNetworkInfo>

int main(int argc, char* argv[])
{
  QCoreApplication test(argc, argv);
  //QtMobility::QSystemNetworkInfo* info = new QtMobility::QSystemNetworkInfo(&test);
  QNetworkConfigurationManager mgr;

  //QTimer::singleShot(5000, &test, SLOT(quit()));
  QTimer::singleShot(100, &test, SLOT(quit()));

  QList<QNetworkConfiguration> active_configs = mgr.allConfigurations(QNetworkConfiguration::Active);

  // iterate through list
  QListIterator<QNetworkConfiguration> i(active_configs);

  // see: http://doc.qt.nokia.com/qtmobility-1.0-tp/bearer-management.html#bearer-cloud-example
  qDebug() << "Network Configurations: " << active_configs.count();
  QNetworkConfigurationManager wireless_connection;

  while (i.hasNext())
  {
    QNetworkConfiguration conn;

    conn = i.next();
    qDebug() << "----";
    qDebug() << "Name: " << conn.name();
    qDebug() << "BearerType: " << conn.bearerType();
    qDebug() << "BearerTypeName: " << conn.bearerTypeName();
  }


  ////qDebug() << info->currentMode();


  return test.exec();
}
