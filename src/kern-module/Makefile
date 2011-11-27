E_ENCODE = $(shell echo $(1) | sed -e 's!_!_1!g' -e 's!/!_2!g')
E_DECODE = $(shell echo $(1) | sed -e 's!_1!_!g' -e 's!_2!/!g')

.PHONY: all

include ${T_BASE}/mk/compk.mk
include ${T_BASE}/mk/template.mk

SRCFILES := $(shell find . '(' '!' -regex '.*/_.*' ')' -and -iname "*.c"  | sed -e 's!\./!!g')

all: ${T_OBJ}/user-sfs/kern-module ${T_OBJ}/user-sfs/kern-module/mod-test.o

${T_OBJ}/user-sfs/kern-module/mod-test.o:
	@echo CC $@
	${V}${CC} -c ${SRCFILES} -o$@

${T_OBJ}/user-sfs/kern-module:
	${V}mkdir -p $@
