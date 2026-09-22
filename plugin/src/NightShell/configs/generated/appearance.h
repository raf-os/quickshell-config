#pragma once

#include "serializableobject.h"

#include <qobject.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>
#include <qproperty.h>
#include <qstring.h>

namespace ns::configs {
class FontFamily : public utils::SerializableObject {
	Q_OBJECT
	QML_ELEMENT
	QML_UNCREATABLE("")

	Q_PROPERTY(QString sans READ default WRITE default NOTIFY sansChanged BINDABLE bindableSans RESET resetSans)
	Q_PROPERTY(QString mono READ default WRITE default NOTIFY monoChanged BINDABLE bindableMono RESET resetMono)

public:
	explicit FontFamily(const QString &className, QObject *root, QObject *parent);
	[[nodiscard]] QBindable<QString> bindableSans() const;
	void resetSans();
	[[nodiscard]] QBindable<QString> bindableMono() const;
	void resetMono();

signals:
	void sansChanged();
	void monoChanged();

private:
	Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(FontFamily, QString, b_sans, "Inter", &FontFamily::sansChanged)
	Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(FontFamily, QString, b_mono, "RobotoMono Nerd Font Propo", &FontFamily::monoChanged)
};

class AppearanceConfig : public utils::SerializableObject {
	Q_OBJECT
	QML_ELEMENT
	QML_UNCREATABLE("")

	Q_PROPERTY(int scaling READ default WRITE default NOTIFY scalingChanged BINDABLE bindableScaling RESET resetScaling)
	Q_PROPERTY(ns::configs::FontFamily *fontFamily READ fontFamily CONSTANT)
public:
	explicit AppearanceConfig(const QString &className, QObject *root, QObject *parent);
	[[nodiscard]] QBindable<int> bindableScaling() const;
	void resetScaling();
	[[nodiscard]] FontFamily *fontFamily();

signals:
	void scalingChanged();

private:
	FontFamily m_fontFamily{"FontFamily", this, this};
	Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(AppearanceConfig, int, b_scaling, 1, &AppearanceConfig::scalingChanged)
};
} // namespace ns::configs