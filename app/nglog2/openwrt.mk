
OBJS=nglog2.o
__TARGET=nglog2
TARGET=$(__TARGET)
LIBS_DEPEND=

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