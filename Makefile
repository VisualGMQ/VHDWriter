BUILD_DIR=build
OBJ_DIR=${BUILD_DIR}/obj
BIN_DIR=${BUILD_DIR}/bin

${BIN_DIR}/FixedVHDWriter:FixedVHDWriter.cpp ${OBJ_DIR}/VHD.o
	g++ FixedVHDWriter.cpp ${OBJ_DIR}/VHD.o -o ${BIN_DIR}/FixedVHDWriter -std=c++11
FixedVHDWriterGUI:FixedVHDWriterGUI.cpp ${OBJ_DIR}/VHD.o
	g++ FixedVHDWriterGUI.cpp ${OBJ_DIR}/VHD.o -o ${BIN_DIR}/FixedVHDWriterGUI `wx-config --libs --cflags` -std=c++11
test:${OBJ_DIR}/test.o ${OBJ_DIR}/VHD.o
	g++ test.cpp ${OBJ_DIR}/VHD.o -o ${BIN_DIR}/test -std=c++11
${OBJ_DIR}/VHD.o:VHD.hpp VHD.cpp
	-mkdir ${BIN_DIR}
	-mkdir ${OBJ_DIR}
	g++ VHD.cpp -c -o ${OBJ_DIR}/VHD.o -std=c++11
.PHONY:clean
clean:
	-rm ${BIN_DIR}/* ${OBJ_DIR}/*
