#include <string.h>

#include "seatest.h"
#include "nstrct.h"

/* Test Data */

nstrct_argument_t arguments[13];
nstrct_argument_value_t values[3];
nstrct_instruction_t instruction;

nstrct_preallocation_t preallocation;
nstrct_argument_t preallocated_arguments[20];
nstrct_argument_value_t preallocated_values[10];

nstrct_cursor_t write_cursor;
nstrct_cursor_t read_cursor;

uint16_t buffer_size = 512;
char buffer[512];


void init() {
  arguments[0].type = NSTRCT_DATATYPE_BOOLEAN;
  arguments[0].value.boolean = NSTRCT_FALSE;
  arguments[1].type = NSTRCT_DATATYPE_INT8;
  arguments[1].value.int8 = 11;
  arguments[2].type = NSTRCT_DATATYPE_INT16;
  arguments[2].value.int16 = 12;
  arguments[3].type = NSTRCT_DATATYPE_INT32;
  arguments[3].value.int32 = 13;
  arguments[4].type = NSTRCT_DATATYPE_INT64;
  arguments[4].value.int64 = 14;
  arguments[5].type = NSTRCT_DATATYPE_UINT8;
  arguments[5].value.uint8 = 21;
  arguments[6].type = NSTRCT_DATATYPE_UINT16;
  arguments[6].value.uint16 = 22;
  arguments[7].type = NSTRCT_DATATYPE_UINT32;
  arguments[7].value.uint32 = 23;
  arguments[8].type = NSTRCT_DATATYPE_UINT64;
  arguments[8].value.uint64 = 24;
  arguments[9].type = NSTRCT_DATATYPE_FLOAT32;
  arguments[9].value.float32 = 5.5;
  arguments[10].type = NSTRCT_DATATYPE_FLOAT64;
  arguments[10].value.float64 = 6.5;
  arguments[11].type = NSTRCT_DATATYPE_STRING;
  arguments[11].value.string.size = 5;
  arguments[11].value.string.ptr = "hello";

  values[0].boolean = NSTRCT_TRUE;
  values[1].boolean = NSTRCT_FALSE;
  values[2].boolean = NSTRCT_TRUE;
  
  arguments[12].type = NSTRCT_DATATYPE_ARRAY;
  arguments[12].array_element_type = NSTRCT_DATATYPE_BOOLEAN;
  arguments[12].array_elements = 3;
  arguments[12].array_values = values;
  
  instruction.code = 245;
  instruction.num_arguments = 13;
  instruction.arguments = arguments;
  
  preallocation.arguments = preallocated_arguments;
  preallocation.array_values = preallocated_values;
  preallocation.max_arguments = 20;
  preallocation.max_array_values = 10;
}

/* Helpers */

void dump_buffer(char * buffer, uint16_t length) {
  uint16_t i;
  for(i=0; i<length; i++) {
    printf("%u ",(uint8_t)buffer[i]);
  }
  printf("\n");
}

void clear_buffer(char * buffer, uint16_t length) {
  uint16_t i;
  for(i=0; i<length; i++) {
    buffer[i] = 0;
  }
}

/* Tests */

void test_instruction() {
  write_cursor = 0;
  read_cursor = 0;
  
  nstrct_full_pack(&instruction, buffer, buffer_size, &write_cursor);
  nstrct_error_t ret = nstrct_preallocated_unpack(&preallocation, buffer, write_cursor, &read_cursor);
  
  assert_true(ret == NSTRCT_ERROR_SUCCESS);
  assert_true(write_cursor == read_cursor);

  assert_true(preallocation.instruction.code == 245);
  assert_true(preallocation.instruction.num_arguments == 13);
  assert_true(preallocation.instruction.arguments[0].type == NSTRCT_DATATYPE_BOOLEAN);
  assert_true(preallocation.instruction.arguments[0].value.boolean == NSTRCT_FALSE);
  assert_true(preallocation.instruction.arguments[1].type == NSTRCT_DATATYPE_INT8);
  assert_true(preallocation.instruction.arguments[1].value.int8 == 11);
  assert_true(preallocation.instruction.arguments[2].type == NSTRCT_DATATYPE_INT16);
  assert_true(preallocation.instruction.arguments[2].value.int16 == 12);
  assert_true(preallocation.instruction.arguments[3].type == NSTRCT_DATATYPE_INT32);
  assert_true(preallocation.instruction.arguments[3].value.int32 == 13);
  assert_true(preallocation.instruction.arguments[4].type == NSTRCT_DATATYPE_INT64);
  assert_true(preallocation.instruction.arguments[4].value.int64 == 14);
  assert_true(preallocation.instruction.arguments[5].type == NSTRCT_DATATYPE_UINT8);
  assert_true(preallocation.instruction.arguments[5].value.uint8 == 21);
  assert_true(preallocation.instruction.arguments[6].type == NSTRCT_DATATYPE_UINT16);
  assert_true(preallocation.instruction.arguments[6].value.uint16 == 22);
  assert_true(preallocation.instruction.arguments[7].type == NSTRCT_DATATYPE_UINT32);
  assert_true(preallocation.instruction.arguments[7].value.uint32 == 23);
  assert_true(preallocation.instruction.arguments[8].type == NSTRCT_DATATYPE_UINT64);
  assert_true(preallocation.instruction.arguments[8].value.uint64 == 24);
  assert_true(preallocation.instruction.arguments[9].type == NSTRCT_DATATYPE_FLOAT32);
  assert_true(preallocation.instruction.arguments[9].value.float32 == 5.5);
  assert_true(preallocation.instruction.arguments[10].type == NSTRCT_DATATYPE_FLOAT64);
  assert_true(preallocation.instruction.arguments[10].value.float64 == 6.5);
  assert_true(preallocation.instruction.arguments[11].type == NSTRCT_DATATYPE_STRING);
  assert_true(preallocation.instruction.arguments[11].value.string.size == 5);
  assert_true(preallocation.instruction.arguments[12].type == NSTRCT_DATATYPE_ARRAY);
  assert_true(preallocation.instruction.arguments[12].array_element_type == NSTRCT_DATATYPE_BOOLEAN);
  assert_true(preallocation.instruction.arguments[12].array_elements == 3);
  assert_true(preallocation.instruction.arguments[12].array_values[0].boolean == NSTRCT_TRUE);
  assert_true(preallocation.instruction.arguments[12].array_values[1].boolean == NSTRCT_FALSE);
  assert_true(preallocation.instruction.arguments[12].array_values[2].boolean == NSTRCT_TRUE);
  assert_true(strcmp(preallocation.instruction.arguments[11].value.string.ptr, "hello"));
}

/* Framework */

void test_library() {
  init();
  test_fixture_start();
  run_test(test_instruction);
  test_fixture_end();
}

void all_tests() {
  test_library();
}

int main(int argc, char** argv) {
  return run_tests(all_tests);
}