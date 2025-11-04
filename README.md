# Robotics 3nd homework
Light following arduino project
# Problem
This project on a larger and more sensitive scale could be adapted to be used for navigation towards a light, for example when in a cave it could lead to where the outside is visible, or it can be used to make a robot that follows light. It can also be used to track the sun's cycle
# Design
The design of this project is comprised of two main parts, tracking and sensitivity, the first part is the light tracking itself, the arduino checks every millisecond if the light values have changed and based on the light values it either rotates the servo clockwise or counterclockwise. The second part is the sensitivity, because the photoresistors are not that accurate there needs to be a certain threshold which the difference between the two photoresistor values has to pass, so we also have the ability to change the sensitivity to get the best results. The current sensitivity gets saved to the EEPROM every second, if it has changed.

# Parts List
| Component        | Quantity |
|------------------|----------|
| Arduino Uno      | 1        |
| Servo            | 1        |
| Push button      | 1        |
| Red LED          | 2        |
| 220 Ohm resistor | 2        |
| 10k Ohm resistor | 3        |
| Photoresistor    | 2        |

# Design and schematics

## Design
<img width="1925" height="1042" alt="Mighty Amur" src="https://github.com/user-attachments/assets/9799f8ab-d643-4ec2-8f66-dbad9ed2bed3" />

## Schematics
<img width="2592" height="2003" alt="Screenshot 2025-11-04 at 11-29-51 Circuit design Mighty Amur - Tinkercad" src="https://github.com/user-attachments/assets/5d76c293-d40e-4bae-aab8-09f46396d871" />

# Demo video

# Encountered problems
- Servo has only a range 150 degrees, where I thought it would have 180, which messed up some of my earlier design
- Photoresistors not giving real values, the grounding on them was incorrect at first which caused random values to be sent to the arduino instead of the real ones
- PWM, servo library and timer interrupt conflicts, because PWM uses different timers for different pins, and the Servo library uses Timer1 for it's calculations some of the pins were unsuitable because they would either make a conflict between timer interrupts or the Servo library.
# Future improvements
- Add LCD display to show the sensitivity more accurately
- Mount the servo to a stationary object
- Add a dc motor and tires to make it into a light following robot
