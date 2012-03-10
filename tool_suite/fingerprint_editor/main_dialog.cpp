#include <assert.h>

#include <QPixmap>
#include <QDebug>

#include "main_dialog.h"
#include "map_scene.h"

MainDialog::MainDialog(QWidget* parent, const QSqlDatabase& db) : QDialog(parent),
    m_db(db), m_map_diagrams(QStringList()), m_fp_map_view(NULL), m_fp_map_view_2(NULL)
{
  MapScene* map_scene = new MapScene;
  MapScene* map_scene_2 = new MapScene;

  assert(db.isOpen());

  m_map_diagrams << "../level_one.png";
  m_map_diagrams << "../level_two.png";
  m_map_diagrams << "../level_three.png";

  // TODO: fixme
  QPixmap* floor_pixmap = new QPixmap("level_one.png");
  QPixmap* floor_pixmap_2 = new QPixmap("level_one.png");

  m_fp_map_view = new QGraphicsView;
  m_fp_map_view_2 = new QGraphicsView;

  setupUi(this);

  map_scene->addPixmap(*floor_pixmap);
  map_scene_2->addPixmap(*floor_pixmap_2);

  m_fp_map_view->setScene(map_scene);
  m_fp_map_view_2->setScene(map_scene_2);

  floor_graphic_layout->addWidget(m_fp_map_view);
  floor_graphic_layout_2->addWidget(m_fp_map_view_2);

  fp_update_floor_scale(fp_zoom_slider->value());
  as_update_floor_scale(as_zoom_slider->value());

#ifdef Q_WS_MAC
  m_fp_map_view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
#endif

  dialog_tab->setCurrentIndex(0);
  m_fp_map_view->setFocus();

  // Remove dialog example data
  fp_loc_id_line_edit->clear();
  fp_x_pos_label->clear();
  fp_y_pos_label->clear();

  // Connect signals of MapScene
  QObject::connect(map_scene, SIGNAL(location_set(const QPointF&)),
      this, SLOT(fp_update_capture_location(const QPointF)));

  QObject::connect(this, SIGNAL(fp_new_capture_canceled()),
      map_scene, SLOT(clear_marker_placement()));

  QObject::connect(this, SIGNAL(fp_new_capture_added(const QPoint&)),
      map_scene, SLOT(clear_marker_placement()));
}

void MainDialog::as_update_floor_scale(int scaling_factor)
{
  m_fp_map_view_2->resetTransform();
  m_fp_map_view_2->scale(100.0/scaling_factor, 100.0/scaling_factor);
}
