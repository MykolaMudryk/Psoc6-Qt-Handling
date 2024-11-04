import QtQuick
import QtQuick.Controls 2.15

ApplicationWindow {
  width: 640
  height: 480
  visible: true
  title: qsTr("LED Control")
  visibility: Window.FullScreen

  ComboBox {
    id: portComboBox

    displayText: "Port: " + currentText

    model: connection.getAvailablePorts()
    onCurrentTextChanged: {
      connection.setupSerialPort(currentText)
    }

    anchors {
      horizontalCenter: parent.horizontalCenter
    }
  }

  Row {
    id: checkboxRow

    anchors {
      horizontalCenter: parent.horizontalCenter
      bottom: slidersRow.top
    }

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
          brightnessSlider.enabled = true
        } else {
          brightnessSlider.enabled = false

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
          blinkingCheckbox.enabled = false
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
          brightnessSlider.enabled = false
        } else {
          brightnessSlider.enabled = true
          frequencySlider.enabled = false
        }
      }
    }

    CheckBox {
      id: sw2PressedCheckbox
      text: "SW2 Pressed"
      width: 120
      height: 50

      enabled: false
      Connections {
        target: ledHandler
        function onSw2Pressed() {
          sw2PressedCheckbox.checked = !sw2PressedCheckbox.checked
        }
      }
    }
  }

  Row {
    id: slidersRow
    anchors.centerIn: parent
    spacing: 80

    Item {
      id: frequencySliderContainer
      width: 230
      height: 100

      Column {
        id: sliderColumn
        spacing: 10
        anchors.centerIn: parent

        Row {
          spacing: 10
          anchors.horizontalCenter: parent.horizontalCenter

          Text {
            id: fromFrequencySlider
            text: qsTr("0.5Hz")
            font.pixelSize: 20
          }

          Slider {
            id: frequencySlider

            from: 0
            to: 5
            stepSize: 1

            width: 150

            snapMode: Slider.SnapAlways

            enabled: false
            value: 0

            property var fqValues: [2000, 400, 222, 154, 118, 100]

            onValueChanged: {

              ledHandler.setBlinkFrequency(fqValues[value])
            }
          }

          Text {
            id: toFrequencySlider
            text: qsTr("10Hz")
            font.pixelSize: 20
          }
        }
      }

      Text {
        id: frequencyText
        text: qsTr("Blinking frequency")
        font.pixelSize: 20

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: sliderColumn.bottom
      }
    }

    Item {
      id: brightnessSliderContainer
      width: 230
      height: 100

      Column {
        id: brightSliderColumn
        spacing: 10
        anchors.centerIn: parent

        Row {
          spacing: 10
          anchors.horizontalCenter: parent.horizontalCenter

          Text {
            id: fromBrightnessSlider
            text: qsTr("0%")
            font.pixelSize: 20
          }

          Slider {
            id: brightnessSlider

            from: 100.0
            stepSize: 10.0
            to: 0

            width: 200

            snapMode: Slider.SnapAlways

            enabled: false

            value: 100.0

            onValueChanged: {
              ledHandler.setBrightness(value)
            }
          }

          Text {
            id: toBrightnessSlider
            text: qsTr("100%")
            font.pixelSize: 20
          }
        }
      }

      Text {
        id: brightnessText
        text: qsTr("Brightness")
        font.pixelSize: 20

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: brightSliderColumn.bottom
      }
    }
  }
}
