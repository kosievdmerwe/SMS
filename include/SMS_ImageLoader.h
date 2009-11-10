#ifndef __SMS_IMAGELOADER_H__
#define __SMS_IMAGELOADER_H__

#include "SMS_Config.h"

#include <string>

class SMS_ImageLoader {
public:
    static SMS_ImageLoader& GetInstance();
    
    //loads image from file
    //returns null on failure
    unsigned char* LoadImage(const std::string& filename, 
                             int& width, int& height);
//unloads a image loaded by LoadImage
    void UnloadImage(unsigned char* imageData);

    //Returns a string describing the result of the last action
    std::string GetLastResult();

private:
    SMS_ImageLoader();
    ~SMS_ImageLoader();

    SMS_ImageLoader(const SMS_ImageLoader& other);
    SMS_ImageLoader& operator=(const SMS_ImageLoader& other);
};//SMS_ImageLoader

#endif//__SMS_IMAGELOADER_H__
