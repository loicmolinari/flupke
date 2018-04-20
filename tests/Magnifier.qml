import QtQuick 2.3
import Quicken.Items 0.1

Magnifier {
    width: 800
    height: 450

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
