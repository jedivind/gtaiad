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
  if (x_pos_label->text().isEmpty() && y_pos_label->text().isEmpty())   //what if the user presses update again. The marker is not cleared.
  {
    //call code to run airodump and get the present location data.feed it into lily's code.
    //call lily's code and other map refreshing code.
   calculate_position_from_data(get_measurement_locations); 
    update_location_changed();
    QMessageBox::warning(this, "Location",
        "Updating for the first time.");
    return;
  }
  else
  {
	//clear marker.
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
  // If the location was updated and x and y have some value but now after some time
  // presses the update again .
  // TODO: Update enables of buttons when loc ID is entered

// Do necessary clearing of widgets and enable/disables.need to design appropriate widgets first.

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



// slot: called when floor is changed.that is when the user clicks update_position again then refresh the floor if it has changed.
void MainDialog::change_floor(int image_index)
{
  MapScene* map_scene = m_map_scenes.at(image_index);
  map_view->setScene(map_scene);

  // Change focus to any place other than the QComboBox
  map_view->setFocus();
}
