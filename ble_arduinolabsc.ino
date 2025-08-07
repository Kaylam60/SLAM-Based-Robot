
#include "BLECStringCharacteristic.h"
#include "EString.h"
#include "RobotCommand.h"
#include <ArduinoBLE.h>
#include "ICM_20948.h" // Click here to get the library: http://librarymanager/All#SparkFun_ICM_20948_IMU
#include "math.h"
#include <Wire.h>
#include "SparkFun_VL53L1X.h"


#define SPI_PORT SPI // Your desired SPI port.       Used only when "USE_SPI" is defined
#define CS_PIN 2     // Which pin you connect CS to. Used only when "USE_SPI" is defined

#define WIRE_PORT Wire // Your desired Wire port.      Used when "USE_SPI" is not defined
// The value of the last bit of the I2C address.
// On the SparkFun 9DoF IMU breakout the default is 1, and when the ADR jumper is closed the value becomes 0
#define AD0_VAL 0

#ifdef USE_SPI
ICM_20948_SPI myICM; // If using SPI create an ICM_20948_SPI object
#else
ICM_20948_I2C myICM; // Otherwise create an ICM_20948_I2C object
#endif

#define SHUTDOWN_PIN 8
SFEVL53L1X distanceSensor1;
SFEVL53L1X distanceSensor2;
//////////// BLE UUIDs ////////////
#define BLE_UUID_TEST_SERVICE "df83d609-9e23-4e0f-98a0-d18a21c100d2"

#define BLE_UUID_RX_STRING "9750f60b-9c9c-4158-b620-02ec9521cd99"

#define BLE_UUID_TX_FLOAT "27616294-3063-4ecc-b60b-3470ddef2938"
#define BLE_UUID_TX_STRING "f235a225-6735-4d73-94cb-ee5dfce9ba83"

ICM_20948_fss_t myFSS;
//////////// BLE UUIDs ////////////

//////////// Global Variables ////////////
BLEService testService(BLE_UUID_TEST_SERVICE);

BLECStringCharacteristic rx_characteristic_string(BLE_UUID_RX_STRING, BLEWrite, MAX_MSG_SIZE);

BLEFloatCharacteristic tx_characteristic_float(BLE_UUID_TX_FLOAT, BLERead | BLENotify);
BLECStringCharacteristic tx_characteristic_string(BLE_UUID_TX_STRING, BLERead | BLENotify, MAX_MSG_SIZE);

// RX
RobotCommand robot_cmd(":|");

// TX
EString tx_estring_value;
float tx_float_value = 0.0;

long interval = 500;
static long previousMillis = 0;
unsigned long currentMillis = 0;
int motorPin1;
int motorPin2;
int speed;
//////////// Global Variables ////////////

double t_array[100];
double temp_array[100];
float pitch[500];
float roll[500];
float pitchg[500];
float rollg[500];
float yawg[500];
float dt=0;
float prev_time=0;
float filtered_pitch[500];
float filtered_roll[500];
const float alpha = 0.1; 
float pitch_f[500];
float roll_f[500];
float yaw_f[500];
int distance;
float distanceInches[500];
float distanceFeet[500];
int distance2;
float distanceInches2[500];
float distanceFeet2[500];
const float Kp=1.2;
const float Ki=0.0002;
const float Kd=0;
float err = 0;
static long prev_millis=0;
static long curr_millis=0;
float prev_error=0.0;
float error_sum=0.0;
int val=50;
int val2=500;
float angle=0;
const float Kpo=15;
const float Kio=5;
const float Kdo=6;
float t=0;
int val3=50;
int turns=0;
float acta=0;
bool orient_run=true;
bool get_distance_reading;
int cal=0;
bool lr=true;
int n=100;

enum CommandTypes
{
    PING,
    SEND_TWO_INTS,
    SEND_THREE_FLOATS,
    ECHO,
    DANCE,
    SET_VEL,
    GET_TIME_MILLIS,
    SEND_TIME_DATA,
    GET_TEMP_READINGS,
    GET_TIME,
    GET_ACC_P_R,
    GET_GYR_P_R_Y,
    ACCEL_LPF,
    GYR_CF,
    TOF,
    RUN,
    RUN_O,
    RR,
    STUNT,
    MAP,
    SC,
    SSC,
    MSC,

};

void
handle_command()
{   
    // Set the command string from the characteristic value
    robot_cmd.set_cmd_string(rx_characteristic_string.value(),
                             rx_characteristic_string.valueLength());

    bool success;
    int cmd_type = -1;

    // Get robot command type (an integer)
    /* NOTE: THIS SHOULD ALWAYS BE CALLED BEFORE get_next_value()
     * since it uses strtok internally (refer RobotCommand.h and 
     * https://www.cplusplus.com/reference/cstring/strtok/)
     */
    success = robot_cmd.get_command_type(cmd_type);

    // Check if the last tokenization was successful and return if failed
    if (!success) {
        return;
    }

    // Handle the command type accordingly
    switch (cmd_type) {
        /*
         * Write "PONG" on the GATT characteristic BLE_UUID_TX_STRING
         */
        case PING:
            tx_estring_value.clear();
            tx_estring_value.append("PONG");
            tx_characteristic_string.writeValue(tx_estring_value.c_str());

            Serial.print("Sent back: ");
            Serial.println(tx_estring_value.c_str());

            break;
        /*
         * Extract two integers from the command string
         */
        case SEND_TWO_INTS:
            int int_a, int_b;

            // Extract the next value from the command string as an integer
            success = robot_cmd.get_next_value(int_a);
            if (!success)
                return;

            // Extract the next value from the command string as an integer
            success = robot_cmd.get_next_value(int_b);
            if (!success)
                return;

            Serial.print("Two Integers: ");
            Serial.print(int_a);
            Serial.print(", ");
            Serial.println(int_b);
            
            break;
        /*
         * Extract three floats from the command string
         */
        case SEND_THREE_FLOATS:
            /*
             * Your code goes here.
             */

            break;
        /*
         * Add a prefix and postfix to the string value extracted from the command string
         */
        case ECHO:

            char char_arr[MAX_MSG_SIZE];

            // Extract the next value from the command string as a character array
            success = robot_cmd.get_next_value(char_arr);
            if (!success)
                return;

            /*
             * Your code goes here.
             */
            tx_estring_value.clear();
            tx_estring_value.append(char_arr);
            tx_characteristic_string.writeValue(tx_estring_value.c_str());
            Serial.print("Robot Says: ");
            Serial.println(tx_estring_value.c_str());
            
            break;
        /*
         * DANCE
         */
        case DANCE:
            Serial.println("Look Ma, I'm Dancin'!");

            break;
        
        /*
         * SET_VEL
         */
        case SET_VEL:

            break;

        case GET_TIME_MILLIS:
            currentMillis=millis();
            tx_estring_value.clear();
            tx_estring_value.append("T:");
            tx_estring_value.append((int)currentMillis);
            tx_characteristic_string.writeValue(tx_estring_value.c_str());
            break;
        
        case SEND_TIME_DATA:
        for (int i=0; i<100; i++){
              t_array[i]=millis();
            }
            for (int i=0; i<100; i++){
              tx_estring_value.clear();
              tx_estring_value.append("T:");
              tx_estring_value.append(t_array[i]);
              tx_characteristic_string.writeValue(tx_estring_value.c_str());
            }
            break;

        case GET_TEMP_READINGS:
            for(int i= 0; i<100; i++){
              t_array[i]=millis();
              temp_array[i]= analogReadTemp();      
              tx_estring_value.clear();
              tx_estring_value.append("T:");
              tx_estring_value.append((int)t_array[i]);
              tx_estring_value.append(",Temp:");
              tx_estring_value.append((float)temp_array[i]);
              tx_characteristic_string.writeValue(tx_estring_value.c_str());

            }
            break;
        
        case GET_TIME:
            long initial_time;
            initial_time=millis();
            currentMillis=millis();
            
            while (currentMillis-initial_time<5000){
              tx_estring_value.clear();
              tx_estring_value.append("T:");
              tx_estring_value.append((int)currentMillis);
              tx_characteristic_string.writeValue(tx_estring_value.c_str());
              currentMillis=millis();
            }
            break;

        case GET_ACC_P_R:
            for( int i=0; i<500; i++){
              if (!myICM.dataReady()) {
              }
              myICM.getAGMT();  
              pitch[i]=(atan2(myICM.accX(),myICM.accZ()))*180/M_PI;
              roll[i]=(atan2(myICM.accY(),myICM.accZ()))*180/M_PI;
              if( pitch[i]>90){
                pitch[i]=180-pitch[i];
              } else if (pitch[i]<-90){
                pitch[i]=-180-pitch[i];
              }
              if( roll[i]>90){
                roll[i]=180-roll[i];
              } else if (roll[i]<-90){
                roll[i]=-180-roll[i];
              }
              tx_estring_value.clear();
              tx_estring_value.append("Pitch:");
              tx_estring_value.append(pitch[i]);
              tx_estring_value.append(",Roll:");
              tx_estring_value.append(roll[i]);
              tx_characteristic_string.writeValue(tx_estring_value.c_str());
              //Serial.print("Pitch:");
              //Serial.print(pitch[i]);
              //Serial.print(",Roll:");
              //Serial.println(roll[i]);
            }
            break;

        case GET_GYR_P_R_Y:
            for (int i=0; i<500; i++){
               if (!myICM.dataReady()) {
              }

              myICM.getAGMT();
              dt = (millis()-prev_time)/1000;
              prev_time = millis();
              pitchg[i] = pitchg[i] + myICM.gyrY() * dt*180/M_PI;
              rollg[i] = rollg[i] + myICM.gyrX() * dt*180/M_PI;
              yawg[i]=yawg[i]+myICM.gyrZ()*dt*180/M_PI;
              if( pitchg[i]>90){
                pitchg[i]=180-pitchg[i];
              } else if (pitchg[i]<-90){
                pitchg[i]=-180-pitchg[i];
              }
              if( rollg[i]>90){
                rollg[i]=180-rollg[i];
              } else if (rollg[i]<-90){
                rollg[i]=-180-rollg[i];
              }
              if( yawg[i]>90){
                yawg[i]=180-yawg[i];
              } else if (yawg[i]<-90){
                yawg[i]=-180-yawg[i];
              }
              tx_estring_value.clear();
              tx_estring_value.append("Pitch:");
              tx_estring_value.append(pitchg[i]);
              tx_estring_value.append(",Roll:");
              tx_estring_value.append(rollg[i]);
              tx_estring_value.append(",Yaw:");
              tx_estring_value.append(yawg[i]);
              tx_characteristic_string.writeValue(tx_estring_value.c_str());
              //Serial.print("Pitch:");
              //Serial.print(pitchg[i]);
              //Serial.print(",Roll:");
              //Serial.print(rollg[i]);
              //Serial.print(",Yaw:");
              //Serial.println(yawg[i]);
            }
          break;
        case ACCEL_LPF:
            for (int i = 0; i < 500; i++) {
              filtered_pitch[i] = alpha * pitch[i] + (1 - alpha) * filtered_pitch[i - 1];
              filtered_roll[i] = alpha * roll[i] + (1 - alpha) * filtered_roll[i - 1];
              tx_estring_value.clear();
              tx_estring_value.append("Filtered Pitch:");
              tx_estring_value.append(filtered_pitch[i]);
              tx_estring_value.append(",Filtered Roll:");
              tx_estring_value.append(filtered_roll[i]);
              tx_characteristic_string.writeValue(tx_estring_value.c_str());
              //Serial.print("Pitch:");
              //Serial.print(filtered_pitch[i]);
              //Serial.print(",Roll:");
              //Serial.println(filtered_roll[i]);
            }
            break;
        case GYR_CF:
            for (int i=0; i<500; i++){
              pitch_f[i]=(pitch_f[i]+pitchg[i]*dt)*(1-alpha) +pitch[i]*alpha;
              roll_f[i]=(roll_f[i]+rollg[i]*dt)*(1-alpha) +roll[i]*alpha;
              tx_estring_value.clear();
              tx_estring_value.append("Filtered Pitch:");
              tx_estring_value.append(pitch_f[i]);
              tx_estring_value.append(",Filtered Roll:");
              tx_estring_value.append(roll_f[i]);
              tx_characteristic_string.writeValue(tx_estring_value.c_str());
              //Serial.print("Pitch:");
              //Serial.print(pitch_f[i]);
              //Serial.print(",Roll:");
              //Serial.println(roll_f[i]);
            }
            break;

        case TOF:
            for(int i=0; i<500 ; i++){
                distanceSensor1.startRanging(); //Write configuration bytes to initiate measurement
            while (!distanceSensor1.checkForDataReady()){
              delay(1);
            }
            distance = distanceSensor1.getDistance(); //Get the result of the measurement from the sensor
            distanceSensor1.clearInterrupt();
            distanceSensor1.stopRanging();

            Serial.print("Distance(mm): ");
            Serial.print(distance);

            distanceInches[i] = distance * 0.0393701;
            distanceFeet[i] = distanceInches[i] / 12.0;

            Serial.print("\tDistance(ft): ");
            Serial.print(distanceFeet[i], 2);
            tx_estring_value.clear();
            tx_estring_value.append("Distance(mm): ");
            tx_estring_value.append(distance);
            //tx_estring_value.append(",tDistance(ft): ");
            //tx_estring_value.append(distanceFeet[i]);
            tx_characteristic_string.writeValue(tx_estring_value.c_str());

  
            distanceSensor2.startRanging(); //Write configuration bytes to initiate measurement
            while (!distanceSensor2.checkForDataReady())
            {
              delay(1);
            }
            distance2 = distanceSensor2.getDistance(); //Get the result of the measurement from the sensor
            distanceSensor2.clearInterrupt();
            distanceSensor2.stopRanging();

            Serial.print("\tDistance2(mm): ");
            Serial.print(distance2);

             distanceInches2[i] = distance2 * 0.0393701;
            distanceFeet2[i] = distanceInches2[i] / 12.0;

            tx_estring_value.append(",Distance2(mm): ");
            tx_estring_value.append(distance2);
            //tx_estring_value.append(",tDistance(ft): ");
            //tx_estring_value.append(distanceFeet2[i]);
            tx_characteristic_string.writeValue(tx_estring_value.c_str());
            Serial.print("\tDistance2(ft): ");
            Serial.print(distanceFeet2[i], 2);

            Serial.println();
            }
            
            break;
        
        case RUN:
              for(int i=0;i<val+1;i--){
                if(val>0){
                  PID(300);
                 
                }else{
                  Spin(motorPin1,motorPin2,0);
                }
                //Serial.println(angle);
                val-=1;
              }
              
            break;

        case RUN_O:
              angle=0;
              for(int i=0;i<val2+1;i--){
                if(val2>0){
                  
                  Orient(0);
                }else{
                  Spin(motorPin1,motorPin2,0);
                }
                //Serial.println(angle);
                val2-=1;
              }
              
            break;
        case RR:
              for(int i=0;i<val3+1;i++){
                if(val3>0){
                  curr_millis=millis();
                  t=(float)curr_millis/1000;
                  distanceSensor1.startRanging();
                  while (!distanceSensor1.checkForDataReady()){
                    delay(1);
                  }
                  distance = distanceSensor1.getDistance(); //Get the result of the measurement from the sensor
                  distanceSensor1.clearInterrupt();
                  distanceSensor1.stopRanging();  
                  motorPin1=6;
                  motorPin2= 12;
                  if(distance<10){
                    Spin(motorPin1,motorPin2,0);
                  }else{
                    Spin(motorPin1,motorPin2,80);
                  }       
                }else{
                  motorPin1=6;
                  motorPin2= 12;
                  Spin(motorPin1,motorPin2,0);
                }
                val3-=1;
                Serial.print("Distance:");
                Serial.print(distance);
                Serial.print(", Time:");
                Serial.println(t);
                tx_estring_value.clear();
                tx_estring_value.append("Time:");
                tx_estring_value.append(t);
                tx_estring_value.append(",distance:");
                tx_estring_value.append(distance);
                tx_characteristic_string.writeValue(tx_estring_value.c_str());
              }
              Serial.println("Command Done");
              break;
        case STUNT:
              for(int i=0;i<50+1;i++){
                
                distanceSensor1.startRanging();
                while (!distanceSensor1.checkForDataReady()){
                    delay(1);
                }
                distance = distanceSensor1.getDistance(); //Get the result of the measurement from the sensor
                distanceSensor1.clearInterrupt();
                distanceSensor1.stopRanging();
                //motorPin1=6;
                //motorPin2=12;
                if(distance<700){
                  angle=0; 
                  Orient(190);
                } else{
                Spin(motorPin1,motorPin2, 0);
                motorPin1=6;
                motorPin2=12;
                
                }
              Spin(motorPin1,motorPin2, 150);
              Serial.print("Distance:");
              Serial.print(distance);
              Serial.print(",pins:");
              Serial.print(motorPin1);
              Serial.println(motorPin2);
              tx_estring_value.clear();
              tx_estring_value.append("distance:");
              tx_estring_value.append(distance);
              tx_estring_value.append(", motorpin1:");
              tx_estring_value.append(motorPin1);
              tx_estring_value.append(", motorpin2:");
              tx_estring_value.append(motorPin2);
              tx_characteristic_string.writeValue(tx_estring_value.c_str());
              }
              Spin( motorPin1, motorPin2,0);
              break;


        case MAP:
        Serial.print("got this!");
              curr_millis=millis();
              prev_millis=curr_millis;
              while (curr_millis<prev_millis+5000 && turns<18 ){
                curr_millis=millis();
                angle=0;
                orient_run=true;
                while (orient_run ){
                  curr_millis=millis();
                  get_distance_reading=false;
                  Orient(20);
                  if (angle<20.5 && angle > 20){
                    orient_run = false;
                  }
                }

                Serial.println("Orient done");
                Spin(motorPin1, motorPin2, 0);
                
                get_distance_reading=true;
                while (get_distance_reading){
                    curr_millis=millis();
                    distanceSensor1.startRanging();
                    while (!distanceSensor1.checkForDataReady()){
                      delay(1);
                     }
                    distance = distanceSensor1.getDistance(); //Get the result of the measurement from the sensor
                    distanceSensor1.clearInterrupt();
                    distanceSensor1.stopRanging();
                    //Sensor 2
                    distanceSensor2.startRanging();
                    while (!distanceSensor2.checkForDataReady()){
                     delay(1);
                     }
                    distance2 = distanceSensor2.getDistance(); //Get the result of the measurement from the sensor
                    distanceSensor2.clearInterrupt();
                    distanceSensor2.stopRanging();
                    //orient_run = true;
                    get_distance_reading=false;
                }
                acta+=angle;
                turns++;
                
                Serial.print(orient_run);
                Serial.print("angle: ");
                Serial.print(angle);
                Serial.print(", Distance1:");
                Serial.print(distance);
                Serial.print(", Distance2:");
                Serial.println(distance2);
                tx_estring_value.clear();
                tx_estring_value.append("Time:");
                tx_estring_value.append((float)curr_millis/1000);
                tx_estring_value.append(", Distance1:");
                tx_estring_value.append((float)distance);
                tx_estring_value.append(", Distance2:");
                tx_estring_value.append((float)distance2);
                tx_estring_value.append(", angle:");
                tx_estring_value.append(angle);
                tx_estring_value.append(", actual:");
                tx_estring_value.append(acta);
                tx_characteristic_string.writeValue(tx_estring_value.c_str());
               
              }
              Spin(motorPin1, motorPin2, 0);
              break;

        case SC:
              curr_millis=millis();
              prev_millis=curr_millis;
              for(int i=0;i<500+1;i++){
                curr_millis=millis();
                //angle=0;
                //Orient(0);
                distanceSensor1.startRanging();
                while (!distanceSensor1.checkForDataReady()){
                    delay(1);
                }
                distance = distanceSensor1.getDistance(); //Get the result of the measurement from the sensor
                distanceSensor1.clearInterrupt();
                distanceSensor1.stopRanging();
                //motorPin1=6;
                //motorPin2=12;
                if(distance<500){
                  angle=0; 
                  while(lr==true){
                    Orient(100);
                    lr=false;
                  }
                  if (orient_run){
                    PID(10);
                    orient_run=false;
                  }
                  while(lr==false){
                    Orient(-100);
                    lr=true;
                  }
                  while (curr_millis>prev_millis+7000 && orient_run==false && curr_millis<prev_millis+8000){
                    Orient(360);
                    
                  }
                  orient_run=true;
                } else{
                Spin(motorPin1,motorPin2, 0);
                motorPin1=6;
                motorPin2=12;
                
                }
              Spin(motorPin1,motorPin2, 200);
              Serial.print("Distance:");
              Serial.print(distance);
              Serial.print(", angle");
              Serial.print(angle);
              Serial.print(", lr:");
              Serial.println(lr);

              tx_estring_value.clear();
              tx_estring_value.append("distance:");
              tx_estring_value.append(distance);
              tx_estring_value.append(", angle:");
              tx_estring_value.append(angle);
              tx_characteristic_string.writeValue(tx_estring_value.c_str());
              }
              Spin( motorPin1, motorPin2,0);
              break;
        case SSC:
              curr_millis=millis();
              prev_millis=curr_millis;
              for(int i=0;i<500+1;i++){
                curr_millis=millis();
                //angle=0;
                //Orient(0);
                distanceSensor1.startRanging();
                while (!distanceSensor1.checkForDataReady()){
                    delay(1);
                }
                distance = distanceSensor1.getDistance(); //Get the result of the measurement from the sensor
                distanceSensor1.clearInterrupt();
                distanceSensor1.stopRanging();
                //motorPin1=6;
                //motorPin2=12;
                if(distance<500){
                  angle=0;
                  Orient(190);
                  // if (orient_run){
                  //   PID(300);
                  //   orient_run=false;
                  // }
                  // while(lr==false){
                  //   Orient(-100);
                  //   lr=true;
                  // }
                  if (curr_millis>prev_millis+7000 && orient_run==false && curr_millis<prev_millis+8000){
                    Orient(360);
                    
                  }
                  orient_run=true;
                } else{
                Spin(motorPin1,motorPin2, 0);
                motorPin1=6;
                motorPin2=12;
                
                }
              Spin(motorPin1,motorPin2, 255);
              Serial.print("Distance:");
              Serial.print(distance);
              Serial.print(", angle");
              Serial.print(angle);
              Serial.print(", lr:");
              Serial.println(lr);

              tx_estring_value.clear();
              tx_estring_value.append("distance:");
              tx_estring_value.append(distance);
              tx_estring_value.append(", angle:");
              tx_estring_value.append(angle);
              tx_characteristic_string.writeValue(tx_estring_value.c_str());
              }
              Spin( motorPin1, motorPin2,0);
              break;

        case MSC:
              Serial.print("got this!");
              curr_millis=millis();
              prev_millis=curr_millis;
              while (curr_millis<prev_millis+5000 && turns<18 ){
                curr_millis=millis();
                angle=0;
                orient_run=true;
                while (orient_run ){
                  curr_millis=millis();
                  get_distance_reading=false;
                  Orient(20);
                  if (angle<19.5 && angle > 20){
                    orient_run = false;
                  }
                }

                Serial.println("Orient done");
                Spin(motorPin1, motorPin2, 0);
                
                get_distance_reading=true;
                while (get_distance_reading){
                    curr_millis=millis();
                    distanceSensor1.startRanging();
                    while (!distanceSensor1.checkForDataReady()){
                      delay(1);
                     }
                    distance = distanceSensor1.getDistance(); //Get the result of the measurement from the sensor
                    distanceSensor1.clearInterrupt();
                    distanceSensor1.stopRanging();
                    //Sensor 2
                    distanceSensor2.startRanging();
                    while (!distanceSensor2.checkForDataReady()){
                     delay(1);
                     }
                    distance2 = distanceSensor2.getDistance(); //Get the result of the measurement from the sensor
                    distanceSensor2.clearInterrupt();
                    distanceSensor2.stopRanging();
                    //orient_run = true;
                    get_distance_reading=false;
                }
                acta+=angle;
                turns++;
                
                Serial.print(orient_run);
                Serial.print("angle: ");
                Serial.print(angle);
                Serial.print(", Distance1:");
                Serial.print(distance);
                Serial.print(", Distance2:");
                Serial.println(distance2);
                tx_estring_value.clear();
                tx_estring_value.append("Time:");
                tx_estring_value.append((float)curr_millis/1000);
                tx_estring_value.append(", Distance1:");
                tx_estring_value.append((float)distance);
                tx_estring_value.append(", Distance2:");
                tx_estring_value.append((float)distance2);
                tx_estring_value.append(", angle:");
                tx_estring_value.append(angle);
                tx_estring_value.append(", actual:");
                tx_estring_value.append(acta);
                tx_characteristic_string.writeValue(tx_estring_value.c_str());
               
              }
              Spin(motorPin1, motorPin2, 0);
              break;
        /* 
         * The default case may not capture all types of invalid commands.
         * It is safer to validate the command string on the central device (in python)
         * before writing to the characteristic.
         */
        default:
            Serial.print("Invalid Command Type: ");
            Serial.println(cmd_type);
            break;
    }
}
void Spin(int motorPin1,int motorPin2,int speed){
  analogWrite(motorPin1, speed+20);
  analogWrite(motorPin2, speed+10);
}

void PID(int target){
  curr_millis=millis();
  dt=(float)(curr_millis-prev_millis)/1000;
  if(dt==0.00){
    dt=0.01;
  }
  distanceSensor1.startRanging();
  while (!distanceSensor1.checkForDataReady()){
              delay(1);
            }
  distance = distanceSensor1.getDistance(); //Get the result of the measurement from the sensor
  distanceSensor1.clearInterrupt();
  distanceSensor1.stopRanging();
  
  err=distance-target;
  float P=Kp*err;
  error_sum=error_sum+err*dt;
  float I=Ki*error_sum;
  float D=Kd*(err-prev_error)/dt;
  float PID= P + I + D;
  prev_millis=millis();
  prev_error=err;
  speed=PID;
  if(PID<0){
    Spin(motorPin1,motorPin2,0);
    speed=-PID;
    motorPin1=7;
    motorPin2=11;
  }else{
      Spin(motorPin1,motorPin2,0);
      motorPin1=6;
      motorPin2=12;
    }
    if (PID=0){
      speed=0;
    }
    if(speed>255){
      speed=255;
    }
  Spin(motorPin1,motorPin2,speed);

  // Serial.print("Distance:");
  // Serial.print(distance);
  // Serial.print(", error:");
  // Serial.print(err);
  // Serial.print(", PID:");
  // Serial.print(PID);
  // Serial.print(", dt:");
  // Serial.print(dt);
  //tx_estring_value.clear();
  //tx_estring_value.append("Time:");
  //tx_estring_value.append((float)curr_millis);
  //tx_estring_value.append(",distance:");
  //tx_estring_value.append(distance);
  //tx_estring_value.append(",PID:");
  //tx_estring_value.append(PID);
  //tx_estring_value.append(",speed:");
  //tx_estring_value.append(speed);
  //tx_characteristic_string.writeValue(tx_estring_value.c_str());
  // Serial.print(", MOTOR_PINS:");
  // Serial.print(motorPin1);
  // Serial.print(motorPin2);
  // Serial.print(", Speed:");
  // Serial.println(speed);
  
}

void Orient(int a){
  curr_millis=millis();
  dt=(float)(curr_millis-prev_millis)/1000;
  if(dt==0.00|dt>0.03){
    dt=0.01;
  }
  if (!myICM.dataReady()) {
              }
  myICM.getAGMT();  
  angle=angle+myICM.gyrZ()*dt;
  err=a-angle;
  float P=Kpo*err;
  error_sum=error_sum+err*dt;
  float I=Kio*error_sum;
  float D=Kdo*(err-prev_error)/dt;
  float PID= P + I + D;
  prev_millis=curr_millis;
  prev_error=err;
  speed=PID;
  int counter=0;
  if(PID<0){
    Spin(motorPin1,motorPin2,0);
    speed=-PID;
    motorPin1=6;
    motorPin2=11;
  }else{
      Spin(motorPin1,motorPin2,0);
      motorPin1=7;
      motorPin2=12;
    }
    if (PID=0){
      if(err!=0||(counter!=5&&counter>0)){
        counter+=1;
      }else{
        counter=0;
        
      }
      if(counter==0){
        speed=0;
      }
    }
    if(speed>225){
      speed=225;
    }
  Spin(motorPin1,motorPin2,speed);
  //tx_estring_value.clear();
  //tx_estring_value.append("Time:");
  //tx_estring_value.append((float)curr_millis);
  //tx_estring_value.append(",yaw:");
  //tx_estring_value.append(angle);
  //tx_estring_value.append(",PID:");
  //tx_estring_value.append(P);
  //tx_estring_value.append(",speed:");
  //tx_estring_value.append(speed);
  //tx_characteristic_string.writeValue(tx_estring_value.c_str());
  //Serial.print("Raw Yaw:");
  //Serial.print(myICM.gyrZ());
  //Serial.print("angle: ");
  //Serial.print(angle);
  //Serial.print(", error:");
  //Serial.print(err);
  //Serial.print(", P:");
  //Serial.print(P);
  //Serial.print(", I:");
  //Serial.print(I);
  //Serial.print(", D:");
  //Serial.print(D);
  //Serial.print(", dt:");
  //Serial.print(dt);
  //Serial.print(", MOTOR_PINS:");
  //Serial.print(motorPin1);
  //Serial.print(motorPin2);
  //Serial.print(", Speed:");
  //Serial.println(speed);
}

void
setup()
{
    Serial.begin(115200);

    BLE.begin();

    // Set advertised local name and service
    BLE.setDeviceName("Artemis BLE");
    BLE.setLocalName("Artemis BLE");
    BLE.setAdvertisedService(testService);

    // Add BLE characteristics
    testService.addCharacteristic(tx_characteristic_float);
    testService.addCharacteristic(tx_characteristic_string);
    testService.addCharacteristic(rx_characteristic_string);

    // Add BLE service
    BLE.addService(testService);

    // Initial values for characteristics
    // Set initial values to prevent errors when reading for the first time on central devices
    tx_characteristic_float.writeValue(0.0);

    /*
     * An example using the EString
     */
    // Clear the contents of the EString before using it
    tx_estring_value.clear();

    // Append the string literal "[->"
    tx_estring_value.append("[->");

    // Append the float value
    tx_estring_value.append(9.0);

    // Append the string literal "<-]"
    tx_estring_value.append("<-]");

    // Write the value to the characteristic
    tx_characteristic_string.writeValue(tx_estring_value.c_str());

    // Output MAC Address
    Serial.print("Advertising BLE with MAC: ");
    Serial.println(BLE.address());

    BLE.advertise();
  
#ifdef USE_SPI
  SPI_PORT.begin();
#else
  WIRE_PORT.begin();
  WIRE_PORT.setClock(400000);
#endif

  //myICM.enableDebugging(); // Uncomment this line to enable helpful debug messages on Serial

  bool initialized = false;
  while (!initialized)
  {

#ifdef USE_SPI
    myICM.begin(CS_PIN, SPI_PORT);
#else
    myICM.begin(WIRE_PORT, AD0_VAL);
#endif

    //Serial.print(F("Initialization of the sensor returned: "));
    //Serial.println(myICM.statusString());
    if (myICM.status != ICM_20948_Stat_Ok)
    {
      Serial.println("Trying again...");
      delay(500);
    }
    else
    {
      initialized = true;
    }
  }
    Wire.begin();

  
    Serial.println("VL53L1X Qwiic Test");

    pinMode(SHUTDOWN_PIN, OUTPUT);
    digitalWrite(SHUTDOWN_PIN,LOW);
    distanceSensor2.setI2CAddress(0x2); //Set address for 2nd sensor
    digitalWrite(SHUTDOWN_PIN,HIGH);

    if (distanceSensor1.begin() != 0)  //Begin returns 0 on a good init
    {
      Serial.println("Sensor 1 failed to begin. Please check wiring. Freezing...");
      while (1)
        ;
    }
    Serial.println("Sensor 1 online!");  
  

    if (distanceSensor2.begin() != 0)  //Begin returns 0 on a good init
    {
      Serial.println("Sensor 2 failed to begin. Please check wiring. Freezing...");
      while (1)
        ;
   }
    Serial.println("Sensor 2 online!");

    //pinMode(motorPin1, OUTPUT);
    //pinMode(motorPin2, OUTPUT);
     myFSS.g = dps500; // (ICM_20948_GYRO_CONFIG_1_FS_SEL_e)
                    // dps250
                    // dps500
                    // dps1000
                    // dps2000

    myICM.setFullScale((ICM_20948_Internal_Gyr), myFSS);
    if (myICM.status != ICM_20948_Stat_Ok)
    {
      Serial.println("Set status!");
      Serial.print(F("setFullScale returned: "));
      Serial.println(myICM.statusString());
    }
}

void
write_data()
{
    currentMillis = millis();
    if (currentMillis - previousMillis > interval) {

        tx_float_value = tx_float_value + 0.5;
        tx_characteristic_float.writeValue(tx_float_value);

        if (tx_float_value > 10000) {
            tx_float_value = 0;
            
        }

        previousMillis = currentMillis;
    }
}

void
read_data()
{
    // Query if the characteristic value has been written by another BLE device
    if (rx_characteristic_string.written()) {
        handle_command();
    }
}

void
loop()
{
    // Listen for connections
    BLEDevice central = BLE.central();

    // If a central is connected to the peripheral
    if (central) {
        Serial.print("Connected to: ");
        Serial.println(central.address());

        // While central is connected
        while (central.connected()) {
            // Send data
            write_data();
            // Read data
            read_data();  
          }
    
        Serial.println("Disconnected");
    }
    
}

