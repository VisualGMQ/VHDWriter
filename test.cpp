#include <iostream>
#include "VHD.hpp"
using namespace std;

int main(int argc, char** argv){
    FixedVHD vhd("resource/Learn-ASM.vhd");
    cout<<vhd;
    return 0;
}