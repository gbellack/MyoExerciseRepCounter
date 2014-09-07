// Copyright (C) 2013-2014 Thalmic Labs Inc.
// Distributed under the Myo SDK license agreement. See LICENSE.txt for details.
#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <string>
#include <deque>
#include <stdio.h>
#include <unistd.h>

// The only file that needs to be included to use the Myo C++ SDK is myo.hpp.
#include "myo/myo.hpp"

using namespace std;

class exercise : public myo::Pose {
public:
    exercise() {
        posTolerance = .3;
    };
    /*
	exercise(myo::Vector3<float> pos1, myo::Vector3<float> pos2, myo::Quaternion<float> rot1_in, myo::Quaternion<float> rot2_in, std::string name_in)
        : posStart(pos1), posEnd(pos2), rot1(rot1_in), rot2(rot2_in), name(name_in) {

        posTolerance = .01;
    };
    */
	
	//each exercise is defined as a hand position and a change in accelerometer value and rotation
	std::string name;
	myo::Pose pose;
	myo::Quaternion<float> posStart, posEnd;
	myo::Quaternion<float> rot1, rot2, rotChange;  //approximate change in rotation from start to finish
	double posTolerance;
	double rotTolerance;
};

/////////////////////// GLOBALS ////////////////////////////////
    exercise currentExercise; 

///////////////////////////////////////////// DEVICE LISTENER /////////////////////////////////////////////

// Classes that inherit from myo::DeviceListener can be used to receive events from Myo devices. DeviceListener
// provides several virtual functions for handling different kinds of events. If you do not override an event, the
// default behavior is to do nothing.
class DataCollector : public myo::DeviceListener {
public:
	bool onArm;
	myo::Arm whichArm;
	
	// These values are set by onOrientationData() and onPose() above.
	int roll_w, pitch_w, yaw_w;
	myo::Quaternion<float> currentRotation;
	myo::Pose currentPose;
	myo::Vector3<float> currentPos, lastPos;
	int holdTime;
	bool isMoving;	//currently doing an exercise
	bool inASet;  //currently doing an set of exercises
    bool haveRepStart, haveRepEnd, wasAtRepStart, wasAtRepEnd;
    int reps;
    myo::Quaternion<float> lastQuat;
	
	DataCollector()
	: onArm(false), roll_w(0), pitch_w(0), yaw_w(0), currentPose(), holdTime(0), isMoving(false), inASet(false), haveRepEnd(false), haveRepStart(false), wasAtRepStart(false), wasAtRepEnd(false), reps(0), lastQuat(myo::Quaternion<float>())
	{
    
    }

	//////////////////////////////// FUNCTIONS ////////////////////////////////////////

    bool closeToV3( const myo::Quaternion<float> & current, const myo::Quaternion<float> & ideal, double tolerance) {
        if (current.x() < ideal.x() - tolerance or current.x() > ideal.x() + tolerance)
            return false;
        if (current.y() < ideal.y() - tolerance or current.y() > ideal.y() + tolerance)
            return false;
        if (current.z() < ideal.z() - tolerance or current.z() > ideal.z() + tolerance)
            return false;
        if (current.w() < ideal.w() - tolerance or current.w() > ideal.w() + tolerance)
            return false;
        
        //cout << "closeToV3 returned true\n";
        return true;
    }

    /*
    void initializeNewExercise() {
        myo::Vector3<float> tempStartPos, tempEndPos;
        myo::Quaternion<float> tempStartRot, tempEndRot;

        cout << "Go to the start position.\n";
        this->isMoving = true;
        while (this->isMoving) {
            sleep(1);
            cout << "stuck in loop\n";
        }
        tempStartPos = this->currentPos;
        tempStartRot = this->currentRotation;
        std::cout << "Got it!\n";
        
        std::cout << "Go to the end position.\n";
        this->isMoving = true;

        while (this->isMoving) {
            sleep(1);
            cout << "stuck in loop\n";
        }
        tempEndPos = this->currentPos;
        tempEndRot = this->currentRotation;
        std::cout << "Got it!";

        currentExercise = *new exercise(tempStartPos, tempEndPos, tempStartRot, tempEndRot, "temp");

        //put in some logic to see if any positions you have are within the
        //tolerances of eachother...I don't really know which would be better/easier/faster
        
        //std::cout << "\nThat looks like a new exercise, please enter a name: ";
        //currentExercise.name = textbox.text;
        //std::cout << "\nDone.";
        exercises.push_back(currentExercise);
    }
     */
     
	void onPose(myo::Myo* myo, uint64_t timestamp, myo::Pose pose) {
		currentPose = pose;
		
		// Start a new exercise when you put thumb to pinky
        if (pose == myo::Pose::thumbToPinky) {
            cout << "Did thumbToPinky\n";
            if (!haveRepStart) {
                haveRepStart = true;
                currentExercise.posStart = lastQuat;
                cout << "Got rep start pos\n";
            }
            else if (!haveRepEnd) {
                haveRepEnd = true;
                currentExercise.posEnd = lastQuat;
                cout << "Got rep end pos\n";
            }
        }
        else if (pose == myo::Pose::fingersSpread) {
            cout << "Did fingersSpread\n";
			if (inASet == false){
				myo->vibrate(myo::Myo::vibrationMedium);
				inASet = true;
                cout << "Now in a set\n";
			}
			else {
				myo->vibrate(myo::Myo::vibrationMedium);
				myo->vibrate(myo::Myo::vibrationMedium);
                cout << "Did " << reps << " reps for this exercise!\n";
                reps = 0;
			}
		}
		
	}
	
    
	// onOrientationData() is called whenever the Myo device provides its current orientation, which is represented
	// as a unit quaternion.
	void onOrientationData(myo::Myo* myo, uint64_t timestamp, const myo::Quaternion<float>& quat)
	{
    lastQuat = quat;
        /*
		if (fabs(currentPos.x() - lastPos.x()) < .05 && fabs(currentPos.y() - lastPos.y()) < .05 && fabs(currentPos.z() - lastPos.z()) < .05){
			holdTime++;
            cout << "Incremented hold time\n";
		} else {
			isMoving = true;
			holdTime = 0;
            cout << "Moving\n";
		}
        
		if (holdTime > 15){
			isMoving = false;
            cout << "Hit > 15 Hold time and isMoving now is false\n";
		}
        */

        if (inASet) {
            if (closeToV3(quat, currentExercise.posStart, currentExercise.posTolerance)) {
                if (!wasAtRepStart) {
                    wasAtRepStart = true;
                    cout << "At position start for rep\n";
                }
                else if (wasAtRepEnd) {
                    reps++;
                    wasAtRepEnd = false;
                    wasAtRepStart = false;
                    cout << "Did a rep!\n";
                }
            }  
            else if (closeToV3(quat, currentExercise.posEnd, currentExercise.posTolerance)) {
                cout << "At position end for rep\n";
                if (wasAtRepStart) {
                    wasAtRepEnd = true;
                }
            }
        }
	}
    
	
	void onAccelerometerData(myo::Myo* myo, uint64_t timestamp, const myo::Vector3<float>& accel) {

	}
	
	// There are other virtual functions in DeviceListener that we could override here, like onAccelerometerData().
	// For this example, the functions overridden above are sufficient.
	
	// We define this function to print the current values that were updated by the on...() functions above.
	void print()
	{
		// Print out the orientation. Orientation data is always available, even if no arm is currently recognized.
		//std::cout << "\nposition: " << wpos << "   " <<xpos << "   " <<ypos << "   " << zpos;
		
		//std::cout << "\nacceleration: " << xaccel << "   " <<yaccel << "   " <<zaccel;
		
		//std::cout << "\nholdtime: " << holdTime;
		std::cout << std::flush;
	}
};


///////////////////////////////////////////// MAIN /////////////////////////////////////////////
int main(int argc, char** argv)
{

    DataCollector collector;

	try {
		
		//connect to Myo
		myo::Hub hub("com.example.hello-myo");
		std::cout << "Attempting to find a Myo..." << std::endl;
		myo::Myo* myo = hub.waitForMyo(10000);
		if (!myo) {
			throw std::runtime_error("Unable to find a Myo!");
		}
		std::cout << "Connected to a Myo armband!" << std::endl << std::endl;
		
		//start data collection
		hub.addListener(&collector);
		
		// Finally we enter our main loop.
		while (1) {
		    hub.run(1000/20);
		}
		
	} catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		std::cerr << "Press enter to continue.";
		std::cin.ignore();
		return 1;
	}
}
