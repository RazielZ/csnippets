CC = gcc

INCLUDE_DIR = src
CFLAGS      = -O0 -g -I${INCLUDE_DIR} -Wall -Wno-unused-parameter -Wno-unused-but-set-variable -Wno-sign-compare -Wextra
LIBS        = -ldl

BIN_DIR = bin
BIN     = ${BIN_DIR}/a.out

SRC_DIR = src
SRC     = socket.c strmisc.c map.c error.c config.c rwlock.c asprintf.c
OBJ_DIR = obj
OBJ     = $(SRC:%.c=${OBJ_DIR}/%.o)
PRE     = pre.h

all: $(BIN)
clean:
	$(RM) ${OBJ_DIR}/*.o ${OBJ_DIR}/*.gch
	${RM} $(BIN)

${OBJ_DIR}/pre.h.gch: ${SRC_DIR}/${PRE}
	@echo " CC  PCH"
	@$(CC) -c $(CFLAGS) -o $@ $<

${BIN}: ${BIN_DIR} ${OBJ_DIR} ${OBJ}
	@echo " LD  $@"
	@$(CC) $(LIBS) $(CFLAGS) -o $@ $(OBJ)

${OBJ_DIR}/%.o: ${SRC_DIR}/%.c ${OBJ_DIR}/${PRE}.gch
	@echo " CC  $@"
	@$(CC) -c $(CFLAGS) -include pre.h -o $@ $<

${OBJ_DIR}:
	@mkdir -p ${OBJ_DIR}
${BIN_DIR}:
	@mkdir -p ${BIN_DIR}

