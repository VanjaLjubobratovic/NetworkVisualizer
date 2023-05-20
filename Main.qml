import QtQuick
import QtQuick.Window 2.3
import QtQuick.Controls 2.5
import QtQuick.Controls.Material 2.3
import QtQuick.Dialogs

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

	MenuBar {
		id: topMenuBar
		Material.theme: Material.Dark
		Menu {
			title: "File"
			Material.theme: Material.Dark
			MenuItem {
				text: "New graph..."
				onTriggered:  {
					console.log("File -> Open selected")
				}
			}
			MenuItem {
				text: "Open file..."
				onTriggered:  {
					fileDialog.open()
					console.log("File -> Open selected")
				}
			}
			MenuItem {
				text: "Save file..."
				onTriggered: {
					console.log("File -> Save selected")
				}
			}
			MenuSeparator{}
			MenuItem {
				text: "Quit..."
				onTriggered:  {
					quitDialog.open()
				}
			}
		}

		Menu {
			title: "Edit"
			Material.theme: Material.Dark
		}

		Menu {
			title: "Help"
			Material.theme: Material.Dark
		}

		z:2
	}

	Qan.GraphView {
		id: graphView
		anchors.fill: parent
		navigable: true
		graph: Qan.Graph {
			objectName: "graph"
			anchors.fill: parent
		}

		z:1
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

		onClicked: {
			graphModel.addNode();
		}

		z:2
	}

	Dialog {
		id: quitDialog
		anchors.centerIn: parent
		Material.theme: Material.Dark
		title: "Are you sure you want to quit?"
		standardButtons: Dialog.Yes | Dialog.Cancel

		onAccepted: {
			console.log("quitDialog -> Yes clicked")
			Qt.quit()
		}
		onRejected: {
			console.log("quitDialog -> Cancel clicked")
		}

		z:2
	}

	FileDialog {
		id: fileDialog
		title: "Open File"
		nameFilters: ["JSON files (*.json)"]

		onAccepted: {
			console.log("Selected file: ", selectedFile)
			graphModel.readSavedGraph(selectedFile)
		}
	}
}
