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
  void fp_update_floor_scale(int scaling_factor);   // Set scale to 1/scaling_factor
  void fp_start_new_capture_mode(void);
  void fp_perform_ap_capture(void);
  void fp_update_capture_location(const QPointF& pos);
  void fp_cancel_new_capture(void);
  void fp_insert_loc_id(void);

  void as_update_floor_scale(int scaling_factor);   // Set scale to 1/scaling_factor

signals:
  void fp_new_capture_canceled(void);
  void fp_new_capture_added(const QPoint&);

private:
  bool fp_validate_loc_id(void);

  const QSqlDatabase& m_db;
  QStringList m_map_diagrams;
  QGraphicsView* m_fp_map_view;
  QGraphicsView* m_fp_map_view_2;
};

#endif
