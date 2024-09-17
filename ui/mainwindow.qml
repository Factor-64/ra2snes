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

    Loader {
        id: loaderr
        anchors.fill: parent
        sourceComponent: loadee
        asynchronous: true
        active: false
        Connections {
            target: ra2snes
            function onAchievementModelReady() {
                loaderr.active = true;
            }
        }
    }

    Component {
        id: loadee
        Rectangle {
            id: achievementForm
            width: parent.width
            height: achievementColumn.implicitHeight
            color: "#222222"
            border.width: 2
            border.color: "#161616"
            radius: 6
            anchors.margins: 10
            clip: false
            ColumnLayout {
                id: achievementColumn
                anchors.fill: parent
                spacing: 6

                Flow {
                    id: sortingSettingsFlow
                    spacing: 10
                    Layout.fillWidth: true
                    Layout.leftMargin: 20
                    Layout.bottomMargin: 10
                    Layout.rightMargin: 20
                    Layout.topMargin: 10
                    Row {
                        spacing: 6
                        Layout.fillWidth: true
                        Text{
                            text: "Sort:"
                            font.family: "Verdana"
                            font.pixelSize: 13
                            color: "#2c97fa"
                            Layout.fillWidth: true
                        }
                        Text{
                            id: normal
                            text: "Normal"
                            font.family: "Verdana"
                            font.pixelSize: 13
                            color: "#cc9900"
                            Layout.fillWidth: true
                            MouseArea {
                                id: mouseAreaNormal
                                anchors.fill: parent
                                hoverEnabled: true
                                onClicked: {
                                    sortedAchievementModel.sortByNormal()
                                }
                                onEntered: normal.state = "hovered"
                                onExited: normal.state = ""
                            }

                            states: [
                                State {
                                    name: "hovered"
                                    PropertyChanges {
                                        target: normal
                                        color: "#c8c8c8"
                                    }
                                }
                            ]

                            transitions: [
                                Transition {
                                    from: ""
                                    to: "hovered"
                                    ColorAnimation {
                                        target: normal
                                        property: "color"
                                        duration: 200
                                    }
                                },
                                Transition {
                                    from: "hovered"
                                    to: ""
                                    ColorAnimation {
                                        target: normal
                                        property: "color"
                                        duration: 200
                                    }
                                }
                            ]
                        }
                        Text{
                            text: "-"
                            font.family: "Verdana"
                            font.pixelSize: 13
                            color: "#2c97fa"
                            Layout.fillWidth: true
                        }
                        Text{
                            id: points
                            text: "Points"
                            font.family: "Verdana"
                            font.pixelSize: 13
                            color: "#cc9900"
                            Layout.fillWidth: true
                            MouseArea {
                                id: mouseAreaPoints
                                anchors.fill: parent
                                hoverEnabled: true
                                onClicked: {
                                    sortedAchievementModel.sortByPoints()
                                }
                                onEntered: points.state = "hovered"
                                onExited: points.state = ""
                            }

                            states: [
                                State {
                                    name: "hovered"
                                    PropertyChanges {
                                        target: points
                                        color: "#c8c8c8"
                                    }
                                }
                            ]

                            transitions: [
                                Transition {
                                    from: ""
                                    to: "hovered"
                                    ColorAnimation {
                                        target: points
                                        property: "color"
                                        duration: 200
                                    }
                                },
                                Transition {
                                    from: "hovered"
                                    to: ""
                                    ColorAnimation {
                                        target: points
                                        property: "color"
                                        duration: 200
                                    }
                                }
                            ]
                        }
                        Text{
                            text: "-"
                            font.family: "Verdana"
                            font.pixelSize: 13
                            color: "#2c97fa"
                            Layout.fillWidth: true
                        }
                        Text{
                            id: title
                            text: "Title"
                            font.family: "Verdana"
                            font.pixelSize: 13
                            color: "#cc9900"
                            Layout.fillWidth: true
                            MouseArea {
                                id: mouseAreaTitleSort
                                anchors.fill: parent
                                hoverEnabled: true
                                onClicked: {
                                    sortedAchievementModel.sortByTitle()
                                }
                                onEntered: title.state = "hovered"
                                onExited: title.state = ""
                            }

                            states: [
                                State {
                                    name: "hovered"
                                    PropertyChanges {
                                        target: title
                                        color: "#c8c8c8"
                                    }
                                }
                            ]

                            transitions: [
                                Transition {
                                    from: ""
                                    to: "hovered"
                                    ColorAnimation {
                                        target: title
                                        property: "color"
                                        duration: 200
                                    }
                                },
                                Transition {
                                    from: "hovered"
                                    to: ""
                                    ColorAnimation {
                                        target: title
                                        property: "color"
                                        duration: 200
                                    }
                                }
                            ]
                        }
                        Text{
                            text: "-"
                            font.family: "Verdana"
                            font.pixelSize: 13
                            color: "#2c97fa"
                            Layout.fillWidth: true
                        }
                        Text{
                            id: type
                            text: "Type"
                            font.family: "Verdana"
                            font.pixelSize: 13
                            color: "#cc9900"
                            Layout.fillWidth: true
                            MouseArea {
                                id: mouseAreaType
                                anchors.fill: parent
                                hoverEnabled: true
                                onClicked: {
                                    sortedAchievementModel.sortByType()
                                }
                                onEntered: type.state = "hovered"
                                onExited: type.state = ""
                            }

                            states: [
                                State {
                                    name: "hovered"
                                    PropertyChanges {
                                        target: type
                                        color: "#c8c8c8"
                                    }
                                }
                            ]

                            transitions: [
                                Transition {
                                    from: ""
                                    to: "hovered"
                                    ColorAnimation {
                                        target: type
                                        property: "color"
                                        duration: 200
                                    }
                                },
                                Transition {
                                    from: "hovered"
                                    to: ""
                                    ColorAnimation {
                                        target: type
                                        property: "color"
                                        duration: 200
                                    }
                                }
                            ]
                        }
                        Item {
                            width: 10
                            height: 10
                        }
                    }
                    Row {
                        Layout.alignment: Qt.AlignRight
                        spacing: 8
                        Layout.fillWidth: true
                        Row {
                            spacing: 8
                            Layout.fillWidth: true
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
                                        sortedAchievementModel.showOnlyMissable();
                                    } else {
                                        sortedAchievementModel.clearMissableFilter();
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
                            spacing: 8
                            Layout.alignment: Qt.AlignRight
                            Layout.fillWidth: true

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
                                        sortedAchievementModel.hideUnlocked();
                                    } else {
                                        sortedAchievementModel.clearUnlockedFilter();
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
                        height: Math.max(72, descriptionText.implicitHeight + (model.timeUnlocked !== "" ? unlockedTime.implicitHeight + 8 : 0) + 24)
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
                                Item {
                                    height: 8
                                }
                                Text {
                                    id: unlockedTime
                                    color: "#7e7e7e"
                                    text: {
                                        if(model.timeUnlocked !== "")
                                            "Unlocked " + model.timeUnlocked
                                        else ""
                                    }
                                    font.family: "Verdana"
                                    font.pixelSize: 10
                                    Layout.alignment: Qt.AlignBottom
                                    Layout.fillWidth: true
                                }
                            }
                        }
                    }
                    ScrollBar.vertical: ScrollBar {
                        policy: ScrollBar.AsNeeded
                    }
                }
                Item {
                    height: 10
                }
            }
        }
    }
    onClosing: {
        sortedAchievementModel.destroy();
    }
}
//Pumpkin
