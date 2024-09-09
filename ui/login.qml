import QtQuick 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15

ApplicationWindow {
    id: window
    width: 600
    height: 600
    visible: true
    title: "ra2snes"
    Material.theme: Material.Dark
    Material.accent: "#ffffff"

    background: Rectangle {
        color: "#1a1a1a"
    }

    property bool loginFailed: false

    function signIn(username, password) {
        ra2snes.signIn(username, password);
    }

    function showErrorMessage() {
        window.loginFailed = true;
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            id: signin_group
            Layout.preferredWidth: 320
            Layout.preferredHeight: 320
            Layout.alignment: Qt.AlignHCenter
            color: "#232323"
            border.width: 2
            border.color: "#161616"
            radius: 6

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 15
                spacing: 0

                Text {
                    id: username_label
                    text: "Username"
                    font.family: "Verdana"
                    font.pixelSize: 13
                    color: "#2c97fa"
                }

                TextField {
                    id: username_input
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignTop
                    font.family: "Verdana"
                    font.pixelSize: 13
                    color: "#c8c8c8"
                    background: Rectangle {
                        color: "#161616"
                        border.width: 1
                        border.color: username_input.focus ? "#ffffff" : "#4b4b4b"
                        radius: 4
                    }
                    Layout.bottomMargin: -10
                }

                Text {
                    id: password_label
                    text: "Password"
                    font.family: "Verdana"
                    font.pixelSize: 13
                    color: "#2c97fa"
                }

                TextField {
                    id: password_input
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignTop
                    font.family: "Verdana"
                    font.pixelSize: 13
                    color: "#c8c8c8"
                    echoMode: TextInput.Password
                    background: Rectangle {
                        color: "#161616"
                        border.width: 1
                        border.color: password_input.focus ? "#ffffff" : "#4b4b4b"
                        radius: 4
                    }
                    Layout.bottomMargin: -8
                }

                Row {
                    id: checkbox_row
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignTop
                    spacing: 8

                    CheckBox {
                        id: remember_checkbox
                        width: 15
                        height: 15

                        indicator: Rectangle {
                            width: 15
                            height: 15
                            radius: 4
                            color: remember_checkbox.checked ? "#005cc8" : "#ffffff"
                            border.color: remember_checkbox.checked ? "#005cc8" : "#4f4f4f"

                            Text {
                                anchors.centerIn: parent
                                text: remember_checkbox.checked ? "\u2713" : ""
                                color: "#ffffff"
                                font.pixelSize: 12
                            }
                        }
                    }

                    Text {
                        text: "Remember Me"
                        font.family: "Verdana"
                        font.pixelSize: 13
                        color: "#2c97fa"
                        verticalAlignment: Text.AlignVCenter
                    }
                    Layout.topMargin: 0
                }

                Button {
                    id: signin_button
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignTop
                    text: "Sign in"
                    font.family: "Verdana"
                    font.pixelSize: 13
                    background: Rectangle {
                        id: buttonBackground
                        color: "#161616"
                        border.width: 1
                        border.color: "#2a2a2a"
                        radius: 4
                    }
                    contentItem: Text {
                        id: buttonText
                        text: "Sign in"
                        color: "#cc9900"
                        font.family: "Verdana"
                        font.pixelSize: 13
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    Layout.topMargin: 5
                    Layout.bottomMargin: -10

                    MouseArea {
                        id: mouseArea
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: {
                            window.signIn(username_input.text, password_input.text);
                        }
                        onEntered: signin_button.state = "hovered"
                        onExited: signin_button.state = ""
                    }

                    states: [
                        State {
                            name: "hovered"
                            PropertyChanges {
                                target: buttonBackground
                                color: "#333333"
                                border.color: "#ffffff"
                            }
                            PropertyChanges {
                                target: buttonText
                                color: "#ffffff"
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
                Item {
                    id: errorMessagePlaceholder
                    Layout.preferredHeight: 13
                    Layout.alignment: Qt.AlignHCenter
                    Layout.topMargin: 5

                    Text {
                        id: errorMessage
                        text: "Login failed. Please try again."
                        font.family: "Verdana"
                        font.pixelSize: 13
                        color: "#ff0000"
                        width: parent.width
                        horizontalAlignment: Text.AlignHCenter
                        visible: window.loginFailed
                    }
                }
            }
        }
    }
    Connections {
        target: ra2snes
        function onLoginFailed() {
            window.showErrorMessage();
        }
    }
}
