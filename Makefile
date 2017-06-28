VPATH := Debugger:Feature:Mesh:Stitching:Util
VLPATH = vlfeat-0.9.20
EIGENPATH = ~/eigen
DIR_OBJ = obj
INC = -I $(EIGENPATH) -I $(VLPATH) -I Debugger -I Feature -I Mesh -I Stitching -I Util -I .
OPENCV_INC = $(shell pkg-config opencv --cflags)
OPENCV_LIB = $(shell pkg-config opencv --libs)
LIB = -L $(VLPATH)/bin/glnxa64 -lvl -fopenmp -lstdc++
FILE =  ColorMap.o ErrorController.o ImageDebugger.o TimeCalculator.o \
		FeatureController.o ImageData.o MultiImages.o \
		Mesh2D.o MeshGrid.o MeshOptimization.o \
		APAP_Stitching.o NISwGSP_Stitching.o Parameter.o \
		Blending.o InputParser.o Statistics.o Transform.o \
		main.o
OBJ = $(patsubst %.o,${DIR_OBJ}/%.o,$(notdir ${FILE}))
MKL_LIB =  -L${MKLROOT}/lib/intel64 -Wl,--no-as-needed -lmkl_intel_lp64 -lmkl_gnu_thread -lmkl_core -lgomp -lpthread -lm -ldl
MKL_INC =  -m64 -I${MKLROOT}/include
CUDA_INC = -I/usr/local/cuda-8.0/include
CUDA_LIB = -L/usr/local/cuda-8.0/lib64 -lcublas -lcusparse -lcudart -lcudadevrt -lcusolver
all: main
%.o: %.cpp
	g++ -std=gnu++14 $(INC) $(OPENCV_INC) $(MKL_INC) $(CUDA_INC) -fopenmp -c -o $(DIR_OBJ)/$@ $< 
main: $(FILE)
	g++ -std=gnu++14 $(INC) $(OPENCV_INC) -o $@ $(OBJ) $(OPENCV_LIB) $(MKL_LIB) $(CUDA_LIB) $(LIB)
clean:
	rm -rf obj/*