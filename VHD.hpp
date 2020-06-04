#ifndef __VHD_HPP__
#define __VHD_HPP__

#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <exception>
#include <cstring>
using namespace std;

void ReadByte(ifstream& f, uint8_t& byte);
void ReadWord(ifstream& f, uint16_t& word);
void ReadDoubleWord(ifstream& f, uint32_t& doubleword);
void ReadQuad(ifstream& f, uint64_t& quad);
void ReadString(ifstream& f, uint8_t* str, int len);

/*
class VHDNotFound:public exception{
public:
    VHDNotFound(const string filename);
    const char* what()const noexcept override;
private:
    const string file;
};
*/

/**
 * @brief 用于操作固定VHD文件的类
 * 
 */
class FixedVHD{
public:
    const static int SECTION_SIZE;

    enum FEATURE{
        NO_FEATURES=0x00,
        TEMPORARY=0x00000001,
        RESERVED=0x00000002
    };

    enum TYPE{
        TYPE_NONE = 0,
        TYPE_FIXED = 2, /* fixed-allocation disk */
        TYPE_DYNAMIC = 3, /* dynamic disk */
        TYPE_DIFF = 4, /* differencing disk */
    };

    struct _FixedVHD_Field_Creator{
        uint8_t author[5];      /**< 创建者,   4字节 */
        uint16_t version[2];    /**< 应用版本, 4字节 */
        uint8_t system[5];      /**< 应用系统, 4字节 */
    };//12字节

    struct FixedVHD_Head{
        uint8_t identification[9];  /**< 开头的标识,8字节 */
        uint32_t feature;           /**< 特性,4字节 */
        uint16_t version[2];        /**< 版本,4字节 */
        uint64_t offset;            /**< 数据偏移 8字节 */
        uint32_t timestamp;         /**< 时间戳 4字节 */
        _FixedVHD_Field_Creator creator;    /**< 创建者字段 12字节 */
        uint64_t init_len;          /**< 初始长度，创建时的初始大小    8字节 */
        uint64_t fixed_len;         /**< 即时长度，VHD的长度         8字节 */
        uint32_t geomentry;         /**< 记录了C/H/S（磁道，磁头，扇区）信息 4字节 */
        uint32_t type;              /**< 类型                      4字节 */
        uint32_t crc;               /**< 校验和                    4字节 */
        uint8_t  uuid[16];          /**< 通用唯一识别码              16字节 */
        uint8_t saved;              /**< 是否在保存状态              1字节 */
        uint8_t hidden;             /**< VDI是否隐藏                1字节 */
        //uint8_t  reserv[427];     /**< 保留字段，全部为0         426字节 */
    };

    FixedVHD();
    explicit FixedVHD(string filename);
    void Open(string filename);
    /**
     * @brief 获得尾部的信息
     * 
     * @return const FixedVHD_Head& 
     */
    const FixedVHD_Head& GetInfo();
    /**
     * @brief 获得整个VHD的大小
     * 
     * @return unsigned int 
     */
    unsigned int GetTotleSize();
    /**
     * @brief 获得除去末尾的信息的大小
     * 
     * @return unsigned int 
     */
    unsigned int GetContentSize();
    /**
     * @brief 获得磁道数
     * 
     * @return unsigned int 
     */
    unsigned int GetParamC();
    /**
     * @brief 获得磁头数
     * 
     * @return unsigned int 
     */
    unsigned int GetParamH();
    /**
     * @brief 获得扇区数
     * 
     * @return unsigned int 
     */
    unsigned int GetParamS();
    /**
     * @brief 读取VHD
     * 
     * 使用LBA模式读取
     * 
     * @param count 读取的扇区号
     * @param buffer 缓冲区，最后的结果会放入里面
     * @return bool
     */
    bool Read(unsigned int count, void* buffer);
    /**
     * @brief 写入扇区
     * 
     * 使用LBA模式写入。如果数据大小不是512的倍数，会将数据后面补0至512倍数
     * 
     * @param start 开始的扇区号
     * @param data 数据
     * @param size 数据大小
     * @return bool
     */
    bool Write(unsigned int start, void* data, int size);
    bool fail();
private:
    FixedVHD_Head head;
    unsigned long long totlesize;
    unsigned long long contentsize;
    string filepath;
    bool failed;
};

ostream& operator<<(ostream& o, FixedVHD& vhd);

#endif