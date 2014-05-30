import QtQuick 2.1
import Deepin.Widgets 1.0

DBaseLine {
    id: root
    objectName: "BaseEditLine"
    
    property string key
    property string text
    property var value // mid-value between ConnectionSession and widget
    
    // update value even if other key changed
    property bool alwaysUpdate: false
    
    signal widgetShown
    
    visible: false
    Binding on visible {
        value: isKeyAvailable()
    }
    onVisibleChanged: {
        if (visible) {
            if (value !== undefined) {
                // reset key if value already defined
                setKey()
            } else {
                // get value only when undefined
                updateValue()
            }
            widgetShown()
        }
        // TODO test
        print("BaseEditLine.onVisibleChanged", visible ? "(show)" : "(hide)", section, key, value)
    }
    Component.onCompleted: {
        if (visible) {
            // send widgetShown() signal is need here
            widgetShown()
        }
    }
    
    // colors
    color: dconstants.contentBgColor
    property color normalBorderColor: dconstants.contentBgColor
    property color normalColor: dconstants.fgColor
    property color errorColor: "#F48914"
    
    // error state
    property bool showErrorConditon
    property bool showError: showErrorConditon && isValueError()
    Connections {
        target: rightLoader.item
        onActiveFocusChanged: {
            print("onActiveFocusChanged", rightLoader.item.activeFocus) //TODO test
            if (!rightLoader.item.activeFocus) {
                showErrorConditon = true
            }
        }
    }
    onShowErrorChanged: {
        // TODO border color or text color
        // border.color = showError ? errorColor : normalBorderColor
        leftLoader.item.color = showError ? errorColor : normalColor
        if (showError) {
            expandSection()
        }
    }
    
    leftMargin: contentLeftMargin
    leftLoader.sourceComponent: DssH2{
        text: root.text
    }
    
    function setKey() {
        print("-> BaseEditLine.setKey()", section, key, value) // TODO test
        generalSetKey(section, key, value)
        updateKeysAlways()
    }

    function getKey() {
        return generalGetKey(section, key)
    }
    
    function updateValue() {
        value = getKey()
        print("-> updateValue()", section, key, value) // TODO test
    }
    
    function isKeyAvailable() {
        return getIndexFromArray(key, availableKeys) != -1
    }
    
    function isValueError() {
        if (root.value == undefined) {
            return false
        }
        return errors[root.key] ? true : false
    }
    
    function getAvailableValues() {
        var valuesJSON = connectionSession.GetAvailableValues(section, key);
        var values = unmarshalJSON(valuesJSON)
        return values
    }
    
    function getAvailableValuesText() {
        var values = getAvailableValues()
        var valuesText = []
        for (var i=0; i<values.length; i++) {
            valuesText.push(values[i].Text)
        }
        return valuesText
    }
    
    function getAvailableValuesTextByValue() {
        var values = getAvailableValues()
        if (values == null) {
            // values is null here so this function should
            // not be called in this case
            print("==> [WARNING] getAvailableValuesTextByValue", values, section, key, value) //TODO test
            return
        }
        for (var i=0; i<values.length; i++) {
            if (values[i].Value === value) {
                return values[i].Text
            }
        }
        print("==> [WARNING] getAvailableValuesTextByValue():", values, section, key, value) //TODO test
        return ""
    }
    
    function getAvailableValuesIndex() {
        var values = getAvailableValues()
        if (values == null) {
            return 0
        }
        for (var i=0; i<values.length; i++) {
            if (values[i].Value === value) {
                return i
            }
        }
        print("==> [WARNING] getAvailableValuesIndex():", values, section, key, value) //TODO test
        return 0
    }
    
    function checkKey() {
        print("check key", section, key, value) // TODO test
        showErrorConditon = true
    }
}