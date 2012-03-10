#include <QPointF>
#include <QMessageBox>
#include <QDebug>
#include <QSqlQuery>

#include "main_dialog.h"
#include "map_scene.h"

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
void MainDialog::fp_update_floor_scale(int scaling_factor)
{
  m_fp_map_view->resetTransform();
  m_fp_map_view->scale(100.0/scaling_factor, 100.0/scaling_factor);
}

// Slot called when the user hits the 'New Capture' button.
void MainDialog::fp_start_new_capture_mode(void)
{
  fp_new_capture_push_button->setEnabled(false);
  fp_new_capture_groupbox->setEnabled(true);
  fp_new_capture_push_button->setEnabled(false);
  fp_floor_combobox->setEnabled(false);
}

bool MainDialog::fp_validate_loc_id(void)
{
  QString loc_id = fp_loc_id_line_edit->displayText();

  if (loc_id.isEmpty())
  {
    QMessageBox::warning(this, "Invalid Loc ID",
        "Location ID can not be blank.");

    return false;
  }
  else
  {
    // Convert entered text to uppercase
    fp_loc_id_line_edit->setText(loc_id.toUpper().trimmed());
  }

  return true;
}

void MainDialog::fp_perform_ap_capture(void)
{
  if (fp_x_pos_label->text().isEmpty() || fp_y_pos_label->text().isEmpty())
  {
    QMessageBox::warning(this, "Invalid Capture Location",
        "No capture location has been identified on the map.");
    return;
  }

  if (!fp_validate_loc_id())
  {
    return;
  }

  // TODO: perform AP capture magic
  // TODO: perform Database magic
}

void MainDialog::fp_insert_loc_id(void)
{
  if (!fp_validate_loc_id())
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

  QPoint new_capture_location(fp_x_pos_label->text().toInt(),
                              fp_y_pos_label->text().toInt());

  q.bindValue(":capture_location_name", fp_loc_id_line_edit->displayText());
  q.bindValue(":capture_location_floor", fp_floor_combobox->currentIndex() + 1);
  q.bindValue(":capture_location_x_pos", fp_x_pos_label->text().toInt());
  q.bindValue(":capture_location_y_pos", fp_y_pos_label->text().toInt());

  bool res = q.exec();

  if (!res)
  {
    QMessageBox::warning(this, "Database Error",
        "Failed during insert of new capture location.");
    return;
  }

  emit fp_new_capture_added(new_capture_location);

  // TODO: exit capture mode

  fp_cancel_new_capture();
  fp_loc_id_line_edit->clear();
}

// slot: called when user clicks on the floor map for red dot placement
void MainDialog::fp_update_capture_location(const QPointF& pos)
{
  // Ignore the update request when the new_capture_groupbox
  // is not enabled.
  // TODO: Update enables of buttons when loc ID is entered
  if (!fp_new_capture_groupbox->isEnabled())
  {
    // Poor design--not really canceled, but gets rid of the dot all the same
    emit fp_new_capture_canceled();
    return;
  }

  QPoint int_pos = pos.toPoint();  // does proper rounding

  fp_x_pos_label->setText(QString::number(int_pos.x()));
  fp_y_pos_label->setText(QString::number(int_pos.y()));
}

// Do necessary clearing of widgets and enable/disables
void MainDialog::fp_cancel_new_capture(void)
{
  fp_x_pos_label->clear();
  fp_y_pos_label->clear();
  fp_new_capture_groupbox->setEnabled(false);
  fp_new_capture_push_button->setEnabled(true);
  fp_floor_combobox->setEnabled(true);

  emit fp_new_capture_canceled();
}
