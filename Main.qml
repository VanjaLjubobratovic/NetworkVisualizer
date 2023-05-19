import QtQuick
import QtQuick.Window 2.3
import QtQuick.Controls 2.5
import QtQuick.Controls.Material 2.3
import QuickQanava 2.0 as Qan
import "qrc:/QuickQanava" as Qan
//import GraphModel 1.0

Window {
	width: 1280
	height: 720
	visible: true
	title: qsTr("Network visualizer")

	/*GraphModel {
		id: graphModel
	}*/

	Qan.GraphView {
		id: graphView
		anchors.fill: parent
		navigable: true
		graph: Qan.Graph {
			objectName: "graph"
			anchors.fill: parent
		}
	}

	RoundButton {
		id: addNodeBtn
		text: "+"
		width: 70
		height: 70
		anchors.bottom: parent.bottom
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.bottomMargin: 10

		Material.background: Material.BlueGrey

		/*onClicked: {
			graphModel.addNode();
		}*/

		onClicked: {
			graphModel.addNode();
		}
	}
}
