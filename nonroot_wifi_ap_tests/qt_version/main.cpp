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

  while (i.hasNext())
  {
    qDebug() << "----";
    qDebug() << "Name: " << active_configs.at(0).name();
    i.next();
  }


  ////qDebug() << info->currentMode();


  return test.exec();
}
