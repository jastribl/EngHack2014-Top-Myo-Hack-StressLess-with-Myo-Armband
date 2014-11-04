//
//  main.cpp
//  Hackathon
//
//  Created by Gillian Maducdoc, Jitin Dodd, Justin Stribling, Mart van Buren on 2014-11-01.
//  Copyright (c) 2014 Group 16. All rights reserved.
//

#include "communications.h"

#include <stdio.h>

#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <string>
#include <algorithm>

int currentComputer = 0;
int totalComputers = -1;
float *angles;
std::clock_t lastActionTime = std::clock();

int numSubscribers() {
    if (totalComputers == -1) {
        std::string output;
        
        std::string command = "/usr/local/bin/redis-cli PUBSUB NUMSUB engHack16";
        std::string str = command;
        char * writable = new char[str.size() + 1];
        std::copy(str.begin(), str.end(), writable);
        writable[str.size()] = '\0';
        
        output = exec(writable);
        
        delete[] writable;
        
        std::string lastLine;
        std::istringstream f(output);
        std::string line;
        while (std::getline(f, line)) {
            lastLine = line;
        }
        
        return atoi(lastLine.c_str());
    } else {
        return totalComputers;
    }
}

std::string exec(char* cmd) {
    FILE* pipe = popen(cmd, "r");
    if (!pipe) return "ERROR";
    char buffer[128];
    std::string result = "";
    while(!feof(pipe)) {
        if(fgets(buffer, 128, pipe) != NULL)
            result += buffer;
    }
    pclose(pipe);
    return result;
}

void redisSetKey(std::string key, std::string value) {
    std::string command = "/usr/local/bin/redis-cli set " + key + " " + value;
    std::string str = command;
    char * writable = new char[str.size() + 1];
    std::copy(str.begin(), str.end(), writable);
    writable[str.size()] = '\0';
    
    exec(writable);
    
    delete[] writable;
}
void redisPublish(std::string value) {
    std::string command = "/usr/local/bin/redis-cli PUBLISH engHack16 " + value;
    std::string str = command;
    char * writable = new char[str.size() + 1];
    std::copy(str.begin(), str.end(), writable);
    writable[str.size()] = '\0';
    
    exec(writable);
    
    delete[] writable;
}
void redisPublishArray(int selectedItem, int angle = 0) {
    std::string publish = "[" + std::to_string(currentComputer) + "," + std::to_string(selectedItem) + "," + std::to_string(angle) + "]";
    redisPublish(publish);
}

void changeSelected(int yaw) {
    int computer = 0;
    float minYawDif = 10000;
    
    for (int i = 0; i < totalComputers; i++) {
        float yawDif = fabsf(angles[i]-yaw);
        float yawDif2 = fabsf(angles[i]-(yaw-18)); // Test full circle offset
        float yawDif3 = fabsf(angles[i]-(yaw+18)); // Test full circle offset
        if (yawDif2 < yawDif) yawDif = yawDif2;
        if (yawDif3 < yawDif) yawDif = yawDif3;
        
        if (yawDif < minYawDif) {
            minYawDif = yawDif;
            computer = i;
        }
    }
    
    if (currentComputer != computer) {
        currentComputer = computer;
        redisPublishArray(0);
    }
}

void action(int code, int info) {
    double duration = ( std::clock() - lastActionTime ) / (double) CLOCKS_PER_SEC;
    duration *= 100;
    //std::cout << "Time elapsed: " << duration << std::endl;
    if (duration > 0.25) {
        redisPublishArray(code, info);
        lastActionTime = std::clock();
        std::cout << "Action " << std::to_string(code) << " triggered (Extra Info: " << info << ")" << std::endl;
    }
}

void initialSetup() {
    totalComputers = numSubscribers();
    if (totalComputers == 0) {
        std::cerr << "Error: No subscribed computers to destroy :(" << std::endl;
        std::cin.ignore();
    }
    angles = (float *) malloc(totalComputers * sizeof(float));
    std::cout << std::to_string(totalComputers) << " Subscribers" << std::endl;
    std::cout << "Point at every computer and stretch your fingers in order from left to right" << std::endl;
}

void setAngle(int computer, float yaw) {
    angles[computer] = yaw;
    std::cout << "Setting computer " << computer << " to yaw " << yaw << std::endl;
}