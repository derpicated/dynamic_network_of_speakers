# Makefile for DNS
CC=gcc
CXX=g++
LDFLAGS = -lmosquittopp -L/usr/local/lib
RM=rm -f
CXXFLAGS= -std=c++11 -Wall -Wextra -Weffc++ -Os -D_GLIBCXX_USE_NANOSLEEP -pthread -D__GCC_HAVE_SYNC_COMPARE_AND_SWAP_1 -D__GCC_HAVE_SYNC_COMPARE_AND_SWAP_2 -D__GCC_HAVE_SYNC_COMPARE_AND_SWAP_4 -D__GCC_HAVE_SYNC_COMPARE_AND_SWAP_8
BUILD_DIR=build
CHECK_BUILD=if [ ! -d "./$(BUILD_DIR)" ];then	\
				mkdir $(BUILD_DIR);     		\
			fi
MKDIR_P = mkdir -p

# Client
dns_client_src=./client/Config.cpp \
				./client/DNSMusic.cpp \
				./client/main.cpp \
				./client/Tokenizer.cpp \
				./client/Topic.cpp \
				./libs/rwf/relative_weight_factor.cpp
# Target / executable
dns_client_src_out=dns_client
$(dns_client_src_out): $(dns_client_src)
	$(CHECK_BUILD)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(dns_client_src) -o $(BUILD_DIR)/$(dns_client_src_out)

# Lib testing
# Relative Weight Factor
rel_wf_lib_src=	./test/relative_weight_factor.cpp \
				./libs/rwf/relative_weight_factor.cpp
# Target / executable
rel_wf_lib_out=rel_wf_lib
$(rel_wf_lib_out): $(rel_wf_lib_src)
	$(CHECK_BUILD)
	$(CXX) $(CXXFLAGS) $(rel_wf_lib_src) -o $(BUILD_DIR)/$(rel_wf_lib_out)

# Clean build dir
clean:
	$(RM) ./$(BUILD_DIR)/*

# Purge the build dir
purge:
	$(RM)r ./$(BUILD_DIR)
