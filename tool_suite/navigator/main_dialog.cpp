#include <assert.h>
#include <sched.h>

#include <QDebug>
#include <QHBoxLayout>
#include <QDialog>
#include <QLabel>

#include <QApplication>
#include <QMessageBox>
#include <QPixmap>
#include <QPointF>
#include <QSqlQuery>
#include <QTimer>
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

  // Give us something interseting to look at
  MapScene* dangling_map_scene = new MapScene("logo-georgia.gif");
  map_view->setScene(dangling_map_scene);

  QObject::connect(update_location_button,SIGNAL(clicked()),this,SLOT(update_location_clicked()));
  QObject::connect(zoom_slider, SIGNAL(sliderMoved(int)), this, SLOT(update_floor_scale(int)));

  m_scanning_dialog = doing_scan_dialog(this);
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

void MainDialog::run_airodump(void)
{
	int ret = system("./airodump-ngm -f 2000 -t 6000 --channel 1,6,11 -w test_data_median --output-format gatech wlan0");

	if(ret)
  {
    /* bad result */
    m_scanning_dialog->hide();
    QMessageBox::warning(this, "Error", "Error returned from wifi scanner.");

    return;
  }

  /* good result */

  m_scanning_dialog->hide();

  process_airodump();
}

void MainDialog::lily_code_execute(FILE* read_results)
{
	read_results = popen("./lilcode","r");
	fread(return_value,sizeof(char),100,read_results);
}

void MainDialog::update_location_clicked(void)
{
  // Don't disable button since our prototype system calls are blocking anyhow
  //update_location_button->setEnabled(false);

  //call code to run airodump and get the present location data.feed it into lily's code.

  m_scanning_dialog->show();

  QTimer::singleShot(100, this, SLOT(run_airodump()));
}

void MainDialog::process_airodump(void)
{
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
    present_position.setX(1710.0);
    present_position.setY(2328.0);
    floor_number = 2;
    update_location_changed(present_position,floor_number,loc_id);
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
	mapscene->set_marker(loc_id,present_position); //to add the marker to the position where the use is right now.
        x_pos_label->setNum(present_position.x());
        y_pos_label->setNum(present_position.y());
  // Ensure this location is visible on the map
  //map_view->ensureVisible(present_position.x(), present_position.y(), 1, 1);
  map_view->centerOn(present_position.x(), present_position.y());
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

QDialog* MainDialog::doing_scan_dialog(QWidget* parent)
{
  QDialog* dialog = new QDialog(parent);
  QHBoxLayout* layout = new QHBoxLayout;
  QPixmap gt_logo_pixmap = QPixmap("logo-georgia.gif");
  gt_logo_pixmap = gt_logo_pixmap.scaledToWidth(100);
  QLabel* gt_logo = new QLabel;
  QLabel* text = new QLabel("WIFI scan in progress...");

  gt_logo->setPixmap(gt_logo_pixmap);
  layout->addWidget(gt_logo);
  layout->addWidget(text);

  dialog->setLayout(layout);
  dialog->setWindowModality(Qt::ApplicationModal);

  return dialog;
}
