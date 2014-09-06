// Copyright (C) 2013-2014 Thalmic Labs Inc.
// Distributed under the Myo SDK license agreement. See LICENSE.txt for details.
#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <string>
#include <deque>
#include "rep.h"

// The only file that needs to be included to use the Myo C++ SDK is myo.hpp.
#include <myo/myo.hpp>

// Classes that inherit from myo::DeviceListener can be used to receive events from Myo devices. DeviceListener
// provides several virtual functions for handling different kinds of events. If you do not override an event, the
// default behavior is to do nothing.

int main(int argc, char** argv)
{
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
		DataCollector collector;
		hub.addListener(&collector);
		
		std::vector<exercise> exercises;
		myo::Vector3<float> tempStartAccel, tempEndAccel;
		myo::Quaternion<float> tempStartRot, tempEndRot;
		
		//start a new exercise
		while (collector.inAnExercise == true){
			
			//first have to check if the exercise exists already
			std::cout << "\nGo to the start position.";
			collector.inARep = true;
			while (collector.inARep) {
				hub.run(1000/20);
			}
			tempStartAccel = collector.currentPos;
			tempStartRot = collector.currentRotation;
			std::cout << "Got it!";
			
			std::cout << "\nGo to the end position.";
			collector.inARep = true;

			while (collector.inARep) {
				hub.run(1000/20);
			}
			tempEndAccel = collector.currentPos;
			tempEndRot = collector.currentRotation;
			std::cout << "Got it!";
			
			exercise currentExercise = *new exercise(tempStartAccel, tempEndAccel, tempStartRot, tempEndRot, "temp");
			
			// Do it again and get a rough tolerance
			std::cout << "\nGo to the start position again.";
			collector.inARep = true;

			while (collector.inARep) {
				hub.run(1000/20);
			}
			tempStartAccel = collector.currentPos;
			tempStartRot = collector.currentRotation;
			std::cout << "Got it!";
			
			std::cout << "\nGo to the end position.";
			collector.inARep = true;

			while (collector.inARep) {
				hub.run(1000/20);
			}
			tempEndAccel = collector.currentPos;
			tempEndRot = collector.currentRotation;
			std::cout << "Got it!";
			//average the distances each accel/rotation are off to come up with some tolerance
			currentExercise.accelTolerance = (fabs(currentExercise.accel1.x() - tempStartAccel.x()) +
			fabs(currentExercise.accel1.y() - tempStartAccel.y()) +
			fabs(currentExercise.accel1.z() - tempStartAccel.z()) +
			fabs(currentExercise.accel2.x() - tempEndAccel.x()) +
			fabs(currentExercise.accel2.y() - tempEndAccel.y()) +
			fabs(currentExercise.accel2.z() - tempEndAccel.z()))/6;
			
			currentExercise.rotTolerance = (fabs(currentExercise.rot1.x() - tempStartRot.x()) +
			fabs(currentExercise.rot1.y() - tempStartRot.y()) +
			fabs(currentExercise.rot1.z() - tempStartRot.z()) +
			fabs(currentExercise.rot2.x() - tempEndRot.x()) +
			fabs(currentExercise.rot2.y() - tempEndRot.y()) +
			fabs(currentExercise.rot2.z() - tempEndRot.z()))/6;
			
			//put in some logic to see if any positions you have are within the
			//tolerances of eachother...I don't really know which would be better/easier/faster
			
			std::cout << "\nThat looks like a new exercise, please enter a name: ";
			//currentExercise.name = textbox.text;
			exercises.push_back(currentExercise);
			std::cout << "\nDone.";
		}
		
		int repCount = 0;
		bool currentInARep = 0, lastInARep = 0;
		
		// Finally we enter our main loop.
		while (1) {
			
			hub.run(1000/20);
			
			lastInARep = currentInARep;
			currentInARep = collector.inARep;
			if (lastInARep == false && currentInARep == true){
				repCount++;
				std::cout << "\nYou've done " << repCount << " reps!";
			}
			collector.print();
		}
		
	} catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		std::cerr << "Press enter to continue.";
		std::cin.ignore();
		return 1;
	}
}
