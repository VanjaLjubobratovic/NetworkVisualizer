import QtQuick          2.7
import QtQuick.Shapes   1.0

import QuickQanava        2.0 as Qan
import "qrc:/QuickQanava" as Qan

Qan.EdgeItem {
	id: edgeItem

	// Private hack for visual connector edge color dynamic modification
	property color color: style ? style.lineColor : Qt.rgba(0.,0.,0.,1.)
	Qan.EdgeTemplate {
		anchors.fill: parent
		edgeItem: parent
		color: parent.color
	}

	/*Rectangle {
		anchors.fill: parent
		color: "blue"
	}*/
}
