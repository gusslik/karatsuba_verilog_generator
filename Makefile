BUILD_DIR ?= build
GEN := $(BUILD_DIR)/generator
TEST_NS ?= 1 2 3 4 5 7 8 9 16 33 64 65 128 255
TEST_DIR ?= tests

.PHONY: all test clean

all:
	cmake -S . -B $(BUILD_DIR)
	cmake --build $(BUILD_DIR)

test: $(GEN)
	@for n in $(TEST_NS); do ./run_one.sh $(TEST_DIR) $(GEN) $$n || exit 1; done
	@echo "ALL TESTS PASSED"

clean:
	rm -rf $(TEST_DIR)