GIT_ADD="git add --all"
GIT_COMMIT="git commit -m 'Automatic commit by debarron'"
GIT_PUSH="git push origin master"

SRC = ./src
LIB = ./lib
BIN = ./bin
RC_HOME = ${RAMCLOUD_HOME}

CC = g++
CC_FLAGS = -Wall -std=c++0x -Wextra -O3

RAMCLOUD_INCLUDE = -I${RC_HOME}/src -I${RC_HOME}/obj.master
RAMCLOUD_LIB = -L${RC_HOME}/obj.master -L${RC_HOME}/install/include/ramcloud -lramcloud
RAMCLOUD_FLAGS = ${RAMCLOUD_INCLUDE} ${RAMCLOUD_LIB}
BUILD_FLAGS = ${RAMCLOUD_FLAGS} -I${SRC} -L${LIB}
OBJ_CC = ${CC} ${CC_FLAGS} -I${SRC} -g -c
APP_CC = ${CC} ${CC_FLAGS} -I${SRC} ${RAMCLOUD_FLAGS} -g -c

RC_PROXY_OBJS = RCEntry RCTable RCRelation
RC_PROXY_LIB = RCProxy
RC_PROXY_TESTS = RCProxyTest

RC_CORE_OBJS = `find ${LIB} | grep RC --colour=never | xargs`

all: $(RC_PROXY_OBJS) $(RC_PROXY_LIB) $(RC_PROXY_TESTS)
.PHONY: all

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
	${APP_CC} ${SRC}/$@.cc -o ${LIB}/$@.o

$(RC_PROXY_TESTS): $(RC_PROXY_OBJS) $(RC_PROXY_LIB)
	@echo ">> ramcloud-proxy Building $@"
	${APP_CC} ${SRC}/$@.cc -o ${BIN}/$@

