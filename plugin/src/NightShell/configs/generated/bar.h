#pragma once

#include "serializableobject.h"

#include <qobject.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>
#include <qproperty.h>

namespace ns::configs {
class Bar : public utils::SerializableObject {
	Q_OBJECT
	QML_ELEMENT
	QML_UNCREATABLE("")

	Q_PROPERTY(int height READ default WRITE default NOTIFY heightChanged BINDABLE bindableHeight RESET resetHeight)

public:
	explicit Bar(const QString &className, QObject *root, QObject *parent);
	[[nodiscard]] QBindable<int> bindableHeight() const;
	void resetHeight();

signals:
	void heightChanged();

private:
	Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(Bar, int, b_height, 32, &Bar::heightChanged)
};
} // namespace ns::configs