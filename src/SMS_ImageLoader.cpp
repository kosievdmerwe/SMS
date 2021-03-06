#include "SMS_ImageLoader.h"
#include "SOIL/SOIL.h"

#include <iostream>
///////////////////////////////////////////////////////////////////////////////
//SMS_ImageLoader
///////////////////////////////////////////////////////////////////////////////
SMS_ImageLoader& SMS_ImageLoader::GetInstance() {
    static SMS_ImageLoader instance;
    return instance;
}//GetInstance
///////////////////////////////////////////////////////////////////////////////
unsigned char* SMS_ImageLoader::LoadImage(const std::string& filename, 
                                          int& width, int& height) {
    int channels;
    return SOIL_load_image(filename.c_str(), 
                           &width, &height, &channels, 
                           SOIL_LOAD_RGBA);
}//LoadImage
///////////////////////////////////////////////////////////////////////////////
void SMS_ImageLoader::UnloadImage(unsigned char* imageData) {
    if (imageData != 0)
        SOIL_free_image_data(imageData);
}//UnloadImage
///////////////////////////////////////////////////////////////////////////////
std::string SMS_ImageLoader::GetLastResult() {
    return SOIL_last_result();
}//GetLastResult
///////////////////////////////////////////////////////////////////////////////
SMS_ImageLoader::SMS_ImageLoader() {
}//SMS_ImageLoader
///////////////////////////////////////////////////////////////////////////////
SMS_ImageLoader::~SMS_ImageLoader() {
}//~SMS_ImageLoader
///////////////////////////////////////////////////////////////////////////////

