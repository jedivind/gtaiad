#include <assert.h>

#include <QDebug>
#include <QMessageBox>
#include <QPixmap>
#include <QPointF>
#include <QSqlQuery>
#include <QBitmap>
#include <QGraphicsPixmapItem>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTimer>
#include <QSqlError>

#include "main_dialog.h"
#include "map_scene.h"

MainDialog::MainDialog(QWidget* parent, const QSqlDatabase& db) : QDialog(parent),
    m_db(db), m_floor_image_filenames(), all_measurement_locations(), table_signal_filter_kludge(0)
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
    qDebug() << q.lastError().text();
    QMessageBox::warning(this, "Database Error",
        "Failed during insert of new capture location.");
    return;
  }

  add_new_capture_point(new_capture_location);

  emit new_capture_added(new_capture_location);

  exit_capture_mode();
}

// slot: called after a new capture location has been added to the database
void MainDialog::add_new_capture_point(const QPoint& point)
{
  int row_idx = locations_table->rowCount();
  locations_table->insertRow(row_idx);

  QTableWidgetItem* loc_id_col = new QTableWidgetItem(loc_id_line_edit->text());
  QTableWidgetItem* x_pos_col = new QTableWidgetItem(
      QString::number(point.x()));
  QTableWidgetItem* y_pos_col = new QTableWidgetItem(
      QString::number(point.y()));

  loc_id_col->setFlags(Qt::ItemIsSelectable);
  x_pos_col->setFlags(Qt::ItemIsSelectable);
  y_pos_col->setFlags(Qt::ItemIsSelectable);

  locations_table->setItem(row_idx, 0, loc_id_col);
  locations_table->setItem(row_idx, 1, x_pos_col);
  locations_table->setItem(row_idx, 2, y_pos_col);

  MapScene* map_scene = static_cast<MapScene*>(map_view->scene());

  map_scene->add_marker(loc_id_line_edit->text(), point);

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
    map_scene->clear_temp_marker();
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
  int floor_number = 0;

  // assumed order is 1, 2, 3
  foreach (const QString& filename, m_floor_image_filenames)
  {
    MapScene* map_scene = new MapScene(filename);
    QList< QPair<QString, QPointF> > measurement_locations;
    floor_number++;

    if (floor_number == 1) map_scene->set_marker_color(Qt::blue);
    else if (floor_number == 2) map_scene->set_marker_color(Qt::cyan);
    else map_scene->set_marker_color(Qt::darkGreen);

    m_map_scenes.append(map_scene);

    measurement_locations = get_measurement_locations(floor_number);
    QListIterator< QPair<QString, QPointF> > itr(measurement_locations);
    while (itr.hasNext())
    {
      QPair<QString, QPointF> measurement_location = itr.next();

      map_scene->add_marker(measurement_location.first, measurement_location.second);
    }
    all_measurement_locations.append(measurement_locations);

    // Connect signals of MapScene
    QObject::connect(map_scene, SIGNAL(temp_marker_set(const QPointF&)),
        this, SLOT(capture_location_changed(const QPointF&)));

    QObject::connect(this, SIGNAL(new_capture_canceled()),
        map_scene, SLOT(clear_temp_marker()));

    QObject::connect(this, SIGNAL(new_capture_added(const QPoint&)),
        map_scene, SLOT(clear_temp_marker()));

    QObject::connect(map_scene, SIGNAL(location_selected(const QString&)),
        this, SLOT(fingerprint_location_dot_selected(const QString&)));

    // Each TableWidgetItem causes a signal.  So extra slot calls must
    // be filtered.
    QObject::connect(locations_table, SIGNAL(currentCellChanged(int, int, int, int)),
        this, SLOT(fingerprint_location_row_changed(int, int, int, int)));
  }
}

// slot: called when floor dropdown is changed
void MainDialog::change_floor(int image_index)
{
  MapScene* map_scene = m_map_scenes.at(image_index);
  map_view->setScene(map_scene);

  // Change focus to any place other than the QComboBox
  map_view->setFocus();

  // Clear any highlighted point
  map_scene->unhighlight_location();

  // Clear and populate the locations table
  locations_table->clearContents();
  QList< QPair<QString, QPointF> > measurement_locations = all_measurement_locations.at(image_index);
  QListIterator< QPair<QString, QPointF> > itr(measurement_locations);
  locations_table->setRowCount(measurement_locations.size());
  int row_idx = 0;
  while (itr.hasNext())
  {
    QPair<QString, QPointF> measurement_location = itr.next();

    QTableWidgetItem* loc_id_col = new QTableWidgetItem(measurement_location.first);
    QTableWidgetItem* x_pos_col = new QTableWidgetItem(
        QString::number(measurement_location.second.x()));
    QTableWidgetItem* y_pos_col = new QTableWidgetItem(
        QString::number(measurement_location.second.y()));

    loc_id_col->setFlags(Qt::ItemIsSelectable);
    x_pos_col->setFlags(Qt::ItemIsSelectable);
    y_pos_col->setFlags(Qt::ItemIsSelectable);

    locations_table->setItem(row_idx, 0, loc_id_col);
    locations_table->setItem(row_idx, 1, x_pos_col);
    locations_table->setItem(row_idx, 2, y_pos_col);

    row_idx++;
  }

  locations_table->sortByColumn(0, Qt::AscendingOrder);
  locations_table->resizeColumnsToContents();
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

// slot to be called when user clicks on a fingerprint dot
void MainDialog::fingerprint_location_dot_selected(const QString& loc_id)
{
  int row_idx = locations_table->rowCount();
  int found_row_idx = -1;

  // Find the location table row for this location
  while (row_idx > 0)
  {
    const QTableWidgetItem* loc_id_col;

    loc_id_col = locations_table->item(row_idx - 1, 0);
    if (loc_id_col->text() == loc_id)
    {
      found_row_idx = row_idx - 1;
      break;
    }

    row_idx--;
  }

  if (found_row_idx >= 0)
  {
    locations_table->selectRow(found_row_idx);

    // Don't call to highlight -- changing table row already generates
    // the event.
    //highlight_selected_capture_point(found_row_idx);
  }
}

// The implementation of the QTableWidget forces this irritating solution.
// Changing the row selection generates very few usable signals.  The one
// I chose, currentCellChanged, is triggered once for each column.  Rather
// than clear & populate the other table three time, use a timer to clear
// a counter to filter the spurrious events.
void MainDialog::reset_table_signal_filter_kludge(void)
{
  table_signal_filter_kludge = 0;
}

void MainDialog::fingerprint_location_row_changed(int current_row, int, int, int)
{
  table_signal_filter_kludge++;

  if (table_signal_filter_kludge < 3)
  {
    return;
  }

  QTableWidgetItem* cell_item = locations_table->item(current_row, 0);

  fingerprints_table->clearContents();

  highlight_selected_capture_point(current_row);

  if (cell_item)
  {
    repopulate_fingerprints_table(cell_item->text());

    // This is awesome!  GUI programming at its finest!
    QTimer::singleShot(250, this, SLOT(reset_table_signal_filter_kludge()));
  }
}

void MainDialog::repopulate_fingerprints_table(const QString& loc_id)
{
  QSqlQuery q(m_db);
  QSqlQuery q_size(m_db);
  bool res;

  q_size.prepare(" \
      SELECT COUNT(capture_fingerprint_bssid) \
      FROM capture_fingerprints \
      WHERE capture_location_name = :capture_location_name");

  q_size.bindValue(":capture_location_name", loc_id);
  res = q_size.exec();

  if (!res)
  {
    QMessageBox::warning(this, "Database Error",
        "Failed during SELECT from capture_fingerprints table. #1");
    return;
  }

  res = q_size.first();

  if (!res)
  {
    QMessageBox::warning(this, "Database Error",
        "Failed during SELECT from capture_fingerprints table. #2");
    return;
  }

  int row_count = q_size.value(0).toInt();

  q.prepare(" \
      SELECT \
        capture_fingerprint_bssid, capture_fingerprint_channel, \
        capture_fingerprint_median_power, capture_fingerprint_ssid \
      FROM capture_fingerprints \
      WHERE capture_location_name = :capture_location_name");

  q.bindValue(":capture_location_name", loc_id);
  res = q.exec();

  if (!res)
  {
    QMessageBox::warning(this, "Database Error",
        "Failed during SELECT from capture_fingerprints table. #3");
    return;
  }

  fingerprints_table->setRowCount(row_count);

  int row_idx = 0;
  while (q.next())
  {
    QTableWidgetItem* bssid_col = new QTableWidgetItem(q.value(0).toString());
    QTableWidgetItem* channel_col = new QTableWidgetItem(q.value(1).toString());
    QTableWidgetItem* power_col = new QTableWidgetItem(q.value(2).toString());
    QTableWidgetItem* ssid_col = new QTableWidgetItem(q.value(3).toString());

    bssid_col->setFlags(Qt::ItemIsSelectable);
    channel_col->setFlags(Qt::ItemIsSelectable);
    power_col->setFlags(Qt::ItemIsSelectable);
    ssid_col->setFlags(Qt::ItemIsSelectable);

    fingerprints_table->setItem(row_idx, 0, bssid_col);
    fingerprints_table->setItem(row_idx, 1, channel_col);
    fingerprints_table->setItem(row_idx, 2, power_col);
    fingerprints_table->setItem(row_idx, 3, ssid_col);

    row_idx++;
  }

  fingerprints_table->resizeColumnsToContents();
}

void MainDialog::highlight_selected_capture_point(int row_index)
{
  QPoint pos;

  // TODO: current_map_scene perhaps?
  int map_index = floor_combobox->currentIndex();
  MapScene* map_scene = m_map_scenes.at(map_index);
  QTableWidgetItem* x_pos_col;
  QTableWidgetItem* y_pos_col;

  x_pos_col = locations_table->item(row_index, 1);
  y_pos_col = locations_table->item(row_index, 2);

  if (!x_pos_col || !y_pos_col)
  {
    return;
  }

  pos = QPoint(x_pos_col->text().toInt(), y_pos_col->text().toInt());
  map_scene->highlight_location(pos);
}
