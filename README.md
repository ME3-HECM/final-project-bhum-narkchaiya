# Use
1. Calibrate the stopping distance by 
2. Calibrate each color card 
3. Select mode with F2 (easy mode) or F3 (hard mode)
4. [Send me on my way.](https://www.youtube.com/watch?v=IGMabBGydC0) 

# Structure of Program
1. initiliasation functions
2. calibration for stopping distance
3. calibration for color detection
4. mode selector
5. maze navigation
	..1. read color
	..2. interpret color
	3. motor action
	- store path
6. return home

## Initialisation functions
- Turn on buggy LEDs
- Assign color click tri-color LEDs to registers
- Enable I2C communication with color click board
- Enable USART4 serial communication for debugging (at 9600 baud rate)
- Assign motor control to registers
- Enable ADC converter
- Set up timer TMR0 interrupt

## Calibration
### Stopping Distance
### Color Detection

## Mode Selector

## Maze Navigation
### Reading Color
### Interpreting Color
### Motor Action
### Storage of Path

## [Return Home](https://www.youtube.com/watch?v=iyFijjikkeM)
