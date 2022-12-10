# Use
1. Calibrate the stopping distance
	- place the blue card 3 ft away from the buggy and press the F3 button
2. Calibrate each color card
	- starting from the red card, place it in directly in front of the buggy and press H3
	- repeat for the rest of the colors (green, blue, yellow, pink, orange, light blue, white) in order
3. Select mode with F2 (easy mode) or F3 (hard mode)
4. [Send me on my way.](https://www.youtube.com/watch?v=IGMabBGydC0) 

# Structure of Program
1. initiliasation functions
2. calibration
	1. for stopping distance
	2. for color detection
3. mode selector
4. maze navigation
	1. read color
	2. interpret color
	3. motor action
	4. store path
5. return home
6. sleepy time

## 1. Initialisation functions
- Turn on buggy LEDs
- Assign color click tri-color LEDs to registers
- Enable I2C communication with color click board
- Enable USART4 serial communication for debugging (at 9600 baud rate)
- Assign motor control to registers
- Enable ADC converter
- Set up timer TMR0 interrupt

## 2. Calibration
### i. Stopping Distance
The program will make use of the clear/"L" channel of the color-click board to determine when there is a card in front of the buggy. As the buggy navigates the maze, the light will be continuously recorded and checked. If the light levels experience a drop below a certain threshold, the buggy will stop.

### ii. Color Detection
The calibration sequence for color will store the LRGB values for the 8 colors in an array. This will be used later in 4.ii to interpret the card during maze navigation.

## 3. Mode Selector
There are two modes (easy/hard) that can be selected with the F2 and F3 buttons on the clicker board. The easy mode will be able to differentiate between red, blue, and green. Whereas, the hard mode will be able to interpret all 8 colors of the cards.
## 4. Maze Navigation
```C
stop(&motorL,&motorR); //stop the buggy
color_read(&RGB_recorded); //read RGB values of card
if (LATDbits.LATD7){color_name = color_processor_easy(&RGB_recorded);} //color detection for easy mode
else {color_name = color_processor_hard(&RGB_recorded,color_calibrated);} //color detection for hard mode 
color_path[j] = color_name; //store color to array
time_path[j] = time; //store time taken from the last action to array
motor_action(color_name,&motorL,&motorR);
```
### i. Reading Color
The LRGB colors will be read and stored into the structure

### ii. Interpreting Color
### iii. Motor Action
The color_name variable will be input to the motor_action() function. This function makes use of the switch...case statement, where each number corresponds to a color. For each color, there will be a specific action summarised below in the table.
Colour | Case | Instruction
---------|---------|---------
Red | 1 | Turn Right 90
Green | 4 | Turn Left 90
Blue | 5 | Turn 180
Yellow | 3 | Reverse 1 square and turn right 90
Pink | 7 | Reverse 1 square and turn left 90
Orange | 2 | Turn Right 135
Light blue | 6 | Turn Left 135 
White | 8 | Finish (return home)
Black | 9 | Maze wall colour
### iv. Storage of Path

## 5. [Return Home](https://www.youtube.com/watch?v=iyFijjikkeM)
