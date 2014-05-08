import QtQuick 2.1
import Deepin.Widgets 1.0
import "../shared"

Column {
    width: parent.width
    property int realHeight: childrenRect.height
    property int itemLabelLeftMargin: 22

    DBaseLine {
        leftLoader.sourceComponent: DssH2 {
            text: dsTr("新建网络")
        }
    }

    DSeparatorHorizontal {}

    Rectangle {
        width: parent.width
        height: childrenRect.height
        color: dconstants.contentBgColor

        ListView {
            id: typeList
            width: parent.width
            height: childrenRect.height
            visible: count != 0

            property string selectItemId: "dsl"
            model: ListModel {}

            Component.onCompleted: {
                model.append({
                    "item_id": "dsl",
                    "item_name": dsTr("DSL")
                })
                model.append({
                    "item_id": "vpn",
                    "item_name": dsTr("VPN")
                })
            }

            delegate: SelectItem {
                labelLeftMargin: itemLabelLeftMargin
                totalItemNumber: typeList.count
                selectItemId: typeList.selectItemId

                onSelectAction: {
                    typeList.selectItemId = itemId
                }
            }
        } // End of typeList
    }

    DSeparatorHorizontal {}

    Row {
        height: 38
        spacing: 10
        anchors.right: parent.right
        anchors.rightMargin: 15

        DTextButton {
            text: dsTr("Cancel")
            anchors.verticalCenter: parent.verticalCenter
            onClicked: stackView.reset()
        }

        DTextButton {
            text: dsTr("Next")
            anchors.verticalCenter: parent.verticalCenter
            onClicked: {
                if(typeList.selectItemId == "dsl"){
                    stackView.push(stackViewPages["newDslPage"])
                    stackView.currentItemId = "newDslPage"
                }
                else if(typeList.selectItemId == "vpn"){
                    stackView.push(stackViewPages["newVpnPage"])
                    stackView.currentItemId = "newVpnPage"
                }
            }
        }
    }

}