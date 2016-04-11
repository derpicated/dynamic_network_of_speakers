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
dns_client_src=./client/main.cpp \
				./client/Config.cpp \
				./client/DNSMusic.cpp \
				./client/DNSDataParser.cpp\
				./client/libs/audio/audio.cpp\
				./client/libs/download/download.cpp\
				./client/libs/jzon/Jzon.cpp\
				./client/libs/topic/Tokenizer.cpp \
				./client/libs/topic/Topic.cpp \
				./client/libs/rwf/relative_weight_factor.cpp
# Target / executable
dns_client_src_out=dns_client
$(dns_client_src_out): $(dns_client_src)
	$(CHECK_BUILD)
	$(CXX) $(CXXFLAGS)  $(dns_client_src) $(LDFLAGS) -o $(BUILD_DIR)/$(dns_client_src_out)

# Lib testing
# Relative Weight Factor
rwf_lib_src=	./test/relative_weight_factor.cpp \
				./client/libs/rwf/relative_weight_factor.cpp
# Target / executable
rwf_lib_src_out=rwf_lib
$(rwf_lib_src_out): $(rwf_lib_src)
	$(CHECK_BUILD)
	$(CXX) $(CXXFLAGS) $(rwf_lib_src) -o $(BUILD_DIR)/$(rwf_lib_src_out)

# Audio Output
audio_test_src=	./test/audio_test.cpp \
				./client/libs/audio/audio.cpp

audio_test_out=audio_test
$(audio_test_out): $(audio_test_src)
	$(CHECK_BUILD)
	$(CXX) $(CXXFLAGS) $(audio_test_src) -o $(BUILD_DIR)/$(audio_test_out)

# Download test
download_test_src=	./test/download_test.cpp \
					./client/libs/download/download.cpp

download_test_out=download_test
$(download_test_out): $(download_test_src)
	$(CHECK_BUILD)
	$(CXX) $(CXXFLAGS) $(download_test_src) -o $(BUILD_DIR)/$(download_test_out)

# Clean build dir
clean:
	$(RM) ./$(BUILD_DIR)/*

# Purge the build dir
purge:
	$(RM)r ./$(BUILD_DIR)
