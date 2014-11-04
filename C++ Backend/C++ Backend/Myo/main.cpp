//
//  main.cpp
//  Hackathon
//
//  Created by Gillian Maducdoc, Jitin Dodd, Justin Stribling, Mart van Buren on 2014-11-01.
//  Copyright (c) 2014 Group 16. All rights reserved.
//

#include <stdio.h>

#define _USE_MATH_DEFINES
#include <cmath>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <string>
#include <algorithm>
#include <cstdio>
#include <ctime>

#include <myo/myo.hpp>

#include "communications.h"

int inSelectionMode = 0;
int inAngleSelectionMode = 1;
int angleSetupCurrentComputer = 0;
int minRateOfChange = 1500;
int minRateOfChangeBullet = 1300;

class MyoListener : public myo::DeviceListener {
public:
    
    std::string lastPose;
    std::string secondLastPose;
    
    std::clock_t lastGunMovement = 0;
    
    int roll_w;
    int pitch_w;
    int yaw_w;
    float roll_f;
    float pitch_f;
    float yaw_f;
    
    std::clock_t timerStart = std::clock();
    float last_roll;
    float last_pitch;
    float last_yaw;
    
    double rateOfChangeRoll;
    double rateOfChangePitch;
    double rateOfChangeYaw;
    
    std::clock_t lastTimeHorizontal = std::clock();
    std::clock_t lastTimeVertical = 0;
    std::clock_t shouldShootRifle = 0;
    
    std::clock_t lastTimeBackward = std::clock();
    std::clock_t lastTimeVerticalBow = 0;
    std::clock_t shouldFireBow = 0;
    
    void onOrientationData(myo::Myo* myo, uint64_t timestamp, const myo::Quaternion<float>& quat)
    {
        using std::atan2;
        using std::asin;
        using std::sqrt;
        using std::max;
        using std::min;
        
        // Calculate Euler angles (roll, pitch, and yaw) from the unit quaternion.
        float roll = atan2(2.0f * (quat.w() * quat.x() + quat.y() * quat.z()),
                           1.0f - 2.0f * (quat.x() * quat.x() + quat.y() * quat.y()));
        float pitch = asin(max(-1.0f, min(1.0f, 2.0f * (quat.w() * quat.y() - quat.z() * quat.x()))));
        float yaw = atan2(2.0f * (quat.w() * quat.z() + quat.x() * quat.y()),
                          1.0f - 2.0f * (quat.y() * quat.y() + quat.z() * quat.z()));
        
        roll_f = ((roll + (float)M_PI)/(M_PI * 2.0f) * 18);
        pitch_f = ((pitch + (float)M_PI/2.0f)/M_PI * 18);
        yaw_f = ((yaw + (float)M_PI)/(M_PI * 2.0f) * 18);
        
        // Convert the floating point angles in radians to a scale from 0 to 18.
        roll_w = static_cast<int>((roll + (float)M_PI)/(M_PI * 2.0f) * 18);
        pitch_w = static_cast<int>((pitch + (float)M_PI/2.0f)/M_PI * 18);
        yaw_w = static_cast<int>((yaw + (float)M_PI)/(M_PI * 2.0f) * 18);
        if (inSelectionMode)
            std::cout << "Roll: " << roll_w << " | Pitch: " << pitch_w << " | Yaw: " << yaw_w << std::endl;
        
        if (inSelectionMode) changeSelected(yaw_f);
        
        if (pitch_f <= 5) lastTimeVertical = std::clock();
        if (pitch_f <= 10 && pitch_f >= 6) {
            if (lastTimeHorizontal < lastTimeVertical) {
                double timeBetweenHorizontalAndVerticle = (lastTimeVertical - lastTimeHorizontal) / (double) CLOCKS_PER_SEC;
                timeBetweenHorizontalAndVerticle *= 100;
                double timeSinceVerticle = ( std::clock() - lastTimeVertical ) / (double) CLOCKS_PER_SEC;
                timeSinceVerticle *= 100;
                if (timeBetweenHorizontalAndVerticle < 1 && timeSinceVerticle < 1) {
                    shouldShootRifle = std::clock();
                }
            }
            lastTimeHorizontal = std::clock();
        }
        
        if (pitch_f >= 6) lastTimeBackward = std::clock();
        if (pitch_f <= 5) {
            if (lastTimeVerticalBow < lastTimeBackward) {
                double timeBetweenVerticleAndBackward = (lastTimeBackward - lastTimeVerticalBow) / (double) CLOCKS_PER_SEC;
                timeBetweenVerticleAndBackward *= 100;
                std::cout << "Time between verticle and backward: " << timeBetweenVerticleAndBackward << std::endl;
                double timeSinceBackward = ( std::clock() - lastTimeBackward ) / (double) CLOCKS_PER_SEC;
                timeSinceBackward *= 100;
                std::cout << "Time since backward: " << timeSinceBackward << std::endl;
                if (timeBetweenVerticleAndBackward < 2 && timeSinceBackward < 2) {
                    shouldFireBow = std::clock();
                    std::cout << "Should fire bow at: " << shouldFireBow << std::endl;
                }
            }
            lastTimeVerticalBow = std::clock();
        }
        
        double duration = ( std::clock() - timerStart ) / (double) CLOCKS_PER_SEC;
        rateOfChangeRoll = -(last_roll - roll_f)/duration;
        rateOfChangePitch = -(last_pitch - pitch_f)/duration;
        rateOfChangeYaw = -(last_yaw - yaw_f)/duration;
        
        //std::cout << "Acceleration " << accelerationPitch << " | Duration: " << duration << " | Rate of change: " << rateOfChangePitch << std::endl;
        
        if (lastPose != "fingersSpread" && pitch_f <= 11 && pitch_f >= 7 && fabsf(rateOfChangePitch) >= minRateOfChangeBullet) {
            //std::cout << "BANG!" << std::endl;
            double timeSinceLastGunMovement = ( std::clock() - lastGunMovement ) / (double) CLOCKS_PER_SEC;
            timeSinceLastGunMovement *= 100;
            if (timeSinceLastGunMovement < 0.25 && rateOfChangePitch < 0) {
                double timeSinceShouldShootRifle = ( std::clock() - shouldShootRifle ) / (double) CLOCKS_PER_SEC;
                timeSinceShouldShootRifle *= 100;
                if (timeSinceShouldShootRifle < 1) {
                    //myo->vibrate(myo::Myo::vibrationMedium);
                    if (!inAngleSelectionMode) action(4);
                    shouldShootRifle = 0;
                } else {
                    if (!inAngleSelectionMode) action(3);
                }
            } else {
                lastGunMovement = std::clock();
            }
        } else if (lastPose == "fingersSpread" && pitch_f <= 5 && rateOfChangePitch >= minRateOfChange) {
            //std::cout << "OUCH KITTY!" << std::endl;
            if (!inAngleSelectionMode && !inSelectionMode) action(2);
        } else if (lastPose == "fingersSpread" && fabsf(rateOfChangeYaw) >= minRateOfChange) {
            //std::cout << "OUCH KITTY!" << std::endl;
            if (!inAngleSelectionMode && !inSelectionMode) action(7);
        } else if (lastPose != "fingersSpread" && pitch_f <= 3 && fabsf(rateOfChangePitch) >= minRateOfChange) {
            //std::cout << "KABLOUI" << std::endl;
            //myo->vibrate(myo::Myo::vibrationLong);
            if (!inAngleSelectionMode) action(5);
        } else if (lastPose == "fist" && fabsf(rateOfChangeYaw) >= minRateOfChange) {
            //std::cout << "BLACK EYE" << std::endl;
            if (!inAngleSelectionMode) action(1);
        }
        
        timerStart = std::clock();
        last_roll = roll_f;
        last_pitch = pitch_f;
        last_yaw = yaw_f;
    }
    
    void onPair(myo::Myo* myo, uint64_t timestamp, myo::FirmwareVersion firmwareVersion)
    {
        knownMyos.push_back(myo);
        std::cout << "Paired with " << identifyMyo(myo) << "." << std::endl;
    }
    
    void onPose(myo::Myo* myo, uint64_t timestamp, myo::Pose pose)
    {
        std::string poseString = pose.toString();
        if (pose == myo::Pose::fingersSpread) {
            if (inAngleSelectionMode) {
                setAngle(angleSetupCurrentComputer, yaw_f);
                myo->vibrate(myo::Myo::vibrationMedium);
                angleSetupCurrentComputer++;
                if (angleSetupCurrentComputer == numSubscribers()) {
                    inAngleSelectionMode = false;
                    //inSelectionMode = true;
                }
                poseString = "rest";
            } else if (inSelectionMode) {
                inSelectionMode = false;
                myo->vibrate(myo::Myo::vibrationShort);
                poseString = "rest";
            } else if (!inSelectionMode && pitch_f >= 16) {
                inSelectionMode = true;
                myo->vibrate(myo::Myo::vibrationShort);
                poseString = "rest";
            } else if (pitch_f >= 5 && pitch_f <= 10) {
                double timeSinceAllowedToFireBow = ( std::clock() - shouldFireBow ) / (double) CLOCKS_PER_SEC;
                timeSinceAllowedToFireBow *= 100;
                std::cout << "Time since allowed to fire bow: " << timeSinceAllowedToFireBow << std::endl;
                if (timeSinceAllowedToFireBow < 4) {
                    if (!inAngleSelectionMode && !inSelectionMode) action(6);
                    shouldFireBow = 0;
                }
            }
        } else if (pose == myo::Pose::waveOut) {
            if (lastPose == "waveIn" && secondLastPose == "waveOut") action(8);
        } else if (pose == myo::Pose::waveIn) {
            if (lastPose == "waveOut" && secondLastPose == "waveIn") action(8);
        } else if (pose == myo::Pose::rest || pose == myo::Pose::fingersSpread) {
            return;
        }
        secondLastPose = lastPose;
        lastPose = poseString;
        poseString += " (Roll: " + std::to_string((int)roll_w) + ")";
        std::cout << "Myo " << identifyMyo(myo) << " switched to pose " << poseString << "." << std::endl;
    }
    
    void onConnect(myo::Myo* myo, uint64_t timestamp, myo::FirmwareVersion firmwareVersion)
    {
        std::cout << "Myo " << identifyMyo(myo) << " has connected." << std::endl;
    }
    
    size_t identifyMyo(myo::Myo* myo) {
        for (size_t i = 0; i < knownMyos.size(); ++i) {
            if (knownMyos[i] == myo) {
                return i + 1;
            }
        }
        return 0;
    }
    
    void onDisconnect(myo::Myo* myo, uint64_t timestamp)
    {
        std::cout << "Myo " << identifyMyo(myo) << " has disconnected." << std::endl;
    }
    
    std::vector<myo::Myo*> knownMyos;
};

int main(int argc, const char * argv[]) {
    initialSetup();
    
    try {
        myo::Hub hub("com.hackathon.myo");
        
        // Instantiate the PrintMyoEvents class we defined above, and attach it as a listener to our Hub.
        MyoListener listener;
        hub.addListener(&listener);
        
        while (1) {
            // Process events for 10 milliseconds at a time.
            hub.run(10);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        std::cerr << "Press enter to continue.";
        std::cin.ignore();
    }
}