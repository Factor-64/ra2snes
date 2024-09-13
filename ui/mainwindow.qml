import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ApplicationWindow {
    visible: true
    width: 1000
    height: 1000
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
        Rectangle {
            id: achievement_group
            Layout.preferredWidth: 320
            Layout.preferredHeight: parent.height - 100
            Layout.alignment: Qt.AlignHCenter
            color: "#232323"
            border.width: 2
            border.color: "#161616"
            radius: 6
            ListView {
                width: parent.width
                height: parent.height - 100
                model: achievementModel
                delegate: Item {
                    width: parent.width
                    height: 100

                    RowLayout {
                        spacing: 10

                        Image {
                            source: model.unlocked ? model.badgeUrl : model.badgeLockedUrl
                            width: 64
                            height: 64
                        }

                        ColumnLayout {
                            Text {
                                id: titleLink
                                text: "" + model.title
                                font.family: "Verdana"
                                font.bold: true
                                color: "#cc9900"
                                MouseArea {
                                    anchors.fill: parent
                                    cursorShape: Qt.PointingHandCursor
                                    onClicked: {
                                        Qt.openUrlExternally(model.achievement_link)
                                    }
                                    onEntered: {
                                        titleLink.color = "#ffffff"
                                    }
                                    onExited: {
                                        titleLink.color = "#cc9900"
                                    }
                                }
                            }
                            Text {
                                text: "(" + model.points + ")"
                                font.family: "Verdana"
                                color: "#2c97fa"
                            }
                            Text {
                                text: "" + model.description
                                font.family: "Verdana"
                                color: "#2c97fa"
                            }
                        }
                    }
                }
            }
        }
    }
}
