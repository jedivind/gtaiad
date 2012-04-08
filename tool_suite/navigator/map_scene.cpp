#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QtCore>
#include <QGraphicsPixmapItem>
#include <QRectF>

#include "map_scene.h"

PermanentMarker::PermanentMarker(const QRectF& rect,
    const QString& loc_id, QGraphicsEllipseItem* parent) : QGraphicsEllipseItem(rect, parent)
{
  m_loc_id = loc_id;
  setToolTip(m_loc_id);
}

// ***************************************************************************

// TODO: How to handle deconstruction?

MapScene::MapScene(const QString& map_filename, QObject* parent) : QGraphicsScene(parent),
    m_pending_capture_loc_ellipseitem_ptr(NULL), m_permanent_marker_color(Qt::black), m_markers_list()
{
  QPixmap floor_pixmap(map_filename);
  QGraphicsPixmapItem* pixmap_item_ptr;

  pixmap_item_ptr = addPixmap(floor_pixmap);
  pixmap_item_ptr->setOpacity(0.4);
}

// slot: called when user clicks the 'cancel' button in the new capture
// location group box of the fingerprints tab.

//use mouse press event for something other than this.

/*void MapScene::mousePressEvent(QGraphicsSceneMouseEvent* e)
{
  QGraphicsItem* item;

  item = itemAt(e->scenePos().x(), e->scenePos().y());

  if (dynamic_cast<PermanentMarker*>(item) != NULL)
  {
    PermanentMarker* marker = static_cast<PermanentMarker*>(item);

    emit location_selected(marker->m_loc_id);
  }
  else
  {
    // TODO: pass this to main using an emit; let main make the decision to
    // place the marker.
    set_temp_marker(e->scenePos());
    emit temp_marker_set(e->scenePos());
  }
}
*/
void MapScene::add_marker(const QString& loc_id, const QPointF& pos)
{
  int i_diameter = qFloor(qSqrt(((int(height()) << 1) + (int(width()) << 1))) + 0.5);
  float diameter = i_diameter / 11;
  PermanentMarker* marker = new PermanentMarker(
      QRectF(pos.x()-diameter/2, pos.y()-diameter/2, diameter, diameter), loc_id);

  marker->setBrush(QBrush(m_permanent_marker_color));

  m_markers_list.append(marker);

  addItem(marker);
 // x_pos_label->setText(pos.x());
 // y_pos_label->setText(pos.y());
}

void MapScene::set_marker_color(const QColor& color)
{
  m_permanent_marker_color = color;
}
