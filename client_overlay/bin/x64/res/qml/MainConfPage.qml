import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2
import pottedmeat7.keyboardinput 1.0

MyMainViewPage {
    id: mainConfPage
    name: "mainConfPage"
    property var initialLoaded: false

    function updateInfo() {  
    }

    content: ColumnLayout {
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        spacing: 7
        Layout.fillHeight: true

        GroupBox {
            Layout.fillWidth: true
            
            background: Rectangle {
                color: myPalette.base
                border.color: myPalette.base
                radius: 1
            }

            ColumnLayout {
                anchors.fill: parent
                Layout.alignment: Qt.AlignHCenter

                RowLayout {
                    spacing: 0

                    MyText {
                        id: headerTitle
                        text: "OpenVR-KeyboardInput"
                        font.pointSize: 22
                    }

                    MyText {
                        text: " "
                        Layout.preferredWidth: 165
                    }  
                }
            }
        }

        GroupBox {
            id: mainConfigBox
            anchors.top: parent.top
            anchors.topMargin: 70

            Layout.fillWidth: true
            
            background: Rectangle {
                color: myPalette.mid
                border.color: myPalette.mid
                radius: 1
            }

            ColumnLayout {
                anchors.fill: parent
                Layout.alignment: Qt.AlignHCenter

                GridLayout {
                    columns: 1

                    MyToggleButton {
                        id: wipEnableToggle
                        text: "Enable Keyboard Input"
                        Layout.maximumWidth: 300
                        Layout.minimumWidth: 300
                        Layout.preferredWidth: 300
                        Layout.fillWidth: true
                        onCheckedChanged: {
                            KeyboardInputTabController.enableKI(checked)
                        }
                    }

                }
            }
        }


        ColumnLayout {
            id: profileColumnLYO
            spacing: 18
            anchors.top: mainConfigBox.bottom
            anchors.topMargin: 20

            GroupBox {

                Layout.fillWidth: true
                
                background: Rectangle {
                    color: myPalette.mid
                    border.color: myPalette.mid
                    radius: 1
                }

                ColumnLayout {
                    anchors.fill: parent

                    RowLayout {
                        spacing: 18

                        MyComboBox {
                            id: profileComboBox
                            Layout.maximumWidth: 290
                            Layout.minimumWidth: 290
                            Layout.preferredWidth: 290
                            Layout.fillWidth: true
                            model: [""]
                            onCurrentIndexChanged: {
                                if (currentIndex > 0) {
                                    applyProfileButton.enabled = true
                                    deleteProfileButton.enabled = true
                                } else {
                                    applyProfileButton.enabled = false
                                    deleteProfileButton.enabled = false
                                }
                            }
                        }

                        MyPushButton {
                            id: applyProfileButton
                            Layout.topMargin: 10
                            enabled: false
                            Layout.preferredWidth: 150
                            text: "Apply"
                            onClicked: {
                                if (profileComboBox.currentIndex > 0) {
                                    KeyboardInputTabController.applyProfile(profileComboBox.currentIndex - 1);
                                    updateInfo()
                                }
                            }
                        }

                        MyPushButton {
                            id: newProfileButton
                            Layout.preferredWidth: 240
                            Layout.topMargin: 10
                            text: "New Profile"
                            onClicked: {
                                newProfileDialog.openPopup()
                            }
                        }

                        MyPushButton {
                            id: deleteProfileButton
                            enabled: false
                            Layout.preferredWidth: 260
                            Layout.topMargin: 10
                            text: "Delete Profile"
                            onClicked: {
                                if (profileComboBox.currentIndex > 0) {
                                    deleteProfileDialog.profileIndex = profileComboBox.currentIndex - 1
                                    deleteProfileDialog.open()
                                }
                            }
                        }
                    }
                }
            }
        }

        Component.onCompleted: {   
            if ( !initialLoaded ) { 
                //updateInfo()
                initProfiles()
            }
            initialLoaded = true
        }

    }

    MyDialogOkCancelPopup {
        id: deleteProfileDialog
        property int profileIndex: -1
        dialogTitle: "Delete Profile?"
        dialogText: "Do you really want to delete this profile?"
        onClosed: {
            if (okClicked) {
                KeyboardInputTabController.deleteProfile(profileIndex)
                reloadProfiles()
            }
        }
    }

    MyDialogOkCancelPopup {
        id: newProfileDialog
        dialogTitle: "Create New Profile"
        dialogWidth: 600
        dialogHeight: 400
        dialogContentItem: ColumnLayout {
            RowLayout {
                Layout.topMargin: 16
                Layout.leftMargin: 16
                Layout.rightMargin: 16
                MyText {
                    text: "Name: "
                }
                MyTextField {
                    id: newProfileName
                    text: ""
                    Layout.fillWidth: true
                    function onInputEvent(input) {
                        text = input
                    }
                }
            }
        }
        onClosed: {
            if (okClicked) {
                if (newProfileName.text == "") {
                    //messageDialog.showMessage("Create New Profile", "ERROR: Empty profile name.")
                } else {
                    KeyboardInputTabController.addProfile(newProfileName.text)
                    reloadProfiles()
                }
            }
        }
        function openPopup() {
            newProfileName.text = ""
            open()
        }
    }

    function initProfiles() {
        //load profiles
        var profiles = [""]
        var profileCount = KeyboardInputTabController.getProfileCount()
        var defaultFound = -1
        for (var i = 0; i < profileCount; i++) {
            var p_name = KeyboardInputTabController.getProfileName(i)
            if ( p_name == "default" ) {
                defaultFound = i
            }
            profiles.push(p_name)
        }
        profileComboBox.model = profiles
        profileComboBox.currentIndex = 0
        if ( defaultFound >= 0 ) {
            KeyboardInputTabController.applyProfile(defaultFound);
            profileComboBox.currentIndex = defaultFound+1
            updateInfo()
        }
    }

    function reloadProfiles() {
        //load profiles
        var profiles = [""]
        var profileCount = KeyboardInputTabController.getProfileCount()
        for (var i = 0; i < profileCount; i++) {
            var p_name = KeyboardInputTabController.getProfileName(i)
            profiles.push(p_name)
        }
        profileComboBox.model = profiles
    }

}
