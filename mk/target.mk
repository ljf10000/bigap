.PHONY: all
all:$(TARGET) install

$(TARGET):endian gen $(OBJS)
	$(dump_common_vars)
	${CC} ${CFLAGS} ${__CFLAGS} ${__LDFLAGS} ${LDFLAGS} -o $(TARGET) $(OBJS) $(lib_depend)
	@if [ -f "$(FILENO_NAME).key" ]; then \
		mkdir -p $(AK_PATH); \
		cp -f $(FILENO_NAME).key $(AK_PATH); \
	fi
%.o:%.c
	${CC} -c ${CFLAGS} ${__CFLAGS} $(INCS) $< -o $@

pre:$(DEPS) $(PRES)
	$(dump_common_vars)
%.d:%.c
	${CC} -MMD -E $(<) ${CFLAGS} ${__CFLAGS} $(INCS) >/dev/null
%.i:%.c
	${CC} -E $(<) ${CFLAGS} ${__CFLAGS} $(INCS) >>$(@)

gen:
	@if [[ -x $(GEN_SH) ]]; then \
		./$(GEN_SH); \
	fi
endian:
	@if [[ -x $(SCRIPT_PATH)/64 ]]; then \
		pushd $(SCRIPT_PATH); \
			./64.sh; \
		popd; \
	fi

.PHONY: check
check:
	$(do_check)



.PHONY: clean
clean:
	$(do_clean)



.PHONY: install
install:
	$(do_install)
