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
5. maze loop
	5.1 read color
	5.2 perform action
	5.3 store path
6. return home

## Initialisation functions
1. Turn on buggy LEDs
2. Assign color click tri-color LEDs to registers
3. Enable I2C communication with color click board
4. Enable USART4 serial communication for debugging (at 9600 baud rate)
5. Assign motor control to registers
6. Enable ADC converter
7. Set up timer TMR0 interrupt

## Calibration
### Stopping Distance
### Color Detection

## Mode Selector
