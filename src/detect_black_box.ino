
//import relevant libraries, for this project, we used the ENES100 library
#include <Enes100.h>
#include <DFRTank.h>
#include <SoftwareSerial.h>


int trigPin = 12;    // Trigger
int echoPin = 11;    // Echo
int trigPin2 = 3;
int echoPin2 = 2;
long duration, cm, duration2, cm2;
#define IRpin_PIN  PIND
#define IRpin 10
#define MAXPULSE 65000
#define RESOLUTION 20 

boolean obstaclecheck = false;
uint16_t pulses[100][2];  // pair is high and low pulse 
uint8_t currentpulse = 0; // index for pulses we're storing
 
SoftwareSerial mySerial(8, 9);
Enes100 enes("Outside the Box", BLACK_BOX, 11, 8, 9);
DFRTank tank;

void FaceForward();
void MoveForwardStart();
void MoveForwardAfter();
int myDistanceSensor();
int readInfraredSensor();


#define abs(x) ((x)>0?(x):-(x))

    void setup() {
        //SoftwareSerial()
        //Serial port begins
        Serial.begin(9600);
        tank.init();
        //Define inputs and outputs
        pinMode(trigPin, OUTPUT);
        pinMode(echoPin, INPUT);
        pinMode(trigPin2, OUTPUT);
        pinMode(echoPin2, INPUT);
        enes.println("Starting Navigation");
        enes.println("Ready to decode IR!");
        enes.println("Ultrasonic sensors also ready to go!");
        while (!enes.updateLocation());
      }

      void loop() {
          // put your main code here, to run repeatedly:
          enes.println("Run FaceForward");
          tank.setLeftMotorPWM(-200);
          tank.setRightMotorPWM(-200);
          delay(500);
          FaceForward();
          
          tank.turnOffMotors();
          enes.updateLocation();
              while(enes.location.x<1.2) {
            //enes.println("Facing forward now!");
             MoveForwardStart();
              while(!enes.updateLocation()) {
                enes.println("Waiting for VS to update location....");
                  }
                }

            enes.updateLocation();
          while(enes.location.x>1.2) {
            tank.setLeftMotorPWM(-255);
            tank.setRightMotorPWM(-255);
            delay(2000);
           boolean check = myDistanceSensor();
           enes.updateLocation();
           tank.turnOffMotors();
           if(check == false){
              MoveForwardAfter();
              enes.updateLocation();
           }
           else {
            enes.updateLocation();
            if(enes.location.y<=1){
              TurnLeft();
              tank.setLeftMotorPWM(-255);
              tank.setRightMotorPWM(-255);
              delay(1000);
              FaceForward();
            }

            if(enes.location.y>1){
              TurnRight();
              tank.setLeftMotorPWM(-255);
              tank.setRightMotorPWM(-255);
              delay(1000);
              FaceForward();
            }
           }
           //search for Infrared pulse from Black Box
            int sensorpulse = readInfraredSensor();
            /*while(sensorpulse==0){
              int tempsensorpulse = readInfraredSensor();
              tank.setLeftMotorPWM(255);
              tank.setRightMotorPWM(-255);
               delay(800);
        tank.setLeftMotorPWM(-255);
        tank.setRightMotorPWM(255);
        delay(800);
        sensorpulse  = tempsensorpulse;
      }*/
      //continue action as long as IR pulse continues to amplify
      while (sensorpulse!= 0){
        Serial.println(sensorpulse);
        tank.setLeftMotorPWM(-255);
        tank.setRightMotorPWM(-255);
        delay(200);
        enes.updateLocation();
        enes.println("Black Box is too far away!");
        while(enes.location.x>2){
          tank.setLeftMotorPWM(-255);
        tank.setRightMotorPWM(-255);
        delay(1000);
        int tempsensorpulse = readInfraredSensor();

      a:  if(tempsensorpulse >=1250){
          Serial.println(currentpulse);
          enes.println("Black Box detected!");
          tank.turnOffMotors();
          Coordinate blackBox(enes.location.x, enes.location.y);
          enes.baseObjective(blackBox);
          enes.navigated();
          enes.endMission();
        tank.turnOffMotors();
        break;
        }
        else {

          enes.println("Black box not close by.. Have to go closer to transmit coordinates");
          tank.setLeftMotorPWM(255);
          tank.setRightMotorPWM(-255);
          delay(1000);
          tank.turnOffMotors();
          tempsensorpulse = readInfraredSensor();
          if(tempsensorpulse>=1250){
            goto a;
          }
          else {
           tank.setLeftMotorPWM(-255);
        tank.setRightMotorPWM(-255);
        delay(1000);
        tank.turnOffMotors();
        while(!enes.updateLocation());
          }
          tank.setLeftMotorPWM(-255);
          tank.setRightMotorPWM(255);
          delay(1000);
          tank.turnOffMotors();
          tempsensorpulse = readInfraredSensor();
          enes.println("Trying to gather Infrared signal while turning...");
          if(tempsensorpulse>=1250){
            goto a;
          }
        }
       }
     }
    }
  }

       //Function to face the right direction before starting motion
       //to account for different orientations
       void FaceForward() {
        //turn to face forward
        enes.println("Running FaceForward");
        enes.updateLocation();
        float thetaTemp = enes.location.theta;
        while (abs(thetaTemp) >0.11) {
          enes.println("Trying to face forward");
          tank.setLeftMotorPWM(-255);
          tank.setRightMotorPWM(255);
          //delay(500);
          //tank.turnOffMotors();
          enes.updateLocation();
          thetaTemp = enes.location.theta;
        }
      }

    //Function to start motion of OSV forward
    void MoveForwardStart() {
        //move forward
        enes.println("Reached moveforward start!");
        tank.setLeftMotorPWM(-255);
        tank.setRightMotorPWM(-255);
        delay(1000);
        tank.turnOffMotors();
        while(!enes.updateLocation());
     }

   //Function to turn the OSV's motors left
   void TurnLeft() {
        //turn left in current position
          tank.turnOffMotors();
          enes.println("Obstacle detected!.. Trying to go past it..");
        
                  while((enes.location.theta)<=1.55) {
      
                          tank.setLeftMotorPWM(255);
                          tank.setRightMotorPWM(-255);
                          delay(500);
                          tank.turnOffMotors();
                          while(!enes.updateLocation());
                   }
    }

    //Function to turn the OSV's motors left
    void TurnRight() {
      //turn right in current position
          tank.turnOffMotors();
          enes.println("Obstacle detected!.. Trying to go past it..");
          
                while((enes.location.theta)>= -1.55){
      
                tank.setLeftMotorPWM(-255);
                tank.setRightMotorPWM(255);
                delay(500);
                 tank.turnOffMotors();
                while(!enes.updateLocation());
               
              }
    }

    //Function to move forward after obstacles are detected
    void MoveForwardAfter() {
        //move forward after detecting obstacles
          delay(1000);
        //move forward
        tank.setLeftMotorPWM(-255);
        tank.setRightMotorPWM(-255);
        delay(500);
        tank.turnOffMotors();
        while(!enes.updateLocation());
    }


   //Function to interface with Ultrasonic sensors
   int myDistanceSensor() {
    // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
    // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
        enes.println("Reached my DIstance sensor!");
        digitalWrite(trigPin, LOW);
        delayMicroseconds(5);
        digitalWrite(trigPin, HIGH);
        delayMicroseconds(10);
        digitalWrite(trigPin, LOW);
        
        pinMode(echoPin, INPUT);
        duration = pulseIn(echoPin, HIGH);
        
        digitalWrite(trigPin2, LOW);
        delayMicroseconds(5);
        digitalWrite(trigPin2, HIGH);
        delayMicroseconds(10);
        digitalWrite(trigPin2, LOW);
 
  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
      
        pinMode(echoPin2, INPUT);
        duration2 = pulseIn(echoPin2, HIGH);
  // Convert the time into a distance
        cm = (duration/2) / 29.1;     // Divide by 29.1 or multiply by 0.0343
        //inches = (duration/2) / 74;   // Divide by 74 or multiply by 0.0135
        delay(250);

        cm2 = (duration2/2) / 29.1;     // Divide by 29.1 or multiply by 0.0343
        //inches2 = (duration2/2) / 74;   // Divide by 74 or multiply by 0.0135
        delay(250);

      if (cm > 40 && cm2> 40) {

            obstaclecheck = false;
            return obstaclecheck;
         }
         //enes.updateLocation();
        else   if (cm < 40 && cm2 >40 ){

            obstaclecheck = true;
            return obstaclecheck;
        }
              
              else if(cm>40  &&  cm2<40){

                return obstaclecheck = true;
            }
        }


   //Function to use the OSV's IR sensor to detect IR pulse
   int readInfraredSensor() {
      uint16_t highpulse, lowpulse;  // temporary storage timing
      highpulse = lowpulse = 0; // start out with no pulse length
      //Serial.println(lowpulse);

      enes.println("Executing IR sensor!");
       while (IRpin_PIN & (1 << IRpin)) {
          // pin is still HIGH
 
          // count off another few microseconds
          highpulse++;
          delayMicroseconds(RESOLUTION);
 
          // If the pulse is too long, we 'timed out' - either nothing
          // was received or the code is finished, so print what
          // we've grabbed so far, and then reset
          if ((highpulse >= MAXPULSE) && (currentpulse != 0)) {
         // printpulses();
          Serial.println("Current PULSE:" + currentpulse);

          currentpulse=0;
          return;
        }
      
      // we didn't time out so lets stash the reading
      pulses[currentpulse][0] = highpulse;
      
  
      // same as above
      while (!(IRpin_PIN & _BV(IRpin))) {
          // pin is still LOW
          lowpulse++;
          delayMicroseconds(RESOLUTION);
      if ((lowpulse >= MAXPULSE)  && (currentpulse != 0)) {
       //printpulses();
       currentpulse=0;
       return;
     }
  }
      pulses[currentpulse][1] = lowpulse;
      
 
      // we read one high-low pulse successfully, continue!
      currentpulse++;
      return currentpulse;
       }
   }

  

    
     
