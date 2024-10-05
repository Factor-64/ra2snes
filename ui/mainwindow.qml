import QtQuick 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15
import CustomModels 1.0

// I apologize to anyone looking at this

ApplicationWindow {
    id: mainWindow
    visible: true
    width: {
        if(userInfoModel)
        {
            if(userInfoModel.width >= 600)
                userInfoModel.width
            else 600
        }
        else
            600
    }

    height: {
        if(userInfoModel)
        {
            if(userInfoModel.height >= 600)
                userInfoModel.height
            else 600
        }
        else
            600
    }
    minimumWidth: 600
    minimumHeight: 600
    title: "ra2snes"

    Material.theme: Material.Dark
    Material.accent: "#eab308"
    color: "#1a1a1a"

    property int windowWidth: width
    property int windowHeight: height
    property string modeFailed: ""
    property bool disableChange: false

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
                                text: qsTr("Sign Out")
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
                                    text: qsTr("Sign Out")
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
                                        ra2snes.saveWindowSize(windowWidth, windowHeight);
                                        ra2snes.signOut();
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
                            Column {
                                anchors.bottom: parent.bottom
                                anchors.right: parent.right
                                anchors.bottomMargin: 10
                                anchors.rightMargin: 20
                                Row {
                                    spacing: 4
                                    Layout.fillWidth: true
                                    CheckBox {
                                        id: changeCheckBox
                                        width: 14
                                        height: 14

                                        indicator: Rectangle {
                                            width: 14
                                            height: 14
                                            radius: 2
                                            color: changeCheckBox.checked ? "#005cc8" : "#ffffff"
                                            border.color: changeCheckBox.checked ? "#005cc8" : "#4f4f4f"

                                            Text {
                                                anchors.centerIn: parent
                                                text: changeCheckBox.checked ? "\u2713" : ""
                                                color: "#ffffff"
                                                font.pixelSize: 12
                                            }
                                        }

                                        onCheckedChanged: {
                                            if(changeCheckBox.checked)
                                            {
                                                disableChange = true;
                                                mouseAreaMode.disableModeChangeButton();
                                            }
                                            else
                                            {
                                                disableChange = false;
                                                mouseAreaMode.enableModeChangeButton();
                                            }
                                            ra2snes.autoChange(changeCheckBox.checked);
                                        }
                                        Component.onCompleted: {
                                            changeCheckBox.checked = userInfoModel.autohardcore;
                                        }
                                    }
                                    Text {
                                        id: autoHardcore
                                        text: qsTr("Auto Hardcore")
                                        font.family: "Verdana"
                                        font.pixelSize: 13
                                        color: "#2c97fa"
                                        verticalAlignment: Text.AlignVCenter
                                        MouseArea {
                                            anchors.fill: parent
                                            onClicked: {
                                                if(changeCheckBox.enabled)
                                                    changeCheckBox.checked = !changeCheckBox.checked
                                            }
                                        }
                                    }
                                }
                                Button {
                                    id: mode_button
                                    font.family: "Verdana"
                                    text: qsTr("Change Mode")
                                    font.pixelSize: 13
                                    background: Rectangle {
                                        id: button_Background
                                        border.width: 1
                                        border.color: "#2a2a2a"
                                        radius: 2
                                        color: {
                                            if(mouseAreaMode.enabled)
                                            {
                                                "#222222"
                                            }
                                            else "#888888"
                                        }
                                    }
                                    contentItem: Text {
                                        id: button_Text
                                        color: {
                                            if(mouseAreaMode.enabled)
                                                if(userInfoModel)
                                                {
                                                   if(userInfoModel.hardcore)
                                                       "#00ff00"
                                                   else
                                                       "#ff0000"
                                                }
                                                else "#000000"
                                            else "#bbbbbb"

                                        }
                                        text: {
                                            if(userInfoModel)
                                            {
                                                if(userInfoModel.hardcore)
                                                    qsTr("Softcore Mode")
                                                else
                                                    qsTr("Hardcore Mode")
                                            }
                                            else ""
                                        }
                                        font.family: "Verdana"
                                        font.pixelSize: 13
                                        horizontalAlignment: Text.AlignHCenter
                                        verticalAlignment: Text.AlignVCenter
                                    }
                                    MouseArea {
                                        id: mouseAreaMode
                                        anchors.fill: parent
                                        hoverEnabled: true
                                        onClicked: {
                                            disableModeChangeButton();
                                            ra2snes.changeMode();
                                        }
                                        Component.onCompleted: {
                                            enableModeChangeButton();
                                        }
                                        onEntered: mode_button.state = "hovered"
                                        onExited: mode_button.state = ""

                                        function disableModeChangeButton()
                                        {
                                            mouseAreaMode.enabled = false;
                                        }

                                        function enableModeChangeButton()
                                        {
                                            if(!disableChange)
                                                mouseAreaMode.enabled = true;
                                        }
                                    }

                                    states: [
                                        State {
                                            name: "hovered"
                                            PropertyChanges {
                                                target: button_Background
                                                color: "#333333"
                                                border.color: "#c8c8c8"
                                            }
                                            PropertyChanges {
                                                target: button_Text
                                                color: "#eeeeee"
                                            }
                                        }
                                    ]

                                    transitions: [
                                        Transition {
                                            from: ""
                                            to: "hovered"
                                            ColorAnimation {
                                                target: button_Background
                                                property: "color"
                                                duration: 200
                                            }
                                            ColorAnimation {
                                                target: button_Background
                                                property: "border.color"
                                                duration: 200
                                            }
                                            ColorAnimation {
                                                target: button_Text
                                                property: "color"
                                                duration: 200
                                            }
                                        },
                                        Transition {
                                            from: "hovered"
                                            to: ""
                                            ColorAnimation {
                                                target: button_Background
                                                property: "color"
                                                duration: 200
                                            }
                                            ColorAnimation {
                                                target: button_Background
                                                property: "border.color"
                                                duration: 200
                                            }
                                            ColorAnimation {
                                                target: button_Text
                                                property: "color"
                                                duration: 200
                                            }
                                        }
                                    ]
                                }
                            }

                            Item {
                                id: errorMessagePlaceholder
                                Layout.preferredHeight: 13
                                anchors.bottom: parent.bottom
                                anchors.left: parent.left
                                anchors.leftMargin: 168
                                anchors.bottomMargin: 40

                                Text {
                                    id: errorMessage
                                    text: mainWindow.modeFailed
                                    font.family: "Verdana"
                                    font.pixelSize: 13
                                    color: "#ff0000"
                                    width: parent.width
                                    opacity: 1
                                    Behavior on opacity {
                                        NumberAnimation {
                                            duration: 500
                                        }
                                    }

                                    Timer {
                                        id: fadeOutTimer
                                        interval: 5000
                                        running: false
                                        repeat: false
                                        onTriggered: {
                                            errorMessage.opacity = 0.0
                                        }
                                    }

                                    function showErrorMessage(error, iserror) {
                                        mainWindow.modeFailed = error;
                                        if(iserror)
                                        {
                                            errorMessage.color = "#ff0000";
                                            mouseAreaMode.enableModeChangeButton();
                                        }
                                        else
                                            errorMessage.color = "#00ff00";
                                        errorMessage.opacity = 1;
                                        fadeOutTimer.restart();
                                    }

                                    Connections {
                                        target: ra2snes
                                        function onChangeModeFailed(error) {
                                            errorMessage.showErrorMessage(error, true);
                                        }
                                    }

                                    Connections {
                                        target: ra2snes
                                        function onDisplayMessage(error, iserror) {
                                            errorMessage.showErrorMessage(error, iserror);
                                        }
                                    }
                                }
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
                                    id: userpfp
                                    source: {
                                        if(userInfoModel)
                                            userInfoModel.pfp
                                        else ""
                                    }
                                    width: 128
                                    height: 128
                                }
                                Column {
                                    spacing: 6
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
                            Rectangle {
                                id: completionIcon
                                anchors.right: parent.right
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.rightMargin: 16
                                color: {
                                    if(gameInfoModel)
                                    {
                                        if(gameInfoModel.mastered)
                                            "#eab308"
                                        else if(gameInfoModel.beaten)
                                            "#d4d4d4"
                                        else "#161616"
                                    }
                                    else "#161616"
                                }
                                radius: 50
                                width: 36
                                height: 36
                                border.width: 2
                                border.color: {
                                    if(gameInfoModel)
                                    {
                                        if(gameInfoModel.mastered)
                                            "#ffd700"
                                        else
                                            "#52525b"
                                    }
                                    else "#000000"
                                }
                                visible: false
                            }

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
                            id: achievementHeaderLoader
                            Layout.fillWidth: true
                            Layout.leftMargin: 20
                            Layout.bottomMargin: 10
                            Layout.rightMargin: 20
                            Layout.topMargin: 10
                            sourceComponent: achievementHeader
                            active: false
                        }
                        Rectangle {
                            id: completionHeader
                            color: "#161616"
                            Layout.fillWidth: true
                            height: 108
                            border.width: 2
                            border.color: "#161616"
                            radius: 6
                            visible: false
                            clip: true

                            Column {
                                spacing: 8
                                anchors.leftMargin: 20
                                anchors.rightMargin: 20
                                anchors.topMargin: 14
                                anchors.bottomMargin: 20
                                anchors.fill: parent
                                Text {
                                    text: {
                                        if(gameInfoModel)
                                        {
                                            if(gameInfoModel.mastered)
                                                qsTr("Mastered")
                                            else if(gameInfoModel.beaten)
                                                qsTr("Beaten")
                                            else qsTr("Unfinished")
                                        }
                                        else ""
                                    }
                                    font.family: "Verdana"
                                    font.pixelSize: 18
                                    color: {
                                        if(gameInfoModel)
                                        {
                                           if(gameInfoModel.mastered)
                                               "#ffd700"
                                           else if(gameInfoModel.beaten)
                                               "#d4d4d4"
                                           else "#4b4b4b"
                                        }
                                        else "black"
                                    }
                                }
                                Column {
                                    spacing: 6
                                    Row {
                                        Text {
                                            text: {
                                                if(gameInfoModel)
                                                    gameInfoModel.completion_count
                                                else ""
                                            }
                                            font.bold: true
                                            font.family: "Verdana"
                                            font.pixelSize: 13
                                            color: "#2c97fa"
                                        }
                                        Text {
                                            text: qsTr(" of ")
                                            font.family: "Verdana"
                                            font.pixelSize: 13
                                            color: "#2c97fa"
                                        }
                                        Text {
                                            text: {
                                                if(gameInfoModel)
                                                    gameInfoModel.achievement_count;
                                                else ""
                                            }
                                            font.family: "Verdana"
                                            font.pixelSize: 13
                                            color: "#2c97fa"
                                        }
                                        Text {
                                            text: qsTr(" achievements")
                                            font.family: "Verdana"
                                            font.pixelSize: 13
                                            color: "#2c97fa"
                                        }
                                    }
                                    Row {
                                        Text {
                                            text: {
                                                if(gameInfoModel)
                                                {
                                                    gameInfoModel.point_count
                                                }
                                                else ""
                                            }
                                            font.bold: true
                                            font.family: "Verdana"
                                            font.pixelSize: 13
                                            color: "#2c97fa"
                                        }
                                        Text {
                                            text: qsTr(" of ")
                                            font.family: "Verdana"
                                            font.pixelSize: 13
                                            color: "#2c97fa"
                                        }
                                        Text {
                                            text: {
                                                if(gameInfoModel)
                                                {
                                                    gameInfoModel.point_total
                                                }
                                                else ""
                                            }
                                            font.family: "Verdana"
                                            font.pixelSize: 13
                                            color: "#2c97fa"
                                        }
                                        Text {
                                            text: qsTr(" points")
                                            font.family: "Verdana"
                                            font.pixelSize: 13
                                            color: "#2c97fa"
                                        }
                                    }
                                }
                            }
                            ProgressBar {
                                id: progressBar
                                anchors.left: parent.left
                                anchors.bottom: parent.bottom
                                width: parent.width
                                height: 8
                                value: {
                                    if (gameInfoModel) {
                                        let val = (gameInfoModel.completion_count / gameInfoModel.achievement_count);
                                        if(val >= 0)
                                            return Math.min(Math.max(val, 0), 1);
                                        else return 0;
                                    } else {
                                        return 0;
                                    }
                                }
                                Item {
                                    width: progressBar.width
                                    height: progressBar.height
                                    Rectangle {
                                        width: parent.width
                                        height: parent.height / 2
                                        color: "#2a2a2a"
                                    }
                                    Rectangle {
                                        width: parent.width
                                        height: parent.height / 2
                                        radius: 6
                                        color: "#2a2a2a"
                                        anchors.bottom: parent.bottom
                                    }
                                }
                                Item {
                                    width: progressBar.width * progressBar.value
                                    height: progressBar.height
                                    Rectangle {
                                        width: parent.width
                                        height: parent.height / 2
                                        color: "#eab308"
                                    }
                                    Rectangle {
                                        width: parent.width
                                        height: parent.height / 2
                                        radius: 6
                                        color: "#eab308"
                                        anchors.bottom: parent.bottom
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
                            function onSwitchingMode() {
                                achievementHeaderLoader.active = false;
                                listViewLoader.active = false;
                                completionHeader.visible = false;
                                completionIcon.visible = false;
                                mouseAreaMode.disableModeChangeButton();
                                changeCheckBox.enabled = false;
                                autoHardcore.color = "#eeeeee";
                            }
                        }

                        Connections {
                            target: ra2snes
                            function onAchievementModelReady() {
                                sortedAchievementModel.clearMissableFilter();
                                sortedAchievementModel.clearUnlockedFilter();
                                sortedAchievementModel.sortByNormal();
                                achievementHeaderLoader.active = true;
                                listViewLoader.active = true;
                                completionHeader.visible = true;
                                let val =(gameInfoModel.completion_count / gameInfoModel.achievement_count);
                                if(val >= 0)
                                    progressBar.value = Math.min(Math.max(val, 0), 1);
                                else progressBar.value = 0;
                                completionIcon.visible = true;
                                mouseAreaMode.enableModeChangeButton();
                                changeCheckBox.enabled = true;
                                autoHardcore.color = "#2c97fa";
                            }
                        }

                        Connections {
                            target: ra2snes
                            function onClearedAchievements() {
                                achievementHeaderLoader.active = false;
                                listViewLoader.active = false;
                                completionHeader.visible = false;
                                completionIcon.visible = false;
                            }
                        }

                        Component {
                            id: achievementHeader
                            Column {
                                Layout.fillWidth: true
                                Layout.leftMargin: 20
                                Layout.bottomMargin: 10
                                Layout.rightMargin: 20
                                Layout.topMargin: 10
                                spacing: 6
                                Row {
                                    Text {
                                        text: qsTr("There are ")
                                        font.family: "Verdana"
                                        font.pixelSize: 13
                                        color: "#2c97fa"
                                        Layout.fillWidth: true
                                    }
                                    Text {
                                        text: {
                                            if(gameInfoModel)
                                                gameInfoModel.achievement_count;
                                            else ""
                                        }
                                        font.family: "Verdana"
                                        font.pixelSize: 13
                                        color: "#2c97fa"
                                        Layout.fillWidth: true
                                        font.bold: true
                                    }
                                    Text {
                                        text: qsTr(" achievements worth ")
                                        font.family: "Verdana"
                                        font.pixelSize: 13
                                        color: "#2c97fa"
                                        Layout.fillWidth: true
                                    }
                                    Text {
                                        text: {
                                            if(gameInfoModel)
                                                gameInfoModel.point_total
                                            else ""
                                        }
                                        font.family: "Verdana"
                                        font.pixelSize: 13
                                        color: "#2c97fa"
                                        Layout.fillWidth: true
                                        font.bold: true
                                    }
                                    Text {
                                        text: qsTr(" points.")
                                        font.family: "Verdana"
                                        font.pixelSize: 13
                                        color: "#2c97fa"
                                        Layout.fillWidth: true
                                    }
                                }
                                Row {
                                    spacing: 6
                                    Rectangle {
                                        id: missableRectangle
                                        width: 20
                                        height: 20
                                        radius: 50
                                        color: "#161616"
                                        Image {
                                            id: missableImage
                                            anchors.centerIn: parent
                                            width: 14
                                            height: 14
                                            source: "./images/missable.svg"
                                        }
                                    }
                                    Row {
                                        Text {
                                            text: qsTr("This set has ")
                                            font.family: "Verdana"
                                            font.pixelSize: 13
                                            color: "#2c97fa"
                                            Layout.fillWidth: true
                                        }
                                        Text {
                                            text: {
                                                if(gameInfoModel)
                                                    gameInfoModel.missable_count
                                                else "0"
                                            }
                                            font.family: "Verdana"
                                            font.pixelSize: 13
                                            color: "#2c97fa"
                                            Layout.fillWidth: true
                                        }
                                        Text {
                                            text: qsTr(" missable achievements")
                                            font.family: "Verdana"
                                            font.pixelSize: 13
                                            color: "#2c97fa"
                                            Layout.fillWidth: true
                                        }
                                    }
                                }
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
                                    Text{
                                        text: "-"
                                        font.family: "Verdana"
                                        font.pixelSize: 13
                                        color: "#2c97fa"
                                        Layout.fillWidth: true
                                    }
                                    Text{
                                        id: time
                                        text: qsTr("Lastest")
                                        font.family: "Verdana"
                                        font.pixelSize: 13
                                        color: "#cc9900"
                                        Layout.fillWidth: true
                                        MouseArea {
                                            id: mouseAreaTime
                                            anchors.fill: parent
                                            hoverEnabled: true
                                            onClicked: {
                                                sortedAchievementModel.sortByTime()
                                            }
                                            onEntered: time.state = "hovered"
                                            onExited: time.state = ""
                                        }

                                        states: [
                                            State {
                                                name: "hovered"
                                                PropertyChanges {
                                                    target: time
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
                                                    target: time
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
                                height: Math.max(72, descriptionText.implicitHeight + (model.timeUnlockedString !== "" ? unlockedTime.implicitHeight + 8 : 0) + 24)
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
                                            Layout.preferredWidth: achievement.width - 120
                                        }
                                        Item {
                                            height: 8
                                        }
                                        Text {
                                            id: unlockedTime
                                            color: "#7e7e7e"
                                            text: {
                                                if(model.timeUnlockedString !== "")
                                                    "Unlocked " + model.timeUnlockedString
                                                else ""
                                            }
                                            font.family: "Verdana"
                                            font.pixelSize: 10
                                            Layout.alignment: Qt.AlignBottom
                                            Layout.fillWidth: true
                                        }
                                    }
                                }
                                Rectangle {
                                    id: typeRectangle
                                    anchors.right: parent.right
                                    anchors.bottom: parent.bottom
                                    anchors.bottomMargin: 4
                                    anchors.rightMargin: 4
                                    width: 28
                                    height: 28
                                    radius: 50
                                    color: "#161616"
                                    visible: model.type !== "" ? true : false
                                    z: 2

                                    Text {
                                        z: 3
                                        id: typeText
                                        anchors.verticalCenter: parent.verticalCenter
                                        anchors.left: parent.left
                                        anchors.leftMargin: 20
                                        font.bold: true
                                        font.family: "Verdana"
                                        font.pixelSize: 10
                                        text: {
                                            if(model.type === "win_condition")
                                                "Win Condition"
                                            else if(model.type === "missable")
                                                "Missable"
                                            else if(model.type === "progression")
                                                "Progression"
                                            else ""
                                        }
                                        color: "#e5e5e5"
                                        visible: false
                                        opacity: 0.0

                                        Behavior on opacity {
                                            NumberAnimation {
                                                duration: 250
                                            }
                                        }

                                        Behavior on anchors.leftMargin {
                                            NumberAnimation {
                                                duration: 250
                                            }
                                        }
                                    }

                                    Image {
                                        z: 4
                                        id: svgImage
                                        anchors.right: parent.right
                                        anchors.verticalCenter: parent.verticalCenter
                                        anchors.rightMargin: 5
                                        width: 18
                                        height: 18
                                        source: {
                                            if(model.type === "win_condition")
                                                "./images/win_condition.svg"
                                            else if(model.type === "missable")
                                                "./images/missable.svg"
                                            else if(model.type === "progression")
                                                "./images/progression.svg"
                                            else ""
                                        }
                                    }

                                    MouseArea {
                                        anchors.fill: parent
                                        onEntered: typeRectangle.state = "hovered"
                                        onExited: typeRectangle.state = ""
                                        hoverEnabled: true
                                    }

                                    states: [
                                        State {
                                            name: "hovered"
                                            PropertyChanges {
                                                target: typeRectangle
                                                width: typeText.width + 38
                                            }
                                            PropertyChanges {
                                                target: typeText
                                                visible: true
                                                anchors.leftMargin: 10
                                                opacity: 1.0
                                            }
                                            PropertyChanges {
                                                target: svgImage
                                            }
                                        }
                                    ]

                                    transitions: [
                                        Transition {
                                            from: ""
                                            to: "hovered"
                                            PropertyAnimation {
                                                target: typeRectangle
                                                property: "width"
                                                duration: 50
                                            }
                                        },
                                        Transition {
                                            from: "hovered"
                                            to: ""
                                            PropertyAnimation {
                                                target: typeRectangle
                                                property: "width"
                                                duration: 200
                                            }
                                        }
                                    ]
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
