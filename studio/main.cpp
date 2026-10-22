#include "studiodocument.h"

#include <QApplication>
#include <QAbstractButton>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QGraphicsSceneMouseEvent>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPixmap>
#include <QWheelEvent>
#include <algorithm>
#include <utility>
#include <QComboBox>
#include <QDockWidget>
#include <QDrag>
#include <QFile>
#include <QFileDialog>
#include <QFormLayout>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsRectItem>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMainWindow>
#include <QMenuBar>
#include <QMessageBox>
#include <QMimeData>
#include <QPainter>
#include <QPalette>
#include <QPushButton>
#include <QScrollBar>
#include <QSpinBox>
#include <QStatusBar>
#include <QToolBar>
#include <QTreeWidget>
#include <QUndoCommand>
#include <QUndoStack>
#include <QVBoxLayout>

#include <qtmaterial/widgets/buttons/qtmaterialfilledbutton.h>
#include <qtmaterial/widgets/buttons/qtmaterialoutlinedbutton.h>
#include <qtmaterial/widgets/buttons/qtmaterialfilledtonalbutton.h>
#include <qtmaterial/widgets/selection/qtmaterialcheckbox.h>
#include <qtmaterial/widgets/selection/qtmaterialradiobutton.h>
#include <qtmaterial/widgets/inputs/qtmaterialoutlinedtextfield.h>
#include <qtmaterial/widgets/surfaces/qtmaterialcard.h>

#include <functional>
#include <memory>

namespace {
QWidget* previewWidget(const Studio::Item& element)
{
    using namespace QtMaterial;
    QWidget* w = nullptr;
    if (element.component == "button.filled") w = new QtMaterialFilledButton;
    else if (element.component == "button.outlined") w = new QtMaterialOutlinedButton;
    else if (element.component == "button.filled-tonal") w = new QtMaterialFilledTonalButton;
    else if (element.component == "selection.checkbox") w = new QtMaterialCheckbox;
    else if (element.component == "selection.radio") w = new QtMaterialRadioButton;
    else if (element.component == "input.text.outlined") w = new QtMaterialOutlinedTextField;
    else if (element.component == "surface.card") w = new QtMaterialCard;
    if (!w) return nullptr;
    if (auto* b = qobject_cast<QAbstractButton*>(w)) b->setText(element.text);
    if (auto* t = qobject_cast<QtMaterialOutlinedTextField*>(w)) t->setPlaceholderText(element.text);
    if (auto* c = qobject_cast<QtMaterialCard*>(w)) c->setTitleText(element.text);
    return w;
}

class CanvasElement final : public QGraphicsRectItem {
public:
    QString elementId;
    std::function<void(const QString&, const QRect&)> changed;
    explicit CanvasElement(const Studio::Item& model, qreal dpr)
        : QGraphicsRectItem(0, 0, model.rect.width(), model.rect.height()), elementId(model.id)
    {
        setPos(model.rect.topLeft());
        setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
        setZValue(1);
        std::unique_ptr<QWidget> widget(previewWidget(model));
        if (widget) {
            widget->resize(model.rect.size());
            widget->ensurePolished();
            QPixmap image(model.rect.width() * dpr, model.rect.height() * dpr);
            image.setDevicePixelRatio(dpr);
            image.fill(Qt::transparent);
            widget->render(&image);
            m_preview = image;
        }
    }
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override
    {
        painter->fillRect(rect(), QColor(248, 248, 248));
        painter->drawPixmap(rect().topLeft(), m_preview);
        painter->setPen(isSelected() ? QPen(QColor(0, 115, 215), 2) : QPen(QColor(190, 190, 190), 1));
        painter->drawRect(rect());
        if (isSelected()) painter->fillRect(QRectF(rect().right()-10, rect().bottom()-10, 10, 10),
                                             QColor(0, 115, 215));
    }
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override
    {
        m_resizing = event->pos().x() >= rect().width()-14 &&
                     event->pos().y() >= rect().height()-14;
        if (m_resizing) { setSelected(true); event->accept(); }
        else QGraphicsRectItem::mousePressEvent(event);
    }
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override
    {
        if (m_resizing) {
            const QPointF local = event->pos();
            const QRectF bounds = scene()->sceneRect();
            const qreal width = qBound(24.0, qRound(local.x()/8.0)*8.0,
                                       bounds.right()-pos().x());
            const qreal height = qBound(24.0, qRound(local.y()/8.0)*8.0,
                                        bounds.bottom()-pos().y());
            setRect(0, 0, width, height);
            update();
            event->accept();
        } else QGraphicsRectItem::mouseMoveEvent(event);
    }
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override
    {
        if (!m_resizing) QGraphicsRectItem::mouseReleaseEvent(event);
        else event->accept();
        m_resizing = false;
        const QRectF canvas = scene()->sceneRect();
        const int x = qBound(0, qRound(pos().x()/8.0)*8,
                             qMax(0, int(canvas.width()) - int(rect().width())));
        const int y = qBound(0, qRound(pos().y()/8.0)*8,
                             qMax(0, int(canvas.height()) - int(rect().height())));
        setPos(x, y);
        if (changed) changed(elementId, QRect(x, y, int(rect().width()), int(rect().height())));
    }
private:
    QPixmap m_preview;
    bool m_resizing = false;
};

class Palette final : public QListWidget {
public:
    explicit Palette(QWidget* parent = nullptr) : QListWidget(parent) { setDragEnabled(true); }
protected:
    void startDrag(Qt::DropActions) override
    {
        const QListWidgetItem* item = currentItem();
        if (!item) return;
        auto* drag = new QDrag(this);
        auto* mime = new QMimeData;
        mime->setData("application/x-qtm3-component", item->data(Qt::UserRole).toString().toUtf8());
        drag->setMimeData(mime);
        drag->exec(Qt::CopyAction);
    }
};

class Canvas final : public QGraphicsView {
public:
    std::function<void(const QString&, const QPoint&)> dropped;
    explicit Canvas(QWidget* parent = nullptr) : QGraphicsView(parent)
    {
        setAcceptDrops(true);
        setRenderHint(QPainter::Antialiasing);
        setDragMode(QGraphicsView::RubberBandDrag);
        setBackgroundBrush(QColor(225, 226, 231));
        setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    }
protected:
    void dragEnterEvent(QDragEnterEvent* event) override
    {
        if (event->mimeData()->hasFormat("application/x-qtm3-component"))
            event->acceptProposedAction();
    }
    void dragMoveEvent(QDragMoveEvent* event) override
    {
        if (event->mimeData()->hasFormat("application/x-qtm3-component"))
            event->acceptProposedAction();
    }
    void dropEvent(QDropEvent* event) override
    {
        if (!event->mimeData()->hasFormat("application/x-qtm3-component")) return;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        const QPoint scenePoint = mapToScene(event->position().toPoint()).toPoint();
#else
        const QPoint scenePoint = mapToScene(event->pos()).toPoint();
#endif
        if (dropped) dropped(QString::fromUtf8(event->mimeData()->data("application/x-qtm3-component")), scenePoint);
        event->acceptProposedAction();
    }
    void wheelEvent(QWheelEvent* event) override
    {
        if (event->modifiers().testFlag(Qt::ControlModifier)) {
            const qreal factor = event->angleDelta().y() > 0 ? 1.1 : 1.0/1.1;
            scale(factor, factor); event->accept();
        } else QGraphicsView::wheelEvent(event);
    }
};

class Window final : public QMainWindow {
public:
    Window()
    {
        setWindowTitle("Material Visual UI Studio — 1.26");
        resize(1280, 820);
        m_canvas = new Canvas;
        m_scene = new QGraphicsScene(this);
        m_canvas->setScene(m_scene);
        setCentralWidget(m_canvas);
        m_canvas->dropped = [this](const QString& component, const QPoint& at) { add(component, at); };
        m_palette = new Palette;
        QFile registry(":/studio/component-registry.json");
        if (registry.open(QIODevice::ReadOnly)) {
            const QJsonArray registryEntries = QJsonDocument::fromJson(registry.readAll()).array();
            for (const QJsonValue& value : registryEntries) {
                const QJsonObject item = value.toObject();
                const Studio::Component* c = Studio::lookup(item.value("id").toString());
                if (!c || item.value("widgetType").toString() != c->widgetType) continue;
                auto* listItem = new QListWidgetItem(item.value("name").toString(), m_palette);
                listItem->setData(Qt::UserRole, c->id);
                listItem->setToolTip(c->id + " — " + c->header);
            }
        }
        connect(m_palette, &QListWidget::itemDoubleClicked, this,
                [this](QListWidgetItem* item) {
            add(item->data(Qt::UserRole).toString(), QPoint(32, 32));
        });
        auto* paletteDock = new QDockWidget("Registry-backed palette", this);
        paletteDock->setWidget(m_palette); addDockWidget(Qt::LeftDockWidgetArea, paletteDock);
        m_tree = new QTreeWidget;
        m_tree->setHeaderLabels({"Element", "Component"});
        connect(m_tree, &QTreeWidget::itemSelectionChanged, this, [this] {
            if (m_sync) return;
            const auto list = m_tree->selectedItems();
            if (!list.isEmpty()) select(list.first()->data(0, Qt::UserRole).toString());
        });
        auto* treeDock = new QDockWidget("Widget tree", this);
        treeDock->setWidget(m_tree); addDockWidget(Qt::RightDockWidgetArea, treeDock);
        auto* panel = new QWidget;
        auto* form = new QFormLayout(panel);
        m_text = new QLineEdit;
        form->addRow("Text / label", m_text);
        connect(m_text, &QLineEdit::editingFinished, this, [this] { editProperties(); });
        auto posField = [this, form](const QString& label, QSpinBox*& field) {
            field = new QSpinBox;
            field->setRange(0, 4096);
            form->addRow(label, field);
            connect(field, &QSpinBox::editingFinished, this, [this] { editProperties(); });
        };
        posField("X", m_x); posField("Y", m_y); posField("Width", m_w); posField("Height", m_h);
        auto* remove = new QPushButton("Delete selection");
        form->addRow(remove);
        connect(remove, &QPushButton::clicked, this, [this] { removeSelected(); });
        auto* inspectorDock = new QDockWidget("Inspector", this);
        inspectorDock->setWidget(panel); addDockWidget(Qt::RightDockWidgetArea, inspectorDock);
        tabifyDockWidget(treeDock, inspectorDock);
        treeDock->raise();
        auto* bar = addToolBar("Studio");
        QAction* undo = m_undo.createUndoAction(this, "Undo"); bar->addAction(undo);
        QAction* redo = m_undo.createRedoAction(this, "Redo"); bar->addAction(redo);
        bar->addSeparator();
        auto* theme = new QComboBox; theme->addItems({"light", "dark"});
        bar->addWidget(new QLabel("Theme ")); bar->addWidget(theme);
        connect(theme, &QComboBox::currentTextChanged, this, [this](const QString& value) {
            if (m_sync) return;
            Studio::Document next = m_document; next.theme = value; change(next);
        });
        m_theme = theme;
        auto* density = new QComboBox; density->addItems({"standard", "compact"});
        bar->addWidget(new QLabel("Density ")); bar->addWidget(density);
        connect(density, &QComboBox::currentTextChanged, this, [this](const QString& value) {
            if (m_sync) return;
            Studio::Document next = m_document; next.density = value; change(next);
        });
        m_density = density;
        auto* dir = new QComboBox; dir->addItems({"ltr", "rtl"});
        bar->addWidget(new QLabel("Direction ")); bar->addWidget(dir);
        connect(dir, &QComboBox::currentTextChanged, this, [this](const QString& value) {
            if (m_sync) return;
            Studio::Document next = m_document; next.direction = value; change(next);
        });
        m_direction = dir;
        auto* viewport = new QComboBox; viewport->addItems({"Compact (360)", "Medium (840)", "Expanded (1200)"});
        bar->addWidget(new QLabel("Viewport ")); bar->addWidget(viewport);
        connect(viewport, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
                [this](int i) { m_viewport = i == 0 ? 360 : (i == 1 ? 840 : 1200); redraw(); });
        auto* dpr = new QComboBox; dpr->addItems({"1x", "2x"});
        bar->addWidget(new QLabel("DPR ")); bar->addWidget(dpr);
        connect(dpr, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
                [this](int i) { m_dpr = i == 0 ? 1 : 2; redraw(); });
        auto* file = menuBar()->addMenu("&File");
        file->addAction("New", this, [this] { change(Studio::Document{}); m_file.clear(); m_undo.clear(); });
        file->addAction("Open...", this, [this] { open(); });
        file->addAction("Save", this, [this] { save(); });
        file->addAction("Save as...", this, [this] { m_file.clear(); save(); });
        file->addSeparator();
        file->addAction("Export Qt Designer .ui...", this, [this] { exportFile(false); });
        file->addAction("Export C++ project...", this, [this] { exportFile(true); });
        auto* samples = menuBar()->addMenu("&Templates");
        for (const QString& name : {"dashboard", "settings", "form"}) {
            samples->addAction(name, this, [this, name] {
                change(Studio::templateDocument(name)); m_file.clear(); m_undo.clear();
            });
        }
        auto* view = menuBar()->addMenu("&View");
        view->addAction("Zoom in", this, [this] { m_canvas->scale(1.2, 1.2); });
        view->addAction("Zoom out", this, [this] { m_canvas->scale(1/1.2, 1/1.2); });
        view->addAction("Reset zoom", this, [this] { m_canvas->resetTransform(); });
        statusBar()->showMessage("8px snap · drag from palette · drag lower-right corner to resize · Ctrl+wheel zoom");
        connect(m_scene, &QGraphicsScene::selectionChanged, this, [this] {
            if (m_sync) return;
            for (QGraphicsItem* i : m_scene->selectedItems()) {
                if (auto* c = dynamic_cast<CanvasElement*>(i)) { m_selected = c->elementId; populateInspector(); return; }
            }
        });
        redraw();
    }
private:
    class Change final : public QUndoCommand {
    public:
        Change(Window* window, Studio::Document before, Studio::Document after)
            : m_window(window), m_before(std::move(before)), m_after(std::move(after)) {}
        void redo() override { m_window->showDocument(m_after); }
        void undo() override { m_window->showDocument(m_before); }
    private:
        Window* m_window;
        Studio::Document m_before, m_after;
    };
    void change(const Studio::Document& next)
    {
        if (Studio::serialize(next) == Studio::serialize(m_document)) return;
        m_undo.push(new Change(this, m_document, next));
    }
    void showDocument(const Studio::Document& d) { m_document = d; redraw(); }
    void add(const QString& type, QPoint at)
    {
        if (!Studio::lookup(type)) return;
        Studio::Document d = m_document;
        const QSize size = type == "surface.card" ? QSize(240, 140) : QSize(200, 52);
        at.setX(qBound(0, (at.x()/8)*8, qMax(0, d.width-size.width())));
        at.setY(qBound(0, (at.y()/8)*8, qMax(0, d.height-size.height())));
        const QString prefix = QString("element_");
        int serial = 1; QString id;
        do {
            id = prefix + QString::number(serial++);
        } while (std::any_of(d.items.begin(), d.items.end(), [&](const Studio::Item& i) { return i.id == id; }));
        d.items.append({id, type, QRect(at, size), Studio::lookup(type)->label});
        m_selected = id; change(d);
    }
    void edit(const QString& id, const QRect& rect)
    {
        Studio::Document next = m_document;
        for (Studio::Item& i : next.items) if (i.id == id) {
            i.rect = rect; break;
        }
        m_selected = id; change(next);
    }
    void editProperties()
    {
        if (m_sync || m_selected.isEmpty()) return;
        Studio::Document next = m_document;
        for (Studio::Item& i : next.items) if (i.id == m_selected) {
            i.text = m_text->text();
            const int x = qMin(m_x->value(), next.width - 24);
            const int y = qMin(m_y->value(), next.height - 24);
            i.rect = QRect(x, y, qBound(24, m_w->value(), next.width-x),
                                   qBound(24, m_h->value(), next.height-y));
        }
        change(next);
    }
    void removeSelected()
    {
        Studio::Document next = m_document;
        for (int n = next.items.size()-1; n >= 0; --n)
            if (next.items.at(n).id == m_selected) next.items.removeAt(n);
        m_selected.clear(); change(next);
    }
    void select(const QString& id)
    {
        m_selected = id; populateInspector();
        m_sync = true;
        for (QGraphicsItem* element : m_scene->items()) {
            if (auto* c = dynamic_cast<CanvasElement*>(element))
                c->setSelected(c->elementId == id);
        }
        m_sync = false;
    }
    void populateInspector()
    {
        m_sync = true;
        m_tree->clearSelection();
        for (int n = 0; n < m_tree->topLevelItemCount(); ++n) {
            QTreeWidgetItem* item = m_tree->topLevelItem(n);
            if (item->data(0, Qt::UserRole).toString() == m_selected) item->setSelected(true);
        }
        for (const Studio::Item& i : m_document.items) if (i.id == m_selected) {
            m_text->setText(i.text);
            m_x->setValue(i.rect.x()); m_y->setValue(i.rect.y());
            m_w->setValue(i.rect.width()); m_h->setValue(i.rect.height());
            m_sync = false; return;
        }
        m_text->clear();
        m_x->setValue(0); m_y->setValue(0); m_w->setValue(24); m_h->setValue(24);
        m_sync = false;
    }
    void redraw()
    {
        m_sync = true;
        m_scene->clear(); m_tree->clear();
        const int visibleWidth = qMin(m_viewport, m_document.width);
        m_scene->setSceneRect(0, 0, visibleWidth, m_document.height);
        const bool dark = m_document.theme == "dark";
        m_scene->setBackgroundBrush(dark ? QColor(42, 42, 48) : Qt::white);
        m_theme->setCurrentText(m_document.theme);
        m_density->setCurrentText(m_document.density);
        m_direction->setCurrentText(m_document.direction);
        for (const Studio::Item& e : m_document.items) {
            auto* frame = new CanvasElement(e, m_dpr);
            frame->changed = [this](const QString& id, const QRect& rect) { edit(id, rect); };
            m_scene->addItem(frame);
            if (e.id == m_selected) frame->setSelected(true);
            auto* tree = new QTreeWidgetItem(m_tree, {e.id, e.component});
            tree->setData(0, Qt::UserRole, e.id);
        }
        m_sync = false;
        populateInspector();
    }
    void open()
    {
        const QString path = QFileDialog::getOpenFileName(this, "Open UI project", {}, "Studio JSON (*.json)");
        if (path.isEmpty()) return;
        Studio::Document loaded; QString error;
        if (!Studio::load(path, &loaded, &error)) {
            QMessageBox::warning(this, "Invalid project", error); return;
        }
        m_selected.clear(); showDocument(loaded); m_file = path; m_undo.clear();
    }
    void save()
    {
        QString path = m_file;
        if (path.isEmpty()) path = QFileDialog::getSaveFileName(this, "Save UI project", {}, "Studio JSON (*.json)");
        if (path.isEmpty()) return;
        QString error;
        if (!Studio::save(path, m_document, &error))
            QMessageBox::warning(this, "Save error", error);
        else { m_file = path; statusBar()->showMessage("Saved " + path, 5000); }
    }
    void exportFile(bool cpp)
    {
        const QString path = cpp ? QFileDialog::getExistingDirectory(this, "Export C++ project")
            : QFileDialog::getSaveFileName(this, "Export Designer .ui", {}, "Qt Designer UI (*.ui)");
        if (path.isEmpty()) return;
        QString error;
        const bool ok = cpp ? Studio::exportCpp(m_document, path, &error)
                            : Studio::exportUi(m_document, path, &error);
        if (!ok) QMessageBox::warning(this, "Export failed", error);
        else statusBar()->showMessage("Exported (pixel parity not guaranteed)", 6500);
    }
    Studio::Document m_document;
    QString m_file, m_selected;
    bool m_sync = false;
    int m_viewport = 840;
    qreal m_dpr = 1;
    QUndoStack m_undo;
    Canvas* m_canvas = nullptr;
    QGraphicsScene* m_scene = nullptr;
    Palette* m_palette = nullptr;
    QTreeWidget* m_tree = nullptr;
    QLineEdit* m_text = nullptr;
    QSpinBox *m_x = nullptr, *m_y = nullptr, *m_w = nullptr, *m_h = nullptr;
    QComboBox *m_theme = nullptr, *m_density = nullptr, *m_direction = nullptr;
};
}
int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    if (argc >= 3) {
        Studio::Document document;
        QString error;
        if (!Studio::load(QString::fromLocal8Bit(argv[2]), &document, &error)) {
            qCritical("%s", qPrintable(error)); return 2;
        }
        const QString op = QString::fromLocal8Bit(argv[1]);
        if (op == "--check") return 0;
        if (argc != 4) return 2;
        const QString output = QString::fromLocal8Bit(argv[3]);
        const bool ok = op == "--export-ui" ? Studio::exportUi(document, output, &error)
            : op == "--export-cpp" ? Studio::exportCpp(document, output, &error) : false;
        if (!ok) qCritical("%s", qPrintable(error));
        return ok ? 0 : 2;
    }
    Window window;
    window.show();
    return app.exec();
}
