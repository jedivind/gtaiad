#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QtCore>

#include "map_scene.h"

MapScene::MapScene(QObject* parent) : QGraphicsScene(parent),
    m_pending_capture_loc_ellipseitem(NULL)
{
}

// slot: called when user clicks the 'cancel' button in the new capture
// location group box of the fingerprints tab.
void MapScene::clear_marker_placement(void)
{
  if (m_pending_capture_loc_ellipseitem)
  {
    this->removeItem((QGraphicsItem*)m_pending_capture_loc_ellipseitem);
    m_pending_capture_loc_ellipseitem = NULL;
  }
}

void MapScene::place_marker(const QPointF& pos, const QColor& color)
{
  int i_diameter = qFloor(qSqrt(((int(this->height()) << 1) + (int(this->width()) << 1))) + 0.5);
  float diameter = i_diameter / 8;

  if (m_pending_capture_loc_ellipseitem)
  {
    clear_marker_placement();
  }

  m_pending_capture_loc_ellipseitem = this->addEllipse(
      QRectF(pos.x()-diameter/2, pos.y()-diameter/2, diameter, diameter),
      QPen(), QBrush(color));
}

void MapScene::mousePressEvent(QGraphicsSceneMouseEvent* e)
{
  place_marker(e->scenePos());
  emit location_set(e->scenePos());
}
