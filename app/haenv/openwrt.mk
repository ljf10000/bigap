
OBJS=haenv.o
__TARGET=haenv
TARGET=$(__TARGET)
LIBS_DEPEND=-lm -ljson-c -lweos

ifeq (1,$(UBACKTRACE))
LIBS_DEPEND+=-lubacktrace
endif

ifeq (2,$(UBACKTRACE))
LIBS_DEPEND+=-lunwind
endif

.PHONY:all
all:$(TARGET)

.PHONY: clean
clean:
	rm -f $(OBJS) $(TARGET)

$(TARGET):$(OBJS)
	${CC} ${CFLAGS} ${LDFLAGS} $(LIBS_DEPEND) -o $(TARGET) $(OBJS)
%.o:%.c
	${CC} -c ${CFLAGS} $< -o $@