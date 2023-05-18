import QtQuick
import QtQuick.Window 2.3
import QtQuick.Controls 2.5
import QtQuick.Controls.Material 2.3
import QuickQanava 2.0 as Qan
import "qrc:/QuickQanava" as Qan

Window {
	width: 1280
	height: 720
	visible: true
	title: qsTr("Network visualizer")

	Qan.GraphView {
		id: graphView
		anchors.fill: parent
		navigable: true
		graph: Qan.Graph {
			objectName: "graph"
			anchors.fill: parent
		}
	}
}
