#ifndef MAIN_DIALOG_H
#define MAIN_DIALOG_H

#include <QDialog>
#include <QGraphicsView>
#include <QPointF>
#include <QStringList>
#include <QSqlDatabase>
#include <QTableWidgetItem>
#include <QPair>
#include <QList>

#include "ui_main_dialog.h"
#include "map_scene.h"

class MainDialog : public QDialog, public Ui::MainDialog
{
  Q_OBJECT

public:
  MainDialog(QWidget* parent = 0, const QSqlDatabase& db = QSqlDatabase());
  void wait(long seconds);

public slots:
  void update_floor_scale(int scaling_factor);   // Set scale to 1/scaling_factor
  void exit_capture_mode(void);
  void update_location_clicked(void);  //when the user clicks update location.
  void capture_location_changed(const QPointF& pos);
  void change_floor(int image_index);

signals:
  void new_capture_canceled(void);
  void new_capture_added(const QPoint&);

private:
  bool validate_loc_id(void);
  void init_floor_scenes(void);

  const QSqlDatabase& m_db;
  char *return_value;
  QStringList m_floor_image_filenames;
  QList<MapScene*> m_map_scenes;
};

#endif
