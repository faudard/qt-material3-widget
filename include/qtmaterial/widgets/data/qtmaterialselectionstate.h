#pragma once

#include <QByteArray>

#include "qtmaterial/qtmaterialglobal.h"

class QAbstractItemView;

namespace QtMaterial {

/**
 * Versioned, application-opt-in selection persistence by stable row ID.
 *
 * The application provides a unique, nonempty QString at stableIdRole on
 * column zero of every loaded row. Identity and source-model lifetimes stay
 * in application code. This works through ordinary proxy-model chains.
 *
 * A restore validates the entire payload and resolves every requested ID
 * before changing the live selection. It never invokes fetchMore(), edits
 * model data or transfers ownership of a model.
 */
class QTMATERIAL3_WIDGETS_EXPORT QtMaterialSelectionState
{
public:
    static QByteArray save(const QAbstractItemView* view, int stableIdRole);

    // Returns false without changing selection on malformed, ambiguous,
    // missing/filtered IDs, incompatible columns or scan-limit exhaustion.
    static bool restore(QAbstractItemView* view, const QByteArray& state,
                        int stableIdRole, int maximumRowsToScan = 250000);
};

} // namespace QtMaterial
