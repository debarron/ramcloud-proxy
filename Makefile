GIT_ADD="git add --all"
GIT_COMMIT="git commit -m 'Automatic commit by debarron'"
GIT_PUSH="git push origin master"

SRC = ./src
LIB = ./lib
BIN = ./bin
RC_HOME = ${RAMCLOUD_HOME}

CC = g++
CC_FLAGS = -Wall -std=c++11 -Wextra -O3

RAMCLOUD_INCLUDE = -I${RC_HOME}/src -I${RC_HOME}/obj.master
RAMCLOUD_LIB = -L${RC_HOME}/obj.master -L${RC_HOME}/install/include/ramcloud -lramcloud
RAMCLOUD_FLAGS = ${RAMCLOUD_INCLUDE} ${RAMCLOUD_LIB}

OBJ_CC = ${CC} ${CC_FLAGS} -I${SRC} -g -c
APP_CC = ${CC} ${CC_FLAGS} -I${SRC} ${RAMCLOUD_FLAGS} -g

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
	${APP_CC} ${SRC}/$@.cc -c -o ${LIB}/$@.o
	${APP_CC} ${RC_PROXY_OBJS_FILE} ${LIB}/$@.o -o ${BIN}/$@

#
#g++ -w -std=c++11 -c -o lib/C_RCProxy_test.o ./src/C_RCProxy_test.c -I/users/dl544/RAMCloud/src -I/users/dl544/RAMCloud/obj.master -L/users/dl544/RAMCloud/obj.master -L/users/dl544/RAMCloud/install/include/ramcloud -lramcloud -I./lib -I./src -Imodules/ramcloud-proxy/src -Imodules/ramcloud-proxy/lib
#
#g++ -w -std=c++11 -c -o lib/RCProxyTest.o ./src/RCProxyTest.cc -I/users/dl544/RAMCloud/src -I/users/dl544/RAMCloud/obj.master -L/users/dl544/RAMCloud/obj.master -L/users/dl544/RAMCloud/install/include/ramcloud -lramcloud -I./lib -I./src -Imodules/ramcloud-proxy/src -Imodules/ramcloud-proxy/lib
#
#g++ -w -std=c++11 -o lib/RCProxyTest ./lib/RCProxyTest.o ./lib/RCEntry.o ./lib/RCTable.o ./lib/RCRelation.o ./lib/RCProxy.o -I/users/dl544/RAMCloud/src -I/users/dl544/RAMCloud/obj.master -L/users/dl544/RAMCloud/obj.master -L/users/dl544/RAMCloud/install/include/ramcloud -lramcloud -I./lib -I./src -Imodules/ramcloud-proxy/src -Imodules/ramcloud-proxy/lib
#


