/************* COPYRIGHT AND CONFIDENTIALITY INFORMATION *********
WARNING: DO NOT REMOVE THIS NOTICE - AUTHORIZATION SUBJECT TO RESTRICTIONS

This copyright document relates to the following software: IDCC Skinetic driver 

This software may only be used in accordance with the license that you will find in 
"license.txt" on the root of the Software or at this adress: 	
Adresse du git contenant la licence

Copyright 2024 InterDigital R&D France
All Rights Reserved
*************************************************************************/
 
#pragma once

#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <string.h>
#include <string>
#include <list>
#include <errno.h>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <map>
#include <regex>
using namespace std;

/*
* This class is only for Unity.
* Allows to print a message in the Unity console
*/
class  Debug
{
public:
	static void Log(const char* message);
	static void Log(const std::string message);
	static void Log(const int message);
	static void Log(const char message);
	static void Log(const float message);
	static void Log(const double message);
	static void Log(const bool message);

private:
	static void send_log(const std::stringstream& ss);
};

/*
* Haptic Data structure
*/
typedef struct {
	//Data By Frames
	std::vector<std::vector <double> >  val; // -1 to 1

	//Reading head
	int id_frames;

	//Default Intensity coefficient
	double coeff_intensity;

	//Continuous Effect
	bool isContinuousEffect;

	//Default pitch
	double pitch;
}HapticData;

/*
* Haptic data to render
*/
struct HapticsQueue {
	//Playing data
	std::list<HapticData*> data_queue;

	//Data to release
	std::vector<HapticData*> to_release;

	//Data Queue Acces trigger
	bool is_reading = false;
};