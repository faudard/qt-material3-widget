#pragma once

#include <QObject>
#include <QtUiPlugin/QDesignerCustomWidgetCollectionInterface>

class QtMaterial3DesignerCollection final
    : public QObject
    , public QDesignerCustomWidgetCollectionInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QDesignerCustomWidgetCollectionInterface")
    Q_INTERFACES(QDesignerCustomWidgetCollectionInterface)

public:
    explicit QtMaterial3DesignerCollection(QObject* parent = nullptr);

    QList<QDesignerCustomWidgetInterface*> customWidgets() const override;

private:
    QList<QDesignerCustomWidgetInterface*> widgets_;
};
