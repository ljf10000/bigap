INCS+=-Isrc

OBJS=src/duktape.o 	\
	module/main.o 	\
	module/global.o \
	module/duk.o 	\
	module/my.o 	\
	module/libc.o 	\
	module/libcurl.o \
	module/libz.o 	\
	module/libbz.o 	\
	module/liblz.o	\
	#end

__TARGET=js
TARGET=$(__TARGET)
LIBS_DEPEND=

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
