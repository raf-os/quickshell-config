import QtQuick

Image {
	id: root

	required property string icon
	required property int size
	property string fallback
	property string path

	asynchronous: true
	width: size
	height: size
	source: {
		let s = `image://qicons/qt/${icon}`;
		if (fallback === "")
			return s;

		const params = [];
		if (fallback !== "")
			params.push(`fallback=${fallback}`);
		if (path !== "")
			params.push(`path=${path}`);
		s += `?${params.join("&")}`;

		return s;
	}
}
