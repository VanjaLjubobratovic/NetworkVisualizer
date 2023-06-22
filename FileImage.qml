import QtQuick 2.7
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3

Item {
	id: fileTransferItem
	width: 30
	height: 30
	x: 10
	y: 0

	Image {
		id: fileImage
		source: "qrc:/FileIcons/containsfiles.png"
		anchors.fill: parent
	}
	/*Label {
		id: fileLabel
		text: "Filename"
		anchors.top: fileImage.bottom
	}*/
}
