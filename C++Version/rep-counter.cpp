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

// GLOBAL FOR NOW
std::vector<exercise> exercises;
DataCollector collector;
exercise currentExercise; 

bool closeToV3( const Vector3<float> & current, const Vector3<float> & ideal, double tolerance) {
    if (current.x() < ideal.x() - tolerance or current.x() > ideal.x() + tolerance)
        return false;
    else if (current.y() < ideal.y() - tolerance or current.y() > ideal.y() + tolerance)
        return false;
    else if (current.z() < ideal.z() - tolerance or current.z() > ideal.z() + tolerance)
        return false;
    
    return true;
}

void initializeNewExercise() {
    
    myo::Vector3<float> tempStartPos, tempEndPos;
    myo::Quaternion<float> tempStartRot, tempEndRot;

    std::cout << "\nGo to the start position.";
    collector.isMoving = true;
    while (collector.isMoving) {
        hub.run(1000/20);
    }
    tempStartPos = collector.currentPos;
    tempStartRot = collector.currentRotation;
    std::cout << "Got it!";
    
    std::cout << "\nGo to the end position.";
    collector.isMoving = true;

    while (collector.isMoving) {
        hub.run(1000/20);
    }
    tempEndPos = collector.currentPos;
    tempEndRot = collector.currentRotation;
    std::cout << "Got it!";

    currentExercise = *new exercise(tempStartPos, tempEndPos, tempStartRot, tempEndRot, "temp");

    //put in some logic to see if any positions you have are within the
    //tolerances of eachother...I don't really know which would be better/easier/faster
    
    //std::cout << "\nThat looks like a new exercise, please enter a name: ";
    //currentExercise.name = textbox.text;
    //std::cout << "\nDone.";
    exercises.push_back(currentExercise);
}

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
		hub.addListener(&collector);
		
		int repCount = 0;
		bool currentInARep = 0, lastInARep = 0;
		
		// Finally we enter our main loop.
		while (1) {
			
			hub.run(1000/20);
			
			lastInARep = currentInARep;
			currentInARep = collector.isMoving;
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
