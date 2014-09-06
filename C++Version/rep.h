//
//  rep.h
//  hello-myo
//
//  Created by Leda Daehler on 9/6/14.
//  Copyright (c) 2014 Thalmic Labs. All rights reserved.
//

#ifndef __hello_myo__rep__
#define __hello_myo__rep__

#include <myo/myo.hpp>
#include <stdio.h>
#include <deque>

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
    bool wasAtRepStart;
    bool wasAtRepFinish;
    int reps;
	
	DataCollector()
	: onArm(false), roll_w(0), pitch_w(0), yaw_w(0), currentPose(), holdTime(0), isMoving(false), inASet(false), wasAtRepStart(false), wasAtRepFinish(false), reps(0)
	{ }
	
	void onPose(myo::Myo* myo, uint64_t timestamp, myo::Pose pose)
	{
		currentPose = pose;
		
		// Start a new exercise when you put thumb to pinky
        if (pose == myo::Pose::thumbToPinky) {
            initializeNewExercise();
        }
        else if (pose == myo::Pose::fingersSpread) {
			if (inASet == false){
				myo->vibrate(myo::Myo::vibrationMedium);
				inASet = true;
			}
			else {
				myo->vibrate(myo::Myo::vibrationMedium);
				myo->vibrate(myo::Myo::vibrationMedium);
				inASet = false;
                cout << "Did " << reps << "reps for this exercise!\n";
			}
		}
		
	}
	
	// onOrientationData() is called whenever the Myo device provides its current orientation, which is represented
	// as a unit quaternion.
	void onOrientationData(myo::Myo* myo, uint64_t timestamp, const myo::Quaternion<float>& quat)
	{
		currentRotation = quat;

        if (inASet) {
            if (closeToV3(currentExercise.posStart, collector.currentPos, currentExercise.posTolerance)) {
                cout << "At position start for rep\n";
                wasAtRepStart = true;
                if (wasAtRepEnd) { 
                    reps++;
                    wasAtRepEnd = false;
                    cout << "Did a rep!\n";
                }
            }  
            else if (closeToV3(currentExercise.posEnd, collector.currentPos, currentExercise.posTolerance)) {
                cout << "At position end for rep\n";
                wasAtRepEnd = true;
            }
        }
	}
	
	void onAccelerometerData(myo::Myo* myo, uint64_t timestamp, const myo::Vector3<float>& accel) {

		lastPos = currentPos;
		currentPos = accel;
		
		if (fabs(currentPos.x() - lastPos.x()) < .05 && fabs(currentPos.y() - lastPos.y()) < .05 && fabs(currentPos.z() - lastPos.z()) < .05){
			//std::cout << "\nNot moving";
			holdTime++;
		} else {
			isMoving = true;
			holdTime = 0;
		}

		if (holdTime > 15){
			isMoving = false;
		}
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

class exercise : public myo::Pose {
public:
	exercise(myo::Vector3<float> pos1, myo::Vector3<float> pos2, myo::Quaternion<float> rot1_in, myo::Quaternion<float> rot2_in, std::string name_in)
        : posStart(pos1), posEnd(pos2), rot1(rot1_in), rot2(rot2_in), name(name_in) {

        posTolerance = .1;
    };
	
	
	
	//each exercise is defined as a hand position and a change in accelerometer value and rotation
	std::string name;
	myo::Pose pose;
	myo::Vector3<float> posStart, posEnd;
	myo::Quaternion<float> rot1, rot2, rotChange;  //approximate change in rotation from start to finish
	double posTolerance;
	double rotTolerance;
};


#endif /* defined(__hello_myo__rep__) */
