SRC = ./src-11
LIB = ./lib
BIN = ./bin
APPS = ./apps

RAMCLOUD_INCLUDE = -I${RAMCLOUD_HOME}/src -I${RAMCLOUD_HOME}/obj.master -I${RAMCLOUD_HOME}/install/include/ramcloud 
RAMCLOUD_LIB = -L${RAMCLOUD_HOME}/obj.master -L${RAMCLOUD_HOME}/install/lib/ramcloud -lramcloud
RAMCLOUD_FLAGS = ${RAMCLOUD_INCLUDE} ${RAMCLOUD_LIB}

RAMCLOUD_PROXY_OBJS = RCWrapper RCWrapper_C
RAMCLOUD_PROXY_TESTS = RCWrapperTest RCWrapper_CTest

all: create-dirs $(RAMCLOUD_PROXY_OBJS) $(RAMCLOUD_PROXY_TESTS)
.PHONY: 

create-dirs:
	[ -d ${LIB} ] || mkdir ${LIB}
	[ -d ${BIN} ] || mkdir ${BIN}
	[ -d ${APPS} ] || mkdir ${APPS}

clean: 
	@echo ">> ramcloud-multiwrite, Cleaning ./lib"
	@rm ${LIB}/* 2< /dev/null || echo "Nothing to remove"
	@echo ">> ramcloud-multiwrite, Cleaning ./bin"
	@rm ${BIN}/* 2< /dev/null || echo "Nothing to remove"

$(RAMCLOUD_PROXY_OBJS): 
	@echo ">> ramcloud-proxy Building $@.o"
	g++ -g -w -std=c++11 -c -o ${LIB}/$@.o ${SRC}/$@.cpp -I${SRC} ${RAMCLOUD_FLAGS}

$(RAMCLOUD_PROXY_TESTS): $(RAMCLOUD_PROXY_OBJS)
	@echo ">> ramcloud-proxy Building $@"
	g++ -g -w -std=c++11 -c -o ${LIB}/$@.o ${SRC}/$@.cpp -I${SRC} ${RAMCLOUD_FLAGS} || \
	g++ -g -w -std=c++11 -c -o ${LIB}/$@.o ${SRC}/$@.c -I${SRC} ${RAMCLOUD_FLAGS}
	g++ -g -w -std=c++11 -o ${BIN}/$@ ${LIB}/$@.o ${LIB}/RCWrapper.o ${LIB}/RCWrapper_C.o \
		-I${SRC} ${RAMCLOUD_FLAGS}


