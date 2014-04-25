// This file is automatically generated, please don't edit manully.
import QtQuick 2.1
import Deepin.Widgets 1.0
import "../components"

BaseEditSection {
    id: sectionVpnL2tpIpsec
    section: "vpn-l2tp-ipsec"
    
    header.sourceComponent: EditDownArrowHeader{
        text: dsTr("IPsec")
    }

    content.sourceComponent: Column { 
        EditLineSwitchButton {
            key: "ipsec-enabled"
            text: dsTr("!!ipsec-enabled")
        }
        EditLineTextInput {
            key: "ipsec-gateway-id"
            text: dsTr("!!ipsec-gateway-id")
        }
        EditLineTextInput {
            key: "ipsec-group-name"
            text: dsTr("!!ipsec-group-name")
        }
        EditLinePasswordInput {
            key: "ipsec-psk"
            text: dsTr("!!ipsec-psk")
        }
    }
}