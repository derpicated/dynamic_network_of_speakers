# Makefile for DNS
CC=gcc
CXX=g++
LDFLAGS = -lmosquittopp -lsfml-system -lsfml-audio -L/usr/local/lib
RM=rm -f
CXXFLAGS= -g -std=c++11 -Wall -Wextra -Weffc++ -D_GLIBCXX_USE_NANOSLEEP -pthread
BUILD_DIR=build
CHECK_BUILD=if [ ! -d "./$(BUILD_DIR)" ];then	\
				mkdir $(BUILD_DIR);     		\
			fi
MKDIR_P = mkdir -p

# DNS Client
dns_client_out=dns_client
dns_client_src=./client/main.cpp \
				./client/dns.cpp \
				./client/data_parser.cpp \
				./client/libs/download/download.cpp \
				./client/libs/config/config_parser.cpp \
				./client/libs/jzon/Jzon.cpp \
				./client/libs/rwf/relative_weight_factor.cpp
$(dns_client_out): $(dns_client_src)
	@echo
	@echo Building DNS Client
	@echo
	$(CHECK_BUILD)
	$(CXX) $(CXXFLAGS) $(dns_client_src) $(LDFLAGS) -o $(BUILD_DIR)/$(dns_client_out)

#################
# Lib testing
#################
# Relative Weight Factor
rwf_test_out=rwf_test
rwf_test_src=	./test/relative_weight_factor_test.cpp \
				./client/libs/rwf/relative_weight_factor.cpp
$(rwf_test_out): $(rwf_test_src)
	@echo
	@echo RWF test
	@echo
	$(CHECK_BUILD)
	$(CXX) $(CXXFLAGS) $(rwf_test_src) -o $(BUILD_DIR)/$(rwf_test_out)

# Audio test
audio_test_out=audio_test
audio_test_src=	./test/audio_test.cpp
$(audio_test_out): $(audio_test_src)
	@echo
	@echo Audio test
	@echo
	$(CHECK_BUILD)
	$(CXX) $(CXXFLAGS) $(audio_test_src) $(LDFLAGS) -o $(BUILD_DIR)/$(audio_test_out)

# Download test
download_test_out=download_test
download_test_src=	./test/download_test.cpp \
					./client/libs/download/download.cpp
$(download_test_out): $(download_test_src)
	@echo
	@echo Download test
	@echo
	$(CHECK_BUILD)
	$(CXX) $(CXXFLAGS) $(download_test_src) -o $(BUILD_DIR)/$(download_test_out)

# Data Parser test
data_parser_test_out=data_parser_test
data_parser_test_src=./test/data_parser_test.cpp \
					./client/data_parser.cpp \
					./client/libs/jzon/Jzon.cpp
$(data_parser_test_out): $(data_parser_test_src)
	@echo
	@echo Data Parser test
	@echo
	$(CHECK_BUILD)
	$(CXX) $(CXXFLAGS) $(data_parser_test_src) -o $(BUILD_DIR)/$(data_parser_test_out)
	cp ./test/test_client_data.json $(BUILD_DIR)
	cp ./test/test_source_data.json $(BUILD_DIR)
	cp ./test/test_client_compose.json $(BUILD_DIR)

# Logger test
logger_test_out=logger_test
logger_test_src=./test/log_test.cpp
$(logger_test_out): $(logger_test_src)
	@echo
	@echo Logger test
	@echo
	$(CHECK_BUILD)
	$(CXX) $(CXXFLAGS) $(logger_test_src) -o $(BUILD_DIR)/$(logger_test_out)

# JZON test
jzon_test_out=jzon_test
jzon_test_src=	./test/jzon_test.cpp \
				./client/libs/jzon/Jzon.cpp
$(jzon_test_out): $(jzon_test_src)
	@echo
	@echo JZON test
	@echo 
	$(CHECK_BUILD)
	$(CXX) $(CXXFLAGS) $(jzon_test_src) -o $(BUILD_DIR)/$(jzon_test_out)

# Config Pareser test
config_parser_test_out = config_parser_test
config_parser_test_src =	./test/config_parser_test.cpp \
							./client/libs/config/config_parser.cpp \
							./client/libs/jzon/Jzon.cpp
$(config_parser_test_out): $(config_parser_test_src)
	@echo
	@echo Config parser test
	@echo
	$(CHECK_BUILD)
	$(CXX) $(CXXFLAGS) $(config_parser_test_src) -o $(BUILD_DIR)/$(config_parser_test_out)

# Make all
all : $(dns_client_out) \
	  $(rwf_test_out) \
	  $(audio_test_out) \
	  $(download_test_out) \
	  $(data_parser_test_out) \
	  $(logger_test_out) \
	  $(jzon_test_out) \
	  $(config_parser_test_out)
.PHONY : all

# Clean build dir
.PHONY : clean
clean:
	$(RM) ./$(BUILD_DIR)/*

# Purge the build dir
.PHONY : purge
purge:
	$(RM)r ./$(BUILD_DIR)
