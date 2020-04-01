LOCAL_LIB = ./lib
LOCAL_BIN = ./bin
LOCAL_SRC= ./src

RC_DIR = /users/dl544/RAMCloud
SOURCE_DIR=./src
BIN_DIR=./bin

INCLUDE_FLAGS = -I${RC_DIR}/src -I${RC_DIR}/obj.master -I${LOCAL_SRC}
LIB_FLAGS = -L${RC_DIR}/obj.master -L${RC_DIR}/install/include/ramcloud -L${LOCAL_LIB}
LIBS = -lramcloud
FLAGS = -std=c++0x -Wall -Wextra -O3 ${INCLUDE_FLAGS} ${LIB_FLAGS} ${LIBS}
FUSE_FLAGS = gcc -Wall cuse.c `pkg-config fuse3 --cflags --libs`

GIT_ADD="git add --all"
GIT_COMMIT="git commit -m 'Automatic commit by debarron'"
GIT_PUSH="git push origin dev"

SRC = ./src
LIB = ./lib
BIN = ./bin
RC_HOME = /users/dl544/RAMCloud

CC = g++
CC_FLAGS = -Wall -std=c++0x -Wextra -O3

RAMCLOUD_INCLUDE = -I${RC_HOME}/src -I${RC_HOME}/obj.master
RAMCLOUD_LIB = -L${RC_HOME}/obj.master -L${RC_HOME}/install/include/ramcloud -lramcloud
RAMCLOUD_FLAGS = ${RAMCLOUD_INCLUDE} ${RAMCLOUD_LIB}

RC_CORE_OBJS = `find ${LIB} | grep RC --colour=never | xargs`
BUILD_FLAGS = ${RAMCLOUD_FLAGS} -I${SRC} -L${LIB}

OBJ_CC = ${CC} ${CC_FLAGS} -I${SRC} -g -c
APP_CC = ${CC} ${CC_FLAGS} -I${SRC} ${RAMCLOUD_FLAGS} -g -c

push: 
	@echo ">> ramcloud-multiwrite, Saving project on github"
	eval ${GIT_ADD}
	eval ${GIT_COMMIT}
	eval ${GIT_PUSH}

clean: 
	@echo ">> ramcloud-multiwrite, Cleaning ./lib"
	@rm ${LIB}/* || echo "Nothing to remove"
	@echo ">> ramcloud-multiwrite, Cleaning ./bin"
	@rm ${BIN}/* || echo "Nothing to remove"

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

RCProxyTest: RC_CORE
	${CC} ${RC_CORE_OBJS} -o ${BIN}/RCProxyTest ${BUILD_FLAGS}

all: RC_CORE RCProxyTest



