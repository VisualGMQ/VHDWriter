#include "VHD.hpp"

const int FixedVHD::SECTION_SIZE = 512;

void ReadByte(ifstream& f, uint8_t& byte){
    f.read((char*)&byte, 1);
}

void ReadWord(ifstream& f, uint16_t& word){
    unsigned char tmp[2];
    f.read((char*)&tmp[1], 1);
    f.read((char*)&tmp[0], 1);
    memcpy(&word, tmp, 2);
}

void ReadDoubleWord(ifstream& f, uint32_t& doubleword){
    unsigned char tmp[4];
    for(int i=3;i>=0;i--)
        f.read((char*)&tmp[i], 1);
    memcpy(&doubleword, tmp, 4);
}

void ReadQuad(ifstream& f, uint64_t& quad){
    unsigned char tmp[8];
    for(int i=7;i>=0;i--)
        f.read((char*)&tmp[i], 1);
    memcpy(&quad, tmp, 8);
}

void ReadString(ifstream& f, uint8_t* str, int len){
    f.read((char*)str, len);
    str[len] = '\0';
}

/*
VHDNotFound::VHDNotFound(const string filename):file(filename){
}

const char* VHDNotFound::what()const noexcept{
    return (file+" not found").c_str();
}
*/

void FixedVHD::Open(string filename){
    ifstream file(filename);
    this->filepath = filename;
    char byte[4];
    char word[8];
    if(file.fail())
        failed = true;
    else
        failed = false;
    file.seekg(-512, ios::end);
    ReadString(file, head.identification, 8);
    ReadDoubleWord(file, head.feature);
    uint8_t tmp4[4];
    for(int i=3;i>=0;i--)
        file.read((char*)&tmp4[i], 1);
    memcpy(&head.version[0], tmp4, 2);
    memcpy(&head.version[1], &tmp4[2], 2);
    ReadQuad(file, head.offset);
    ReadDoubleWord(file, head.timestamp);
    ReadString(file, head.creator.author, 4);
    for(int i=3;i>=0;i--)
        file.read((char*)&tmp4[i], 1);
    memcpy(&head.creator.version[0], tmp4, 2);
    memcpy(&head.creator.version[1], &tmp4[2], 2);
    ReadString(file, head.creator.system, 4);
    ReadQuad(file, head.init_len);
    ReadQuad(file, head.fixed_len);
    ReadDoubleWord(file, head.geomentry);
    ReadDoubleWord(file, head.type);
    ReadDoubleWord(file, head.crc);
    uint8_t tmp16[16];
    for(int i=15;i>=0;i--)
        file.read((char*)&tmp16[i], 1);
    memcpy(head.uuid, tmp16, 16);
    ReadByte(file, head.saved);
    ReadByte(file, head.hidden);
    file.seekg(0, ios::end);
    totlesize = file.tellg();
    contentsize =  totlesize - 512;
    file.close();
}

FixedVHD::FixedVHD():failed(true),totlesize(0),contentsize(0){}

bool FixedVHD::fail(){
    return failed;
}

FixedVHD::FixedVHD(string filename):filepath(filename){
    this->Open(filename);
}

const FixedVHD::FixedVHD_Head& FixedVHD::GetInfo(){
    return head;
}

unsigned int FixedVHD::GetTotleSize(){
    return totlesize;
}

unsigned int FixedVHD::GetContentSize(){
    return contentsize;
}

unsigned int FixedVHD::GetParamC(){
    return (head.geomentry>>16)&0xffff;
}

unsigned int FixedVHD::GetParamH(){
    return (head.geomentry>>8)&0xff;
}

unsigned int FixedVHD::GetParamS(){
    return (head.geomentry)&0xff;
}

ostream& operator<<(ostream& o, FixedVHD& vhd){
    o.setf(ios_base::hex);
    FixedVHD::FixedVHD_Head head = vhd.GetInfo();
    cout<<"totole size:"<<vhd.GetTotleSize()<<"Bytes";
    int mb = vhd.GetTotleSize()/1024/1024;
    if(mb!=0)
        cout<<"\t"<<mb<<"MB";
    cout<<endl;
    cout<<"content size:"<<vhd.GetContentSize()<<"Bytes";
    mb = vhd.GetContentSize()/1024/1024;
    if(mb!=0)
        cout<<"\t"<<mb<<"MB";
    cout<<endl;
    o<<"C/H/S（磁道，磁头，扇区）:"<<vhd.GetParamC()<<"/"<<vhd.GetParamH()<<"/"<<vhd.GetParamS()<<endl;
    o<<"feature:";
    switch(head.feature){
        case FixedVHD::FEATURE::NO_FEATURES:
            o<<"no features"<<endl;
            break;
        case FixedVHD::FEATURE::RESERVED:
            o<<"reserved"<<endl;
            break;
        case FixedVHD::FEATURE::TEMPORARY:
            o<<"temporary"<<endl;
            break;
    }
    o<<"version:"<<head.version[0]<<"."<<head.version[1]<<endl;
    o<<"offset:"<<head.offset<<endl;
    time_t time = head.timestamp-0xB492F400;
    tm* t = localtime(&time);
    o<<"created time:"<<asctime(t);
    o<<"author:"<<head.creator.author<<endl;
    o<<"author's version:"<<head.creator.version[0]<<"."<<head.creator.version[1]<<endl;
    o<<"created system:"<<head.creator.system<<endl;
    o<<"initlen:"<<head.init_len<<endl;
    o<<"fixed_len:"<<head.fixed_len<<endl;
    o<<"type:";
    switch(head.type){
        case FixedVHD::TYPE::TYPE_DIFF:
            o<<"fixed allocation disk"<<endl;
            break;
        case FixedVHD::TYPE::TYPE_NONE:
            o<<"unknow disk"<<endl;
            break;
        case FixedVHD::TYPE::TYPE_DYNAMIC:
            o<<"dynamic disk"<<endl;
            break;
        case FixedVHD::TYPE::TYPE_FIXED:
            o<<"fixed disk"<<endl;
            break;
    }
    o<<"crc:"<<head.crc<<endl;
    o<<"uuid:";
    for(int i=0;i<16;i++)
        o<<(int)head.uuid[i];
    o<<endl;
    o<<"saved:"<<boolalpha<<(bool)head.saved<<endl;
    o<<"hidden:"<<(bool)head.hidden<<endl;
    o<<dec;
    return o;
}

bool FixedVHD::Read(unsigned int count, void* buffer){
    if(count>GetContentSize()/SECTION_SIZE){
        cerr<<"read beyond bound"<<endl;
        return false;
    }
    ifstream file(filepath);
    file.seekg(SECTION_SIZE*count, ios::beg);
    file.read((char*)buffer, SECTION_SIZE);
    file.close();
    return true;
}

bool FixedVHD::Write(unsigned int start, void* data, int size){
    if((size+start)*SECTION_SIZE>contentsize){
        cerr<<"Beyond the scope of VHD"<<endl;
        return false;
    }
    if(size<0){
        cerr<<"size most big than 0"<<endl;
        return false;
    }
    if(size==0)
        return false;
    cout<<"write "<<size<<" at "<<start<<endl;
    //app模式是一定写在后面的，seekp也无效。所以这里不能用app模式
    fstream file(filepath, ios::binary|ios::out|ios::in);
    file.seekp(start*SECTION_SIZE, ios::beg);
    char* buffer = nullptr;
    unsigned int final_size;
    if(size%SECTION_SIZE!=0)
        final_size = (size/SECTION_SIZE+1)*SECTION_SIZE;
    else
        final_size = size;
    cout<<"final size:"<<final_size<<endl;
    buffer = new char[final_size];
    memset(buffer, 0, final_size);
    memcpy(buffer, data, size);
    file.write(buffer, final_size);
    file.close();
    delete buffer;
    return true;
}