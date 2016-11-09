
OBJS=duktape.o \
	duk.o \
	global.o \
	libbz.o \
	libc.o \
	libcurl.o \
	liblz.o \
	libz.o \
	my.o \
	#end
__TARGET=libjs
TARGET=$(__TARGET).so
LIBS_DEPEND=

.PHONY:all
all:$(TARGET)
	
.PHONY: clean
clean:
	rm -f $(OBJS) $(TARGET)

$(TARGET):$(OBJS)
	${CC} ${CFLAGS} ${LDFLAGS} $(LIBS_DEPEND) -o $(TARGET) $(OBJS)
%.o:%.c
	${CC} -c ${CFLAGS} $@ $(OBJS)) $< -o $@
