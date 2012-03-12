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
void MapScene::clear_temp_marker(void)
{
  if (m_pending_capture_loc_ellipseitem_ptr)
  {
    removeItem((QGraphicsItem*)m_pending_capture_loc_ellipseitem_ptr);
    m_pending_capture_loc_ellipseitem_ptr = NULL;
  }
}

void MapScene::set_temp_marker(const QPointF& pos, const QColor& color)
{
  int i_diameter = qFloor(qSqrt(((int(height()) << 1) + (int(width()) << 1))) + 0.5);
  float diameter = i_diameter / 11;

  if (m_pending_capture_loc_ellipseitem_ptr)
  {
    clear_temp_marker();
  }

  m_pending_capture_loc_ellipseitem_ptr = addEllipse(
      QRectF(pos.x()-diameter/2, pos.y()-diameter/2, diameter, diameter),
      QPen(), QBrush(color));
}

void MapScene::mousePressEvent(QGraphicsSceneMouseEvent* e)
{
  set_temp_marker(e->scenePos());
  emit location_set(e->scenePos());
}

void MapScene::add_marker(const QString& loc_id, const QPointF& pos)
{
  int i_diameter = qFloor(qSqrt(((int(height()) << 1) + (int(width()) << 1))) + 0.5);
  float diameter = i_diameter / 11;
  PermanentMarker* marker = new PermanentMarker(
      QRectF(pos.x()-diameter/2, pos.y()-diameter/2, diameter, diameter), loc_id);

  marker->setBrush(QBrush(m_permanent_marker_color));

  m_markers_list.append(marker);

  addItem(marker);
}

void MapScene::set_marker_color(const QColor& color)
{
  m_permanent_marker_color = color;
}
