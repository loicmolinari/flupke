import QtQuick 2.3

Rectangle {
    width: 800
    height: 450
    color: Qt.rgba(0.3, 0.1, 0.4, 1.0)

    Text {
        anchors.centerIn: parent
        text: "Quicken"
        color: "white"
        font.pointSize: 106
        font.kerning: true
        font.family: "EB Garamond"
        renderType: Text.NativeRendering
    }
}
