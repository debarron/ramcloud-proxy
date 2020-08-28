GIT_ADD="git add --all"
GIT_COMMIT="git commit -m 'Automatic commit by debarron'"
GIT_PUSH="git push origin master"

SRC = ./src
LIB = ./lib
BIN = ./bin
RC_HOME = ${RAMCLOUD_HOME}

CC = g++
CC_FLAGS = -Wall -std=c++11 -Wextra -O3

RAMCLOUD_INCLUDE = -I${RC_HOME}/src -I${RC_HOME}/obj.master -I${RC_HOME}/install/include/ramcloud 
RAMCLOUD_LIB = -L${RC_HOME}/obj.master -L${RC_HOME}/install/lib -lramcloud
RAMCLOUD_FLAGS = ${RAMCLOUD_INCLUDE} ${RAMCLOUD_LIB}

OBJ_CC = ${CC} ${CC_FLAGS} -I${SRC} -g -c
APP_CC = ${CC} ${CC_FLAGS} -I${SRC} -I${LIB} ${RAMCLOUD_FLAGS} -g

RC_PROXY_OBJS_FILE = $(shell find ${LIB} | grep '\.o' --colour=never | xargs)
RC_PROXY_OBJS = RCEntry RCTable RCRelation
RC_PROXY_LIB = RCProxy
RC_PROXY_TESTS = RCProxyTest

all: create-dirs $(RC_PROXY_OBJS) $(RC_PROXY_LIB) $(RC_PROXY_TESTS)
.PHONY: all

create-dirs:
	[ -d ${LIB} ] || mkdir ${LIB}
	[ -d ${BIN} ] || mkdir ${BIN}

push: 
	@echo ">> ramcloud-multiwrite, Saving project on github"
	eval ${GIT_ADD}
	eval ${GIT_COMMIT}
	eval ${GIT_PUSH}

clean: 
	@echo ">> ramcloud-multiwrite, Cleaning ./lib"
	@rm ${LIB}/* 2< /dev/null || echo "Nothing to remove"
	@echo ">> ramcloud-multiwrite, Cleaning ./bin"
	@rm ${BIN}/* 2< /dev/null || echo "Nothing to remove"


$(RC_PROXY_OBJS): 
	@echo ">> ramcloud-proxy Building $@.o"
	${OBJ_CC} ${SRC}/$@.cc -o ${LIB}/$@.o

$(RC_PROXY_LIB): $(RC_PROXY_OBJS)
	@echo ">> ramcloud-proxy Building $@"
	${APP_CC} ${SRC}/$@.cc -c -o ${LIB}/$@.o

$(RC_PROXY_TESTS): $(RC_PROXY_OBJS) $(RC_PROXY_LIB)
	@echo ">> ramcloud-proxy Building $@"
	${APP_CC} -I${LIB} ${SRC}/$@.cc -c -o ${LIB}/$@.o
	${CC} ${CC_FLAGS} -o ${BIN}/$@ ${LIB}/$@.o ${RC_PROXY_OBJS_FILE} ${RAMCLOUD_FLAGS} -I${SRC} -I${LIB}

RCWrapper.o:
	g++ -g -w -std=c++11 -c -o src-11/RCWrapper.o ./src-11/RCWrapper.cpp \
		-I/users/dl544/RAMCloud/src \
		-I/users/dl544/RAMCloud/obj.master \
		-I/users/dl544/RAMCloud/install/include/ramcloud \
		-I/users/dl544/RAMCloud/install/lib/ramcloud \
		-I./src-11 \
		-L/users/dl544/RAMCloud/obj.master \
		-L/users/dl544/RAMCloud/install/include/ramcloud \
		-L/users/dl544/RAMCloud/install/lib/ramcloud \
		-lramcloud

RCWrapperTest.o: RCWrapper.o
	g++ -g -w -std=c++11 -c -o ./src-11/RCWrapperTest.o ./src-11/RCWrapperTest.cpp \
		-I/users/dl544/RAMCloud/src \
		-I/users/dl544/RAMCloud/obj.master \
		-I/users/dl544/RAMCloud/install/include/ramcloud \
		-I/users/dl544/RAMCloud/install/lib/ramcloud \
		-I./src-11 \
		-L/users/dl544/RAMCloud/obj.master \
		-L/users/dl544/RAMCloud/install/include/ramcloud \
		-L/users/dl544/RAMCloud/install/lib/ramcloud \
		-lramcloud

RCWrapperTest: RCWrapperTest.o RCWrapper.o
	g++ -g -w -std=c++11 -o ./src-11/RCWrapperTest ./src-11/RCWrapperTest.o ./src-11/RCWrapper.o \
		-I/users/dl544/RAMCloud/src \
		-I/users/dl544/RAMCloud/obj.master \
		-I/users/dl544/RAMCloud/install/include/ramcloud \
		-I/users/dl544/RAMCloud/install/lib/ramcloud \
		-I./src-11 \
		-L/users/dl544/RAMCloud/obj.master \
		-L/users/dl544/RAMCloud/install/include/ramcloud \
		-L/users/dl544/RAMCloud/install/lib/ramcloud \
		-lramcloud

RCWrapper_C.o: RCWrapper.o
	g++ -g -w -std=c++11 -c -o ./src-11/RCWrapper_C.o ./src-11/RCWrapper_C.cpp \
		-I/users/dl544/RAMCloud/src \
		-I/users/dl544/RAMCloud/obj.master \
		-I/users/dl544/RAMCloud/install/include/ramcloud \
		-I/users/dl544/RAMCloud/install/lib/ramcloud \
		-I./src-11 \
		-L/users/dl544/RAMCloud/obj.master \
		-L/users/dl544/RAMCloud/install/include/ramcloud \
		-L/users/dl544/RAMCloud/install/lib/ramcloud \
		-lramcloud

RCWrapper_CTest.o: RCWrapper.o RCWrapper_C.o
	g++ -g -w -std=c++11 -c -o ./src-11/RCWrapper_CTest.o ./src-11/RCWrapper_CTest.cpp \
		-I/users/dl544/RAMCloud/src \
		-I/users/dl544/RAMCloud/obj.master \
		-I/users/dl544/RAMCloud/install/include/ramcloud \
		-I/users/dl544/RAMCloud/install/lib/ramcloud \
		-I./src-11 \
		-L/users/dl544/RAMCloud/obj.master \
		-L/users/dl544/RAMCloud/install/include/ramcloud \
		-L/users/dl544/RAMCloud/install/lib/ramcloud \
		-lramcloud

RCWrapper_CTest: RCWrapper.o RCWrapper_C.o RCWrapper_CTest.o
	g++ -g -w -std=c++11 -o ./src-11/RCWrapper_CTest ./src-11/RCWrapper_CTest.o \
		./src-11/RCWrapper_C.o ./src-11/RCWrapper.o \
		-I/users/dl544/RAMCloud/src \
		-I/users/dl544/RAMCloud/obj.master \
		-I/users/dl544/RAMCloud/install/include/ramcloud \
		-I/users/dl544/RAMCloud/install/lib/ramcloud \
		-I./src-11 \
		-L/users/dl544/RAMCloud/obj.master \
		-L/users/dl544/RAMCloud/install/include/ramcloud \
		-L/users/dl544/RAMCloud/install/lib/ramcloud \
		-lramcloud




#g++ -w -std=c++11 -c -o lib/C_RCProxy_test.o ./src/C_RCProxy_test.c -I/users/dl544/RAMCloud/src -I/users/dl544/RAMCloud/obj.master -L/users/dl544/RAMCloud/obj.master -L/users/dl544/RAMCloud/install/include/ramcloud -lramcloud -I./lib -I./src -Imodules/ramcloud-proxy/src -Imodules/ramcloud-proxy/lib
#
#g++ -w -std=c++11 -c -o lib/RCProxyTest.o ./src/RCProxyTest.cc -I/users/dl544/RAMCloud/src -I/users/dl544/RAMCloud/obj.master -L/users/dl544/RAMCloud/obj.master -L/users/dl544/RAMCloud/install/include/ramcloud -lramcloud -I./lib -I./src -Imodules/ramcloud-proxy/src -Imodules/ramcloud-proxy/lib
#
#g++ -w -std=c++11 -o lib/RCProxyTest ./lib/RCProxyTest.o ./lib/RCEntry.o ./lib/RCTable.o ./lib/RCRelation.o ./lib/RCProxy.o -I/users/dl544/RAMCloud/src -I/users/dl544/RAMCloud/obj.master -L/users/dl544/RAMCloud/obj.master -L/users/dl544/RAMCloud/install/include/ramcloud -lramcloud -I./lib -I./src -Imodules/ramcloud-proxy/src -Imodules/ramcloud-proxy/lib
#


