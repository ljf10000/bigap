
OBJS=js.o

__TARGET=libjs
TARGET=$(__TARGET).so
LIBS_DEPEND=-lc -lm -ljson-c

ifeq (1,$(UBACKTRACE))
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
