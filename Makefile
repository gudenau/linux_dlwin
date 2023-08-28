SOURCE		:=	source
INCLUDE		:=	include public
BUILD		:=	build
TARGET		:=	libdlwin.so

CC			:=	clang
CXX			:=	clang++
LD			:=	clang

COMFLAGS	:=	-O0 -Wall -Wextra -Werror -g -c -fvisibility=hidden \
				$(addprefix -I,$(INCLUDE)) \
				-D__dlwin_build__
ASFLAGS		:=	
CFLAGS		:=	$(COMFLAGS) -std=c17
CXXFLAGS	:=	$(COMFLAGS) -std=c++20 -fno-exceptions -fno-rtti
LDFLAGS		:=	-shared -fuse-ld=lld -fvisibility=hidden

rwildcard	=	$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

FILES_S		:=	$(call rwildcard,$(SOURCE),*.s)
FILES_C		:=	$(call rwildcard,$(SOURCE),*.c)
FILES_CXX	:=	$(call rwildcard,$(SOURCE),*.cpp)
FILES_O		:=	$(patsubst $(SOURCE)/%.s,$(BUILD)/%.s.o,$(FILES_S)) \
				$(patsubst $(SOURCE)/%.c,$(BUILD)/%.c.o,$(FILES_C)) \
				$(patsubst $(SOURCE)/%.cpp,$(BUILD)/%.cpp.o,$(FILES_CXX))

all: $(TARGET)

clean:
	@rm -vf $(FILES_O) $(TARGET)

$(TARGET): $(FILES_O)
	@mkdir -p $(@D)
	$(LD) -o $@ $^ $(LDFLAGS)

$(BUILD)/%.s.o: $(SOURCE)/%.s
	@mkdir -p $(@D)
	$(AS) $(ASFLAGS) -o $@ $^

$(BUILD)/%.c.o: $(SOURCE)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD)/%.cpp.o: $(SOURCE)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -o $@ $^
