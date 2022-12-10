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
	1. for color interpretation
	2. for detection of the card in front
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

### i. Color Interpretation
The calibration sequence for color will store the LRGB values for the 8 colors in an array. This will be used later in 4.ii to interpret the card during maze navigation.
A window of 2 seconds is available to have the correct colored card in front of the buggy (indicated by H3 LED turning on), and another two seconds to change to the next card (H3 LED off).
### ii. Card Detection
The program will make use of the clear/"L" channel of the color-click board to determine when there is a card in front of the buggy. As the buggy navigates the maze, the light will be continuously recorded and checked. If the light levels experience a drop below a certain threshold, the buggy will stop.
To be robust, the bounds will be set between the highest and lowest "L" values of the colors. This happens to be white and blue, respectively.
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
The LRGB colors will be read and stored into the structure RGB_recorded with the function color_read().

### ii. Interpreting Color
Then, for the easy mode, the recorded RGB values will be interpreted by the function color_processor_easy(). In hard mode, the program will choose to use the color_processor_hard() instead.
### iii. Motor Action
The color_name variable will be input to the motor_action() function. This function makes use of the switch...case statement, where each number corresponds to a color. For each color, there will be a specific action summarised below in the table.
Colour | Case | Instruction
---------|---------|---------
Red | 1 | Turn Right 90
Green | 2 | Turn Left 90
Blue | 3 | Turn 180
Yellow | 4 | Reverse 1 square and turn right 90
Pink | 5 | Reverse 1 square and turn left 90
Orange | 6 | Turn Right 135
Light blue | 7 | Turn Left 135 
White | 8 | Finish (return home)
Black | 9 | Maze wall colour

### iv. Storage of Path
Finally, two arrays will be used to record the path of the buggy - color_path and time_path. The former will store the id of the color (color_name) and the latter will store the time taken for the previous action to complete.

## 5. [Return Home](https://www.youtube.com/watch?v=iyFijjikkeM)
To return back to the entrance of the mine, the buggy will first spin around 180 degrees. Then, the program will iterate through the stored path, performing the opposite actions from the path it took. This makes use of the function motor_action_return().
