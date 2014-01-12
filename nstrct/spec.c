#include <string.h>

#include "seatest.h"
#include "nstrct.h"

/* Test Data */

nstrct_argument_t arguments1[13];
nstrct_argument_value_t values1[3];
nstrct_instruction_t instruction;

void init() {
  arguments1[0].type = NSTRCT_DATATYPE_BOOLEAN;
  arguments1[0].value.boolean = NSTRCT_FALSE;
  arguments1[1].type = NSTRCT_DATATYPE_INT8;
  arguments1[1].value.int8 = 11;
  arguments1[2].type = NSTRCT_DATATYPE_INT16;
  arguments1[2].value.int16 = 12;
  arguments1[3].type = NSTRCT_DATATYPE_INT32;
  arguments1[3].value.int32 = 13;
  arguments1[4].type = NSTRCT_DATATYPE_INT64;
  arguments1[4].value.int64 = 14;
  arguments1[5].type = NSTRCT_DATATYPE_UINT8;
  arguments1[5].value.uint8 = 21;
  arguments1[6].type = NSTRCT_DATATYPE_UINT16;
  arguments1[6].value.uint16 = 22;
  arguments1[7].type = NSTRCT_DATATYPE_UINT32;
  arguments1[7].value.uint32 = 23;
  arguments1[8].type = NSTRCT_DATATYPE_UINT64;
  arguments1[8].value.uint64 = 24;
  arguments1[9].type = NSTRCT_DATATYPE_FLOAT32;
  arguments1[9].value.float32 = 5.5;
  arguments1[10].type = NSTRCT_DATATYPE_FLOAT64;
  arguments1[10].value.float64 = 6.5;
  arguments1[11].type = NSTRCT_DATATYPE_STRING;
  arguments1[11].value.string.size = 5;
  arguments1[11].value.string.ptr = "hello";

  values1[0].boolean = NSTRCT_TRUE;
  values1[1].boolean = NSTRCT_FALSE;
  values1[2].boolean = NSTRCT_TRUE;
  
  arguments1[12].type = NSTRCT_DATATYPE_ARRAY;
  arguments1[12].array_element_type = NSTRCT_DATATYPE_BOOLEAN;
  arguments1[12].array_elements = 3;
  arguments1[12].array_values = values1;
  
  instruction.code = 245;
  instruction.num_arguments = 13;
  instruction.arguments = arguments1;
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
  char buffer[nstrct_instruction_length(&instruction)];
  nstrct_cursor_t write_cursor = 0;
  nstrct_cursor_t read_cursor = 0;

  nstrct_pack_instruction(&instruction, buffer, &write_cursor);
  nstrct_pack_arguments(instruction.arguments, &instruction.num_arguments, buffer, &write_cursor);

  nstrct_instruction_t instruction2;
  nstrct_unpack_instruction(&instruction2, buffer, &read_cursor);
  nstrct_argument_t arguments2[instruction2.num_arguments];
  nstrct_argument_value_t values2[instruction2.num_array_elements];
  instruction2.arguments = arguments2;
  nstrct_unpack_arguments(instruction2.arguments, &instruction2.num_arguments, values2, buffer, &read_cursor);

  assert_true(instruction2.code == 245);
  assert_true(instruction2.num_arguments == 13);
  assert_true(instruction2.arguments[0].type == NSTRCT_DATATYPE_BOOLEAN);
  assert_true(instruction2.arguments[0].value.boolean == NSTRCT_FALSE);
  assert_true(instruction2.arguments[1].type == NSTRCT_DATATYPE_INT8);
  assert_true(instruction2.arguments[1].value.int8 == 11);
  assert_true(instruction2.arguments[2].type == NSTRCT_DATATYPE_INT16);
  assert_true(instruction2.arguments[2].value.int16 == 12);
  assert_true(instruction2.arguments[3].type == NSTRCT_DATATYPE_INT32);
  assert_true(instruction2.arguments[3].value.int32 == 13);
  assert_true(instruction2.arguments[4].type == NSTRCT_DATATYPE_INT64);
  assert_true(instruction2.arguments[4].value.int64 == 14);
  assert_true(instruction2.arguments[5].type == NSTRCT_DATATYPE_UINT8);
  assert_true(instruction2.arguments[5].value.uint8 == 21);
  assert_true(instruction2.arguments[6].type == NSTRCT_DATATYPE_UINT16);
  assert_true(instruction2.arguments[6].value.uint16 == 22);
  assert_true(instruction2.arguments[7].type == NSTRCT_DATATYPE_UINT32);
  assert_true(instruction2.arguments[7].value.uint32 == 23);
  assert_true(instruction2.arguments[8].type == NSTRCT_DATATYPE_UINT64);
  assert_true(instruction2.arguments[8].value.uint64 == 24);
  assert_true(instruction2.arguments[9].type == NSTRCT_DATATYPE_FLOAT32);
  assert_true(instruction2.arguments[9].value.float32 == 5.5);
  assert_true(instruction2.arguments[10].type == NSTRCT_DATATYPE_FLOAT64);
  assert_true(instruction2.arguments[10].value.float64 == 6.5);
  assert_true(instruction2.arguments[11].type == NSTRCT_DATATYPE_STRING);
  assert_true(instruction2.arguments[11].value.string.size == 5);
  assert_true(instruction2.arguments[12].type == NSTRCT_DATATYPE_ARRAY);
  assert_true(instruction2.arguments[12].array_element_type == NSTRCT_DATATYPE_BOOLEAN);
  assert_true(instruction2.arguments[12].array_elements == 3);
  assert_true(instruction2.arguments[12].array_values[0].boolean == NSTRCT_TRUE);
  assert_true(instruction2.arguments[12].array_values[1].boolean == NSTRCT_FALSE);
  assert_true(instruction2.arguments[12].array_values[2].boolean == NSTRCT_TRUE);
  assert_true(strcmp(instruction2.arguments[11].value.string.ptr, "hello"));
}

void test_frame() {
  char buffer[nstrct_frame_length(&instruction)];
  nstrct_cursor_t read_cursor = 0;
  nstrct_cursor_t write_cursor = 0;
  
  nstrct_pack_frame(&instruction, buffer, &write_cursor);
  uint16_t ret = nstrct_frame_available(buffer, write_cursor);
  assert_true(ret == NSTRCT_ERROR_SUCCESS);
  
//  dump_buffer(buffer, write_cursor);
  
  nstrct_instruction_t instruction2;
  nstrct_unpack_frame(&instruction2, buffer, &read_cursor);
  nstrct_argument_t arguments2[instruction2.num_arguments];
  nstrct_argument_value_t values2[instruction2.num_array_elements];
  instruction2.arguments = arguments2;
  nstrct_unpack_arguments(instruction2.arguments, &instruction2.num_arguments, values2, buffer, &read_cursor);
  
  assert_true(instruction2.code == 245);
  assert_true(instruction2.num_arguments == 13);
}

/* Framework */

void test_library() {
  test_fixture_start();
  run_test(test_instruction);
  run_test(test_frame);
  test_fixture_end();
}

void all_tests() {
  test_library();
}

int main(int argc, char** argv) {
  init();
  return run_tests(all_tests);
}