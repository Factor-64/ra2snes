import QtQuick 2.15
import QtQuick.Controls 2.15

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: qsTr("RA2Snes")

    Column {
        anchors.centerIn: parent

        Text {
            text: qsTr("Welcome, you are now logged in!")
        }

        Text {
            id: currentGameText
            text: qsTr("Current Game: ") + ra2snes.currentGame
        }
    }
}
