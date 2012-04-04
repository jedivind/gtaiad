#include <QApplication>
#include <QSqlDatabase>
#include <QMessageBox>
#include <QDebug>
#include <QSqlError>
#include <QFileInfo>
#include <QDialog>

#include "main_dialog.h"
//#include "map_scene.h"

#define DB_FILENAME "klaus.db"

int main(int argc, char* argv[])
{
  QApplication app(argc, argv);

  // Validate an ability to connect to the database and kill the program if it
  // will probably not work.
  //
  // NOTE: Validating the database and having empty QDialog objects like this
  // is not very elegant.  The empty QDialog objects was added because on OSX
  // the "modal" QMessageBox::critical windows would not be modal to the
  // desktop without an underlying window.  So this empty QDialog is raised to
  // the foreground and then hidden so that the critical messagebox can then be
  // in the foreground.
  if (!QSqlDatabase::isDriverAvailable("QSQLITE")) { QDialog empty;

    empty.show();
    empty.raise();
    empty.hide();

    QMessageBox::critical(&empty, "Error",
        "No sqlite3 support.  Unable to connect to the database.");

    return 1;
  }

  if (!QFileInfo(DB_FILENAME).exists())
  {
    QDialog empty;

    empty.show();
    empty.raise();
    empty.hide();

    QMessageBox::critical(&empty, "Error",
        "Database file <i>" DB_FILENAME "</i> not found.");

    return 1;
  }

  QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");

  db.setDatabaseName(DB_FILENAME);

  db.open();

  if (!db.isOpen())
  {
    QDialog empty;

    empty.show();
    empty.raise();
    empty.hide();

    QMessageBox::critical(&empty, "Error",
        QString("Database file <i>" DB_FILENAME "</i> failed to open."
        "<p>Error string: ") + db.lastError().text());

    return 1;
  }

  // Check for an expected table name
  if (!db.tables().contains("capture_locations"))
  {
    QDialog empty;

    empty.show();
    empty.raise();
    empty.hide();

    QMessageBox::critical(&empty, "Error",
        "Expected tables missing from database file <i>" DB_FILENAME "</i>.  "
        "The file may need to be reconstructed.");

    return 1;
  }

  MainDialog* main_dialog = new MainDialog(0);


  main_dialog->show();
  main_dialog->raise();

  // Following may be unnecessary.  On OSX, it is not necessary.
  //app.activeWindow();

  return app.exec();
}
