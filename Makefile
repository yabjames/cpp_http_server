.RECIPEPREFIX = >

SRC_DIR:=./src
BUILD_DIR:=./build
INC_DIR:=./include

CC:=g++
CFLAGS:=-Wall -Wextra -std=c++20 $(addprefix -I,$(INC_DIRS))

SRC:=$(wildcard $(SRC_DIR)/*.cpp)
OBJ:=$(SRC:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
DEP:=$(OBJ:%.o=%.d)

EXE:=a.out

$(EXE): $(OBJ)
> $(CC) -o $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
> @mkdir -p $(dir $@) # dir keeps the trailing slash
> $(CC) $(CFLAGS) -MMD -g -c $^ -o $@

clean:
> rm -r $(BUILD_DIR)

-include $(DEP)
