import QtQuick 2.7
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.1

import QuickQanava 2.0 as Qan
import "qrc:/QuickQanava" as Qan

ListView {
	id: nodesListView

	model: undefined

	property var graphView: undefined

	clip: true
	spacing: 4
	focus: true
	flickableDirection: Flickable.VerticalFlick

	highlightFollowsCurrentItem: false
	highlight: Rectangle {
		visible: nodesListView.currentItem !== undefined &&
				 nodesListView.currentItem !== null
		x: 0
		y: nodesListView.currentItem ? nodesListView.currentItem.y : 0
		width: nodesListView.width
		height: nodesListView.currentItem ? nodesListView.currentItem.height : 0
		color: "#e5829e"
		opacity: 0.6
		radius: 3
		Behavior on y {
			SpringAnimation {
				duration: 200
				spring: 1
				damping: 0.2
			}
		}
	}

	Menu {
		id: nodeMenu
		title: qsTr("Nodes")
		property var node: undefined

		Material.theme: Material.Dark

		MenuItem {
			text: qsTr("Center On")
			enabled: nodesListView.graphView !== undefined
			onTriggered: {
				if(nodesListView.graphView && nodeMenu.node && nodeMenu.node.item)
					graphView.centerOn(nodeMenu.node.item)
			}
		}
	}

	delegate: Item {
		id: nodeDelegate
		width: ListView.view.width
		height: 30
		Label {
			id: nodeLabel
			text: "Label " + itemData.label
		}
		MouseArea {
			anchors.fill: nodeDelegate
			acceptedButtons: Qt.AllButtons
			onClicked: {
				nodeMenu.node = itemData
				nodesListView.currentIndex = index
				if(mouse.button == Qt.RightButton)
					nodeMenu.popup()
			}
		}
	}
}
