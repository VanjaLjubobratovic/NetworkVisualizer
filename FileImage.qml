import QtQuick 2.7
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3

Item {
	id: fileTransferItem
	width: 100
	height: 100
	ColumnLayout {
		Image {
			id: fileImage
			source: "qrc:/FileIcons/containsfiles.png"
			width: 60
			height: 50
		}
		Label {
			id: fileLabel
			text: "Filename"
		}
	}
	z:4
}
