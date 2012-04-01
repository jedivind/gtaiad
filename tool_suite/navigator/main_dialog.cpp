#include <assert.h>

#include <QDebug>
#include <QMessageBox>
#include <QPixmap>
#include <QPointF>
#include <QSqlQuery>
#include <time.h>

#include <dos.h>

#include "main_dialog.h"
#include "map_scene.h"

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

  //change_floor(0);

  update_floor_scale(zoom_slider->value());

#ifdef Q_WS_MAC
  map_view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
#endif

  // Remove dialog example data
  x_pos_label->clear();
  y_pos_label->clear();

}


void MainDialog:wait(long seconds)
{
	seconds *= 1000;
	sleep(seconds);
}

// Slot called when the zoom slider position is changed by the end user.
void MainDialog::update_floor_scale(int scaling_factor)
{
  map_view->resetTransform();
  map_view->scale(100.0/scaling_factor, 100.0/scaling_factor);
}


int MainDialog::run_airodump(void)
{

	//vinay's code.
}


void MainDialog::lily_code_execute(void)
{
FILE *read_results = popen("./lilcode","r");
fread(return_value,sizeof(char),100,read_results);
}

void MainDialog::update_location_clicked(void)
{
    //call code to run airodump and get the present location data.feed it into lily's code.
    return_value = run_airodump();
    wait(60);
    //present_location_data = run_airodump();
    //call lily's code and other map refreshing code.
    lily_code_execute();
    //parse the return value

    int validate_loc_id = get_measurement_locations_from_DB(present_location_data); 
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
  Object::connect(map_scene, SIGNAL(sliderMoved(int value)),
        this, SLOT(update_floor_scale(int value)));

}

void MainDialog::update_location_changed(char *loc_id,int floor_number)
{
	init_floor_scenes();
	update_floor_scale(100);
	change_floor(floor_number);
	MapScene *mapscene = new MapScene;
	mapscene->add_marker(loc_id,position); //to add the marker to the position where the use is right now.

}


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
