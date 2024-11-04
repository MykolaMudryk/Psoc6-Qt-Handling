![Test task video](testtask.gif)

# LED Control GUI Application

This is a GUI application using **C++**, **C**, **Qt**, and **QML** that allows users to control an LED via **UART** on the **PSoC6** microcontroller. The hardware side of the application was developed using **ModusToolbox** in the **C** programming language.

## Application Features

### LED Mode Control

The user can switch between three modes for LED:

- **Off** – Turns LED off.
- **On** – Turns LED on.
- **Blink** – Makes LED blink at a specified frequency.

### Adjustable Blinking Frequency

- The blinking frequency of LED can be adjusted using a slider within a range of **0.5 Hz to 10 Hz**.

### Adjustable Brightness

- The brightness of LED can be set from **0% to 100%** using a slider.

### Button Control (SW2)

- When the SW2 button is pressed, the brightness of LED is set to **100%**, and the blinking frequency is set to **2 Hz**. 
- These changes are reflected in the GUI by automatically marking the checkbox as checked.
