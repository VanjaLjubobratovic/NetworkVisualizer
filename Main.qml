import QtQuick
import QtQuick.Window 2.3
import QtQuick.Controls 2.5
import QtQuick.Controls.Material 2.3
import QtQuick.Dialogs
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects

import QuickQanava 2.0 as Qan
import "qrc:/QuickQanava" as Qan
import CustomElems 1.0 as Custom
import "qrc:/" as Custom

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
						drawingControls.visible = true
					} else {
						graphElement.connectorEnabled = false
						drawingControls.visible = false
					}
					graphModel.toggleDrawing()
				}
			}

			MenuItem {
				text: "Auto arrange nodes"
				id: arrangeNodes

				onTriggered: {
					graphModel.forceDirectedLayout();
				}
			}
		}

		Menu {
			title: "View"
			Material.theme: Material.Dark

			MenuItem {
				id: legendCheckbox
				text: "Legend"
				checkable: true
				checked: false

				onCheckedChanged: {
					legendRectangle.visible = checked
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
				addNodeBtn.addingNode = false
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
		id: drawingControls
		anchors.bottom: parent.bottom
		anchors.horizontalCenter: parent.horizontalCenter
		spacing: 10
		height: 100
		visible: false

		RoundButton {
			id: addNodeBtn
			property bool addingNode: false
			text: "+"
			Layout.preferredHeight: 70
			Layout.preferredWidth: 70
			visible: true

			Material.background: Material.BlueGrey

			onClicked: {
				addingNode = !addingNode
				tooltip.visible = !tooltip.visible
				tooltip.x = addNodeBtn.x
				tooltip.y = addNodeBtn.y
			}

			onAddingNodeChanged: {
				Material.background = addingNode ? Material.accent : Material.BlueGrey
				graphModel.addingNode = addingNode
			}

			z:2
		}

		RoundButton {
			id: activeBtn
			property bool active: true

			Material.background: active ? Material.Blue : Material.Grey
			text: active ? "Active" : "Inactive"

			Layout.preferredHeight: 70
			Layout.preferredWidth: 150
			visible: true

			onClicked: {
				active = !active
				Material.background = active ? Material.Blue : Material.Grey
				text = active ? "Active" : "Inactive"
				graphModel.newActive = active
			}
		}

		RoundButton {
			id: maliciousBtn
			property bool malicious: false

			Material.background: malicious ? Material.Red : Material.Blue
			text: malicious ? "Malicious" : "Good"

			Layout.preferredHeight: 70
			Layout.preferredWidth: 150
			visible: true

			onClicked: {
				malicious = !malicious
				Material.background = malicious ? Material.Red : Material.Blue
				text = malicious ? "Malicious" : "Good"
				graphModel.newMalicious = malicious
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
			id: networkItem
			color: "#424242"
			Label {
				id: networkItemLabel
				font.pixelSize: 16
				padding: 8
				color: "white"
			}
		}

		Rectangle {
			id: nodesItem
			color: "#424242"
			ColumnLayout {
				anchors.fill: parent
				anchors.margins: 10
				anchors.top: parent.top
				anchors.right: parent.right

				NodeListView {
					Layout.fillWidth: true
					Layout.fillHeight: true
					model: graphElement.nodes
					graphView: graphView
				}
				z:3
			}
		}

		Rectangle {
			id: edgesItem
			color: "#424242"
			Label {
				id: edgesItemLabel
				color: "white"
			}
		}

		z: 3
	}

	Rectangle {
		id: legendRectangle
		color: "#424242"
		anchors.top: parent.top
		anchors.right: parent.right
		visible: false
		width: 200
		height: 250

		ColorLegend {}

		z:3
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
				tooltip.x = mouseX + 10
				tooltip.y = mouseY
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
