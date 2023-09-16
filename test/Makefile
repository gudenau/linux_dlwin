SOURCE		:=	source
INCLUDE		:=	include ../public
BUILD		:=	build
TARGET		:=	test
LIBS		:=	dlwin

CC			:=	clang
LD			:=	clang

CFLAGS		:=	-O0 -Wall -Wextra -Werror -g -c -std=c17 \
				$(addprefix -I,$(INCLUDE))
LDFLAGS		:=	-fuse-ld=lld -L../cmake-build-debug \
				$(addprefix -l,$(LIBS))

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
