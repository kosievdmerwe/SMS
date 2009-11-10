#include "SMS_File.h"

//TODO: add endianness support

//Internal File Format
//<type> <data>
//<type> is the type of binary data and <data> is the actual data
//Strings have a special type that indicate the length of the string data
//
//Types:
//bool    : t (truth)
//int8_t  : b (byte)
//int16_t : s (short)
//int32_t : i (int)
//int64_t : l (long)
//uint8_t : B
//uint16_t: S
//uint32_t: I
//uint64_t: L
//float   : f (float)
//double  : d (double)
//string  : c<len> (cstring)(where <len> is 4 bytes describing the length)

///////////////////////////////////////////////////////////////////////////////
//SMS_File
///////////////////////////////////////////////////////////////////////////////
SMS_File::SMS_File() {
    mFile = NULL;
    ClearError();
}//SMS_File
///////////////////////////////////////////////////////////////////////////////
SMS_File::SMS_File(const std::string& filename, const std::string& type) {
    mFile = NULL;
    ClearError();
    Open(filename, type);
}//SMS_File
///////////////////////////////////////////////////////////////////////////////
SMS_File::~SMS_File() {
    Close();
}//~SMS_File
///////////////////////////////////////////////////////////////////////////////
bool SMS_File::IsOpened() {
    return mFile != NULL;
}//IsOpened
///////////////////////////////////////////////////////////////////////////////
bool SMS_File::Open(const std::string& filename, const std::string& type) {
    Close();
    if (type != "w" && type != "r") {
        SetError(UNSUPPORTED_TYPE);
        return false;
    }
    mType = type; 
    mFile = fopen(filename.c_str(), (type+"b").c_str());
    if (mFile == NULL) {
        SetError(FAILED_OPENING);
        mType = "";
        return false;
    }
    return true;
}//Open
///////////////////////////////////////////////////////////////////////////////
void SMS_File::Close() {
    if (IsOpened())
        fclose(mFile);
    mFile = NULL;
    ClearError();
}//Close
///////////////////////////////////////////////////////////////////////////////
bool SMS_File::IsWriteable() {
    if (IsOpened())
        return mType == "w";
    return true;
}//IsWriteable ///////////////////////////////////////////////////////////////////////////////
bool SMS_File::IsReadable() {
    if (IsOpened())
        return mType == "r";
    return false;
}//IsReadable
///////////////////////////////////////////////////////////////////////////////
bool SMS_File::IsEOF() {
    if (IsOpened())
        return feof(mFile) != 0;
    return false;
}//IsEOF
///////////////////////////////////////////////////////////////////////////////
bool SMS_File::HasError() {
    return mErrorType != NO_ERROR;
}//HasError
///////////////////////////////////////////////////////////////////////////////
SMS_File::ErrorType SMS_File::GetError() {
    return mErrorType;
}//GetError
///////////////////////////////////////////////////////////////////////////////
std::string SMS_File::GetErrorString() {
    return mErrorString;
}//GetErrorString
///////////////////////////////////////////////////////////////////////////////
void SMS_File::ClearError() {
    SetError(NO_ERROR);
}//ClearError
///////////////////////////////////////////////////////////////////////////////
bool SMS_File::WriteBool(bool val) {
    if (!CheckWrite())
        return false;

    int8_t temp = (val)?1:0;
    fputc('t', mFile);
    if (!Write(temp))
        return false;

    return CheckFileError();
}//WriteBool
///////////////////////////////////////////////////////////////////////////////
bool SMS_File::WriteInt8(int8_t val) {
    if (!CheckWrite())
        return false;

    fputc('b', mFile);
    if (!Write(val))
        return false;

    return CheckFileError();
}//WriteInt8
///////////////////////////////////////////////////////////////////////////////
bool SMS_File::WriteInt16(int16_t val) {
    if (!CheckWrite())
        return false;

    fputc('s', mFile);
    if (!Write(val))
        return false;

    return CheckFileError();
}//WriteInt16
///////////////////////////////////////////////////////////////////////////////
bool SMS_File::WriteInt32(int32_t val) {
    if (!CheckWrite())
        return false;

    fputc('i', mFile);
    if (!Write(val))
        return false;

    return CheckFileError();
}//WriteInt32
///////////////////////////////////////////////////////////////////////////////
bool SMS_File::WriteInt64(int64_t val) {
    if (!CheckWrite())
        return false;

    fputc('l', mFile);
    if (!Write(val))
        return false;

    return CheckFileError();
}//WriteInt64
///////////////////////////////////////////////////////////////////////////////
bool SMS_File::WriteUInt8(uint8_t val) {
    if (!CheckWrite())
        return false;

    fputc('B', mFile);
    if (!Write(val))
        return false;

    return CheckFileError();
}//WriteUInt8
///////////////////////////////////////////////////////////////////////////////
bool SMS_File::WriteUInt16(uint16_t val) {
    if (!CheckWrite())
        return false;

    fputc('S', mFile);
    if (!Write(val))
        return false;

    return CheckFileError();
}//WriteUInt16
///////////////////////////////////////////////////////////////////////////////
bool SMS_File::WriteUInt32(uint32_t val) {
    if (!CheckWrite())
        return false;

    fputc('I', mFile);
    if (!Write(val))
        return false;

    return CheckFileError();
}//WriteUInt32
///////////////////////////////////////////////////////////////////////////////
bool SMS_File::WriteUInt64(uint64_t val) {
    if (!CheckWrite())
        return false;

    fputc('L', mFile);
    if (!Write(val))
        return false;

    return CheckFileError();
}//WriteUInt64
///////////////////////////////////////////////////////////////////////////////
bool SMS_File::WriteFloat(float val) {
    if (!CheckWrite())
        return false;

    fputc('f', mFile);

    if (!Write(val))
        return false;

    return CheckFileError();
}//WriteFloat
///////////////////////////////////////////////////////////////////////////////
bool SMS_File::WriteDouble(double val) {
    if (!CheckWrite())
        return false;

    fputc('d', mFile);
    
    if (!Write(val))
        return false;

    return CheckFileError();
}//WriteDouble
///////////////////////////////////////////////////////////////////////////////
bool SMS_File::WriteString(const std::string& val) {
    if (!CheckWrite())
        return false;

    fputc('c', mFile);
    uint32_t size = val.size();
    if (!Write(size))
        return false;
    size = val.size();
    for (uint32_t i = 0; i < size; ++i) {
        fputc(val[i], mFile);
    }

    return CheckFileError();
}//WriteString
///////////////////////////////////////////////////////////////////////////////
bool SMS_File::ReadBool() {
    if (!CheckRead())
        return bool();
    
    int type = fgetc(mFile);
    if (!CheckReadType('t', type))
        return bool();
    int8_t temp;
    if (!Read(temp))
        return bool();

    return temp != 0;
}//ReadBool
///////////////////////////////////////////////////////////////////////////////
int8_t SMS_File::ReadInt8() {
    if (!CheckRead())
        return int8_t();
    
    int type = fgetc(mFile);
    if (!CheckReadType('b', type))
        return int8_t();
    int8_t ans;
    if (!Read(ans))
        return int8_t();

    return ans;
}//ReadInt8
///////////////////////////////////////////////////////////////////////////////
int16_t SMS_File::ReadInt16() {
    if (!CheckRead())
        return int16_t();
    
    int type = fgetc(mFile);
    if (!CheckReadType('s', type))
        return int16_t();
    int16_t ans;
    if(!Read(ans))
        return int16_t();

    return ans;
}//ReadInt16
///////////////////////////////////////////////////////////////////////////////
int32_t SMS_File::ReadInt32() {
    if (!CheckRead())
        return int32_t();
    
    int type = fgetc(mFile);
    if (!CheckReadType('i', type))
        return int32_t();
    int32_t ans;
    if(!Read(ans))
        return int32_t();

    return ans;
}//ReadInt32
///////////////////////////////////////////////////////////////////////////////
int64_t SMS_File::ReadInt64() {
    if (!CheckRead())
        return int64_t();
    
    int type = fgetc(mFile);
    if (!CheckReadType('l', type))
        return int64_t();
    int64_t ans;
    if(!Read(ans))
        return int64_t();

    return ans;
}//ReadInt64
///////////////////////////////////////////////////////////////////////////////
uint8_t SMS_File::ReadUInt8() {
    if (!CheckRead())
        return uint8_t();
    
    int type = fgetc(mFile);
    if (!CheckReadType('B', type))
        return uint8_t();
    uint8_t ans;
    if (!Read(ans))
        return uint8_t();

    return ans;
}//ReadUInt8
/////////////////////////////////////////////////////////////////////////////// 
uint16_t SMS_File::ReadUInt16() {
    if (!CheckRead())
        return uint16_t();
    
    int type = fgetc(mFile);
    if (!CheckReadType('s', type))
        return uint16_t();
    uint16_t ans;
    if (!Read(ans))
        return uint16_t();

    return ans;
}//ReadUInt16
///////////////////////////////////////////////////////////////////////////////
uint32_t SMS_File::ReadUInt32() {
    if (!CheckRead())
        return uint32_t();
    
    int type = fgetc(mFile);
    if (!CheckReadType('I', type))
        return uint32_t();
    uint32_t ans;
    if (!Read(ans))
        return uint32_t();

    return ans;
}//ReadUInt32
///////////////////////////////////////////////////////////////////////////////
uint64_t SMS_File::ReadUInt64() {
    if (!CheckRead())
        return uint64_t();
    
    int type = fgetc(mFile);
    if (!CheckReadType('L', type))
        return uint64_t();
    uint64_t ans;
    if (!Read(ans))
        return uint64_t();

    return ans;
}//ReadUInt64
///////////////////////////////////////////////////////////////////////////////
float SMS_File::ReadFloat() {
    if (!CheckRead())
        return float();
    
    int type = fgetc(mFile);
    if (!CheckReadType('f', type))
        return float();
    float ans;
    if (!Read(ans))
        return float();

    return ans;
}//ReadFloat
///////////////////////////////////////////////////////////////////////////////
double SMS_File::ReadDouble() {
    if (!CheckRead())
        return double();
    
    int type = fgetc(mFile);
    if (!CheckReadType('s', type))
        return double();
    double ans;
    if (!Read(ans))
        return double();
    
    return ans;
}//ReadDouble
///////////////////////////////////////////////////////////////////////////////
std::string SMS_File::ReadString() {
    if (!CheckRead())
        return std::string();
    
    int type = fgetc(mFile);
    if (!CheckReadType('c', type))
        return std::string();

    uint32_t size;
    if (!Read(size))
        return std::string();

    std::string ans;
    ans.resize(size);

    for (uint32_t i = 0; i < size; ++i) {
        int temp = fgetc(mFile);
        if (!CheckReadResult(temp))
            return std::string();
        ans[i] = static_cast<char>(temp);
    }
    
    return ans;
}//ReadString
///////////////////////////////////////////////////////////////////////////////
void SMS_File::SetError(SMS_File::ErrorType type, const std::string& message) {
    mErrorType = type;
    if (mErrorType < NO_ERROR || mErrorType > UNKNOWN_ERROR)
        mErrorType = UNKNOWN_ERROR;

    if (message != "")
        mErrorString = message;
    else {
        switch (type) {
        case NO_ERROR:
            mErrorString = "No error";
            break;
        case UNSUPPORTED_TYPE:
            mErrorString  = "Attempted to open file with unsupported or ";
            mErrorString += "unknown file opening type.";
            break;
        case FAILED_OPENING:
            mErrorString = "File failed to open.";
            break;
        case FILE_NOT_OPENED:
            mErrorString = "No file is open to read or write to";
            break;
        case NON_READABLE:
            mErrorString = "Cannot read from non-readable file";
            break;
        case NON_WRITEABLE:
            mErrorString = "Cannot write to non-writeable file";
            break;
        case INVALID_FORMAT:
            mErrorString = "File being read is invalidly formatted";
            break;
        case END_OF_FILE:
            mErrorString = "Reached end of file";
            break;
        case INVALID_TYPE:
            mErrorString = "Trying to read invalid type";
            break;
        case UNKNOWN_ERROR:
        default:
            mErrorString = "Unknown error occured";
            break;
        }
    }
}//SetError
///////////////////////////////////////////////////////////////////////////////
//returns true if the file can be written to
bool SMS_File::CheckWrite() {
    if (!IsOpened()) {
        SetError(FILE_NOT_OPENED);
        return false;
    }
    if (!IsWriteable()) {
        SetError(NON_WRITEABLE);
        return false;
    }
    return true;
}//CheckWrite
///////////////////////////////////////////////////////////////////////////////
//returns true if the file can be read
bool SMS_File::CheckRead() {
    if (!IsOpened()) {
        SetError(FILE_NOT_OPENED);
        return false;
    } if (!IsReadable()) {
        SetError(NON_READABLE);
        return false;
    }
    if (IsEOF()) {
        SetError(END_OF_FILE);
        return false;
    }
    return true;
}//CheckRead
///////////////////////////////////////////////////////////////////////////////
//Check if the file has an error
//Returns true if this is so
bool SMS_File::CheckFileError() {
    if (ferror(mFile) != 0) {
        SetError(UNKNOWN_ERROR);
        return false;
    }
    return true;
}
//CheckError
///////////////////////////////////////////////////////////////////////////////
//Check whether "is" is valid and checks if "is" is what it "shouldbe"
//Returns true on when this is so and false if this isn't
bool SMS_File::CheckReadType(char shouldbe, int is) {
    if (is == EOF) {
        SetError(END_OF_FILE);
        return false;
    }
    char temp_is = static_cast<char>(is);
    if (!IsValidType(temp_is)) {
        SetError(INVALID_FORMAT);
        return false;
    }
    
    if (shouldbe != temp_is) {
        std::string error = "Error while reading ";
        error += GetTypeName(shouldbe);
        error += ", got type: ";
        error += GetTypeName(temp_is);
        SetError(INVALID_TYPE, error);
        return false;
    }
    
    return true;
}//CheckType
///////////////////////////////////////////////////////////////////////////////
//Checks if the read value is valid, returns true if this is the case
bool SMS_File::CheckReadResult(int read) {
    if (read == EOF) {
        SetError(INVALID_FORMAT);
        return false;
    }
    return true;
}//CheckReadResult
///////////////////////////////////////////////////////////////////////////////
//Returns whether the type exists, returns true if this is the case
bool SMS_File::IsValidType(char type) {
    switch(type) {
    case 't':
    case 'b':
    case 's':
    case 'i':
    case 'l':
    case 'B':
    case 'S':
    case 'I':
    case 'L':
    case 'f':
    case 'd':
    case 'c':
        return true;
    default:
        return false;
    }
}//IsValidType
///////////////////////////////////////////////////////////////////////////////
//Returns the name of a type as designated by type
std::string SMS_File::GetTypeName(char type) {
    switch(type) {
    case 't':
        return "bool";
    case 'b':
        return "int8_t";
    case 's':
        return "int16_t";
    case 'i':
        return "int32_t";
    case 'l':
        return "int64_t";
    case 'B':
        return "uint8_t";
    case 'S':
        return "uint16_t";
    case 'I':
        return "uint32_t";
    case 'L':
        return "uint64_t";
    case 'f':
        return "float";
    case 'd':
        return "double";
    case 'c':
        return "string";
    default:
        return "unknown type";
    }
}//GetTypeName
///////////////////////////////////////////////////////////////////////////////
//Writes val to the file
template<typename T>
bool SMS_File::Write(T val) {
    size_t numWrote = fwrite(&val, sizeof(T), 1, mFile);
    if (numWrote != 1) {
        SetError(UNKNOWN_ERROR);
        return false;
    }
    return true;
}//Write
///////////////////////////////////////////////////////////////////////////////
//Reads from the file and stores the result in result
//Returns true if reading succeeds and false otherwise
template<typename T>
bool SMS_File::Read(T& result) {
    size_t numRead = fread(&result, sizeof(T), 1, mFile);
    if (numRead != 1) {
        SetError(INVALID_FORMAT);
        return false;
    }
    return true;
}//Read
///////////////////////////////////////////////////////////////////////////////
