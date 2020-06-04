#include "VHD.hpp"

void PrintHelp(){
    cout<<"FixedVHDWriter"<<endl
        <<"version:\t1.0"<<endl
        <<"author:\t\tVisualGMQ"<<endl
        <<"data:\t\t2019.12.11"<<endl
        <<"--------------------------"<<endl
        <<"FixedVHDWriter <src filename> <VHD filename> <LBA count>";
}

int main(int argc, char** argv){
    if(argc!=4){
        PrintHelp();
        return 1;
    }
    string srcfilename = argv[1];
    string vhdfilename = argv[2];
    unsigned int lbacount = atoi(argv[3]);
    cout<<"write <"<<srcfilename<<">"<<" into lba "<<lbacount<<" of <"<<vhdfilename<<">"<<endl;
    FixedVHD vhd(vhdfilename);
    if(vhd.fail()){
        cerr<<"fatal error"<<endl;
        cerr<<vhdfilename<<" not found"<<endl;
        return 2;
    }
    ifstream srcfile(srcfilename);
    if(srcfile.fail()){
        cerr<<"fatal error"<<endl;
        cerr<<srcfilename<<" not found"<<endl;
        return 3;
    }
    srcfile.seekg(0, ios::end);
    const unsigned int size = srcfile.tellg();
    char buffer[size];
    srcfile.seekg(0, ios::beg);
    srcfile.read(buffer, size);
    cout<<"writing ..."<<endl;
    vhd.Write(lbacount, buffer, size);
    cout<<"wrote finished"<<endl;
    return 0;
}