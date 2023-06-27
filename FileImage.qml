import QtQuick 2.7
import QtQml 2.3
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
	property int diagonal: 0
	property real runningTime: 0
	property string sender //Either dst or src node

	signal transferEnded(objectId: string)

	Image {
		id: fileImage
		source: "qrc:/FileIcons/containsfiles.png"
		anchors.fill: parent
	}

	Image {
		id: resultImage
		anchors.fill: parent
		visible: false
		z:2
	}

	function startFileTransfer(direction, runningTime) {
		xAnim.to = direction.x
		yAnim.to = direction.y

		calculateDiagonal()

		fileTransferItem.runningTime = runningTime

		xAnim.duration = runningTime
		yAnim.duration = runningTime

		animation.start()
	}

	function onParentDimensionsChanged() {
		animation.stop()

		//Old distance to destination
		var toX = xAnim.to
		var toY = yAnim.to
		var distance = Math.sqrt(Math.pow((toX - fileTransferItem.x), 2) + Math.pow((toY - fileTransferItem.y), 2))

		var ratio = diagonal / distance

		//console.log("OLD DIAG: ", diagonal)

		//Calculating new values
		parentWidth = fileTransferItem.parent.width
		parentHeight = fileTransferItem.parent.height
		calculateDiagonal() //new diagonal
		var newDist = diagonal / ratio

		//console.log("DIST: ", distance, " DIAG: ", diagonal, " NEW: ", newDist, " R: ", ratio)

		//Calculate direction vector
		var senderCoords, receiverCoords
		if(sender == "src") {
			senderCoords = fileTransferItem.parent.p1
			receiverCoords = fileTransferItem.parent.p2
		} else {
			senderCoords = fileTransferItem.parent.p2
			receiverCoords = fileTransferItem.parent.p1
		}

		//When I just subtract two QPointFs I just get NaN for some reason
		var direction = Qt.point(receiverCoords.x - senderCoords.x, receiverCoords.y - senderCoords.y)

		//Normalize direction vector
		var dirLen = Math.sqrt(Math.pow(direction.x, 2) + Math.pow(direction.y, 2))
		var normalizedDir = Qt.point(direction.x / dirLen, direction.y / dirLen)

		//New point coords
		var newPoint = Qt.point(receiverCoords.x - (normalizedDir.x * newDist), receiverCoords.y - (normalizedDir.y * newDist))


		/*console.log("DIR: ", direction, " NORM: ", normalizedDir, " NEW: ", newPoint,
					" OLD: ", Qt.point(fileTransferItem.x, fileTransferItem.y))*/

		fileTransferItem.x = newPoint.x
		fileTransferItem.y = newPoint.y

		//Modify animation
		xAnim.to = receiverCoords.x
		xAnim.from = newPoint.x
		yAnim.to = receiverCoords.y
		yAnim.from = newPoint.y

		//Setting appropriate time left for animation
		//Otherwise my code and animation would fight
		//and results would be ugly
		var progress = 1 - (newDist / diagonal)
		var elapsed = progress * runningTime
		xAnim.duration = runningTime - elapsed
		yAnim.duration = runningTime - elapsed

		console.log("Remaining time: ", progress * runningTime)
		animation.start()
	}

	function calculateDiagonal() {
		fileTransferItem.diagonal = Math.sqrt(Math.pow(fileTransferItem.parentHeight, 2) + Math.pow(fileTransferItem.parentWidth, 2))
	}

	function animateResult(result: bool) {
		if(result) {
			resultImage.source = "qrc:/FileIcons/accepted.png"
		} else {
			resultImage.source = "qrc:/FileIcons/rejected.png"
		}

		resultImage.visible = true
		resultImage.x = fileImage.x
		resultImage.y = fileImage.y
		resultAnimation.start()
	}

	onXChanged: {
		if ((fileTransferItem.x == xAnim.to && fileTransferItem.y == yAnim.to)) {
			console.log("Arrived")
			transferEnded(fileTransferItem.objectName)
		}
	}

	onYChanged: {
		if ((fileTransferItem.x == xAnim.to && fileTransferItem.y == yAnim.to)) {
			console.log("Arrived")
			transferEnded(fileTransferItem.objectName)
		}
	}


	ParallelAnimation {
		id: animation
		PropertyAnimation {
			id: xAnim
			target: fileTransferItem
			property: "x"
			to: 0
			duration: 10000
		}

		PropertyAnimation {
			id: yAnim
			target: fileTransferItem
			property: "y"
			to: 0
			duration: 10000

		}
	}

	SequentialAnimation {
		id: resultAnimation

		PropertyAnimation {
			target: resultImage
			property: "opacity"
			from: 0
			to: 1
			duration: 500
		}

		ParallelAnimation {
			PropertyAnimation {
				target: resultImage
				property: "opacity"
				from: 1
				to: 0
				duration: 500
			}

			PropertyAnimation {
				target: fileImage
				property: "opacity"
				from: 1
				to: 0
				duration: 500
			}
		}
	}

	onParentChanged: {
		if(fileTransferItem.parent !== null) {
			parentWidth = fileTransferItem.parent.width
			parentHeight = fileTransferItem.parent.height
		}
	}

}
