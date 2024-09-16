import QtQuick 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15
import CustomModels 1.0

ApplicationWindow {
    visible: true
    width: 1000
    height: 1000
    minimumWidth: 480
    minimumHeight: 480
    Material.theme: Material.Dark
    Material.accent: "#ffffff"
    color: "#1a1a1a"

    AchievementSortFilterProxyModel {
        id: sortedAchievementModel
        sourceModel: achievementModel
    }

    function sortModelByPoints() {
        sortedAchievementModel.sortByPoints();
    }

    function onlyShowMissable() {
        sortedAchievementModel.showOnlyMissable();
    }

    function hideUnlocked() {
        sortedAchievementModel.hideUnlocked();
    }

    function clearMissableFilter() {
        sortedAchievementModel.clearMissableFilter();
    }

    function clearUnlockedFilter() {
        sortedAchievementModel.clearUnlockedFilter();
    }

    Connections {
        target: ra2snes
        function onAchievementModelReady() {
            console.log("Recieved");
        }
    }

    Rectangle {
        id: achievementForm
        width: parent.width
        height: parent.height
        color: "#222222"
        border.width: 2
        border.color: "#161616"
        radius: 6
        anchors.margins: 10
        clip: false
        ColumnLayout {
            anchors.fill: parent
            spacing: 6

            Flow {
                id: sortingSettingsFlow
                Layout.fillWidth: true
                spacing: 6
                Layout.leftMargin: 8
                Layout.rightMargin: 8
                Layout.topMargin: 4
                Layout.bottomMargin: 4

                Row {
                    spacing: 4
                    Text{
                        text: "Sort:"
                        font.family: "Verdana"
                        font.pixelSize: 13
                        color: "#2c97fa"
                        Layout.fillWidth: true
                    }
                    Text{
                        text: "Normal"
                        font.family: "Verdana"
                        font.pixelSize: 13
                        color: "#2c97fa"
                        Layout.fillWidth: true
                    }
                    Text{
                        text: "-"
                        font.family: "Verdana"
                        font.pixelSize: 13
                        color: "#2c97fa"
                        Layout.fillWidth: true
                    }
                    Text{
                        text: "Points"
                        font.family: "Verdana"
                        font.pixelSize: 13
                        color: "#2c97fa"
                        Layout.fillWidth: true
                    }
                    Text{
                        text: "-"
                        font.family: "Verdana"
                        font.pixelSize: 13
                        color: "#2c97fa"
                        Layout.fillWidth: true
                    }
                    Text{
                        text: "Title"
                        font.family: "Verdana"
                        font.pixelSize: 13
                        color: "#2c97fa"
                        Layout.fillWidth: true
                    }
                    Text{
                        text: "-"
                        font.family: "Verdana"
                        font.pixelSize: 13
                        color: "#2c97fa"
                        Layout.fillWidth: true
                    }
                    Text{
                        text: "Type"
                        font.family: "Verdana"
                        font.pixelSize: 13
                        color: "#2c97fa"
                        Layout.fillWidth: true
                    }
                }
                RowLayout {
                    spacing: 15
                    Row {
                        spacing: 8
                        Layout.alignment: Qt.AlignRight
                        CheckBox {
                            id: missableCheckBox
                            width: 15
                            height: 15

                            indicator: Rectangle {
                                width: 15
                                height: 15
                                radius: 4
                                color: missableCheckBox.checked ? "#005cc8" : "#ffffff"
                                border.color: missableCheckBox.checked ? "#005cc8" : "#4f4f4f"

                                Text {
                                    anchors.centerIn: parent
                                    text: missableCheckBox.checked ? "\u2713" : ""
                                    color: "#ffffff"
                                    font.pixelSize: 12
                                }
                            }

                            onCheckedChanged: {
                                if (missableCheckBox.checked) {
                                    onlyShowMissable();
                                } else {
                                    clearMissableFilter();
                                }
                            }
                        }
                        Text {
                            text: "Only show missables"
                            font.family: "Verdana"
                            font.pixelSize: 13
                            color: "#2c97fa"
                            verticalAlignment: Text.AlignVCenter
                        }
                    }
                    Row {
                        Layout.alignment: Qt.AlignRight
                        spacing: 8
                        CheckBox {
                            id: hideCheckBox
                            width: 15
                            height: 15

                            indicator: Rectangle {
                                width: 15
                                height: 15
                                radius: 4
                                color: hideCheckBox.checked ? "#005cc8" : "#ffffff"
                                border.color: hideCheckBox.checked ? "#005cc8" : "#4f4f4f"

                                Text {
                                    anchors.centerIn: parent
                                    text: hideCheckBox.checked ? "\u2713" : ""
                                    color: "#ffffff"
                                    font.pixelSize: 12
                                }
                            }

                            onCheckedChanged: {
                                if (hideCheckBox.checked) {
                                    hideUnlocked();
                                } else {
                                    clearUnlockedFilter();
                                }
                            }
                        }
                        Text {
                            text: "Hide unlocked achievements"
                            font.family: "Verdana"
                            font.pixelSize: 13
                            color: "#2c97fa"
                            verticalAlignment: Text.AlignVCenter
                        }
                    }
                }
            }
            ListView {
                id: achievementlist
                Layout.fillWidth: true
                Layout.fillHeight: true
                anchors.margins: 10
                model: sortedAchievementModel
                clip: true
                delegate: Rectangle {
                    height: Math.max(72, descriptionText.implicitHeight + 24)
                    id: achievement
                    Component.onCompleted: {
                        if (parent !== null)
                        {
                            anchors.left = parent.left
                            anchors.right = parent.right
                            anchors.leftMargin = 20
                            anchors.rightMargin = 20
                        }
                    }
                    color: index % 2 == 0 ? "#282828" : "#222222"
                    opacity: 1
                    z: 1

                    Row {
                        spacing: 10
                        anchors.leftMargin: 8
                        anchors.rightMargin: 8
                        anchors.topMargin: 4
                        anchors.bottomMargin: 4
                        anchors.fill: parent

                        Image {
                            id: badge
                            source: model.unlocked ? model.badgeUrl : model.badgeLockedUrl
                            width: 64
                            height: 64
                            Layout.alignment: Qt.AlignVCenter
                        }

                        ColumnLayout {
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignVCenter
                            spacing: 0
                            Row {
                                spacing: 8
                                Text {
                                    id: titleText
                                    text: model.title
                                    color: "#cc9900"
                                    font.family: "Verdana"
                                    font.pixelSize: 13
                                    Layout.fillWidth: true
                                    wrapMode: Text.WordWrap
                                    MouseArea {
                                        id: mouseArea
                                        anchors.fill: parent
                                        hoverEnabled: true
                                        onClicked: {
                                            Qt.openUrlExternally(model.achievementLink)
                                        }
                                        onEntered: titleText.state = "hovered"
                                        onExited: titleText.state = ""
                                    }

                                    states: [
                                        State {
                                            name: "hovered"
                                            PropertyChanges {
                                                target: titleText
                                                color: "#c8c8c8"
                                            }
                                        }
                                    ]

                                    transitions: [
                                        Transition {
                                            from: ""
                                            to: "hovered"
                                            ColorAnimation {
                                                target: titleText
                                                property: "color"
                                                duration: 200
                                            }
                                        },
                                        Transition {
                                            from: "hovered"
                                            to: ""
                                            ColorAnimation {
                                                target: titleText
                                                property: "color"
                                                duration: 200
                                            }
                                        }
                                    ]
                                }
                                Text {
                                    text: "(" + model.points + ")"
                                    font.family: "Verdana"
                                    font.pixelSize: 13
                                    color: "#2c97fa"
                                    Layout.fillWidth: true
                                }
                            }
                            Text {
                                id: descriptionText
                                text: model.description
                                font.family: "Verdana"
                                font.pixelSize: 13
                                color: "#2c97fa"
                                Layout.fillWidth: true
                                wrapMode: Text.WordWrap
                                Layout.preferredWidth: achievement.width - 100
                            }
                        }
                    }
                }
                ScrollBar.vertical: ScrollBar {
                    policy: ScrollBar.AsNeeded
                }
            }
        }
    }
    onClosing: {
        sortedAchievementModel.destroy();
    }
}
