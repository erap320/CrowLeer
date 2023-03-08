TARGET_EXEC ?= crowleer

BUILD_DIR ?= build
SRC_DIRS ?= CrowLeer $(BUILD_DIR)/cpp-netlib/include
LIB_DIRS ?= $(BUILD_DIR)/cpp-netlib/_build/src

SRCS := $(shell find $(SRC_DIRS) -name *.cpp -or -name *.c -or -name *.s)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(SRC_DIRS)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))
LIB_FLAGS := $(addprefix -L,$(LIB_DIRS))

DEFS ?= -DLINUX
CPPFLAGS ?= $(INC_FLAGS) -MMD
LDFLAGS ?= -lcurl -lnetwork-uri

all: libnetwork-uri $(BUILD_DIR)/$(TARGET_EXEC)

libnetwork-uri: $(BUILD_DIR)/cpp-netlib $(BUILD_DIR)/cpp-netlib/_build/src/libnetwork-uri.a

$(BUILD_DIR)/cpp-netlib:
	git clone https://github.com/cpp-netlib/uri.git $(BUILD_DIR)/cpp-netlib

$(BUILD_DIR)/cpp-netlib/_build/src/libnetwork-uri.a:
	mkdir -p $(BUILD_DIR)/cpp-netlib/_build
	cd $(BUILD_DIR)/cpp-netlib/_build ; \
		cmake -DUri_BUILD_TESTS=OFF -DUri_BUILD_DOCS=OFF .. && \
		make -j4

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CXX) $(DEFS) $(OBJS) -o $@ $(LIB_FLAGS) $(LDFLAGS)

# assembly
$(BUILD_DIR)/%.s.o: %.s
	$(MKDIR_P) $(dir $@)
	$(AS) $(DEFS) $(ASFLAGS) -c $< -o $@

# c source
$(BUILD_DIR)/%.c.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(DEFS) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# c++ source
$(BUILD_DIR)/%.cpp.o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CXX) $(DEFS) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@


.PHONY: clean

clean:
	$(RM) -r $(BUILD_DIR)

-include $(DEPS)

MKDIR_P ?= mkdir -p