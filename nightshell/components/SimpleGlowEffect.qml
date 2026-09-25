import QtQuick

WrappedMultiEffect {
	id: root

	shadowEnabled: shouldEnable
	blurMax: 12
	blurMultiplier: 1
	brightness: shouldEnable ? 0.2 : 0
	contrast: shouldEnable ? 0.3 : 0
}
