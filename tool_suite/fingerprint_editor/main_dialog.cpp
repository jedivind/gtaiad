#include <assert.h>

#include <QDebug>
#include <QMessageBox>
#include <QPixmap>
#include <QPointF>
#include <QSqlQuery>

#include "main_dialog.h"
#include "map_scene.h"

MainDialog::MainDialog(QWidget* parent, const QSqlDatabase& db) : QDialog(parent),
    m_db(db), m_map_diagrams(QStringList())
{
  MapScene* map_scene = new MapScene;

  assert(db.isOpen());

  m_map_diagrams << "../level_one.png";
  m_map_diagrams << "../level_two.png";
  m_map_diagrams << "../level_three.png";

  // TODO: fixme
  QPixmap* floor_pixmap = new QPixmap("level_one.png");

  setupUi(this);

  map_scene->addPixmap(*floor_pixmap);

  map_view->setScene(map_scene);

  update_floor_scale(zoom_slider->value());

#ifdef Q_WS_MAC
  map_view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
#endif

  map_view->setFocus();

  // Remove dialog example data
  loc_id_line_edit->clear();
  x_pos_label->clear();
  y_pos_label->clear();

  // Connect signals of MapScene
  QObject::connect(map_scene, SIGNAL(location_set(const QPointF&)),
      this, SLOT(update_capture_location(const QPointF)));

  QObject::connect(this, SIGNAL(new_capture_canceled()),
      map_scene, SLOT(clear_marker_placement()));

  QObject::connect(this, SIGNAL(new_capture_added(const QPoint&)),
      map_scene, SLOT(clear_marker_placement()));
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

// Slot called when the user hits the 'New Capture' button.
void MainDialog::enter_capture_mode(void)
{
  new_capture_push_button->setEnabled(false);
  new_capture_groupbox->setEnabled(true);
  new_capture_push_button->setEnabled(false);
  floor_combobox->setEnabled(false);
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

  // TODO: exit capture mode

  exit_capture_mode();
  loc_id_line_edit->clear();
}

// slot: called when user clicks on the floor map for red dot placement
void MainDialog::update_capture_location(const QPointF& pos)
{
  // Ignore the update request when the new_capture_groupbox
  // is not enabled.
  // TODO: Update enables of buttons when loc ID is entered
  if (!new_capture_groupbox->isEnabled())
  {
    // Poor design--not really canceled, but gets rid of the dot all the same
    emit new_capture_canceled();
    return;
  }

  QPoint int_pos = pos.toPoint();  // does proper rounding

  x_pos_label->setText(QString::number(int_pos.x()));
  y_pos_label->setText(QString::number(int_pos.y()));
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
