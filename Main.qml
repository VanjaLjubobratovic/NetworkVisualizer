import QtQuick
import QtQuick.Window 2.3
import QtQuick.Controls 2.5
import QtQuick.Controls.Material 2.3
import QtQuick.Dialogs
import QtQuick.Layouts

import QuickQanava 2.0 as Qan
import "qrc:/QuickQanava" as Qan
import CustomElems 1.0 as Custom

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
						activeBtn.visible = true
						maliciousBtn.visible = true
					} else {
						graphElement.connectorEnabled = false
						addNodeBtn.visible = false
						activeBtn.visible = false
						maliciousBtn.visible = false
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

			MenuItem {
				text: "info test"
				onTriggered: {
					nodeInfoWindow.visible = true;
				}
			}
		}

		z:2
	}

	/*Qan.GraphView {
		id: graphView
		objectName: "graphView"
		anchors.fill: parent
		navigable: true
		graph: Qan.Graph {
			id: graphElement
			objectName: "graph"
			anchors.fill: parent
			connectorEnabled: false;

			onNodeInserted: {
				tooltip.visible = false
			}

			onNodeDoubleClicked: (node) => {
				var info = graphModel.getNodeInfo(node)
				nodeInfoWindow.visible = true
				infoLabel.text = info
			}

		}

		Keys.onPressed: (event) => {
			if(event.key == Qt.Key_Delete) {
				console.log("Delete pressed");
				event.accepted = true
				graphModel.removeSelected()
			}
		}

		z:1
	}*/

	Qan.GraphView {
		id: graphView
		objectName: "graphView"
		anchors.fill: parent
		navigable: true
		graph: Custom.CustomNetworkGraph {
			id: graphElement
			objectName: "graph"
			anchors.fill: parent
			connectorEnabled: false;

			onNodeInserted: {
				tooltip.visible = false
			}

			onNodeDoubleClicked: (node) => {
				var info = graphModel.getNodeInfo(node)
				nodeInfoWindow.visible = true
				infoLabel.text = info
			}

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

	RowLayout {
		anchors.bottom: parent.bottom
		anchors.horizontalCenter: parent.horizontalCenter
		spacing: 10
		height: 100
		width: 100


		RoundButton {
			id: addNodeBtn
			text: "+"
			width: 70
			height: 70
			visible: false

			Material.background: Material.BlueGrey

			onClicked: {
				graphModel.readyToInsertNode(activeBtn.active, maliciousBtn.malicious)
				tooltip.visible = !tooltip.visible
				tooltip.x = addNodeBtn.x
				tooltip.y = addNodeBtn.y
			}

			z:2
		}

		RoundButton {
			id: activeBtn
			property bool active: true

			Material.background: active ? Material.Blue : Material.Grey
			text: active ? "Active" : "Inactive"

			width: 70
			height: 70
			visible: false

			onClicked: {
				active = !active
				Material.background = active ? Material.Blue : Material.Grey
				text = active ? "Active" : "Inactive"
			}
		}

		RoundButton {
			id: maliciousBtn
			property bool malicious: false

			Material.background: malicious ? Material.Red : Material.Blue
			text: malicious ? "Malicious" : "Good"

			width: 70
			height: 70
			visible: false

			onClicked: {
				malicious = !malicious
				Material.background = malicious ? Material.Red : Material.Blue
				text = malicious ? "Malicious" : "Good"
			}

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
			//id: networkItem
			color: "#424242"
			Label {
				id: networkItemLabel
				font.pixelSize: 16
				padding: 8
				color: "white"
			}
		}

		Rectangle {
			//id: nodesItem
			color: "#424242"
			Label {
				id: nodesItemLabel
				color: "white"
			}
		}

		Rectangle {
			//id: edgesItem
			color: "#424242"
			Label {
				id: edgesItemLabel
				color: "white"
			}
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

	ToolTip {
		id: tooltip
		//timeout: 1500
		visible: false

		background: Rectangle {
			color: "#424242"
			radius: 5
		}

		Label {
			id: toolLabel
			text: "Click anywhere to add node!"
			font.pixelSize: 12
		}
	}

	Window {
		id: nodeInfoWindow
		title: "Node info"
		width: 200
		height: 200
		visible: false
		modality: Qt.NonModal
		flags: Qt.Tool

		Rectangle {
			width: parent.width
			height: parent.height
			color: "lightgray"

			Label {
				id: infoLabel
				text: "Popup Content"
				anchors.centerIn: parent
				font.bold: true
			}
		}

	}

	Timer {
		id: infoTimer
		interval: 500
		running: true
		repeat: true

		onTriggered: {
			networkItemLabel.text = graphModel.getNetworkInfo()
			infoTimer.interval = 2000
		}
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

	//Mouse area for getting cursor coordinates for tooltips
	MouseArea {
		id: tooltipMouseArea
		anchors.fill: parent
		hoverEnabled: true

		onPositionChanged: {
			if (tooltip.visible) {
				tooltip.x = mouseX + 5
				tooltip.y = mouseY - 5
			}
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
