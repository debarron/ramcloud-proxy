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

RC_CORE_OBJS = `find ${LIB} | grep RC --colour=never | xargs`
BUILD_FLAGS = ${RAMCLOUD_FLAGS} -I${SRC} -L${LIB}

OBJ_CC = ${CC} ${CC_FLAGS} -I${SRC} -g -c
APP_CC = ${CC} ${CC_FLAGS} -I${SRC} ${RAMCLOUD_FLAGS} -g -c

build_dirs:
	mkdir ${LIB} && mkdir ${BIN}

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

${LIB}/RCEntry.o: 
	@echo ">> ramcloud-multiwrite, Building RCEntry.o"
	${OBJ_CC} ${SRC}/RCEntry.cc -o ${LIB}/RCEntry.o 

${LIB}/RCTable.o: 
	@echo ">> ramcloud-multiwrite, Building RCTable.o"
	${OBJ_CC} ${SRC}/RCTable.cc -o ${LIB}/RCTable.o 

${LIB}/RCRelation.o: 
	@echo ">> ramcloud-multiwrite, Building RCRelation.o"
	${OBJ_CC} ${SRC}/RCRelation.cc -o ${LIB}/RCRelation.o 

${LIB}/RCProxy.o:  
	@echo ">> ramcloud-multiwrite, Building RCProxy.o"
	${APP_CC} ${SRC}/RCProxy.cc -o ${LIB}/RCProxy.o

${LIB}/RCProxyTest.o: 
	@echo ">> ramcloud-multiwrite, Building RCProxyTest.o"
	${APP_CC} ${SRC}/RCProxyTest.cc -o ${LIB}/RCProxyTest.o

RC_CORE: ${LIB}/RCEntry.o ${LIB}/RCTable.o ${LIB}/RCRelation.o ${LIB}/RCProxy.o ${LIB}/RCProxyTest.o

RCProxyTest: build_dirs RC_CORE
	${CC} ${RC_CORE_OBJS} -o ${BIN}/RCProxyTest ${BUILD_FLAGS}

all: RC_CORE RCProxyTest



