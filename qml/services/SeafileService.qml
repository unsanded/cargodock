import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.cargodock 1.0
import "../shared"

ServiceObject {
    id: service
    serviceModel: SeafileModel {
        id: seafileModel
    }

    name: "seafile"
    icon: Qt.resolvedUrl("seafile.png")
    usesEncryption: true
    serviceName: "seafile"
    Component.onCompleted: {

        var server = serviceModel.getServer();
        console.log(server);
    }


    serviceDelegate: ServiceDelegate {
        title: "<b>Sea</b>file"
        subtitle: "Connect to a <b>sea</b>file server."
        iconSource: service.icon
    }

    serviceConfigurator: Component {
        Dialog {

            signal serviceConfigured(string serviceName,
                                     string icon,
                                     variant properties)

            property var properties

            onPropertiesChanged: {
                nameEntry.text = properties["name"];
                addressEntry.text = properties["address"];
                loginEntry.text = properties["login"];
            }


            SilicaFlickable {
                anchors.fill: parent
                contentHeight: column.height
                Column {
                    id: column
                    width: parent.width

                    DialogHeader {
                        title: "configure seafile"
                    }

                    Item {
                        width: 1
                        height: Theme.paddingLarge
                    }

                    //TODO: add image

                    Item {
                        width: 1
                        height: Theme.paddingLarge
                    }

                    TextField {
                        id: nameEntry

                        anchors.left: parent.left
                        anchors.right: parent.right

                        focus: true
                        label: "Name"
                        placeholderText: "Enter name"
                    }

                    TextField {
                        id: addressEntry

                        anchors.left: parent.left
                        anchors.right: parent.right

                        inputMethodHints: Qt.ImhUrlCharactersOnly
                        label: "Server"
                        placeholderText: "Enter server address or ip"
                        text: "http://myserver.org:8000/api2"
                        validator: RegExpValidator {
                            regExp: /https?:\/\/[^ ]+/
                        }
                    }
                    SeafileServer{
                        id: testServer
                        baseAddress: addressEntry.text
                        onStatusChanged: {
                            if(status === SeafileServer.UnknownConnectable)
                                connectionStatus.text="";
                            if(status === SeafileServer.UnConnectable)
                                connectionStatus.text="not connected";
                            if(status === SeafileServer.Connectable)
                                connectionStatus.text="connected";
                            if(status === SeafileServer.Authenticated)
                                connectionStatus.text="authenticated";
                        }
                    }
                    Row{
                        Button{
                            text: "test connection";
                            onClicked: {
                                testServer.ping(false);
                            }
                        }
                        Label{
                            id: connectionStatus
                            text: qsTr("not connected");
                        }
                    }

                    SectionHeader {
                        text: "Authorization"
                    }

                    TextField {
                        id: loginEntry

                        anchors.left: parent.left
                        anchors.right: parent.right

                        inputMethodHints: Qt.ImhNoPredictiveText + Qt.ImhNoAutoUppercase
                        placeholderText: "Enter email"
                        label: "email or username"
                    }

                    PasswordField {
                        id: passwordEntry

                        anchors.left: parent.left
                        anchors.right: parent.right

                        placeholderText: "Enter password"
                        label: "Password"
                    }
                    Button{
                        text: "log in";
                        onClicked: {
                            testServer.aquireToken(loginEntry.text, passwordEntry.text);
                        }
                    }
                }
                ScrollDecorator { }
            }

            canAccept: testServer.status == SeafileServer.Authenticated;

            onAccepted: {
                var token = testServer.authenticationToken;
                console.log(token)

                var props = {
                    "name": nameEntry.text,
                    "address": addressEntry.text,
                    "login": loginEntry.text,
                    "token:blowfish": token
                }

                serviceConfigured(service.serviceName,
                                  service.icon,
                                  props);
            }

        }//Dialog
    }//Component
}
