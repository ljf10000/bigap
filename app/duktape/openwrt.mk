INCS+=-Isrc

OBJS=module/main.o

__TARGET=js
TARGET=$(__TARGET)
LIBS_DEPEND=-lc -lm -ljson-c -ljs

ifeq (1,$(UBACKTRACE))
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
