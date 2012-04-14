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
    m_location_dot_ptr(NULL), m_location_ring_ptr(NULL)
{
  QPixmap floor_pixmap(map_filename);
  QGraphicsPixmapItem* pixmap_item_ptr;

  pixmap_item_ptr = addPixmap(floor_pixmap);
  if (map_filename != "logo-georgia.gif")
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
void MapScene::set_marker(const QString& loc_id, const QPointF& pos)
{
  int i_diameter = qFloor(qSqrt(((int(height()) << 1) + (int(width()) << 1))) + 0.5);
  float diameter;

  // Remove any existing marker
  if (m_location_dot_ptr)
  {
    removeItem((QGraphicsItem*)m_location_dot_ptr);
    m_location_dot_ptr = NULL;
  }

  if (m_location_ring_ptr)
  {
    removeItem((QGraphicsItem*)m_location_ring_ptr);
    m_location_ring_ptr = NULL;
  }

  // Red ring around the dot
  diameter = i_diameter / 6;

  QColor pen_color(Qt::red);
  pen_color.setAlpha(150);
  QPen pen = QPen(pen_color);
  pen.setWidth(i_diameter / 20);

  m_location_ring_ptr = addEllipse(
      QRectF(pos.x()-diameter/2, pos.y()-diameter/2, diameter, diameter),
      pen, QBrush());



  // The black dot
  diameter = i_diameter / 11;

  m_location_dot_ptr = addEllipse(
      QRectF(pos.x()-diameter/2, pos.y()-diameter/2, diameter, diameter),
      QPen(), QBrush(Qt::black));



#ifdef NONO
  PermanentMarker* marker = new PermanentMarker(
      QRectF(pos.x()-diameter/2, pos.y()-diameter/2, diameter, diameter), loc_id);

  marker->setBrush(QBrush(m_permanent_marker_color));

  addItem(marker);
#endif
 // x_pos_label->setText(pos.x());
 // y_pos_label->setText(pos.y());
}
