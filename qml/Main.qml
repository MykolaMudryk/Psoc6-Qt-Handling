import QtQuick
import QtQuick.Controls 2.15

ApplicationWindow {
  width: 640
  height: 480
  visible: true
  title: qsTr("LED Control")
  visibility: Window.FullScreen
  Row {
    spacing: 10
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.verticalCenter: parent.verticalCenter
    CheckBox {
      id: onCheckbox
      text: "On"

      width: 100
      height: 50

      onCheckedChanged: {
        if (checked) {
          ledHandler.setOn()
          offCheckbox.checked = false
          blinkingCheckbox.enabled = true
        } else {
          frequencySlider.enabled = false
          blinkingCheckbox.checked = false
          blinkingCheckbox.enabled = false
        }
      }
    }

    CheckBox {
      id: offCheckbox
      text: "Off"

      width: 100
      height: 50

      enabled: onCheckbox.checked
      onCheckedChanged: {
        if (checked) {
          ledHandler.setOff()
          onCheckbox.checked = false
          frequencySlider.enabled = false
          blinkingCheckbox.checked = false
          blinkingCheckbox.enabled = false
        }
      }
    }

    CheckBox {
      id: blinkingCheckbox
      text: "Blink"

      width: 100
      height: 50

      enabled: false
      onCheckedChanged: {
        if (checked) {
          ledHandler.setBlink()
          frequencySlider.enabled = true
        } else {

          frequencySlider.enabled = false
        }
      }
    }

    Text {
      id: fromFrequencySlider
      text: qsTr("0.5")

      font.pixelSize: 20
    }

    Slider {
      id: frequencySlider
      from: 0.5
      stepSize: 1
      to: 10
      snapMode: Slider.SnapAlways

      width: 200

      enabled: false
      value: 5
      onValueChanged: {
        ledHandler.setBlinkFrequency(value)
      }
    }
    Text {
      id: toFrequencySlider
      text: qsTr("10")

      font.pixelSize: 20
    }
  }
}
