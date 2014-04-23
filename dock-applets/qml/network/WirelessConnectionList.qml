import QtQuick 2.0
import Deepin.Widgets 1.0

Column {
    id: wirelessDeviceList
    width: parent.width

    property string wirelessDevicePath: "/"
    property string inConnectingApPath: "/"

    Rectangle {
        width: parent.width - 10
        height: 1
        color: Qt.rgba(1, 1, 1, 0.1)
        anchors.horizontalCenter: parent.horizontalCenter
    }

    ListView {
        width: parent.width
        height: Math.min(childrenRect.height, 255)
        model: accessPointsModel
        delegate: WirelessConnectionItem{
            devicePath: wirelessDevicePath
            inConnectingApPath: wirelessDeviceList.inConnectingApPath
        }
        visible: accessPointsModel.count > 0

        DScrollBar {
            flickable: parent
        }
    }

    DBaseLine{
        visible: accessPointsModel.count == 0
        color: "transparent"
        leftLoader.sourceComponent: DssH2{
            font.pixelSize: 12
            text: dsTr("Scanning...")
        }
    }

    ListModel {
        id: accessPointsModel

        function getIndexByApPath(path){
            for(var i; i<count; i++){
                var obj = get(i)
                if(obj.apPath == path){
                    return i
                }
            }
            return -1
        }

        function getInsertPosition(apProperty){
            var position = count
            for(var i; i<count; i++){
                var obj = get(i)
                if(apProperty[3] != obj.apPath && apProperty[2] >= obj.apSignal){
                    position = i
                    break
                }
            }
            return position
        }
    }

    Timer {
        id: scanTimer
        interval: 100
        running: true
        onTriggered: {
            var accessPoints = dbusNetwork.GetAccessPoints(wirelessDevicePath)
            inConnectingApPath = "/"
            accessPointsModel.clear()

            for(var i in accessPoints){
                var ap = dbusNetwork.GetAccessPointProperty(accessPoints[i])
                accessPointsModel.append({
                    "apName": ap[0],
                    "apSecured": ap[1],
                    "apSecuredInEap": ap[2],
                    "apSignal": ap[3],
                    "apPath": ap[4]
                })
            }
            sortModel()
        }
    }

    function sortModel()
    {
        var n;
        var i;
        for (n=0; n < accessPointsModel.count; n++){
            for (i=n+1; i < accessPointsModel.count; i++)
            {
                if (accessPointsModel.get(n).apSignal < accessPointsModel.get(i).apSignal)
                {
                    accessPointsModel.move(i, n, 1);
                    n=0; // Repeat at start since I can't swap items i and n
                }
            }
        }
    }
}