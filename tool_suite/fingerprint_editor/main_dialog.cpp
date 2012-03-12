#include <assert.h>

#include <QDebug>
#include <QMessageBox>
#include <QPixmap>
#include <QPointF>
#include <QSqlQuery>

#include "main_dialog.h"
#include "map_scene.h"

MainDialog::MainDialog(QWidget* parent, const QSqlDatabase& db) : QDialog(parent),
    m_db(db), m_floor_image_filenames(QStringList())
{
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

// TODO: Multi-modal thoughts
//        enter_capture_mode
//            - can add new locations
//            - can leave capture mode
//            - can place capture locations on map
//        exit_capture_mode
//            - can enter capture mode
//            - can select existing capture
//            - can select existing capture and delete it
//            - can change floor
//            - can select existing capture and redo it
//

// Slot called when the zoom slider position is changed
void MainDialog::update_floor_scale(int scaling_factor)
{
  map_view->resetTransform();
  map_view->scale(100.0/scaling_factor, 100.0/scaling_factor);
}

bool MainDialog::validate_loc_id(void)
{
  QString loc_id = loc_id_line_edit->displayText();

  if (loc_id.isEmpty())
  {
    QMessageBox::warning(this, "Invalid Loc ID",
        "Location ID can not be blank.");

    return false;
  }
  else
  {
    // Convert entered text to uppercase
    loc_id_line_edit->setText(loc_id.toUpper().trimmed());
  }

  return true;
}

void MainDialog::run_airodump_clicked(void)
{
  if (x_pos_label->text().isEmpty() || y_pos_label->text().isEmpty())
  {
    QMessageBox::warning(this, "Invalid Capture Location",
        "No capture location has been identified on the map.");
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

void MainDialog::insert_location_id_clicked(void)
{
  if (!validate_loc_id())
  {
    return;
  }

  QSqlQuery q(m_db);

  q.prepare(" \
      INSERT INTO capture_locations ( \
        capture_location_name, capture_location_floor, \
        capture_location_x_pos, capture_location_y_pos) \
      VALUES (:capture_location_name, :capture_location_floor, \
              :capture_location_x_pos, :capture_location_y_pos)");

  QPoint new_capture_location(x_pos_label->text().toInt(),
                              y_pos_label->text().toInt());

  q.bindValue(":capture_location_name", loc_id_line_edit->displayText());
  q.bindValue(":capture_location_floor", floor_combobox->currentIndex() + 1);
  q.bindValue(":capture_location_x_pos", x_pos_label->text().toInt());
  q.bindValue(":capture_location_y_pos", y_pos_label->text().toInt());

  bool res = q.exec();

  if (!res)
  {
    QMessageBox::warning(this, "Database Error",
        "Failed during insert of new capture location.");
    return;
  }

  emit new_capture_added(new_capture_location);

  exit_capture_mode();
  loc_id_line_edit->clear();
}

// slot: called when user clicks on the floor map for red dot placement
void MainDialog::capture_location_changed(const QPointF& pos)
{
  // Ignore the update request when the new_capture_groupbox
  // is not enabled.
  // TODO: Update enables of buttons when loc ID is entered
  if (!new_capture_groupbox->isEnabled())
  {
    MapScene* map_scene;

    map_scene = static_cast<MapScene*>(map_view->scene());
    map_scene->clear_marker_placement();
    return;
  }

  QPoint int_pos = pos.toPoint();  // does proper rounding

  x_pos_label->setText(QString::number(int_pos.x()));
  y_pos_label->setText(QString::number(int_pos.y()));
}

// Slot called when the user hits the 'New Capture' button.
void MainDialog::enter_capture_mode(void)
{
  new_capture_push_button->setEnabled(false);
  new_capture_groupbox->setEnabled(true);
  new_capture_push_button->setEnabled(false);
  floor_combobox->setEnabled(false);
  loc_id_line_edit->setFocus();
}

// Do necessary clearing of widgets and enable/disables
void MainDialog::exit_capture_mode(void)
{
  x_pos_label->clear();
  y_pos_label->clear();
  new_capture_groupbox->setEnabled(false);
  new_capture_push_button->setEnabled(true);
  floor_combobox->setEnabled(true);

  emit new_capture_canceled();
}

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

    QObject::connect(this, SIGNAL(new_capture_canceled()),
        map_scene, SLOT(clear_marker_placement()));

    QObject::connect(this, SIGNAL(new_capture_added(const QPoint&)),
        map_scene, SLOT(clear_marker_placement()));
  }
}

// slot: called when floor dropdown is changed
void MainDialog::change_floor(int image_index)
{
  MapScene* map_scene = m_map_scenes.at(image_index);
  map_view->setScene(map_scene);

  // Change focus to any place other than the QComboBox
  map_view->setFocus();
}
