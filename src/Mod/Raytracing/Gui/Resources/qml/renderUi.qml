import QtQuick 1.1
Item {
    id: renderTaskWidget

    // Signals
    signal previewWindow()
    signal preview()
    signal render()

    // Slots
    function renderRunning() { renderActive = true }
    function renderStopped() { renderActive = false }

    // Properties
    property bool renderActive: false


    width: 300
    height: 800
    anchors.fill: parent

    Rectangle {
        width: 100
        height: 62
        color: "#403d3d"
        anchors.fill: parent
        MouseArea {
            anchors.fill: parent
            onPressed: parent.focus = true
        }

        Column {
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.top: parent.top
            anchors.topMargin: 25
            spacing: 10
            Row {
                spacing: 10

                 Button {
                     id: previewButton
                     anchors.bottom: parent.bottom
                     anchors.bottomMargin: 0
                     text: qsTr("Preview")
                     onClicked: renderTaskWidget.preview()
                     enabled: !renderTaskWidget.renderActive
                 }
                 Button {
                     id: previeWindowButton
                     anchors.bottom: parent.bottom
                     anchors.bottomMargin: 0
                     text: qsTr("Preview Window")
                     onClicked: renderTaskWidget.previewWindow()
                     enabled: !renderTaskWidget.renderActive
                 }
            }
            Row {
                width: parent.width
                Button {
                    id: renderButton
                    width: parent.width
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 0
                    text: qsTr("Render")
                    enabled: !renderTaskWidget.renderActive
                    onClicked: renderTaskWidget.render()

                }
            }
            Row {
                Column {
                    spacing: 10
                    Text {
                        color: "#f9f9f9"
                        text: qsTr("Render Properties:")
                        font.pointSize: 12
                    }
                    Row {
                        spacing: 20

                        NumberInput {
                            id: sizeX
                            numDigits: 4
                            text: renderFeature.getOutputX().toString()
                            inputMask: "0000"
                            onValueChanged: renderFeature.setOutputX(parseInt(sizeX.text))
                            suffix: "px"
                            label: "x:"
                        }
                        NumberInput {
                            id: sizeY
                            numDigits: 4
                            text: renderFeature.getOutputY().toString()
                            inputMask: "0000"
                            onValueChanged: renderFeature.setOutputY(parseInt(sizeY.text))
                            suffix: "px"
                            label: "y:"
                        }

                     } // Row End
                     Row {
                        width: parent.width
                        NumberInput {
                            id: updateInterval
                            width: parent.width
                            numDigits: 4
                            text: (renderFeature.getUpdateInterval() / 1000).toString() // in seconds
                            inputMask: "0000"
                            onValueChanged: renderFeature.setUpdateInterval(parseInt(updateInterval.text) * 1000) // in milliseconds
                            suffix: "s"
                            label: "Update Interval:"
                        }

                     } // Row End
                     Row {
                         width: parent.width

                         ComboBox {
                             function  getLabel(myId) {
                                 for(var i = 0; i < renderPreset.listview.count; i++) {
                                     var listItem = listview.contentItem.children[i];
                                     if(listItem.presetId == myId) {return listItem.presetLabel; }
                                 }
                                 return "";
                             }
                             id: renderPreset
                             model: presetsModel
                             property string preset;
                             width: parent.width
                             height: 20
                             onComboClicked: renderFeature.setRenderPreset(renderPreset.preset)
                             delegate: Item {
                                 id: presetsDelegate
                                 width: parent.width;
                                 height: presDelLabel.height + presDelDesc.height + 15

                                 // Due to lazy loading these need to be set
                                 property string presetId: id
                                 property string presetLabel: label

                                 Text {
                                     id: presDelLabel
                                     text: label
                                     anchors.verticalCenter: parent.center
                                     anchors.right: parent.right
                                     anchors.rightMargin: 10
                                     color: "#fff"
                                 }

                                 Text {
                                     id: presDelDesc
                                     text: description
                                     anchors.top: presDelLabel.bottom
                                     anchors.topMargin: 5
                                     anchors.right: parent.right
                                     anchors.rightMargin: 10
                                     width: parent.width
                                     horizontalAlignment: TextInput.AlignRight
                                     font.pointSize: 7
                                     color: "#fff"
                                     opacity: 0.6
                                     states: State {name: "display"; when: preDelMouseArea.containsMouse
                                                    PropertyChanges { target: presDelDesc; opacity: 1.0} }
                                 }
                                 MouseArea {
                                     id: presDelMouseArea
                                     hoverEnabled: true;
                                     anchors.fill: parent
                                     onClicked: {
                                         comboBox.state = ""
                                         var prevSelection = chosenItemText.text
                                         chosenItemText.text = label
                                         if(chosenItemText.text != prevSelection){
                                             renderPreset.preset = parent.presetId
                                             comboBox.comboClicked();
                                         }
                                         listView.currentIndex = index;
                                     }
                                 }
                             }
                             selectedItem: getLabel(renderFeature.getRenderPreset())
                         }
                     } // Row End
                     Row {
                         width: parent.width
                         ComboBox {
                             function  getLabel(myId) {
                                 for(var i = 0; i < renderTemplate.listview.count; i++) {
                                     var listItem = listview.contentItem.children[i];
                                     if(listItem.templateId == myId) {return listItem.templateLabel; }
                                 }
                                 return "";
                             }
                             id: renderTemplate
                             model: templatesModel
                             property string templateName;
                             width: parent.width
                             height: 20
                             onComboClicked: renderFeature.setRenderTemplate(renderPreset.templateName)
                             delegate: Item {
                                 id: templatesDelegate
                                 width: parent.width;
                                 height: tempDelLabel.height + tempDelDesc.height + 15

                                 // Due to lazy loading these need to be set
                                 property string templateId: id
                                 property string templateLabel: label

                                 Text {
                                     id: tempDelLabel
                                     text: label
                                     anchors.verticalCenter: parent.center
                                     anchors.right: parent.right
                                     anchors.rightMargin: 10
                                     color: "#fff"
                                 }

                                 Text {
                                     id: tempDelDesc
                                     text: description
                                     anchors.top: tempDelLabel.bottom
                                     anchors.topMargin: 5
                                     anchors.right: parent.right
                                     anchors.rightMargin: 10
                                     width: parent.width
                                     horizontalAlignment: TextInput.AlignRight
                                     font.pointSize: 7
                                     color: "#fff"
                                     opacity: 0.6
                                     states: State {name: "display"; when: tempDelMouseArea.containsMouse
                                                    PropertyChanges { target: tempDelDesc; opacity: 1.0} }
                                 }
                                 MouseArea {
                                     id: tempDelMouseArea
                                     hoverEnabled: true;
                                     anchors.fill: parent
                                     onClicked: {
                                         comboBox.state = ""
                                         var prevSelection = chosenItemText.text
                                         chosenItemText.text = label
                                         if(chosenItemText.text != prevSelection){
                                             renderTemplate.templateName = parent.templateId
                                             comboBox.comboClicked();
                                         }
                                         listView.currentIndex = index;
                                     }
                                 }
                             }
                             selectedItem: getLabel(renderFeature.getRenderTemplate())
                         }
                     } // Row End


                } // Column End
            }
        }
    }
}
