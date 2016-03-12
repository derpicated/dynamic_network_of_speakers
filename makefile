# Makefile for DNS
CC=gcc
CXX=g++
RM=rm -f
CPPFLAGS=-std=c++11
BUILD_DIR=build
CHECK_BUILD=if [ ! -d "./$(BUILD_DIR)" ];then	\
				mkdir $(BUILD_DIR);     		\
			fi
MKDIR_P = mkdir -p

# Lib testing
# Relative Weight Factor
rel_wf_lib_src=	./test/relative_weight_factor.cpp \
				./libs/rwf/relative_weight_factor.cpp
# Target / executable
rel_wf_lib_out=rel_wf_lib
$(rel_wf_lib_out): $(rel_wf_lib_src)
	$(CHECK_BUILD)
	$(CXX) $(CPPFLAGS) $(rel_wf_lib_src) -o $(BUILD_DIR)/$(rel_wf_lib_out)

# Clean build dir
clean:
	$(RM) ./$(BUILD_DIR)/*

# Purge the build dir
purge:
	$(RM)r ./$(BUILD_DIR)
