#include <assert.h>

#include <QPixmap>
#include <QDebug>

#include "main_dialog.h"
#include "map_scene.h"

MainDialog::MainDialog(QWidget* parent, const QSqlDatabase& db) : QDialog(parent),
    m_db(db), m_map_diagrams(QStringList()), m_map_view(NULL), m_map_view_2(NULL)
{
  MapScene* map_scene = new MapScene;

  assert(db.isOpen());

  m_map_diagrams << "../level_one.png";
  m_map_diagrams << "../level_two.png";
  m_map_diagrams << "../level_three.png";

  // TODO: fixme
  QPixmap* floor_pixmap = new QPixmap("level_one.png");

  m_map_view = new QGraphicsView;

  setupUi(this);

  map_scene->addPixmap(*floor_pixmap);

  m_map_view->setScene(map_scene);

  floor_graphic_layout->addWidget(m_map_view);

  update_floor_scale(zoom_slider->value());

#ifdef Q_WS_MAC
  m_map_view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
#endif

  m_map_view->setFocus();

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
