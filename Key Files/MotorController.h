#pragma once

#include "Arduino.h" 

//Motorcontroller class to easily manage the motors used in the buggy

// 1 - forward, 3 - left, 4 - right, 0 - stop
// The class also takes right and left motor offset to account for difference in rotation or a badly placed motor
// To use the class intitialise the motorcontroller with motor pin values (and offsets) as parameters and use moveTo method to move the buggy accordingly  

class MotorController{

  private:
  
    int _M1_D1; 
    int _M1_D2;
    int _M2_D1;
    int _M2_D2;
    float leftOffset;
    float rightOffset;

    int forwardSignal;
    int backwardSignal;
    
  public:
  
    MotorController(int m1d1, int m1d2, int m2d1, int m2d2){
      
      _M1_D1 = m1d1;
      _M1_D2 = m1d2;
      _M2_D1 = m2d1;
      _M2_D2 = m2d2;

      rightOffset = 1;
      leftOffset = 1;

      pinMode(_M1_D1, OUTPUT);
      pinMode(_M1_D2, OUTPUT);
      pinMode(_M2_D1, OUTPUT);
      pinMode(_M2_D2, OUTPUT);
      
    }

    MotorController(int m1d1, int m1d2, int m2d1, int m2d2, float rOffset, float lOffset){
      
      _M1_D1 = m1d1;
      _M1_D2 = m1d2;
      _M2_D1 = m2d1;
      _M2_D2 = m2d2;

      rightOffset = rOffset;
      leftOffset = lOffset;

      pinMode(_M1_D1, OUTPUT);
      pinMode(_M1_D2, OUTPUT);
      pinMode(_M2_D1, OUTPUT);
      pinMode(_M2_D2, OUTPUT);
      
    }

    MotorController(){

    }

    ~MotorController(){
      
    }

    void moveTo(int motorDirection, int motorSpeed){

      if(motorSpeed > 0){

        if(motorSpeed > 255){

          forwardSignal = 255;
          backwardSignal = 0;

        } else {

          forwardSignal = motorSpeed;
          backwardSignal = 0;
          
        }

        

      } else if(motorSpeed < 0){
        
        if(motorSpeed < -255){

          forwardSignal = 0;
          backwardSignal = 255;

        } else {

          forwardSignal = 0;
          backwardSignal = -motorSpeed;
          
        }

       

      }

      if(motorDirection == 1){
    
        analogWrite(_M1_D1, forwardSignal * rightOffset);
        analogWrite(_M1_D2, backwardSignal * rightOffset);
        analogWrite(_M2_D1, forwardSignal * leftOffset);
        analogWrite(_M2_D2, backwardSignal * leftOffset);
        
      } else if(motorDirection == 3){

        analogWrite(_M1_D1, forwardSignal * rightOffset);
        analogWrite(_M1_D2, backwardSignal * rightOffset);
        analogWrite(_M2_D1, backwardSignal * leftOffset);
        analogWrite(_M2_D2, forwardSignal * leftOffset);
        
      } else if(motorDirection == 4){
    
        analogWrite(_M1_D1, backwardSignal * rightOffset);
        analogWrite(_M1_D2, forwardSignal * rightOffset);
        analogWrite(_M2_D1, forwardSignal * leftOffset);
        analogWrite(_M2_D2, backwardSignal * leftOffset);
        
      } else if(motorDirection == 0) {
    
        analogWrite(_M1_D1, 0);
        analogWrite(_M1_D2, 0);
        analogWrite(_M2_D1, 0);
        analogWrite(_M2_D2, 0);
            
      }
    
    }
    
    void fullStop(){
    
      analogWrite(_M1_D1, 0);
      analogWrite(_M1_D2, 0);
      analogWrite(_M2_D1, 0);
      analogWrite(_M2_D2, 0);
      delay(10);
      
    }
      
};
