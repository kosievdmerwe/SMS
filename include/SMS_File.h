#ifndef __SMS_FILE_H__
#define __SMS_FILE_H__

#include "SMS_Config.h"

#include <stdint.h>
#include <string>
#include <cstdio>

//An wrapper for easy writing and reading of binary files.
//This class only supports reading from files written by it.
class SMS_File {
public:
    enum ErrorType {
        NO_ERROR,        //when there is no error
        UNSUPPORTED_TYPE,//when the type used for opening a file is 
                         //invalid or unsupported
        FAILED_OPENING,  //general error when opening a file fails
        FILE_NOT_OPENED, //reading or writing when no file is opened
        NON_READABLE,    //reading from a non-readable file
        NON_WRITEABLE,   //write to non-writeable file
        INVALID_FORMAT,  //when the file format doesn't fit the specification
        END_OF_FILE,     //reading past end of file
        INVALID_TYPE,    //reading invalid type
        UNKNOWN_ERROR    //Unknown error
    };

public:
    SMS_File();
    //types
    //r - read, w - write
    //valid combinations: r, w
    SMS_File(const std::string& filename, const std::string& type="r");
    virtual ~SMS_File();

    bool IsOpened();
    bool Open(const std::string& filename, const std::string& type="r");
    void Close();

    bool IsWriteable();
    bool IsReadable();
    bool IsEOF();

    bool HasError();
    ErrorType GetError();
    std::string GetErrorString();
    void ClearError();
    
    //All the following functions return true on success and 
    //false on failure
    bool WriteBool(bool val);
    bool WriteInt8(int8_t val);
    bool WriteInt16(int16_t val);
    bool WriteInt32(int32_t val);
    bool WriteInt64(int64_t val);
    bool WriteUInt8(uint8_t val);
    bool WriteUInt16(uint16_t val);
    bool WriteUInt32(uint32_t val);
    bool WriteUInt64(uint64_t val);
    bool WriteFloat(float val);
    bool WriteDouble(double val);
    bool WriteString(const std::string& val);

    //On failure to read these functions return type()
    //And set an error message
    bool ReadBool();
    int8_t ReadInt8();
    int16_t ReadInt16();
    int32_t ReadInt32();
    int64_t ReadInt64();
    uint8_t ReadUInt8();
    uint16_t ReadUInt16();
    uint32_t ReadUInt32();
    uint64_t ReadUInt64();
    float ReadFloat();
    double ReadDouble();
    std::string ReadString();

private:
    //To prevent copying of files
    SMS_File(const SMS_File& other);
    SMS_File& operator=(const SMS_File& other);

    void SetError(ErrorType type, const std::string& message = "");
    bool CheckWrite();
    bool CheckRead();
    bool CheckReadType(char shouldbe, int is);
    bool CheckReadResult(int read);
    bool CheckFileError();
    bool IsValidType(char type);
    std::string GetTypeName(char type);

    template<typename T>
    bool Write(T val);
    template<typename T>
    bool Read(T& result);

    FILE* mFile;    
    std::string mType;
    ErrorType mErrorType;
    std::string mErrorString;
};//SMS_File

#endif//__SMS_FILE_H__
