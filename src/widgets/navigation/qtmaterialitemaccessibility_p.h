#pragma once

#include <QAccessibleObject>
#include <QAccessibleWidget>
#include <QCoreApplication>
#include <QKeyEvent>
#include <QPointer>
#include <QVector>
#include <functional>
#include <utility>

#ifndef QT_NO_ACCESSIBILITY
namespace QtMaterialItemAccessibility {

struct ItemAccess {
    std::function<int()> count;
    std::function<int()> current;
    std::function<QString(int, QAccessible::Text)> text;
    std::function<QRect(int)> rect;
    std::function<QAccessible::Role(int)> role;
    std::function<QAccessible::State(int)> state;
    std::function<void(int)> select;
};

// QObject ownership lets Qt's accessibility cache invalidate item interfaces
// when the widget is destroyed or its item count shrinks.
class ItemObject final : public QObject {
public:
    ItemObject(QWidget* widget, int row, const ItemAccess& callbacks)
        : QObject(widget), owner(widget), index(row), access(callbacks) {}
    QPointer<QWidget> owner;
    int index;
    ItemAccess access;
};

class ItemInterface final : public QAccessibleObject, public QAccessibleActionInterface {
public:
    explicit ItemInterface(ItemObject* item) : QAccessibleObject(item), m_item(item) {}
    bool isValid() const override {
        return m_item && m_item->owner && m_item->index < m_item->access.count();
    }
    QAccessibleInterface* parent() const override {
        return isValid() ? QAccessible::queryAccessibleInterface(m_item->owner) : nullptr;
    }
    int childCount() const override { return 0; }
    QAccessibleInterface* child(int) const override { return nullptr; }
    int indexOfChild(const QAccessibleInterface*) const override { return -1; }
    QRect rect() const override {
        if (!isValid() || !m_item->owner->isVisible()) { return {}; }
        const QRect local = m_item->access.rect(m_item->index);
        return QRect(m_item->owner->mapToGlobal(local.topLeft()), local.size());
    }
    QWindow* window() const override {
        return isValid() ? m_item->owner->window()->windowHandle() : nullptr;
    }
    QString text(QAccessible::Text type) const override {
        return isValid() ? m_item->access.text(m_item->index, type) : QString();
    }
    void setText(QAccessible::Text, const QString&) override {}
    QAccessible::Role role() const override {
        return isValid() ? m_item->access.role(m_item->index) : QAccessible::NoRole;
    }
    QAccessible::State state() const override {
        if (!isValid()) { QAccessible::State result; result.invalid = true; return result; }
        auto result = m_item->access.state(m_item->index);
        result.disabled |= !m_item->owner->isEnabled();
        result.invisible = !m_item->owner->isVisible();
        result.focused = result.selected && m_item->owner->hasFocus();
        return result;
    }
    void* interface_cast(QAccessible::InterfaceType type) override {
        return type == QAccessible::ActionInterface
            ? static_cast<QAccessibleActionInterface*>(this) : nullptr;
    }
    QStringList actionNames() const override {
        if (!isValid() || state().disabled || role() == QAccessible::Separator) { return {}; }
        return {pressAction(), setFocusAction()};
    }
    void doAction(const QString& action) override {
        if (!actionNames().contains(action)) { return; }
        const QPointer<QWidget> owner = m_item->owner;
        const auto select = m_item->access.select;
        select(m_item->index);
        if (!owner) { return; }
        owner->setFocus(Qt::OtherFocusReason);
        if (action == pressAction()) {
            QKeyEvent event(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier);
            QCoreApplication::sendEvent(owner, &event);
        }
    }
    QStringList keyBindingsForAction(const QString& action) const override {
        return action == pressAction() ? QStringList{QStringLiteral("Enter"), QStringLiteral("Space")} : QStringList{};
    }
private:
    QPointer<ItemObject> m_item;
};

class ItemWidgetInterface final : public QAccessibleWidget {
public:
    ItemWidgetInterface(QWidget* widget, QAccessible::Role role, ItemAccess access)
        : QAccessibleWidget(widget, role), m_access(std::move(access)) {}
    ~ItemWidgetInterface() override { for (const auto& child : m_children) { delete child.data(); } }
    int childCount() const override {
        const int count = isValid() ? m_access.count() : 0;
        while (m_children.size() > count) { delete m_children.takeLast().data(); }
        return count;
    }
    QAccessibleInterface* child(int index) const override {
        if (index < 0 || index >= childCount()) { return nullptr; }
        while (m_children.size() < childCount()) {
            auto* item = new ItemObject(widget(), m_children.size(), m_access);
            QAccessible::registerAccessibleInterface(new ItemInterface(item));
            m_children.push_back(item);
        }
        return QAccessible::queryAccessibleInterface(m_children.at(index));
    }
    int indexOfChild(const QAccessibleInterface* item) const override {
        for (int i = 0; i < childCount(); ++i) { if (child(i) == item) { return i; } }
        return -1;
    }
    QAccessibleInterface* childAt(int x, int y) const override {
        for (int i = 0; i < childCount(); ++i) {
            auto* item = child(i);
            if (item && item->rect().contains(x, y)) { return item; }
        }
        return nullptr;
    }
    QAccessibleInterface* focusChild() const override {
        return isValid() && widget()->hasFocus() ? child(m_access.current()) : nullptr;
    }
private:
    ItemAccess m_access;
    mutable QVector<QPointer<ItemObject>> m_children;
};

inline void notifyItems(QWidget* widget) {
    if (!QAccessible::isActive()) { return; }
    auto* root = QAccessible::queryAccessibleInterface(widget);
    if (!root) { return; }
    for (int i = 0; i < root->childCount(); ++i) {
        auto* item = root->child(i);
        if (!item) { continue; }
        QAccessible::State changed;
        changed.selected = true; changed.checked = true; changed.disabled = true; changed.focused = true;
        QAccessibleStateChangeEvent event(item, changed);
        QAccessible::updateAccessibility(&event);
    }
    if (widget->hasFocus()) {
        if (auto* focused = root->focusChild()) {
            QAccessibleEvent event(focused, QAccessible::Focus);
            QAccessible::updateAccessibility(&event);
        }
    }
}

inline void notifyStructure(QWidget* widget) {
    if (!QAccessible::isActive()) { return; }
    if (auto* root = QAccessible::queryAccessibleInterface(widget)) {
        root->childCount(); // Invalidates cached item objects removed from the widget.
    }
    QAccessibleEvent event(widget, QAccessible::ObjectReorder);
    QAccessible::updateAccessibility(&event);
}
} // namespace QtMaterialItemAccessibility
#endif
