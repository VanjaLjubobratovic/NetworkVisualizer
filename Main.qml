import QtQuick
import QtQuick.Window 2.3
import QtQuick.Controls 2.5
import QtQuick.Controls.Material 2.3
import QtQuick.Dialogs

import QuickQanava 2.0 as Qan
import "qrc:/QuickQanava" as Qan

Window {
	width: 1280
	height: 720
	visible: true
	title: qsTr("Network visualizer")

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
					graphModel.clearGraph();
				}
			}
			MenuItem {
				text: "Open file..."
				onTriggered:  {
					console.log("File -> Open selected")
					openFile()
				}
			}
			MenuItem {
				text: "Save file..."
				onTriggered: {
					console.log("File -> Save selected")
					saveFile()
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
			MenuItem {
				text: "Enable drawing"
				id: drawingCheckbox
				checkable: true
				checked: false

				onCheckedChanged: {
					if(checked) {
						graphElement.connectorEnabled = true
						addNodeBtn.enabled = true
					} else {
						graphElement.connectorEnabled = false
						addNodeBtn.enabled = false
					}
				}
			}
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
			id: graphElement
			objectName: "graph"
			anchors.fill: parent
			connectorEnabled: false;
		}

		Keys.onPressed: (event) => {
			if(event.key == Qt.Key_Delete) {
				console.log("Delete pressed");
				event.accepted = true
				graphModel.removeSelected()
			}
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
		enabled: false

		Material.background: Material.BlueGrey

		onClicked: {
			graphModel.drawNewNode();
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

	function openFile() {
		fileDialog.fileMode = FileDialog.OpenFile
		fileDialog.title = "Open File"
		fileDialog.open()
	}

	function saveFile() {
		fileDialog.fileMode = FileDialog.SaveFile
		fileDialog.title = "Save File"
		fileDialog.open()
	}

	FileDialog {
		id: fileDialog
		nameFilters: ["JSON files (*.json)"]

		onAccepted: {
			if(fileDialog.fileMode == FileDialog.OpenFile) {
				console.log("Opening file: ", selectedFile)
				graphModel.readFromFile(selectedFile)
			} else {
				console.log("Saving file: ", selectedFile)
				graphModel.saveToFile(selectedFile)
			}
		}
	}
}
