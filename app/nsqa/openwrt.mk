
OBJS=nsqa.o \
	cli.o \
	instance.o \
	proto.o \
	timer.o \
	#end

__TARGET=nsqa
TARGET=$(__TARGET)
LIBS_DEPEND=-lc -lm -ljson-c -lweos

ifeq (1,$(UBACKTRACE))
LIBS_DEPEND+=-lubacktrace
endif

ifeq (2,$(UBACKTRACE))
LIBS_DEPEND+=-lunwind
endif

CFLAGS+=

.PHONY:all
all:$(TARGET)

.PHONY: clean
clean:
	rm -f $(OBJS) $(TARGET)

$(TARGET):$(OBJS)
	${CC} ${CFLAGS} ${LDFLAGS} $(LIBS_DEPEND) -o $(TARGET) $(OBJS)
%.o:%.c
	${CC} -c ${CFLAGS} $< -o $@