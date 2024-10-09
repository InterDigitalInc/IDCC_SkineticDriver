/************* COPYRIGHT AND CONFIDENTIALITY INFORMATION *********
WARNING: DO NOT REMOVE THIS NOTICE - AUTHORIZATION SUBJECT TO RESTRICTIONS

This copyright document relates to the following software: IDCC Skinetic driver 

This software may only be used in accordance with the license that you will find in 
"license.txt" on the root of the Software or at this adress: 	
Adresse du git contenant la licence

Copyright 2024 InterDigital R&D France
All Rights Reserved
*************************************************************************/
 
#include "utils.h"
#include "PortAudio.h"
#include "AudioFile.h"

/*
* For DLL
*/
#ifdef DLLPROJECT_EXPORTS
#   define EXPORT __declspec(dllexport)
#else
#   define EXPORT __declspec(dllimport)
#endif

class MPEG_Haptic_SKINETICDRIVER {

	public://Constructor of Haptic Driver
        MPEG_Haptic_SKINETICDRIVER()
        {
            std::cout << "\n################### Creating HapticDriver ###################\n" << endl;
            //Check if Port Audio is closed and if it's not, close it. This clean the buffer of the SoundCard
            err = Pa_Terminate();
            if (err != paNoError)
                // If no problem:
                printf("Pa Clean: %s\n", Pa_GetErrorText(err));
            else
                std::cout << "Pa clean: " << Pa_GetErrorText(err) << endl;
            // Initialize Port Audio
            err = Pa_Initialize();
            if (err != paNoError) {
                //If can't initialize
                printf("PortAudio error: %s\n", Pa_GetErrorText(err));
                exit;
            }
            else {
                std::cout << "CreatingDriver: " << Pa_GetErrorText(err) << endl;
            }
            //Setting default OutputParameters values
            numDevices = Pa_GetDeviceCount();
            outputParameters.device = Pa_GetDefaultOutputDevice();
            if (outputParameters.device == paNoDevice) {
                std::cout << "Err : No Default Output" << endl;
                exit;
            }
            outputParameters.channelCount = Pa_GetDeviceInfo(outputParameters.device)->maxOutputChannels;
            outputParameters.sampleFormat = paFloat32;
            outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
            outputParameters.hostApiSpecificStreamInfo = NULL;
            std::cout << "Default Ouput: " << Pa_GetDeviceInfo(outputParameters.device)->name << endl;
            queueData = new HapticsQueue;
        }

        ~MPEG_Haptic_SKINETICDRIVER() {
            //Destructor for Haptic Driver
            std::cout << "\n################### Destructing HapticDriver ###################\n" << endl;
            err = Pa_CloseStream(stream);
            if (err != paNoError) {
                printf("PortAudio error: %s\n", Pa_GetErrorText(err));
            }
            //Terminate Port Audio
            err = Pa_Terminate();
            if (err != paNoError)
                printf("PortAudio error: %s\n", Pa_GetErrorText(err));
            else
                std::cout << "DestructingDriver: " << Pa_GetErrorText(err) << endl;
            Debug::Log("Destructed");
        }
        /*
        * Function of HapticDriver class
        */

        //Show information
        void showDefaultOutput();
        int findId(string s, int sr);

        //Intern fnct for haptic driver
        void changeDevice(int id_device);
        void abort_stream();
        void OpenHapticStream();
        void addHapticData(HapticData* StreamedData);
        void removeHapticData(HapticData* StreamedData);
        void addLibraryData(const char* oldpath, string name, bool isContinuous);
        HapticData* createHapticDataFromLibrary(const char* name, std::vector<int> channel_list);
        static vector<vector<double>> managePitch(HapticsQueue* HQ);
        static int HapticCallBack(const void* inputBuffer, void* outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData);
        void change_Intensity(HapticData* data, double intensity);
        void change_Pitch(HapticData* data, double pitch);
        static std::vector<double> resample(const std::vector<double>& input, int new_size);
        void addMPEGLibraryData(const char* path, string receivedString, bool isContinuous);
        static void StreamFinished(void* userData);
private:
    /*
    * Parameters for Haptic Driver
    * - err : Port Audio parameter where "errors" are stored
    * - numDevices : Number of devices
    * - outputParameters : Port Audio parameter with output info for the streaming
    * - stream : Stream where the sound will be played
    * - haptic_library: Library of haptic effects, the name of the haptic effect is the key
    * - queueData: Haptic Data rendered in realtime by the driver
    */
    PaError err;
    int numDevices;
    PaStreamParameters outputParameters;
    PaStream* stream;
    std::map<string, HapticData*> haptic_library;
    HapticsQueue* queueData;

};

extern "C"
{
    typedef void(*FuncCallBack)(const char* message, int size);
    static FuncCallBack callbackInstance = nullptr;
    EXPORT void RegisterDebugCallback(FuncCallBack cb);
    
    EXPORT MPEG_Haptic_SKINETICDRIVER* createHapticDriver();
    EXPORT void debugDeviceInfo();
    EXPORT void terminate_haptic(MPEG_Haptic_SKINETICDRIVER* HD);
    EXPORT void changeDefaultDevice(MPEG_Haptic_SKINETICDRIVER* HD, int id);
    EXPORT void debugDefaultInfo(MPEG_Haptic_SKINETICDRIVER* HD);
    EXPORT int findIDDevice(MPEG_Haptic_SKINETICDRIVER* HD, const char* device_name, int rs);
    EXPORT void openHapticStream(MPEG_Haptic_SKINETICDRIVER* HD);
    EXPORT void addHapticData(MPEG_Haptic_SKINETICDRIVER* HD, HapticData* StreamedData);
    EXPORT void removeHapticData(MPEG_Haptic_SKINETICDRIVER* HD, HapticData* StreamedData);
    EXPORT HapticData* readWav(const char* oldpath, bool isContinuous);
    EXPORT void closeHapticStream(MPEG_Haptic_SKINETICDRIVER* HD);
    EXPORT void addEffectLibrary(MPEG_Haptic_SKINETICDRIVER* HD, const char* path, const char* name, bool isContinuous);
    EXPORT HapticData* createHapticDataFromLibrary(MPEG_Haptic_SKINETICDRIVER* HD, const char* name, int* arr, int size);
    EXPORT void changeRealtimeIntensity(MPEG_Haptic_SKINETICDRIVER* HD, HapticData* data, double intensity);
    EXPORT void changeRealtimePitch(MPEG_Haptic_SKINETICDRIVER* HD, HapticData* data, double pitch);
    EXPORT void addMPEGEffectLibrary(MPEG_Haptic_SKINETICDRIVER* HD, const char* path, const char* name, bool isContinuous);
}