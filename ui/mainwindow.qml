import QtQuick 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15
import CustomModels 1.0

// I apologize to anyone looking at this

ApplicationWindow {
    visible: true
    width: {
        if(userInfoModel)
        {
            if(userInfoModel.width >= 480)
                userInfoModel.width
            else 480
        }
        else
            480
    }

    height: {
        if(userInfoModel)
        {
            if(userInfoModel.height >= 480)
                userInfoModel.height
            else 480
        }
        else
            480
    }
    minimumWidth: 480
    minimumHeight: 480
    title: "ra2snes"
    Material.theme: Material.Dark
    Material.accent: "#ffffff"
    color: "#1a1a1a"

    property int windowWidth: width
    property int windowHeight: height

    onWidthChanged: windowWidth = width
    onHeightChanged: windowHeight = height

    AchievementSortFilterProxyModel {
        id: sortedAchievementModel
        sourceModel: achievementModel
    }
    Flickable {
        id: flickable
        anchors.fill: parent
        contentHeight: infoColumn.height
        flickableDirection: Flickable.VerticalFlick

        /*AnimatedImage {
            id: loadingIcon
            source: "./loading.gif"
            Layout.alignment: Qt.AlignCenter
            width: 96
            height: 96
            playing: true
            paused: false
        }*/

        ColumnLayout {
            id: infoColumn
            width: Math.min(parent.width,840)
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 10

            Loader {
                id: mainLoader
                Layout.fillHeight: true
                Layout.fillWidth: true
                anchors.margins: 10
                sourceComponent: mainComponent
                active: true
            }

            Component {
                id: mainComponent
                Rectangle {
                    id: contentForm
                    implicitHeight: contentColumn.implicitHeight
                    color: "#222222"
                    border.width: 2
                    border.color: "#161616"
                    radius: 6
                    anchors.margins: 10
                    clip: false

                    ColumnLayout {
                        id: contentColumn
                        anchors.fill: parent
                        spacing: 6
                        Rectangle {
                            color: "#161616"
                            Layout.fillWidth: true
                            height: 168
                            Button {
                                id: logout_button
                                anchors.top: parent.top
                                anchors.right: parent.right
                                anchors.topMargin: 10
                                anchors.rightMargin: 20
                                text: qsTr("Sign out")
                                font.family: "Verdana"
                                font.pixelSize: 13
                                background: Rectangle {
                                    id: buttonBackground
                                    color: "#222222"
                                    border.width: 1
                                    border.color: "#2a2a2a"
                                    radius: 2
                                }
                                contentItem: Text {
                                    id: buttonText
                                    text: "Log Out"
                                    color: "#ff0000"
                                    font.family: "Verdana"
                                    font.pixelSize: 13
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                }
                                MouseArea {
                                    id: mouseAreaLogout
                                    anchors.fill: parent
                                    hoverEnabled: true
                                    onClicked: {
                                        ra2snes.logOut();
                                    }
                                    onEntered: logout_button.state = "hovered"
                                    onExited: logout_button.state = ""
                                }

                                states: [
                                    State {
                                        name: "hovered"
                                        PropertyChanges {
                                            target: buttonBackground
                                            color: "#333333"
                                            border.color: "#c8c8c8"
                                        }
                                        PropertyChanges {
                                            target: buttonText
                                            color: "#eeeeee"
                                        }
                                    }
                                ]

                                transitions: [
                                    Transition {
                                        from: ""
                                        to: "hovered"
                                        ColorAnimation {
                                            target: buttonBackground
                                            property: "color"
                                            duration: 200
                                        }
                                        ColorAnimation {
                                            target: buttonBackground
                                            property: "border.color"
                                            duration: 200
                                        }
                                        ColorAnimation {
                                            target: buttonText
                                            property: "color"
                                            duration: 200
                                        }
                                    },
                                    Transition {
                                        from: "hovered"
                                        to: ""
                                        ColorAnimation {
                                            target: buttonBackground
                                            property: "color"
                                            duration: 200
                                        }
                                        ColorAnimation {
                                            target: buttonBackground
                                            property: "border.color"
                                            duration: 200
                                        }
                                        ColorAnimation {
                                            target: buttonText
                                            property: "color"
                                            duration: 200
                                        }
                                    }
                                ]
                            }
                            Row {
                                id: userRow
                                spacing: 16
                                anchors.leftMargin: 20
                                anchors.rightMargin: 20
                                anchors.topMargin: 20
                                anchors.bottomMargin: 20
                                anchors.fill: parent
                                Image {
                                    source: {
                                        if(userInfoModel)
                                            userInfoModel.pfp
                                        else ""
                                    }
                                    width: 128
                                    height: 128
                                }
                                Column {
                                    Text {
                                        id: user
                                        text: {
                                            if(userInfoModel)
                                                userInfoModel.username
                                            else ""
                                        }
                                        color: "#cc9900"
                                        font.bold: true
                                        font.family: "Verdana"
                                        font.pixelSize: 24
                                        MouseArea {
                                            id: mouseAreaUser
                                            anchors.fill: parent
                                            hoverEnabled: true
                                            onClicked: {
                                                Qt.openUrlExternally(userInfoModel.link)
                                            }
                                            onEntered: user.state = "hovered"
                                            onExited: user.state = ""
                                        }

                                        states: [
                                            State {
                                                name: "hovered"
                                                PropertyChanges {
                                                    target: user
                                                    color: "#c8c8c8"
                                                }
                                            }
                                        ]

                                        transitions: [
                                            Transition {
                                                from: ""
                                                to: "hovered"
                                                ColorAnimation {
                                                    target: user
                                                    property: "color"
                                                    duration: 200
                                                }
                                            },
                                            Transition {
                                                from: "hovered"
                                                to: ""
                                                ColorAnimation {
                                                    target: user
                                                    property: "color"
                                                    duration: 200
                                                }
                                            }
                                        ]
                                    }
                                    Row {
                                        Text {
                                            text: qsTr("Points: ")
                                            color: "#2c97fa"
                                            font.bold: true
                                            font.family: "Verdana"
                                            font.pixelSize: 13
                                        }
                                        Text {
                                            text: {
                                                if(userInfoModel)
                                                {
                                                    if(userInfoModel.hardcore)
                                                        "" + userInfoModel.hardcore_score
                                                    else
                                                        "" + userInfoModel.softcore_score
                                                }
                                                else ""
                                            }
                                            font.family: "Verdana"
                                            font.pixelSize: 13
                                            color: "#2c97fa"
                                        }
                                    }
                                    Row {
                                        Text {
                                            text: qsTr("Mode: ")
                                            color: "#2c97fa"
                                            font.bold: true
                                            font.family: "Verdana"
                                            font.pixelSize: 13
                                        }
                                        Text {
                                            text: {
                                                if(userInfoModel)
                                                {
                                                    if(userInfoModel.hardcore)
                                                        qsTr("Hardcore")
                                                    else
                                                        qsTr("Softcore")
                                                }
                                                else ""
                                            }
                                            font.family: "Verdana"
                                            font.pixelSize: 13
                                            color: {
                                                if(userInfoModel)
                                                {
                                                    if(userInfoModel.hardcore)
                                                        "#ff0000"
                                                    else
                                                        "#00ff00"
                                                }
                                                else "#000000"
                                            }
                                        }
                                    }
                                }
                            }
                        }

                        Text {
                            Layout.topMargin: 8
                            Layout.leftMargin: 20
                            Layout.rightMargin: 20
                            Layout.bottomMargin: 0
                            Layout.fillWidth: true
                            color: "#2c97fa"
                            font.bold: true
                            font.family: "Verdana"
                            font.pixelSize: 13
                            text: qsTr("Currently Playing")
                        }

                        Rectangle {
                            color: "#161616"
                            Layout.leftMargin: 20
                            Layout.bottomMargin: 10
                            Layout.rightMargin: 20
                            Layout.fillWidth: true
                            height: 52
                            border.width: 2
                            border.color: "#161616"
                            radius: 6
                            Row {
                                spacing: 10
                                anchors.leftMargin: 8
                                anchors.rightMargin: 8
                                anchors.topMargin: 8
                                anchors.bottomMargin: 8
                                anchors.fill: parent
                                Image {
                                    source: {
                                        if(gameInfoModel)
                                            gameInfoModel.image_icon_url
                                        else ""
                                    }
                                    width: 36
                                    height: 36
                                }
                                Column {
                                    Layout.fillWidth: true
                                    spacing: 4
                                    Text {
                                        id: game
                                        text: {
                                            if(userInfoModel)
                                                gameInfoModel.title
                                            else ""
                                        }
                                        color: "#cc9900"
                                        font.family: "Verdana"
                                        font.pixelSize: 13
                                        MouseArea {
                                            id: mouseAreaGame
                                            anchors.fill: parent
                                            hoverEnabled: true
                                            onClicked: {
                                                Qt.openUrlExternally(gameInfoModel.game_link)
                                            }
                                            onEntered: game.state = "hovered"
                                            onExited: game.state = ""
                                        }

                                        states: [
                                            State {
                                                name: "hovered"
                                                PropertyChanges {
                                                    target: game
                                                    color: "#c8c8c8"
                                                }
                                            }
                                        ]

                                        transitions: [
                                            Transition {
                                                from: ""
                                                to: "hovered"
                                                ColorAnimation {
                                                    target: game
                                                    property: "color"
                                                    duration: 200
                                                }
                                            },
                                            Transition {
                                                from: "hovered"
                                                to: ""
                                                ColorAnimation {
                                                    target: game
                                                    property: "color"
                                                    duration: 200
                                                }
                                            }
                                        ]
                                    }
                                    Row {
                                        Layout.fillWidth: true
                                        spacing: 4
                                        Image {
                                            source: {
                                                if(gameInfoModel)
                                                    gameInfoModel.console_icon
                                                else ""
                                            }
                                            width: 18
                                            height: 18
                                            MouseArea {
                                                id: mouseAreaGameIcon
                                                anchors.fill: parent
                                                hoverEnabled: true
                                                onClicked: {
                                                    Qt.openUrlExternally(gameInfoModel.game_link)
                                                }
                                                onEntered: game.state = "hovered"
                                                onExited: game.state = ""
                                            }

                                            states: [
                                                State {
                                                    name: "hovered"
                                                    PropertyChanges {
                                                        target: game
                                                        color: "#c8c8c8"
                                                    }
                                                }
                                            ]

                                            transitions: [
                                                Transition {
                                                    from: ""
                                                    to: "hovered"
                                                    ColorAnimation {
                                                        target: game
                                                        property: "color"
                                                        duration: 200
                                                    }
                                                },
                                                Transition {
                                                    from: "hovered"
                                                    to: ""
                                                    ColorAnimation {
                                                        target: game
                                                        property: "color"
                                                        duration: 200
                                                    }
                                                }
                                            ]
                                        }
                                        Text {
                                            text: {
                                                if(gameInfoModel)
                                                    gameInfoModel.console
                                                else ""
                                            }
                                            font.family: "Verdana"
                                            font.pixelSize: 13
                                            color: "#2c97fa"
                                        }
                                    }
                                }
                            }
                        }

                        Loader {
                            id: listViewLoader
                            Layout.fillWidth: true
                            Layout.leftMargin: 20
                            Layout.bottomMargin: 10
                            Layout.rightMargin: 20
                            Layout.topMargin: 10
                            sourceComponent: listViewComponent
                            active: false
                        }

                        Connections {
                            target: ra2snes
                            function onAchievementModelReady() {
                                listViewLoader.active = true;
                            }
                        }

                        Component {
                            id: listViewComponent

                            Flow {
                                id: sortingSettingsFlow
                                spacing: 6
                                Layout.fillWidth: true
                                Layout.leftMargin: 20
                                Layout.bottomMargin: 10
                                Layout.rightMargin: 20
                                Layout.topMargin: 10

                                RowLayout {
                                    id: sortingTextRow
                                    spacing: 6
                                    Layout.fillWidth: true
                                    Text{
                                        text: qsTr("Sort:")
                                        font.family: "Verdana"
                                        font.pixelSize: 13
                                        color: "#2c97fa"
                                        Layout.fillWidth: true
                                    }
                                    Text{
                                        id: normal
                                        text: qsTr("Normal")
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
                                        text: qsTr("Points")
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
                                        text: qsTr("Title")
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
                                        text: qsTr("Type")
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
                                }

                                Item {
                                    id: dynamicSpacer
                                    height: 1
                                }

                                Binding {
                                    target: dynamicSpacer
                                    property: "width"
                                    value: Math.max(0,sortingSettingsFlow.width - sortingTextRow.width - sortingCheckBoxes.width - (sortingSettingsFlow.spacing * 3))
                                }

                                RowLayout {
                                    id: sortingCheckBoxes
                                    Layout.alignment: Qt.AlignRight
                                    Layout.fillWidth: true
                                    spacing: 14

                                    Row {
                                        spacing: 4
                                        Layout.fillWidth: true
                                        CheckBox {
                                            id: missableCheckBox
                                            width: 14
                                            height: 14

                                            indicator: Rectangle {
                                                width: 14
                                                height: 14
                                                radius: 2
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
                                            text: qsTr("Only show missables")
                                            font.family: "Verdana"
                                            font.pixelSize: 13
                                            color: "#2c97fa"
                                            verticalAlignment: Text.AlignVCenter
                                            MouseArea {
                                                anchors.fill: parent
                                                onClicked: {
                                                    missableCheckBox.checked = !missableCheckBox.checked
                                                }
                                            }
                                        }
                                    }
                                    Row {
                                        spacing: 4
                                        Layout.fillWidth: true

                                        CheckBox {
                                            id: hideCheckBox
                                            width: 14
                                            height: 14

                                            indicator: Rectangle {
                                                width: 14
                                                height: 14
                                                radius: 2
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
                                            text: qsTr("Hide unlocked achievements")
                                            font.family: "Verdana"
                                            font.pixelSize: 13
                                            color: "#2c97fa"
                                            verticalAlignment: Text.AlignVCenter
                                            MouseArea {
                                                anchors.fill: parent
                                                onClicked: {
                                                    hideCheckBox.checked = !hideCheckBox.checked
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        ListView {
                            id: achievementlist
                            implicitHeight: contentHeight
                            Layout.fillWidth: true
                            layoutDirection: Qt.Vertical
                            anchors.margins: 10
                            interactive: false
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
                        }
                        Item {
                            height: 10
                        }
                    }
                }
            }
        }
        ScrollBar.vertical: ScrollBar {
               policy: ScrollBar.AsNeeded
           }
    }
    onClosing: {
        ra2snes.saveWindowSize(windowWidth, windowHeight);
        sortedAchievementModel.destroy();
        delete userInfoModel;
        delete gameInfoModel;
    }
}
//Pumpkin
