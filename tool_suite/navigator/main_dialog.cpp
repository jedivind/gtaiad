#include <assert.h>

#include <QDebug>
#include <QMessageBox>
#include <QPixmap>
#include <QPointF>
#include <QSqlQuery>

#include "median.cpp"
//#include "parse.cpp"  will not need this since there is the DB to read from.
#include "main_dialog.h"
//#include "map_scene.h"

MainDialog::MainDialog(QWidget* parent, const QSqlDatabase& db) : QDialog(parent),
    m_db(db), m_floor_image_filenames(QStringList())
{
  MapScene* map_scene = new MapScene;

  assert(db.isOpen());

  setupUi(this);
  m_floor_image_filenames << "../level_one.jpg";
  m_floor_image_filenames << "../level_two.jpg";
  m_floor_image_filenames << "../level_three.jpg";

  init_floor_scenes();

  change_floor(0);

  update_floor_scale(zoom_slider->value());

#ifdef Q_WS_MAC
  map_view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
#endif

  // Remove dialog example data
  loc_id_line_edit->clear();
  x_pos_label->clear();
  y_pos_label->clear();

}

// Slot called when the zoom slider position is changed by the end user.
void MainDialog::update_floor_scale(int scaling_factor)
{
  map_view->resetTransform();
  map_view->scale(100.0/scaling_factor, 100.0/scaling_factor);
}


void MainDialog::update_location_clicked(void)
{
  if (x_pos_label->text().isEmpty() && y_pos_label->text().isEmpty())
  {
    //call code to run airodump and get the present location data.feed it into lily's code.
    //call lily's code and other map refreshing code.
   calculate_position_from_data(get_measurement_locations); 
    update_location_changed();
    QMessageBox::warning(this, "Location",
        "Updating for the first time.");
    return;
  }

  if (!validate_loc_id())
  {
    return;
  }

  // TODO: perform AP capture magic
  // TODO: perform Database magic

  //emit new_capture_added(new_capture_location);
}

// slot: called when user clicks on the floor map for red dot placement
void MainDialog::update_location_changed(const QPointF& pos)
{
  // If the location was updated and x and y have some value but now after some time
  // presses the update again .
  // TODO: Update enables of buttons when loc ID is entered
    MapScene* map_scene;

    map_scene = (MapScene*)(map_view->scene());
    map_scene->clear_marker_placement();

  QPoint int_pos = pos.toPoint();  // does proper rounding

  x_pos_label->setText(QString::number(int_pos.x()));
  y_pos_label->setText(QString::number(int_pos.y()));
}

// Do necessary clearing of widgets and enable/disables.need to design appropriate widgets first.
/*void MainDialog::exit_capture_mode(void)
{
  x_pos_label->clear();
  y_pos_label->clear();
  new_capture_groupbox->setEnabled(false);
  new_capture_push_button->setEnabled(true);
  floor_combobox->setEnabled(true);

  emit new_capture_canceled();
}*/

// Generate the MapScene object for each floor map.  Store the scenes in
// the QList<T> generic container.
void MainDialog::init_floor_scenes(void)
{
  // assumed order is 1, 2, 3
  foreach (const QString& filename, m_floor_image_filenames)
  {
    MapScene* map_scene = new MapScene;
    QPixmap floor_pixmap(filename);
    map_scene->addPixmap(floor_pixmap);

    m_map_scenes.append(map_scene);

    // Connect signals of MapScene
    QObject::connect(map_scene, SIGNAL(location_set(const QPointF&)),
        this, SLOT(capture_location_changed(const QPointF&)));
  }
}

// read from the database all of a floor's loc_ids and xpos,ypos information
QList< QPair<QString, QPointF> > MainDialog::get_measurement_locations(int floor_number)
{
  QSqlQuery q(m_db);
  QList< QPair<QString, QPointF> > rvalue;

  q.prepare(" \
      SELECT \
        capture_location_name, capture_location_x_pos, capture_location_y_pos \
      FROM capture_locations \
      WHERE capture_location_floor = :capture_location_floor");

  q.bindValue(":capture_location_floor", floor_number);

  bool res = q.exec();

  if (!res)
  {
    QMessageBox::warning(this, "Database Error",
        "Failed during SELECT from capture_locations table.");
    return rvalue;
  }

  while (q.next())
  {
    QPair<QString, QPointF> tuple;

    tuple.first = q.value(0).toString();
    tuple.second = QPointF(q.value(1).toFloat(), q.value(2).toFloat());

    rvalue.append(tuple);
  }

  return rvalue;
}


// slot: called when floor is changed.that is when the user clicks update_position again then refresh the floor if it has changed.
void MainDialog::change_floor(int image_index)
{
  MapScene* map_scene = m_map_scenes.at(image_index);
  map_view->setScene(map_scene);

  // Change focus to any place other than the QComboBox
  map_view->setFocus();
}
