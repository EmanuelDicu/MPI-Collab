TARGET = mpi_collab

CC = mpicc
CFLAGS = -Wall -Wextra -pedantic-errors

OUTDIR = .
DIR_OBJ = ./obj
SUBDIR = array cluster helper workload 
INC_SUBDIR = $(addsuffix /inc, $(SUBDIR))
SRC_SUBDIR = $(addsuffix /src, $(SUBDIR))

INC = $(wildcard *.h $(foreach fd, $(INC_SUBDIR), $(fd)/*.h))
SRC = $(wildcard *.c $(foreach fd, $(SRC_SUBDIR), $(fd)/*.c))
OBJ = $(addprefix $(DIR_OBJ)/, $(SRC:c=o))
INC_DIR = -I./ $(addprefix -I, $(INC_SUBDIR))

build: $(TARGET) 

PHONY := $(TARGET)
$(TARGET): $(OBJ)
	$(CC) -o $(OUTDIR)/$@ $(OBJ) 

$(DIR_OBJ)/%.o: %.c $(INC)
	mkdir -p $(@D)
	$(CC) -o $@ -c $< $(CFLAGS) $(INC_DIR)

PHONY += clean
clean:
	rm -rf $(TARGET) $(DIR_OBJ)/*

PHONY += echoes
echoes:
	@echo "SUBDIR: $(SUBDIR)"
	@echo "INC_SUBDIR: $(INC_SUBDIR)"
	@echo "SRC_SUBDIR: $(SRC_SUBDIR)"
	@echo "INC files: $(INC)"
	@echo "SRC files: $(SRC)"
	@echo "OBJ files: $(OBJ)"
	@echo "INC DIR: $(INC_DIR)"

.PHONY = $(PHONY)