SOURCE		:=	source
INCLUDE		:=	../include
BUILD		:=	build
TARGET		:=	../test.dll

PREFIX		:=	x86_64-w64-mingw32-
CC			:=	$(PREFIX)gcc
LD			:=	$(PREFIX)gcc

CFLAGS		:=	-O3 -Wall -Wextra -Werror -c -std=c17 -fPIC -m64 \
				$(addprefix -I,$(INCLUDE))
LDFLAGS		:=	-shared -m64

rwildcard	=	$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

FILES_C		:=	$(call rwildcard,$(SOURCE),*.c)
FILES_O		:=	$(patsubst $(SOURCE)/%.c,$(BUILD)/%.c.o,$(FILES_C))

all: $(TARGET)

clean:
	@rm -vf $(FILES_O) $(TARGET)

$(TARGET): $(FILES_O)
	@mkdir -p $(@D)
	$(LD) -o $@ $^ $(LDFLAGS)

$(BUILD)/%.c.o: $(SOURCE)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ $^
