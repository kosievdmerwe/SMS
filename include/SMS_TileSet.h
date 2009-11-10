#ifndef __SMS_TILESET_H__
#define __SMS_TILESET_H__

#include "SMS_Config.h"

#include <stdint.h>
#include <string>
#include <vector>

#include "SMS_Tile.h"

class SMS_TileSet {
public:
    enum ErrorType {
        NO_ERROR,
        IMAGE_LOAD_FAIL,
        ALPHA_IMAGE_LOAD_FAIL,
        IMAGES_SIZES_DIFFER,
        TEXTURES_LOAD_FAIL,
        TILE_SIZE_IS_ZERO,
        TILE_SIZE_TOO_BIG,
        UNKNOWN_ERROR
    };
public:
    SMS_TileSet();
    SMS_TileSet(const std::string& filename);
    virtual ~SMS_TileSet();

    bool IsOpened();
    bool Open(const std::string& filename);
    void Close();
    bool Save(const std::string& filename);

    bool HasError();
    ErrorType GetError();
    std::string GetErrorString();
    void ClearError();

    void SetImage(const std::string& filename);
    void SetHasAlphaImage(bool hasAlphaImage);
    void SetAlphaImage(const std::string& filename);
    std::string GetImageName();
    std::string GetAlphaImageName();
    bool GetHasAlphaImage();
    bool ReloadImages();
    
    bool SetTileWidth(uint16_t width);
    bool SetTileHeight(uint16_t height);
    bool SetTileSize(uint16_t width, uint16_t height);
    uint16_t GetTileWidth();
    uint16_t GetTileHeight();

    uint32_t GetNumTiles();
    SMS_Tile GetTile(uint32_t tilenum);//returns default tile on failure
    bool InsertTile(uint32_t tilenum, const SMS_Tile& tile);
    bool ReplaceTile(uint32_t tilenum, const SMS_Tile& tile);
    bool PushTile(const SMS_Tile& tile);
    bool DeleteTile(uint32_t tilenum);
    
    uint32_t GetNumTileBlocks();

    bool IsValid();
    bool Validize();

    void SetCurrentTime(uint32_t time);
    uint32_t GetCurrentTime();

    bool DrawTile(uint32_t tilenum, int x, int y);

    void BeginBatch();
    bool DrawTileBatch(uint32_t tilenum, int x, int y);
    void EndBatch();

private:
    //prevent copying
    SMS_TileSet(const SMS_TileSet& other);
    SMS_TileSet& operator=(const SMS_TileSet& other);
    
    void SetError(ErrorType type, const std::string& message = "");

    void Reset();

    bool IsTileSizeValid(uint16_t size, bool isWidth);

    
    //sets error on failure and frees textures from opengl
    //Assumes the texture ids have been stored in mTextures,
    //the width and height of the textures in mTextureWidth and mTextureHeight
    //and the tile width and height in mTileWidth and mTileHeight
    //and that the number of tileblocks has been set
    bool LoadTextures(unsigned char* image, int imageWidth, int imageHeight);
    void UnloadTextures();

private:
    bool mOpened;

    uint16_t mTileWidth;
    uint16_t mTileHeight;
    std::string mImageName;
    std::string mAlphaImageName;
    bool mHasAlphaImage;
    uint32_t mNumTileBlocks;
    
    std::vector<unsigned int> mTextures;
    uint32_t mTextureWidth;
    uint32_t mTextureHeight;
    std::vector<SMS_Tile> mTiles;

    uint32_t mCurrentTexture;
    bool mBatchMode;

    uint32_t mCurrentTime;

    ErrorType mErrorType;
    std::string mErrorString;
};//SMS_TileSet

#endif//__SMS_TILESET_H__
