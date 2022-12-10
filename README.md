# Use
1. Calibrate the stopping distance
- place the blue card 3 ft away from the buggy and press the H3 button
2. Calibrate each color card
- starting from the red card, place it in directly in front of the buggy and press H3
- repeat for the rest of the colors (green, blue, yellow, pink, orange, light blue, white) in order
3. Select mode with F2 (easy mode) or F3 (hard mode)
4. [Send me on my way.](https://www.youtube.com/watch?v=IGMabBGydC0) 

# Structure of Program
1. initiliasation functions
2. calibration for stopping distance
3. calibration for color detection
4. mode selector
5. maze navigation
	1. read color
	2. interpret color
	3. motor action
	4. store path
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
The program will make use of the clear/"L" channel of the color-click board to determine when there is a card in front of the buggy. As the buggy navigates the maze, the light will be continuously recorded and checked. If the light levels experience a drop below a certain threshold, the buggy will stop.

### Color Detection

## Mode Selector

## Maze Navigation
### Reading Color
### Interpreting Color
### Motor Action
### Storage of Path

## [Return Home](https://www.youtube.com/watch?v=iyFijjikkeM)
