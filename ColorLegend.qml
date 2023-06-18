import QtQuick
import QtQuick.Window 2.3
import QtQuick.Controls 2.5
import QtQuick.Controls.Material 2.3
import QtQuick.Dialogs
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects

ColumnLayout {
	spacing: 5
	Label {
		text: "Legend"
		color: "white"
		font.pixelSize: 16
		font.bold: true
		Layout.topMargin: 10
		Layout.leftMargin: 10
	}

	RowLayout {
		Layout.margins: 10
		Rectangle {
			width: 20
			height: 20
			radius: 6
			border.width: 1
			border.color: "darkblue"
			gradient: Gradient {
				GradientStop { position: 0.0; color: "#368bfc" }
				GradientStop { position: 1.0; color: "#c5dbfa" }
			}
		}

		Label {
			text: "Active non-malicious"
			color: "white"
		}
	}

	RowLayout {
		Layout.margins: 10
		Rectangle {
			width: 20
			height: 20
			radius: 6
			border.width: 1
			border.color: "#222254"
			gradient: Gradient {
				GradientStop { position: 0.0; color: "#3d6599" }
				GradientStop { position: 1.0; color: "#3f5f8c" }
			}
		}

		Label {
			text: "Inactive non-malicious"
			color: "white"
		}
	}

	RowLayout {
		Layout.margins: 10
		Rectangle {
			width: 20
			height: 20
			radius: 6
			border.width: 1
			border.color: "darkred"
			gradient: Gradient {
				GradientStop { position: 0.0; color: "#fc3636" }
				GradientStop { position: 1.0; color: "#fac5c5" }
			}
		}

		Label {
			text: "Active malicious"
			color: "white"
		}
	}

	RowLayout {
		Layout.margins: 10
		Rectangle {
			width: 20
			height: 20
			radius: 6
			border.width: 1
			border.color: "#542222"
			gradient: Gradient {
				GradientStop { position: 0.0; color: "#993d3d" }
				GradientStop { position: 1.0; color: "#994d4d" }
			}
		}

		Label {
			text: "Inactive malicious"
			color: "white"
		}
	}
}

