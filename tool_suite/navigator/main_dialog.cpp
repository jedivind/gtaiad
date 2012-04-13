#include <assert.h>

#include <QDebug>
#include <QMessageBox>
#include <QPixmap>
#include <QPointF>
#include <QSqlQuery>
#include <time.h>
#include <stdio.h>
#include <unistd.h>

#include "main_dialog.h"
#include "map_scene.h"

MainDialog::MainDialog(QWidget* parent) : QDialog(parent),
     m_floor_image_filenames(QStringList())
{

  //assert(db.isOpen());
  setupUi(this);
  m_floor_image_filenames << "../level_one.jpg";
  m_floor_image_filenames << "../level_two.jpg";
  m_floor_image_filenames << "../level_three.jpg";

  MapScene *map_scene = static_cast<MapScene*>(map_view->scene());

  init_floor_scenes();

  //change_floor(0);
  //zoom_slider->setSliderDown(true);
  update_floor_scale(zoom_slider->value());

  destination_label->clear();

#ifdef Q_WS_MAC
  map_view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
#endif

  // Remove dialog example data
  x_pos_label->clear();
  y_pos_label->clear();

  QObject::connect(update_location_button,SIGNAL(clicked()),this,SLOT(update_location_clicked()));
  QObject::connect(zoom_slider, SIGNAL(sliderMoved(int)), this, SLOT(update_floor_scale(int)));
}


void MainDialog::wait(long seconds)
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
	int ret_1= system("ifconfig wlan0 down");
	int ret_2 = system("iwconfig wlan0 mode monitor");
	int ret_3 = system("ifconfig wlan0 up");
	int ret_4 = system("./airodump-ngm -f 2000 -t 6000 --channel 1,6,11 -w test_data_median --output-format gatech wlan0");
	//vinay's code.i
	if(ret_1 && ret_2 && ret_3 && ret_4)
	{
	qDebug() << "returning";
	return 1;
	}
}


void MainDialog::lily_code_execute(FILE* read_results)
{
	read_results = popen("./lilcode","r");
	fread(return_value,sizeof(char),100,read_results);
}

void MainDialog::update_location_clicked(void)
{

update_location_button->setEnabled(false);
    //call code to run airodump and get the present location data.feed it into lily's code.
    if(run_airodump())

	system("ifconfig wlan0 down");
	system("iwconfig wlan0 mode managed");
	system("ifconfig wlan0 up");
	//run_airodump();
	update_location_button->setEnabled(true);
    //wait(60);
    //update the popup boxes.
    FILE *read_results;
    //present_location_data = run_airodump();
    //call lily's code and other map refreshing code.
    //lily_code_execute(read_results);
    //parse the return value
    char buffer[100];
    QString loc_id;
    int xpos,ypos,floor_number;
    bool x_pos,y_pos;
    /*while(fgets(buffer,100,read_results)!=NULL)
	{
        int length = strlen(buffer);
        while(length!=0)
        {
        if(buffer[length] == ',' && y_pos==false)
		{
		int ypos = (int)buffer[length+1] - 48;
		y_pos = true;
		}
        if(buffer[length] == ',' && x_pos == false)
		{
		int xpos = (int)buffer[length+1] - 48;
		x_pos = true;
		}
        if(length == 1)
		{
		int floor = (int)buffer[length] - 48; 
		break;
		}
    	//get from lily's code
	//location; 
	}
	}	
	QPointF present_position;
	present_position.setX(xpos);
        present_position.setY(ypos);
        x_pos_label->setText((QString)xpos);
        y_pos_label->setText((QString)ypos);
    // = get_measurement_locations_from_DB(present_location_data); */
    QPointF present_position;
    present_position.setX(100.0);
    present_position.setY(100.0);
    floor_number = 1;
    update_location_changed(present_position,floor_number,loc_id);
    QMessageBox::warning(this, "Location",
        "Updating for the first time.");
    //return;
  // TODO: perform AP capture magic
  // TODO: perform Database magic
  //MapScene *map_scene = static_cast<MapScene*>(map_view->scene());
  //QObject::connect(this,SIGNAL(update_location_button()),this,SLOT(update_location_clicked()));
}

void MainDialog::update_location_changed(const QPointF& present_position,int floor_number,const QString& loc_id)
{
	//init_floor_scenes();
	//update_floor_scale(1000);
	change_floor(floor_number);
  update_floor_scale(zoom_slider->value());
	MapScene *mapscene = static_cast<MapScene*>(map_view->scene());
	mapscene->add_marker(loc_id,present_position); //to add the marker to the position where the use is right now.
        x_pos_label->setNum(present_position.x());
        y_pos_label->setNum(present_position.y());
  // Ensure this location is visible on the map
  map_view->ensureVisible(present_position.x(), present_position.y(), 1, 1);
}


// Do necessary clearing of widgets and enable/disables.need to design appropriate widgets first.

// Generate the MapScene object for each floor map.  Store the scenes in
// the QList<T> generic container.
void MainDialog::init_floor_scenes(void)
{
  // assumed order is 1, 2, 3
  foreach (const QString& filename, m_floor_image_filenames)
  {
    MapScene* map_scene = new MapScene(filename);
    m_map_scenes.append(map_scene);

    // Connect signals of MapScene
    //QObject::connect(map_scene, SIGNAL(location_set(const QPointF&)),
      //  this, SLOT(capture_location_changed(const QPointF&)));
  }
}



// slot: called when floor is changed.that is when the user clicks update_position again then refresh the floor if it has changed.
void MainDialog::change_floor(int floor_number)
{
  MapScene* map_scene = m_map_scenes.at(floor_number); //get the floor number and change it accordingly.
  qDebug() << map_scene;
  map_view->setScene(map_scene);

  // Change focus to any place other than the QComboBox
  map_view->setFocus();
}
