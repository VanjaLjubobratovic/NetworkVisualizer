import QtQuick
import QtQuick.Window 2.3
import QtQuick.Controls 2.5
import QtQuick.Controls.Material 2.3
import QtQuick.Dialogs
import QtQuick.Layouts

import QuickQanava 2.0 as Qan
import "qrc:/QuickQanava" as Qan

Window {
	id: mainWindow
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
						addNodeBtn.visible = true
					} else {
						graphElement.connectorEnabled = false
						addNodeBtn.visible = false
					}
					graphModel.toggleDrawing()
				}
			}

			MenuItem {
				text: "Auto arrange nodes"
				id: arrangeNodes

				onTriggered: {
					graphModel.forceDirectedLayout(graphModel.ge);
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
		objectName: "graphView"
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
		visible: false

		Material.background: Material.BlueGrey

		onClicked: {
			graphModel.readyToInsertNode();
		}

		z:2
	}


	StackLayout {
		id: infoStackView
		anchors.bottom: infoTabBar.top
		anchors.right: parent.right
		width: parent.width * 0.3
		height: parent.height * 0.4
		visible: true

		currentIndex: infoTabBar.currentIndex

		Rectangle {
			id: networkItem
			color: "#424242"
		}

		Rectangle {
			id: nodesItem
			color: "#424242"
		}

		Rectangle {
			id: edgesItem
			color: "#424242"
		}

		z: 3
	}

	RoundButton {
		id: infoMinimizeBtn
		text: infoStackView.visible ? "V" : "Ʌ"
		anchors.right: infoStackView.right
		anchors.top: infoStackView.top
		width: 30
		height: 30
		font.pixelSize: 10
		radius: 0

		onClicked: {
			minimizeWindow()
		}

		z:3
	}

	TabBar {
		id: infoTabBar
		anchors.bottom: parent.bottom
		anchors.left: infoStackView.left
		width: infoStackView.width
		font.pixelSize: 12
		visible: true
		contentHeight: 30

		Material.theme: Material.Dark

		TabButton {
			id: netTab
			text: "Network"
			onClicked: {
				infoStackView.currentIndex = 0
				maximizeWindow()
			}

		}

		TabButton {
			id: nodeTab
			text: "Nodes"
			onClicked: {
				infoStackView.currentIndex = 1
				maximizeWindow()
			}
		}

		TabButton {
			id: edgeTab
			text: "Edges"
			onClicked: {
				infoStackView.currentIndex = 2
				maximizeWindow()
			}
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

	ParallelAnimation {
		id: minimizeAnimation

		SequentialAnimation {
			PropertyAnimation {
				target: infoStackView
				property: "opacity"
				to: 0
				duration: 100
			}

			PropertyAnimation {
				target: infoStackView
				property: "height"
				to: 0
				duration: 300
			}
		}

		SequentialAnimation {
			PropertyAnimation {
				target: infoMinimizeBtn
				property: "opacity"
				to: 0
				duration: 300
			}
		}
	}

	SequentialAnimation {
		id: maximizeAnimation
		PropertyAnimation {
			target: infoStackView
			property: "opacity"
			to: 100
			duration: 100
		}

		PropertyAnimation {
			target: infoStackView
			property: "height"
			to: mainWindow.height * 0.4
			duration: 300
		}

		PropertyAnimation {
			target: infoMinimizeBtn
			property: "opacity"
			to: 100
			duration: 400
		}
	}

	function minimizeWindow() {
		minimizeAnimation.start()
	}

	function maximizeWindow() {
		maximizeAnimation.start()
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
