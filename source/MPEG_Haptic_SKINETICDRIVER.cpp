/************* COPYRIGHT AND CONFIDENTIALITY INFORMATION *********
WARNING: DO NOT REMOVE THIS NOTICE - AUTHORIZATION SUBJECT TO RESTRICTIONS

This copyright document relates to the following software: IDCC Skinetic driver 

This software may only be used in accordance with the license that you will find in 
"license.txt" on the root of the Software or at this adress: 	
Adresse du git contenant la licence

Copyright 2024 InterDigital R&D France
All Rights Reserved
*************************************************************************/
 
#include "MPEG_Haptic_SKINETICDRIVER.h"
#include "PortAudio.h"
#include "AudioFile.h"
#include <IOHaptics/include/IOJson.h>
#include <Synthesizer/include/Helper.h>
#include <Tools/include/InputParser.h>
#include <Tools/include/OHMData.h>
#include <Types/include/Haptics.h>
#include <filesystem>
using haptics::io::IOJson;
using haptics::synthesizer::Helper;
using haptics::tools::InputParser;
using haptics::types::Haptics;
#define SAMPLE_RATE (48000)
#define FRAMES_PER_BUFFER (6400)
//------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------- DEBUG LOG PART -----------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------
void  Debug::Log(const char* message) {
    if (callbackInstance != nullptr)
        callbackInstance(message, (int)strlen(message));
}

void  Debug::Log(const std::string message) {
    const char* tmsg = message.c_str();
    if (callbackInstance != nullptr)
        callbackInstance(tmsg, (int)strlen(tmsg));
}

void  Debug::Log(const int message) {
    std::stringstream ss;
    ss << message;
    send_log(ss);
}

void  Debug::Log(const char message) {
    std::stringstream ss;
    ss << message;
    send_log(ss);
}

void  Debug::Log(const float message) {
    std::stringstream ss;
    ss << message;
    send_log(ss);
}

void  Debug::Log(const double message) {
    std::stringstream ss;
    ss << message;
    send_log(ss);
}

void Debug::Log(const bool message) {
    std::stringstream ss;
    if (message)
        ss << "true";
    else
        ss << "false";

    send_log(ss);
}

void Debug::send_log(const std::stringstream& ss) {
    const std::string tmp = ss.str();
    const char* tmsg = tmp.c_str();
    if (callbackInstance != nullptr)
        callbackInstance(tmsg, (int)strlen(tmsg));
}
//-------------------------------------------------------------------

//Create a callback delegate
void RegisterDebugCallback(FuncCallBack cb) {
    callbackInstance = cb;
}


//---------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------- Haptic Driver DLL -----------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------
extern "C"
{
    /*
    * Create an instance of the SKINETIC Haptic Driver
    */
    MPEG_Haptic_SKINETICDRIVER* createHapticDriver() {
        Debug::Log("HapticDriver Created");
        return new MPEG_Haptic_SKINETICDRIVER();
    }

    /*
    * Kill and clean the HapticDriver instance.
    * Input:
    * HD -> Haptic Driver instance to close.
    */
    void terminate_haptic(MPEG_Haptic_SKINETICDRIVER* HD) {
        HD->~MPEG_Haptic_SKINETICDRIVER();
    }

    /*
    * Debug devices properties plugged to the PC.
    */
    void debugDeviceInfo() {
        Debug::Log("\n################### Showing All Devices ###################\n");
        const   PaDeviceInfo* deviceInfo;
        for (int i = 0; i < Pa_GetDeviceCount(); i++)
        {
            deviceInfo = Pa_GetDeviceInfo(i);
            string s = "Name: ";
            s.append(deviceInfo->name);
            s.append("| ID: ");
            s.append(to_string(i));
            s.append("| Ouput Channel: ");
            s.append(to_string(deviceInfo->maxOutputChannels));
            s.append("| Default Sample Rate: ");
            s.append(to_string(deviceInfo->defaultSampleRate));
            Debug::Log(s);
        }
    }

    /*
    * Find the ID of the device to use from his name
    * Input :
    * s -> name of the device
    * sr-> sample rate of the device
    * Output :
    * Return the id of the device, if the device is not found, return -1
    */
    int findIDDevice(MPEG_Haptic_SKINETICDRIVER* HD, const char* device_name, int sr) {
        string s = device_name;
        return HD->findId(s, sr);
    }

    /*
    * Change the ouput device where haptic effects should be rendered
    * Input:
    * HD-> The Haptic Driver
    * id-> id of the device
    */
    void changeDefaultDevice(MPEG_Haptic_SKINETICDRIVER* HD, int id) {
        HD->changeDevice(id);
    }

    /*
    * Debug the properties of the default device of the Haptic Driver
    * Input:
    * HD-> The Haptic Driver
    */
    void debugDefaultInfo(MPEG_Haptic_SKINETICDRIVER* HD) {
        Debug::Log("\n################### Showing Default Device ###################\n");
        HD->showDefaultOutput();
    }

    /*
    *Read a wav from a path and tell is this HapticData is continuous and return a HapticData Object.
    * Input:
    * HD-> The Haptic Driver
    * oldpath-> path to the .wav file
    * isContinuous-> True if the hapticdata should be continuous, else false.
    *
    */
    HapticData* readWav(const char* oldpath, bool isContinuous) {
        AudioFile<double> audioFile;
        string path = oldpath;
        audioFile.load(path);
        int numChannels = audioFile.getNumChannels();
        int numSamples = audioFile.getNumSamplesPerChannel();
        HapticData* data = new HapticData;
        data->val = audioFile.samples;
        data->id_frames = 0;
        data->coeff_intensity = 1;
        data->pitch = 1;
        data->isContinuousEffect = isContinuous;
        return data;
    }

    /*
    * Unity Function called to open the Haptic Stream
    */
    void openHapticStream(MPEG_Haptic_SKINETICDRIVER* HD) {
        HD->OpenHapticStream();
    }

    /*
    * Unity Function called to close the Haptic Stream
    */
    void closeHapticStream(MPEG_Haptic_SKINETICDRIVER* HD) {
        HD->abort_stream();
    }

    /*
    * Unity Function called to add Haptic data into the Haptic Reading Queue
    */
    void addHapticData(MPEG_Haptic_SKINETICDRIVER* HD, HapticData* data) {
        HD->addHapticData(data);
    }

    /*
    * Unity Function called to remove Continuous Haptic Data.
    */
    void removeHapticData(MPEG_Haptic_SKINETICDRIVER* HD, HapticData* data) {
        HD->removeHapticData(data);
    }

    /*
    * Add a new Haptic effect in WAV format to the Effect Library
    * Input:
    * HD-> The HapticDriver
    * path-> Path to the .wav file
    * name-> Name of the effect
    * isContinuous-> Effect is continuous or not
    */
    void addEffectLibrary(MPEG_Haptic_SKINETICDRIVER* HD, const char* path, const char* name, bool isContinuous) {
        std::string receivedString(name);
        HD->addLibraryData(path, receivedString, isContinuous);
    }

    /*
    * Add a new Haptic effect in HJIF format to the Effect Library
    * Input:
    * HD-> The HapticDriver
    * path-> Path to the .hjif file
    * name-> Name of the effect
    * isContinuous-> Effect is continuous or not
    */
    void addMPEGEffectLibrary(MPEG_Haptic_SKINETICDRIVER* HD, const char* path, const char* name, bool isContinuous) {
        std::string receivedString(name);
        HD->addMPEGLibraryData(path, receivedString, isContinuous);
    }

    /*
    * Create a HapticData using its name from the Effect Library
    * Input:
    * HD-> The Haptic Driver
    * name-> The effect name
    * arr-> List of the channel where the effect should be played
    * size-> Size of the channel list
    * Output:
    * HapticData create from the library
    */
    HapticData* createHapticDataFromLibrary(MPEG_Haptic_SKINETICDRIVER* HD, const char* name, int* arr, int size) {
        vector<int> vec;
        for (int i = 0; i < size; i++) {
            vec.push_back(arr[i]);

        }
        HapticData* data = HD->createHapticDataFromLibrary(name, vec);
        return data;
    }


    /*
    * Change the coefficient intensity of the HapticData in parameter
    * Input:
    * HD-> The Haptic Driver
    * data-> The Haptic Data
    * intensity-> new intensity for the haptic data
    */
    void changeRealtimeIntensity(MPEG_Haptic_SKINETICDRIVER* HD, HapticData* data, double intensity) {
        HD->change_Intensity(data, intensity);

    }

    /*
    * Change the pitch of the HapticData in parameter
    * Input:
    * HD-> The Haptic Driver
    * data-> The Haptic Data
    * pitch-> new pitch for the haptic data
    */
    void changeRealtimePitch(MPEG_Haptic_SKINETICDRIVER* HD, HapticData* data, double pitch) {
        HD->change_Pitch(data, pitch);

    }
    

}



//---------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------- Haptic Driver Functions -----------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------


/*
* Find the ID of the device to use from his name
* Input :
* s -> name of the device
* sr-> sample rate of the device
* Output :
* Return the id of the device, if the device is not found, return -1
*/
int MPEG_Haptic_SKINETICDRIVER::findId(string s, int sr) {

    const   PaDeviceInfo* deviceInfo;
    for (int i = 0; i < numDevices; i++)
    {
        deviceInfo = Pa_GetDeviceInfo(i);
        if (regex_match(deviceInfo->name, regex("(.*()" + s + ")(.)*")) && deviceInfo->defaultSampleRate == sr) {
            return i;
        }
    }
    return -1;

}

/*
* Change the device of OutputParameters
* Input:
* - int index_device -> id of the device to use
*/
void MPEG_Haptic_SKINETICDRIVER::changeDevice(int index_device) {
    std::cout << "\n################### Changing Default Device ###################\n" << endl;
    outputParameters.device = index_device;
    if (outputParameters.device == paNoDevice) {
        std::cout << "Err : No Default Output" << endl;
        exit;
    }
    //Setting OutputParameters
    outputParameters.channelCount = Pa_GetDeviceInfo(outputParameters.device)->maxOutputChannels;
    outputParameters.sampleFormat = paFloat32;
    outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;
    std::cout << "New Ouput: " << Pa_GetDeviceInfo(outputParameters.device)->name << endl;
}

/*
* Show the default OutputParameters
*/
void MPEG_Haptic_SKINETICDRIVER::showDefaultOutput() {
    std::cout << "\n################### Showing Default Device ###################\n" << endl;

    //For unity
    string s = "Name: ";
    s.append(Pa_GetDeviceInfo(outputParameters.device)->name);
    s.append("| Max Output: ");
    s.append(to_string(Pa_GetDeviceInfo(outputParameters.device)->maxOutputChannels));
    s.append("| Default Sample Rate: ");
    s.append(to_string(Pa_GetDeviceInfo(outputParameters.device)->defaultSampleRate));
    Debug::Log(s);

    //C++ console
    std::cout << "Name: " << Pa_GetDeviceInfo(outputParameters.device)->name << endl;
    std::cout << "Max Input: " << Pa_GetDeviceInfo(outputParameters.device)->maxInputChannels << endl;
    std::cout << "Max Output: " << Pa_GetDeviceInfo(outputParameters.device)->maxOutputChannels << endl;
    std::cout << "Default Sample Rate: " << Pa_GetDeviceInfo(outputParameters.device)->defaultSampleRate << endl;
}


/*
* Open the Haptic Stream with the HapticCallBack
*/
void MPEG_Haptic_SKINETICDRIVER::OpenHapticStream() {

    err = Pa_OpenStream(
        &stream,
        NULL,
        &outputParameters,
        SAMPLE_RATE,
        FRAMES_PER_BUFFER,
        paClipOff,
        HapticCallBack,
        queueData);

    err = Pa_SetStreamFinishedCallback(stream, &StreamFinished);
    if (err != paNoError) {
        printf("PortAudio error: %s\n", Pa_GetErrorText(err));
        exit;
    }
    //Stop stream before the start to clean the buffer
    err = Pa_StopStream(stream);
    if (err != paNoError) {
        printf("PortAudio error: %s\n", Pa_GetErrorText(err));
        exit;
    }
    // Start the stream
    err = Pa_StartStream(stream);
    if (err != paNoError) {
        printf("PortAudio error: %s\n", Pa_GetErrorText(err));
        exit;
    }
}


/*
* Last version of the Haptic Callback for haptic stream
*/
int MPEG_Haptic_SKINETICDRIVER::HapticCallBack(const void* inputBuffer, void* outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData) {
    //Cast Data in the good shape
    HapticsQueue* dataQueue = (HapticsQueue*)userData;

    float* out = (float*)outputBuffer;

    //Prevent unused variable warnings
    (void)timeInfo;
    (void)statusFlags;
    (void)inputBuffer;
    vector<vector<double>> data = managePitch(dataQueue);
    for (unsigned long i = 0; i < framesPerBuffer; i++) {
        for (int y = 0; y < 20; y++) {
            *out++ = data[y][i];
        }

        // To Test (in order the remove the section bellow) This new method allow to remove the data from the list after being read.


    }
    dataQueue->is_reading = true;
    auto iter = dataQueue->data_queue.begin();
    for (; iter != dataQueue->data_queue.end();)
    {
        int tmp_id_frames = (*iter)->id_frames;
        if (tmp_id_frames >= (*iter)->val[0].size()) {
            if ((*iter)->isContinuousEffect) {
                (*iter)->id_frames = 0;
                ++iter;
            }
            else {
                iter = dataQueue->data_queue.erase(iter);
            }
        }
        else {
            ++iter;
        }
    }
    dataQueue->is_reading = false;
    for (int z = 0; z < dataQueue->to_release.size(); z++) {
        HapticData* datatmp = dataQueue->to_release[z];
        dataQueue->data_queue.remove(datatmp);
    }
    dataQueue->to_release = vector<HapticData*>();

    dataQueue = nullptr;
    out = nullptr;
    return paContinue;
}
vector<vector<double>> MPEG_Haptic_SKINETICDRIVER::managePitch(HapticsQueue* HQ) {
    std::vector<std::vector <double> >  haptic_d;
    for (int i = 0; i < 20; i++) {
        vector<double> tmp(FRAMES_PER_BUFFER, 0.0);
        haptic_d.push_back(tmp);
    }
    for (HapticData* d : HQ->data_queue) {
        double pitch = d->pitch;
        int starting_id = d->id_frames;
        int end_id = starting_id + FRAMES_PER_BUFFER * pitch;
        int final_id = end_id;
        int first_id_cont = -1;
        for (int i = 0; i < 20; i++) {
            vector<double> vec_data_tmp;
            //Fill Buffer
            for (int y = starting_id; y < end_id; y++) {
                if (y < d->val[i].size()) {
                    vec_data_tmp.push_back(d->val[i][y]);
                }
                else {
                    if (d->isContinuousEffect) {
                        //cout << "full and continuous" << endl;
                        if (first_id_cont == -1) {
                            first_id_cont = y;
                        }
                        final_id = y - first_id_cont;
                        //cout << final_id << endl;
                        vec_data_tmp.push_back(d->val[i][final_id]);
                        //cout << "full and continuous end" << endl;
                    }
                    else {
                        vec_data_tmp.push_back(0);
                    }
                }

            }
            vector<double> final_data;
            if (pitch != 1) {
                final_data = resample(vec_data_tmp, FRAMES_PER_BUFFER);
            }
            else {
                final_data = vec_data_tmp;
            }

            for (int z = 0; z < FRAMES_PER_BUFFER; z++) {
                double tmp_value = (haptic_d[i][z] + final_data[z]) * d->coeff_intensity;
                if (tmp_value >= 1) {
                    haptic_d[i][z] = 1;
                }
                else if (tmp_value <= -1) {
                    haptic_d[i][z] = -1;
                }
                else {
                    haptic_d[i][z] = tmp_value;
                }
            }




        }
        d->id_frames = final_id;


    }
    return haptic_d;


}

std::vector<double> MPEG_Haptic_SKINETICDRIVER::resample(const std::vector<double>& input, int new_size) {
    std::vector<double> output(new_size);

    // Calculate the ratio of the new size to the old size
    double ratio = static_cast<double>(input.size() - 1) / (new_size - 1);

    for (int i = 0; i < new_size; ++i) {
        // Find the corresponding position in the original array
        double pos = ratio * i;
        int idx = static_cast<int>(pos);
        double frac = pos - idx;

        // Linear interpolation
        if (idx + 1 < input.size()) {
            output[i] = input[idx] * (1 - frac) + input[idx + 1] * frac;
        }
        else {
            output[i] = input[idx];
        }
    }
    return output;
}

/*
* Close the stream
*/
void MPEG_Haptic_SKINETICDRIVER::abort_stream() {
    err = Pa_CloseStream(stream);
    if (err != paNoError) {
        printf("PortAudio error: %s\n", Pa_GetErrorText(err));
        exit;
    }
}

/*
* Add a HapticData to the reading haptic queue.
* Input:
* StreamedData -> HapticData to add.
*/
void MPEG_Haptic_SKINETICDRIVER::addHapticData(HapticData* StreamedData)
{
    if (!StreamedData->isContinuousEffect) {
        HapticData* data = new HapticData;
        data->coeff_intensity = StreamedData->coeff_intensity;
        data->id_frames = 0;
        data->pitch = StreamedData->pitch;
        data->isContinuousEffect = false;
        data->val = StreamedData->val;
        queueData->data_queue.push_back(data);
    }
    else {
        queueData->data_queue.push_back(StreamedData);
    }

}

/*
* Remove a continuous HapticData from the reading haptic queue.
* Input:
* StreamedData -> HapticData to remove.
*/
void MPEG_Haptic_SKINETICDRIVER::removeHapticData(HapticData* StreamedData) {

    try {
        queueData->to_release.push_back(StreamedData);
    }
    catch (const std::exception& e) {
        queueData->data_queue.remove(StreamedData);
    }

}

/*
* Add a new effect to the library using .wav file.
*/
void MPEG_Haptic_SKINETICDRIVER::addLibraryData(const char* oldpath, string name, bool isContinuous) {
    AudioFile<double> audioFile;
    string path = oldpath;
    audioFile.load(path);
    int numChannels = audioFile.getNumChannels();
    int numSamples = audioFile.getNumSamplesPerChannel();
    HapticData* data = new HapticData;
    data->val = audioFile.samples;
    data->id_frames = 0;
    data->coeff_intensity = 1;
    data->pitch = 1;
    data->isContinuousEffect = isContinuous;
    haptic_library[name] = data;
    //haptic_library.insert(std::pair{ name, data });
    std::cout << "Added to library" << endl;
}

/*
* Add a new Haptic Effect to the library from a HJIF File
*/
void MPEG_Haptic_SKINETICDRIVER::addMPEGLibraryData(const char* path, string receivedString, bool isContinuous) {
    Debug::Log("################### Start Reading Mpeg ###################");
    Haptics hapticFile;

    //Load the .hjif
    IOJson::loadFile(path, hapticFile);
    hapticFile.linearize();

    //Time of the effect described in .hjif
    const double timeLength = Helper::getTimeLength(hapticFile);

    //Return the haptic data into a matrix N*M, N the number of channels, M the number of samples
    std::vector<std::vector<double>> samples = Helper::getHapticDataFromFile(hapticFile, timeLength, SAMPLE_RATE, 0);
    int numSamples = samples[0].size();
    int nb_channel = samples.size();

    HapticData* data = new HapticData;
    data->val = samples;
    data->id_frames = 0;
    data->coeff_intensity = 1;
    data->pitch = 1;
    data->isContinuousEffect = isContinuous;
    haptic_library[receivedString] = data;

    std::cout << "Added to library" << endl;


}

/*
* Create an Haptic Data object from the library using the name effect and the list of the channel where the haptic effect should be rendered.
*/
HapticData* MPEG_Haptic_SKINETICDRIVER::createHapticDataFromLibrary(const char* name, std::vector<int> channel_list) {
    HapticData* data = new HapticData;
    HapticData* toplay = haptic_library.at(name);
    std::vector<std::vector <double> >  haptic_d;
    for (int i = 0; i < 20; i++) {
        if (std::find(channel_list.begin(), channel_list.end(), i) != channel_list.end()) {
            /* v contains x */
            vector<double> tmp;
            haptic_d.push_back(tmp);
            for (int y = 0; y < toplay->val[0].size(); y++) {
                haptic_d[i].push_back(toplay->val[0][y]);
            }

        }
        else {
            std::vector<double> vec(toplay->val[0].size(), 0);
            haptic_d.push_back(vec);
        }
    }
    data->val = haptic_d;
    data->id_frames = 0;
    data->coeff_intensity = toplay->coeff_intensity;
    data->pitch = toplay->pitch;
    data->isContinuousEffect = toplay->isContinuousEffect;
    return data;
}

/*
* Change the intensity coefficient of the haptic effect.
*/
void MPEG_Haptic_SKINETICDRIVER::change_Intensity(HapticData* data, double intensity) {
    data->coeff_intensity = intensity;

}

/*
* Change the pitch coefficient of the haptic effect.
*/
void MPEG_Haptic_SKINETICDRIVER::change_Pitch(HapticData* data, double pitch) {
    data->pitch = pitch;

}

/*
* Fnct called whyen a stream is finished
* Input:
* - userData -> Data Sound
*/
void MPEG_Haptic_SKINETICDRIVER::StreamFinished(void* userData) {
    std::cout << "################### Stream finished ###################" << endl;
}