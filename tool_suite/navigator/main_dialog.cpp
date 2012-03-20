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


void MainDialog::run_airodump()
{
	//vinay's code.
}


void MainDialog::update_location_clicked(void)
{
    //call code to run airodump and get the present location data.feed it into lily's code.
    run_airodump();
    //present_location_data = run_airodump();
    //call lily's code and other map refreshing code.
    validate_loc_id = get_measurement_locations_from_DB(present_location_data); 
      if (!validate_loc_id())
  	{
    		QMessageBox::warning(this,"Update Location again");
  	}

    update_location_changed(validate_loc_id,floor_number);
    QMessageBox::warning(this, "Location",
        "Updating for the first time.");
    return;
  // TODO: perform AP capture magic
  // TODO: perform Database magic

  //emit new_capture_added(new_capture_location);
}

void MainDialog::update_location_changed(char *loc_id,int floor_number)
{
	init_floor_scenes();
	update_floor_scale(100);
	change_floor(floor_number);

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
void MainDialog::change_floor(int floor_number)
{
  MapScene* map_scene = m_map_scenes.at(floor_number); //get the floor number and change it accordingly.
  map_view->setScene(map_scene);

  // Change focus to any place other than the QComboBox
  map_view->setFocus();
}
