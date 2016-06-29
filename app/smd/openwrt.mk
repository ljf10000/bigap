
OBJS=smd.o
__TARGET=smd
TARGET=$(__TARGET)
LIBS_DEPEND=-ljson-c

ifdef UBACKTRACE
LIBS_DEPEND+=-lubacktrace
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