#ifndef MAP_SCENE_H
#define MAP_SCENE_H

#include <QWidget>
#include <QGraphicsSceneMouseEvent>
#include <QPointF>
#include <QGraphicsScene>
#include <QColor>
#include <QList>
#include <QGraphicsEllipseItem>

// structure holding information to represent the measurement locations
class PermanentMarker : public QGraphicsEllipseItem
{
public:
  PermanentMarker(const QRectF& rect, const QString& loc_id, QGraphicsEllipseItem* parent=0);

  QString m_loc_id;
};

class MapScene : public QGraphicsScene
{
  Q_OBJECT

public:
  MapScene(const QString& map_filename, QObject* parent=0);
  void add_marker(const QString& loc_id, const QPointF& pos);
  void set_marker_color(const QColor& color = Qt::black);

signals:
//  void location_selected(const QString& loc_id);

private:

  QGraphicsEllipseItem* m_pending_capture_loc_ellipseitem_ptr;
  QColor m_permanent_marker_color;
  QGraphicsItemGroup* m_markers_group;   // Permanent markers
  QList<PermanentMarker*> m_markers_list;
};

#endif
