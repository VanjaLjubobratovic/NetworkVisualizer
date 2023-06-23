import QtQuick 2.7
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3

Item {
	id: fileTransferItem
	width: 30
	height: 30
	x: 0
	y: 0

	property int parentWidth: 0
	property int parentHeight: 0

	Image {
		id: fileImage
		source: "qrc:/FileIcons/containsfiles.png"
		anchors.fill: parent
	}

	function startFileTransfer(direction) {
		xAnim.to = direction.x
		yAnim.to = direction.y
		animation.start();
	}

	function onParentHeightChanged() {
		var oldW = parentWidth
		var oldH = parentHeight
		parentWidth = fileTransferItem.parent.width
		parentHeight = fileTransferItem.parent.height

		console.log(parentWidth, oldW)

		var ratioH = parentHeight / oldH
		var ratioW = parentWidth / oldW

		console.log(ratioH, ratioW)

		fileTransferItem.x *= ratioW
		fileTransferItem.y *= ratioH

		//console.log(fileTransferItem.x, " ", fileTransferItem.y)

		//updatePosition()
	}

	function updatePosition() {
		if(fileTransferItem.parent == null)
			return
		var diagonalLen = Math.sqrt(Math.pow(parentWidth, 2) + Math.pow(parentHeight, 2))
		var dx = (parentWidth - diagonalLen) / 2
		var dy = (parentHeight - diagonalLen) / 2

		fileTransferItem.x += dx
		fileTransferItem.y += dy
		console.log(dx, " ", dy)
	}

	ParallelAnimation {
		id: animation
		PropertyAnimation {
			id: xAnim
			target: fileTransferItem
			property: "x"
			to: 0
			duration: 2000
		}

		PropertyAnimation {
			id: yAnim
			target: fileTransferItem
			property: "y"
			to: 0
			duration: 2000
		}
	}

	onParentChanged: {
		if(fileTransferItem.parent !== null) {
			parentWidth = fileTransferItem.parent.width
			parentHeight = fileTransferItem.parent.height
		}
	}


}
