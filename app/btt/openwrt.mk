
OBJS=btt.o
__TARGET=btt
TARGET=$(__TARGET)
LIBS_DEPEND=-ljson-c

ifdef UBACKTRACE
LIBS_DEPEND+=-lubacktrace
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