#include <QWidget>
#include <QGraphicsSceneMouseEvent>
#include <QPointF>
#include <QGraphicsScene>
#include <QColor>

class MapScene : public QGraphicsScene
{
  Q_OBJECT

public:
  MapScene(QObject* parent = 0);

signals:
  void location_set(const QPointF& pos);

public slots:
  void place_marker(const QPointF& pos, const QColor& color = Qt::red);
  void clear_marker_placement(void);

private:
  void mousePressEvent(QGraphicsSceneMouseEvent* e);

  QGraphicsEllipseItem* m_fp_pending_capture_loc_ellipseitem;
};
