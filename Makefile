CCPP = g++
CC = gcc
TARGET = fxfer_test
FLAGS = -lc -fpermissive
BUILD_DIR = build

INCLUDES = \
-IanyOption     \
-Iuart           \
-IfileXferLib/inc \

C_SOURCES = \
fileXferLib/src/fileXfer.c \
fileXferLib/src/fileXferUtils.c \
platform/fileXferCallbacks.c \

CPP_SOURCES = \
main.cpp \
anyOption/anyoption.cpp \
uart/pc_uart.cpp \
platform/fileXferPlatform.cpp \

DEFS =  \

OBJECTS = $(addprefix $(BUILD_DIR)/, $(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))

OBJECTS += $(addprefix $(BUILD_DIR)/, $(notdir $(CPP_SOURCES:.cpp=.o)))
vpath %.cpp $(sort $(dir $(CPP_SOURCES)))

$(TARGET): $(OBJECTS)
	$(CCPP) $(OBJECTS) $(INCLUDES) $(FLAGS)  $(DEFS) -o $(BUILD_DIR)/$(TARGET)

$(BUILD_DIR)/%.o: %.c $(BUILD_DIR)
	$(CC) -c $(INCLUDES) $(DEFS) $< -o $@

$(BUILD_DIR)/%.o: %.cpp $(BUILD_DIR)
	$(CCPP) -c $(INCLUDES) $(DEFS) $< -o $@

$(BUILD_DIR):
	mkdir $@

clean:
	rm -fR $(BUILD_DIR)
	rm -f ${TARGET}
