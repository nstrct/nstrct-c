#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#include "nstrct.h"

typedef enum {
  RM_GENERATE, RM_PROCESS, RM_INFO,
} run_mode_t;

float float32_max;
double float64_max;

void generate() {
  nstrct_argument_t arguments[14];
  arguments[0].type = NSTRCT_DATATYPE_BOOLEAN;
  arguments[0].value.boolean = NSTRCT_FALSE;
  arguments[1].type = NSTRCT_DATATYPE_INT8;
  arguments[1].value.int8 = INT8_MIN;
  arguments[2].type = NSTRCT_DATATYPE_INT16;
  arguments[2].value.int16 = INT16_MIN;
  arguments[3].type = NSTRCT_DATATYPE_INT32;
  arguments[3].value.int32 = INT32_MIN;
  arguments[4].type = NSTRCT_DATATYPE_INT64;
  arguments[4].value.int64 = INT64_MIN;
  arguments[5].type = NSTRCT_DATATYPE_UINT8;
  arguments[5].value.uint8 = UINT8_MAX;
  arguments[6].type = NSTRCT_DATATYPE_UINT16;
  arguments[6].value.uint16 = UINT16_MAX;
  arguments[7].type = NSTRCT_DATATYPE_UINT32;
  arguments[7].value.uint32 = UINT32_MAX;
  arguments[8].type = NSTRCT_DATATYPE_UINT64;
  arguments[8].value.uint64 = UINT64_MAX;
  arguments[9].type = NSTRCT_DATATYPE_FLOAT32;
  arguments[9].value.float32 = float32_max;
  arguments[10].type = NSTRCT_DATATYPE_FLOAT64;
  arguments[10].value.float64 = float64_max;
  arguments[11].type = NSTRCT_DATATYPE_STRING;
  arguments[11].value.string = nstrct_to_string("hello world");
  arguments[12].type = NSTRCT_DATATYPE_STRING;
  arguments[12].value.string = nstrct_to_string("");
  
  nstrct_argument_value_t values[2];
  values[0].uint16 = 2443;
  values[1].uint16 = 3443;
  arguments[13].type = NSTRCT_DATATYPE_ARRAY;
  arguments[13].array_element_type = NSTRCT_DATATYPE_UINT16;
  arguments[13].array_values = values;
  arguments[13].array_elements = 2;
  
  nstrct_instruction_t instruction;
  instruction.code = UINT16_MAX;
  instruction.num_arguments = 14;
  instruction.arguments = arguments;
  
  char buffer[nstrct_instruction_length(&instruction)];
  uint16_t write_cursor = 0;
  nstrct_pack_instruction(&instruction, buffer, &write_cursor);
  nstrct_pack_arguments(instruction.arguments, &instruction.num_arguments, buffer, &write_cursor);
  
  uint32_t _write_cursor = htonl(write_cursor);
  char length[4];
  memcpy(&length, &_write_cursor, 4);
  
  fwrite(length, sizeof(uint32_t), 1, stdout);
  fwrite(buffer, sizeof(char), write_cursor, stdout);
}

void assert(const char * info, bool exp) {
  if(!exp) {
    printf("%s",info);
    exit(1);
  }
}

void process() {
  char length_buffer[4];
  fread(length_buffer, sizeof(uint32_t), 1, stdin);
  uint32_t length;
  memcpy(&length, &length_buffer, 4);
  length = ntohl(length);
  
  char data_buffer[length];
  fread(data_buffer, sizeof(char), length, stdin);
  uint16_t read_cursor = 0;

  nstrct_instruction_t instruction;
  nstrct_unpack_instruction(&instruction, data_buffer, &read_cursor);
  nstrct_argument_t arguments[instruction.num_arguments];
  nstrct_argument_value_t values[instruction.num_array_elements];
  instruction.arguments = arguments;
  nstrct_unpack_arguments(instruction.arguments, &instruction.num_arguments, values, data_buffer, &read_cursor);
  
  assert("    boolean value error\n", instruction.arguments[0].value.boolean == 0);
  assert("    int8 value error\n", instruction.arguments[1].value.int8 == INT8_MIN);
  assert("    int16 value error\n", instruction.arguments[2].value.int16 == INT16_MIN);
  assert("    int32 value error\n", instruction.arguments[3].value.int32 == INT32_MIN);
  assert("    int64 value error\n", instruction.arguments[4].value.int64 == INT64_MIN);
  assert("    uint8 value error\n", instruction.arguments[5].value.uint8 == UINT8_MAX);
  assert("    uint16 value error\n", instruction.arguments[6].value.uint16 == UINT16_MAX);
  assert("    uint32 value error\n", instruction.arguments[7].value.uint32 == UINT32_MAX);
  assert("    uint64 value error\n", instruction.arguments[8].value.uint64 == UINT64_MAX);
  assert("    float32 value error\n", instruction.arguments[9].value.float32 == float32_max);
  assert("    float64 value error\n", instruction.arguments[10].value.float64 == float64_max);
  nstrct_string_t string1 = nstrct_to_string("hello world");
  assert("    string value error\n", memcmp(instruction.arguments[11].value.string.ptr, string1.ptr, 11) == 0);
  assert("    string value error\n", instruction.arguments[12].value.string.size == 0);
  assert("    array uint16 value 1 error\n", instruction.arguments[13].array_values[0].uint16 == 2443);
  assert("    array uint16 value 2 error\n", instruction.arguments[13].array_values[1].uint16 == 3443);
  
  printf("    tests passed\n");
  exit(0);
}

void info() {
  printf("Usage: cross_platform_test [options]\n");
  printf("  -p : process bytes at STDIN and return result\n");
  printf("  -g : generate bytes and write to STDOUT\n");
}

int main(int argc, char** argv) {
  float32_max = 3.4028234663852886*pow(10,38);
  float64_max = 1.7976931348623157*pow(10,308);

  run_mode_t mode = RM_INFO;
  int i;
  for (i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-g") == 0) {
      mode = RM_GENERATE;
    } else if (strcmp(argv[i], "-p") == 0) {
      mode = RM_PROCESS;
    }
  }
  switch(mode) {
    case RM_GENERATE: generate(); break;
    case RM_PROCESS: process(); break;
    case RM_INFO: info(); break;
  }
}