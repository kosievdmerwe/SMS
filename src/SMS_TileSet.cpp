#include "SMS_TileSet.h"
#include "SMS_OpenGL.h"
#include "SMS_ImageLoader.h"

#include <vector>
#include <utility>
#include <algorithm>

//File format version 1.00
//-----------
//<string>"SMS TileSet" #header
//<uint16_t>100         #version
//
//<string> *image name*
//<bool> *has alpha image* [<string> *alpha image*]
//
//<uint16_t> *tile width*
//<uint16_t> *tile height*
//
//<uint32_t> *num tiles*
//
//#static tiles
//<uint8_t> 0
//<uint32_t> *tile block*
//
//#animated tiles
//<uint8_t> 1
//<uint16_t> *animation type*
//<uint32_t> *delay*
//<uint32_t> *num frames*
//[<uint32_t> *tile block*]+

//Internal constants and structs
namespace {
    //Only consider this amount of extra textures
    const uint32_t MAX_NUM_EXTRA_TEXTURES = 3;


    //When loading textures this is used to store possible texture sizes
    //So an optimal size can be selected
    struct PossibleTexSize {
        PossibleTexSize(int textureWidth, int textureHeight, 
                        size_t numTextures, uint32_t wastage) :
            mTextureWidth(textureWidth), mTextureHeight(textureHeight),
            mNumTextures(numTextures), mWastage(wastage)
        {};

        bool operator<(const PossibleTexSize& other) const {
            return mNumTextures < other.mNumTextures || 
                (mNumTextures == other.mNumTextures && mWastage < other.mWastage);
        }

        int mTextureWidth;
        int mTextureHeight;
        uint32_t mNumTextures;
        uint32_t mWastage;
    };
}


///////////////////////////////////////////////////////////////////////////////
//SMS_TileSet
///////////////////////////////////////////////////////////////////////////////
SMS_TileSet::SMS_TileSet() {
    Reset();
}//SMS_TileSet
///////////////////////////////////////////////////////////////////////////////
SMS_TileSet::SMS_TileSet(const std::string& filename) {
    Reset();
    Open(filename);
}//SMS_TileSet
///////////////////////////////////////////////////////////////////////////////
SMS_TileSet::~SMS_TileSet() {
    Reset();
}//~SMS_TileSet
///////////////////////////////////////////////////////////////////////////////
bool SMS_TileSet::IsOpened() {
    return mOpened;
}//IsOpened
///////////////////////////////////////////////////////////////////////////////
bool SMS_TileSet::Open(const std::string& filename) {
}//Open
///////////////////////////////////////////////////////////////////////////////
void SMS_TileSet::Close() {
    Reset();
}//Close
///////////////////////////////////////////////////////////////////////////////
bool SMS_TileSet::Save(const std::string& filename) {
}//Save
///////////////////////////////////////////////////////////////////////////////
bool SMS_TileSet::HasError() {
    return mErrorType!=NO_ERROR;
}//HasError
///////////////////////////////////////////////////////////////////////////////
SMS_TileSet::ErrorType SMS_TileSet::GetError() {
    return mErrorType;
}//GetError
///////////////////////////////////////////////////////////////////////////////
std::string SMS_TileSet::GetErrorString() {
    return mErrorString;
}//GetErrorString
///////////////////////////////////////////////////////////////////////////////
void SMS_TileSet::ClearError() {
    SetError(NO_ERROR);
}//ClearError
///////////////////////////////////////////////////////////////////////////////
void SMS_TileSet::SetImage(const std::string& filename) {
    mImageName = filename;
}//SetImage
///////////////////////////////////////////////////////////////////////////////
void SMS_TileSet::SetHasAlphaImage(bool hasAlphaImage) {
    mHasAlphaImage = hasAlphaImage;
}//SetHasAlphaImage
///////////////////////////////////////////////////////////////////////////////
void SMS_TileSet::SetAlphaImage(const std::string& filename) {
    mAlphaImageName = filename;
    if (filename != "") {
        SetHasAlphaImage(true);
    } else {
        SetHasAlphaImage(false);
    }
}//SetAlphaImage
///////////////////////////////////////////////////////////////////////////////
std::string SMS_TileSet::GetImageName() {
    return mImageName;
}//GetImageName
///////////////////////////////////////////////////////////////////////////////
std::string SMS_TileSet::GetAlphaImageName() {
    return mAlphaImageName;
}//GetAlphaImageName
///////////////////////////////////////////////////////////////////////////////
bool SMS_TileSet::GetHasAlphaImage() {
    return mHasAlphaImage;
}//GetHasAlphaImage
///////////////////////////////////////////////////////////////////////////////
#include <iostream>
using namespace std;
bool SMS_TileSet::ReloadImages() {
    UnloadTextures();

    if (!(IsTileSizeValid(mTileWidth, true) && IsTileSizeValid(mTileHeight, false)))
        return false;

    SMS_ImageLoader& il = SMS_ImageLoader::GetInstance();

    int imageWidth, imageHeight;
    unsigned char* image = il.LoadImage(mImageName, imageWidth, imageHeight);

    //check if image loaded
    if (image == 0) {
        std::string errorMessage = "Image failed to load: ";
        errorMessage += il.GetLastResult();
        SetError(IMAGE_LOAD_FAIL, errorMessage);
        return false;
    }

    int alphaImageWidth, alphaImageHeight;
    unsigned char* alphaImage = 0;
    if (mHasAlphaImage) {
        alphaImage = il.LoadImage(mAlphaImageName, alphaImageWidth, alphaImageHeight);
        //check if alpha image loaded
        if (image == 0) {
            std::string errorMessage = "Alpha image failed to load: ";
            errorMessage += il.GetLastResult();
            SetError(IMAGE_LOAD_FAIL, errorMessage);
            il.UnloadImage(image);
            return false;
        }

        //check file sizes match
        if (imageWidth != alphaImageWidth || imageHeight != alphaImageHeight) {
            SetError(IMAGES_SIZES_DIFFER);
            il.UnloadImage(image);
            il.UnloadImage(alphaImage);
            return false;
        }

        //set alpha from alpha image, using only red for speed issues
        for (int i = 0; i < imageWidth * imageHeight; ++i) {
            image[i*4+3] = alphaImage[i*4];
        }
        il.UnloadImage(alphaImage);
        alphaImage = NULL;
    }

    GLint maxTextureSize = 0;
    GLCheck(glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize));

    //Max power of two size
    int maxPOTSize = 1;
    while (maxPOTSize < maxTextureSize)
        maxPOTSize *= 2;

    uint32_t mNumTileBlocks = (imageWidth/mTileWidth)*(imageHeight/mTileHeight);

    std::vector<PossibleTexSize> possibleSizes;

    for (int texWidth = maxPOTSize; texWidth > 0; texWidth /= 2) {
        if (texWidth < mTileWidth)
            break;
        for (int texHeight = texWidth; texHeight > 0; texHeight /= 2) {
            if (texHeight < mTileHeight)
                break;

            uint32_t numColumns = texWidth/mTileWidth;
            uint32_t numRows = texHeight/mTileHeight;

            uint32_t numTextures = mNumTileBlocks/(numColumns*numRows);
            numTextures += (mNumTileBlocks%(numColumns*numRows) > 0)?1:0;

            uint32_t extraTileBlocks = mNumTileBlocks%(numColumns*numRows);
            uint32_t wastedTileBlocks = numColumns*numRows - 
                mNumTileBlocks%(numColumns*numRows);
            if (extraTileBlocks == 0)
                wastedTileBlocks = 0;

            //wasted space due to tiles not filling textures
            uint32_t wastage = numTextures*
                (texWidth*texHeight-mTileWidth*mTileHeight*numColumns*numRows);
            wastage += mTileWidth*mTileHeight*(wastedTileBlocks);

            //either add to possible sizes
            //or if the number of textures already and the wastage is less
            //then replace that possible size
            size_t index = possibleSizes.size();
            for (size_t i = 0; i < possibleSizes.size(); ++i) {
                if (possibleSizes[i].mNumTextures == numTextures) {
                    index = i;
                    break;
                }
            }
            if (index == possibleSizes.size()) {
                possibleSizes.push_back(PossibleTexSize(texWidth, texHeight, 
                                                        numTextures, wastage));
            } else {
                if (wastage < possibleSizes[index].mWastage) {
                    possibleSizes[index].mTextureWidth = texWidth;
                    possibleSizes[index].mTextureHeight = texHeight;
                    possibleSizes[index].mWastage = wastage;
                }
            }
        }
    }

    std::sort(possibleSizes.begin(), possibleSizes.end());
    
    size_t minIndex = 0;
    for (size_t i = 0; i < possibleSizes.size(); ++i) {
        if (possibleSizes[i].mNumTextures - possibleSizes[0].mNumTextures > 
            MAX_NUM_EXTRA_TEXTURES)
            break;
        if (possibleSizes[i].mWastage < possibleSizes[minIndex].mWastage)
            minIndex = i;
    }
    mTextureWidth = possibleSizes[minIndex].mTextureWidth;
    mTextureHeight = possibleSizes[minIndex].mTextureHeight;
    uint32_t numTextures = possibleSizes[minIndex].mNumTextures;
    
    GLuint* textures = new GLuint[numTextures];
    GLCheck(glGenTextures(numTextures, textures));
    for (uint32_t i = 0; i < numTextures; ++i)
        mTextures.push_back(static_cast<unsigned int>(textures[i]));
    delete[] textures;
    
    if (!LoadTextures(image, imageWidth, imageHeight)) {
        il.UnloadImage(image);

        return false;
    }

    il.UnloadImage(image);
    return true;
}//ReloadImages
///////////////////////////////////////////////////////////////////////////////
bool SMS_TileSet::SetTileWidth(uint16_t width) {
    if (!IsTileSizeValid(width, true)) {
        return false;
    }
    mTileWidth = width;
    return true;
}//SetTileWidth
///////////////////////////////////////////////////////////////////////////////
bool SMS_TileSet::SetTileHeight(uint16_t height) { if (!IsTileSizeValid(height, false)) {
    }
    mTileHeight = height;
    return true;
}//SetTileHeight
///////////////////////////////////////////////////////////////////////////////
bool SMS_TileSet::SetTileSize(uint16_t width, uint16_t height) {
    return SetTileWidth(width) && SetTileHeight(height);
}//SetTileSize
///////////////////////////////////////////////////////////////////////////////
uint16_t SMS_TileSet::GetTileWidth() {
    return mTileWidth;
}//GetTileWidth
///////////////////////////////////////////////////////////////////////////////
uint16_t SMS_TileSet::GetTileHeight() {
    return mTileHeight;
}//GetTileHeight
///////////////////////////////////////////////////////////////////////////////
uint32_t SMS_TileSet::GetNumTiles() {
    return mTiles.size();
}//GetNumTiles
///////////////////////////////////////////////////////////////////////////////
SMS_Tile SMS_TileSet::GetTile(uint32_t tilenum) {
}//GetTile<<
///////////////////////////////////////////////////////////////////////////////
bool SMS_TileSet::InsertTile(uint32_t tilenum, const SMS_Tile& tile) {
}//InsertTile
///////////////////////////////////////////////////////////////////////////////
bool SMS_TileSet::ReplaceTile(uint32_t tilenum, const SMS_Tile& tile) {
}//ReplaceTile
///////////////////////////////////////////////////////////////////////////////
bool SMS_TileSet::PushTile(const SMS_Tile& tile) {
}//PushTile
///////////////////////////////////////////////////////////////////////////////
bool SMS_TileSet::DeleteTile(uint32_t tilenum) {
}//DeleteTile
///////////////////////////////////////////////////////////////////////////////
uint32_t SMS_TileSet::GetNumTileBlocks() {
    return mNumTileBlocks;
}//GetNumTileBlocks
///////////////////////////////////////////////////////////////////////////////
bool SMS_TileSet::IsValid() {
}//IsValid
///////////////////////////////////////////////////////////////////////////////
bool SMS_TileSet::Validize() {
}//Validize
///////////////////////////////////////////////////////////////////////////////
void SMS_TileSet::SetCurrentTime(uint32_t time) {
    mCurrentTime = time;
}//SetCurrentTime
///////////////////////////////////////////////////////////////////////////////
uint32_t SMS_TileSet::GetCurrentTime() {
    return mCurrentTime;
}//GetCurrentTime
///////////////////////////////////////////////////////////////////////////////
bool SMS_TileSet::DrawTile(uint32_t tilenum, int x, int y) {
    //TODO: fix to work
    if (mTextures.size() > 0) {
        GLCheck(glEnable(GL_TEXTURE_2D));
        GLCheck(glBindTexture(GL_TEXTURE_2D, mTextures[0]));
        glBegin(GL_QUADS);
            glTexCoord2f(0, 0); glVertex2i(x, y);
            glTexCoord2f(0, 1); glVertex2i(x, y+mTextureHeight/2);
            glTexCoord2f(1, 1); glVertex2i(x+mTextureWidth/2, y+mTextureHeight/2);
            glTexCoord2f(1, 0); glVertex2i(x+mTextureWidth/2, y);
        glEnd();
    }

}//DrawTile
///////////////////////////////////////////////////////////////////////////////
void SMS_TileSet::BeginBatch() {
}//BeginBatch
///////////////////////////////////////////////////////////////////////////////
bool SMS_TileSet::DrawTileBatch(uint32_t tilenum, int x, int y) {
}//DrawTileBatch
///////////////////////////////////////////////////////////////////////////////
void SMS_TileSet::EndBatch() {
}//EndBatch
///////////////////////////////////////////////////////////////////////////////
void SMS_TileSet::SetError(SMS_TileSet::ErrorType type, const std::string& message) {
    mErrorType = type;
    mErrorString = message;
}//SetError
///////////////////////////////////////////////////////////////////////////////
void SMS_TileSet::Reset() {
    mOpened = false;
    mTileWidth = mTileHeight = 0;
    mImageName = "";
    mAlphaImageName = "";
    mNumTileBlocks = 0;
    mHasAlphaImage = false;

    UnloadTextures();
    mTiles = std::vector<SMS_Tile>();

    mCurrentTexture = 0;
    mBatchMode = false;

    mCurrentTime = 0;

    SetError(NO_ERROR);
}//Reset
///////////////////////////////////////////////////////////////////////////////
bool SMS_TileSet::IsTileSizeValid(uint16_t size, bool isWidth) {
    std::string errorMessage = (isWidth)?"Width ":"Height ";
    if (size == 0) {
        errorMessage += "is zero.";
        SetError(TILE_SIZE_IS_ZERO, errorMessage);
        return false;
    }
    
    GLint maxTextureSize = 0;
    GLCheck(glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize));
    if (size > maxTextureSize) {
        errorMessage += "is too large for system.";
        SetError(TILE_SIZE_TOO_BIG, errorMessage);
        return false;
    }

    return true;
}//IsTileSizeValid
///////////////////////////////////////////////////////////////////////////////
bool SMS_TileSet::LoadTextures(unsigned char* image, int imageWidth, int imageHeight) {
    uint32_t tileBlocksPerTex = (mTextureWidth/mTileWidth) * 
                                (mTextureHeight/mTileHeight);

    size_t bufferlen = mTextureWidth*mTextureHeight*4;
    unsigned char* buffer = new unsigned char[bufferlen];
    for (size_t curtex = 0; curtex < mTextures.size(); ++curtex) {
        //reset buffer to transparent
        for (size_t i = 0; i < bufferlen; ++i)
            buffer[i] = 0;

        //copy tiles over
        //TODO
        
        //create texture
        GLCheck(glBindTexture(GL_TEXTURE_2D, mTextures[curtex]));
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 
                     mTextureWidth, mTextureHeight, 0, 
                     GL_RGBA, GL_UNSIGNED_BYTE, buffer);
        if (glGetError() != GL_NO_ERROR) {
            SetError(TEXTURES_LOAD_FAIL);
            UnloadTextures();
            delete[] buffer;
            return false;
        }

        GLCheck(glTexParameteri(GL_TEXTURE_2D, 
                                GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GLCheck(glTexParameteri(GL_TEXTURE_2D, 
                                GL_TEXTURE_MAG_FILTER, GL_NEAREST));
        GLCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP));
        GLCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP));
    }
    UnloadTextures();
    delete[] buffer;
    return true;
}//LoadTextures
///////////////////////////////////////////////////////////////////////////////
void SMS_TileSet::UnloadTextures() {
    for (size_t i = 0; i < mTextures.size(); ++i)  {
        GLCheck(glDeleteTextures(1, &mTextures[i]));
    }
    mTextures = std::vector<unsigned int>();
    mTextureWidth = mTextureHeight = 0;
}//UnloadTextures
///////////////////////////////////////////////////////////////////////////////
