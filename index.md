ECE 4160

My name is Kofi Ohene Adu (kao65). This page contains the labs I have completed for this course. I am an electrical and computer engineering student, with an interest in robotics and languages. I like video games, playing the guitar, and pretty much like listening to music in general.
Welcome to my Fast Robots page! This is a work in progress!

# Lab 1: Introduction to Artemis 
## Part 1
The objective of this part of the lab is to test Artemis board functionality, by testing the LED, Temperature sensor, Serial output, and microphone.

### "Example 1: Blink it Up"
The “Blink it Up” example was used for this portion of the lab. The code, when compiled would turn the LED on for a specified amount of time, then would turn it off. Below is the result:

[![blink test](http://img.youtube.com/vi/sf7wSSynCig/0.jpg)](https://www.youtube.com/watch?v=sf7wSSynCig)

*Note: Will take you to the video itself*

### "Example 4: Serial"
This example found under Artemis examples, was used to test the serial monitor output. It would print out any messages that were typed in the input space, as seen in the image below.

![ex4_serial](assets/lab1/ex_4serial.png)

*Serial test Image*

### "Example 2: Analog Read"
This example  was used to test the temperature sensor on the Artemis board.  The test would send the temperature reading and time to the serial monitor and would slightly change the temperature number as I applied warm air. ( Change is tiny, about 0.2 from 33.7 to 33.9, likely due to the surrounding room temperature being warmer than usual). Below is the result

[![temp test](http://img.youtube.com/vi/-SnpcKFRFgw/0.jpg)](https://www.youtube.com/shorts/-SnpcKFRFgw)

*Note: Will take you to the video itself*

### "PDM/Example 1: Microphone Output"
The microphone output example was used to show the frequencies picked up by the Artemis board. There would be a slight change as I released a higher-pitched whistle in time and would decrease as I stopped. Below is an image and video of this:

![mic](assets/lab1/ex_1micout.png)

*Mic test Image*

[![mic test](http://img.youtube.com/vi/9HuIhQY1Gsg/0.jpg)](https://www.youtube.com/shorts/9HuIhQY1Gsg)

*Note: Will take you to the video itself*

## Part 2: Bluetooth Setup
The objective of this part of the lab was to establish a Bluetooth connection between the Artemis board and the computer and to indicate its functionality. In this section, we worked in tandem with both Arduino and Python (using Jupiter lab) to connect the Artemis through Bluetooth, then send data to the board over Bluetooth as well

### Prelab
We were tasked with installing the virtual environment that was to be used during the rest of the labs and installing the necessary packages for it. Next was to unzip the codebase to be used during this lab and the later ones also.
Next was to find and change the necessary MAC address and UUIDs for our individual Artemis boards. The change is to be made in the ble_arduino.ino  file in the ble codebase. Since many boards could share the same MAC address, also using their UUIDs is a recommended step. This change would be replicated in the connection.yaml file shown.

![mac address](assets/lab1/mac.png)

*Mac Address*

![uuid](assets/lab1/uuid.png)

*UUID*

![arduino connections](assets/lab1/connections_ard.png)
![connections](assets/lab1/connections.png)

*Connections in both Arduino and Python files*

### The Connect

![bleconnect](assets/lab1/connect.png)
*How ble connection is done*

### "ECHO" Command
An ECHO command was to be implemented to test the connection between the computer and the Artemis board. This command would send a character string to the board from the Python code, then when printed, it would send the phrase back to the Python code.

![echo](assets/lab1/echo.png)
*Echo Implementation*

![echoresult](assets/lab1/echoresult.png)

*Echo results*

*Note:* Whenever a new command was to be created it was to be added to the CommandTypes list. Each command would have an assigned value addressed to it and then we would need to add that command and its value to the cmdtypes file on Jupyter Notebook, as shown below.

![cmd_arduino](assets/lab1/cmdarduino.png)
![cmd](assets/lab1/cmd.png)

###  "GET_TIME_MILLIS" Command
This implementation involved receiving the current time from the Artemis board. This involved using the millis() function to get the current time in milliseconds and then convert it to an int. The result was presented as a string. The implementation is shown below.

![get_time_miliis](assets/lab1/get_time.png)
### GET_TIME Command

![get_time](assets/lab1/get_time2.png)

### Notification Handler
The notification handler is used to automatically receive the data sent by the Artemis board without manually calling the data each time.  For the code, I decided to use an array at each initialization of the code so that all the data would be put in an array, whether it was a single value or an assortment of them. This piece of Python code will be very helpful as we create multiple functions to collect data (including those containing arrays)

![notif_handler](assets/lab1/notif_handler.png)
Whenever the notification handler is being used, it is “told” to collect the data received by calling the start_notify() function as shown below. 

![start_notify](assets/lab1/gtmpy.png)
This is done only once for the handler to function. To stop the handler from collecting information, we just issue the stop_notify() command on the Python file 

![stop notif](assets/lab1/stpnotif.png)
### "SEND_TIME_DATA" Command
This command was used to get each time reading for 50 seconds. Similar to the GET_TIME_MILLIS command it would get the current time at each iteration of the loop and then put it in a time array to be received by the notification handler. 

![std_ard](assets/lab1/send_t_data.png)
![stdpython](assets/lab1/std.png)
### "GET_TEMP_READINGS" Command
This command involved extracting many temperature readings in the span of a few seconds. For this code, I created another array to hold the temperature reading and then appended each value in the array to its corresponding time value. The data was then sent as strings by a for loop to the Python code. This reduced the need to have another command  to send the data just like in the SEDN_TIME_DATA command

![temp_readings](assets/lab1/temp_readings.png)
![temp_readings_py](assets/lab1/temppy.png)

### A Limitation
The Artemis Board has a maximum storage of 384 kB of RAM. If we sampled 16-bit values at an average frequency every 50 seconds, we would be able to create about less than 30 values before the storage on the Artemis board would run out. Sending data in groups would thus optimize the speed of data acquisition from the Artemis board.



# Lab 2
### Setup the IMU
### Accelerometer
### Gyroscope 
### Sample Data
### Stunt























