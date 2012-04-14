#ifndef MAP_SCENE_H
#define MAP_SCENE_H

#include <QWidget>
#include <QGraphicsSceneMouseEvent>
#include <QPointF>
#include <QGraphicsScene>
#include <QColor>
#include <QList>
#include <QGraphicsEllipseItem>


class MapScene : public QGraphicsScene
{
  Q_OBJECT

public:
  MapScene(const QString& map_filename, QObject* parent=0);
  void set_marker(const QPointF& pos);

signals:
//  void location_selected(const QString& loc_id);

private:

#ifdef NONO
  QColor m_permanent_marker_color;
#endif
  QGraphicsEllipseItem* m_location_dot_ptr;
  QGraphicsEllipseItem* m_location_ring_ptr;
};

#endif
