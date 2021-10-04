# kn-helium-scale
Intern project - Helium based weight scale
## Table of Contents
* [Welcome](#welcome)
* [Getting Started](#getting-started)
* [Technologies](#technologies)
* [Credits](#credits)


## Welcome
Welcome to the Helium Scale project, a scale that runs on the growing Helium Network! This scale obtains data through load cells and sends it over
to the Helium Network through the Helium Console Application. This data is then integrated over to a Tago.io Dashboard.
Developed during an internship with Parley Labs.

## Getting Started
To get started on setting up the overall project, please follow the Hackster.io link for the complete instructions:
https://www.hackster.io/422272/lorawan-scale-b5e2c4

Please follow https://learn.sparkfun.com/tutorials/qwiic-scale-hookup-guide#introduction for the complete hardware setup of the scale.

This repository includes the complete and functional code for obtaining scale data along with the code to 
transmit the data. Additionally, a decoder file has been provided.

## Technologies
This projects uses the following:

Arduino IDE https://www.arduino.cc/en/software

Additionally, a very important library that will be used is the one developed by SparkFun. To add this library to your sketch:

1. Open up the Arduino IDE
2. Find Sketch in the top left and hover over Include Library
3. Click on Manage Libraries 
4. Search for: SparkFun Qwiic Scale NAU7802 Arduino Library and install the latest version

Helium Console https://console.helium.com 

Tago.io https://tago.io

For this project, we are using the Heltec CubeCell HTCC AB01 board. For additional board set up please follow guide provided by Helium:

https://docs.helium.com/use-the-network/devices/development/heltec/cubecell-dev-board/arduino/


## Running the Code
The decoder.js file is the decoder to be used in the Helium Console application. More information on how functions work on Console: https://docs.helium.com/use-the-network/console/functions/

The payload_parser.js file is the parser that Tago.io uses for data display

To run the code provided in the Arduino IDE you must first upload the file onto the board, which can be done by clicking on the arrow in the top left corner or by shortcutting cntrl + shift + u

The code provided also allows for the user to monitor the weight that is obtained in real time through the serial monitor. To display an organized list of data through the serial monitor:

1. upload the code to the board
2. ctrl + shift + m to display serial monitor

## Credits
Special thanks to Bryan for mentoring and guiding throughout the process of the project's development!




