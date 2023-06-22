import QtQuick 2.7
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3

Item {
	id: fileTransferItem
	width: 30
	height: 30
	x: 0
	y: 0

	Image {
		id: fileImage
		source: "qrc:/FileIcons/containsfiles.png"
		anchors.fill: parent

		Component.onCompleted: {
			//animation.start();
		}

	}

	function startFileTransfer(direction) {
		xAnim.to = direction.x
		yAnim.to = direction.y
		animation.start();
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
}
