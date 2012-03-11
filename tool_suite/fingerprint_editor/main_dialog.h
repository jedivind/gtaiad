#ifndef MAIN_DIALOG_H
#define MAIN_DIALOG_H

#include <QDialog>
#include <QGraphicsView>
#include <QPointF>
#include <QStringList>
#include <QSqlDatabase>

#include "ui_main_dialog.h"

class MainDialog : public QDialog, public Ui::MainDialog
{
  Q_OBJECT

public:
  MainDialog(QWidget* parent = 0, const QSqlDatabase& db = QSqlDatabase());

public slots:
  void update_floor_scale(int scaling_factor);   // Set scale to 1/scaling_factor
  void enter_capture_mode(void);
  void exit_capture_mode(void);
  void run_airodump_clicked(void);
  void update_capture_location(const QPointF& pos);
  void insert_location_id_clicked(void);

signals:
  void new_capture_canceled(void);
  void new_capture_added(const QPoint&);

private:
  bool validate_loc_id(void);

  const QSqlDatabase& m_db;
  QStringList m_map_diagrams;
};

#endif
