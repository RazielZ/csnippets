CC = gcc

INCLUDE_DIR = src
CFLAGS      = -O0 -g -I${INCLUDE_DIR} -Wall -Wno-unused-parameter -Wno-unused-but-set-variable -Wno-sign-compare -Wextra
LIBS        = 

ifdef rwtest
TEST  = rwlock_test.c
LIBS += -lpthread
else
TEST  = tests.c
endif

BIN_DIR = bin
BIN     = ${BIN_DIR}/a

SRC_DIR = src
SRC     = socket.c strmisc.c map.c error.c config.c rwlock.c asprintf.c ${TEST}
OBJ_DIR = obj
OBJ     = $(SRC:%.c=${OBJ_DIR}/%.o)
PRE     = pre.h

define compile
	@echo " CC  $@"
	@$(CC) -c $(CFLAGS) $1 -o $@ $<
endef

all: $(BIN)
clean:
	$(RM) ${OBJ_DIR}/*.o ${OBJ_DIR}/*.gch $(BIN)

${OBJ_DIR}/pre.h.gch: ${SRC_DIR}/${PRE}
	${compile}

${BIN}: ${BIN_DIR} ${OBJ_DIR} ${OBJ}
	@echo " LD  $@"
	@$(CC) $(LIBS) $(CFLAGS) -o $@ $(OBJ)

${OBJ_DIR}/%.o: ${SRC_DIR}/%.c ${OBJ_DIR}/${PRE}.gch
	${call compile, -include $(PRE)}

${OBJ_DIR}:
	@mkdir -p ${OBJ_DIR}
${BIN_DIR}:
	@mkdir -p ${BIN_DIR}

