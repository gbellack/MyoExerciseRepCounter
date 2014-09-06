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

class DataCollector : public myo::DeviceListener {
public:
	bool onArm;
	myo::Arm whichArm;
	
	// These values are set by onOrientationData() and onPose() above.
	int roll_w, pitch_w, yaw_w;
	myo::Quaternion<float> currentRotation;
	myo::Pose currentPose;
	myo::Vector3<float> currentPos, lastPos;
	int holdTime = 0;
	bool inARep = false;	//currently doing an exercise
	bool inAnExercise = false;  //currently doing an set of exercises
	
	DataCollector()
	: onArm(false), roll_w(0), pitch_w(0), yaw_w(0), currentPose(), inARep(false), inAnExercise(false)
	{ }
	
	void onPose(myo::Myo* myo, uint64_t timestamp, myo::Pose pose)
	{
		currentPose = pose;
		
		// Start a new exercise when you spread your hand
		if (pose == myo::Pose::fingersSpread) {
			if (inAnExercise == false){
				myo->vibrate(myo::Myo::vibrationMedium);
				inAnExercise = true;
			}
			else {
				myo->vibrate(myo::Myo::vibrationMedium);
				myo->vibrate(myo::Myo::vibrationMedium);
				inAnExercise = false;
			}
		}
		
	}
	
	// onOrientationData() is called whenever the Myo device provides its current orientation, which is represented
	// as a unit quaternion.
	void onOrientationData(myo::Myo* myo, uint64_t timestamp, const myo::Quaternion<float>& quat)
	{
		currentRotation = quat;
		
	}
	
	void onAccelerometerData(myo::Myo* myo, uint64_t timestamp, const myo::Vector3<float>& accel) {

		lastPos = currentPos;
		currentPos = accel;
		
		if (fabs(currentPos.x() - lastPos.x()) < .05 && fabs(currentPos.y() - lastPos.y()) < .05 && fabs(currentPos.z() - lastPos.z()) < .05){
			//std::cout << "\nNot moving";
			holdTime++;
		} else {
			//std::cout << "\n";
			
			inARep = true;
			holdTime = 0;
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
		if (holdTime > 15){
			inARep = false;
			//std::cout << "\nYou did a rep!";
		}
		std::cout << std::flush;
	}
};

class exercise : public myo::Pose {
public:
	exercise(myo::Vector3<float> accel1_in, myo::Vector3<float> accel2_in, myo::Quaternion<float> rot1_in,
			 myo::Quaternion<float> rot2_in, std::string name_in)
	: accel1(accel1_in), accel2(accel2_in), rot1(rot1_in), rot2(rot2_in), name(name_in) {};
	
	
	
	//each exercise is defined as a hand position and a change in accelerometer value and rotation
	std::string name;
	myo::Pose pose;
	myo::Vector3<float> accel1, accel2, accelChange;	//approximate change in accelerometer from start to finish
	myo::Quaternion<float> rot1, rot2, rotChange;  //approximate change in rotation from start to finish
	double accelTolerance;
	double rotTolerance;
};


#endif /* defined(__hello_myo__rep__) */
