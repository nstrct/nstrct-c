/*
 *  NSTRCT BINARY PROTOCOL
 */

#include <stdio.h>
#include <string.h>

#include "nstrct.h"

/* API Helpers */

uint64_t htonq(uint64_t v) {
  uint16_t test = 66;
  if(test != htons(test)) {
    uint64_t h = 0;
    uint8_t *p = (uint8_t *)&v;
    h |= (uint64_t)*p++ << 56;
    h |= (uint64_t)*p++ << 48;
    h |= (uint64_t)*p++ << 40;
    h |= (uint64_t)*p++ << 32;
    h |= (uint64_t)*p++ << 24;
    h |= (uint64_t)*p++ << 16;
    h |= (uint64_t)*p++ << 8;
    h |= (uint64_t)*p   << 0;
    return h;
  } else {
    return v;
  }
}

uint64_t ntohq(uint64_t v) {
  uint16_t test = 66;
  if(test != ntohs(test)) {
    uint64_t h;
    uint8_t *p = (uint8_t *)&h;
    *p++ = (uint8_t) (v>>56 & 0xff);
    *p++ = (uint8_t) (v>>48 & 0xff);
    *p++ = (uint8_t) (v>>40 & 0xff);
    *p++ = (uint8_t) (v>>32 & 0xff);
    *p++ = (uint8_t) (v>>24 & 0xff);
    *p++ = (uint8_t) (v>>16 & 0xff);
    *p++ = (uint8_t) (v>>8  & 0xff);
    *p   = (uint8_t) (v>>0  & 0xff);
    return h;
  } else {
    return v;
  }
}

float htonf(float v) {
  int16_t test = 66;
  if(test != htons(test)) {
    union {
      float f;
      uint32_t i;
    } swap;
    swap.f = v;
    swap.i = htonl(swap.i);
    return swap.f;
  } else {
    return v;
  }
}

float ntohf(float v) {
  int16_t test = 66;
  if(test != ntohs(test)) {
    union {
      float f;
      uint32_t i;
    } swap;
    swap.f = v;
    swap.i = ntohl(swap.i);
    return swap.f;
  } else {
    return v;
  }
}

double htond(double v) {
  int16_t test = 66;
  if(test != htons(test)) {
    union {
      double d;
      uint64_t i;
    } swap;
    swap.d = v;
    swap.i = htonq(swap.i);
    return swap.d;
  } else {
    return v;
  }
}

double ntohd(double v) {
  int16_t test = 66;
  if(test != ntohs(test)) {
    union {
      double d;
      uint64_t i;
    } swap;
    swap.d = v;
    swap.i = ntohq(swap.i);
    return swap.d;
  } else {
    return v;
  }
}

nstrct_string_t nstrct_to_string(const char * str) {
  nstrct_string_t string;
  string.size = strlen(str);
  string.ptr = str;
  return string;
}

/* API Length Calculation */

uint16_t nstrct_instruction_length(nstrct_instruction_t * instruction) {
    return 5+nstrct_arguments_length(instruction->arguments, &instruction->num_arguments);
}

uint8_t nstrct_datatype_length(nstrct_datatype_t * datatype, nstrct_argument_value_t * value) {
  switch(*datatype) {
    case NSTRCT_DATATYPE_BOOLEAN: return 1;
    case NSTRCT_DATATYPE_INT8: return 1;
    case NSTRCT_DATATYPE_INT16: return 2;
    case NSTRCT_DATATYPE_INT32: return 4;
    case NSTRCT_DATATYPE_INT64: return 8;
    case NSTRCT_DATATYPE_UINT8: return 1;
    case NSTRCT_DATATYPE_UINT16: return 2;
    case NSTRCT_DATATYPE_UINT32: return 4;
    case NSTRCT_DATATYPE_UINT64: return 8;
    case NSTRCT_DATATYPE_FLOAT32: return 4;
    case NSTRCT_DATATYPE_FLOAT64: return 8;
    case NSTRCT_DATATYPE_STRING: return 1 + value->string.size;
    case NSTRCT_DATATYPE_ARRAY: return 0;
  }
  return 0;
}

uint16_t nstrct_arguments_length(nstrct_argument_t * arguments, uint8_t * num_arguments) {
  uint16_t counter = 0;
  uint16_t i;
  for(i=0; i<*num_arguments; i++) {
    counter += 1;
    if(arguments[i].type == NSTRCT_DATATYPE_ARRAY) {
      counter += 2;
      uint16_t ii;
      for(ii=0; ii<arguments[i].array_elements; ii++) {
        counter += nstrct_datatype_length(&arguments[i].array_element_type, &arguments[i].array_values[ii]);
      }
    } else {
      counter += nstrct_datatype_length(&arguments[i].type, &arguments[i].value);
    }
  }
  return counter;
}

uint16_t nstrct_count_array_elements(nstrct_instruction_t * instruction) {
  uint16_t counter = 0;
  uint16_t i;
  for(i=0; i<instruction->num_arguments; i++) {
    if(instruction->arguments[i].type == NSTRCT_DATATYPE_ARRAY) {
      counter += instruction->arguments[i].array_elements;
    }
  }
  return counter;
}

/* Packing & Unpacking */

void nstrct_pack_instruction(nstrct_instruction_t * instruction, nstrct_write_buffer_t buffer, nstrct_cursor_t * cursor) {
  uint16_t code = htons(instruction->code);
  nstrct_write_buffer(buffer, &code, 2, cursor);
  nstrct_write_buffer(buffer, &instruction->num_arguments, 1, cursor);
  instruction->num_array_elements = nstrct_count_array_elements(instruction);
  uint16_t num_array_elements = htons(instruction->num_array_elements);
  nstrct_write_buffer(buffer, &num_array_elements, 2, cursor);
}

void nstrct_unpack_instruction(nstrct_instruction_t * instruction, nstrct_read_buffer_t buffer, nstrct_cursor_t * cursor) {
  uint16_t code;
  nstrct_read_buffer(buffer, &code, 2, cursor);
  instruction->code = ntohs(code);
  nstrct_read_buffer(buffer, &instruction->num_arguments, 1, cursor);
  uint16_t num_array_elements;
  nstrct_read_buffer(buffer, &num_array_elements, 2, cursor);
  instruction->num_array_elements = ntohs(num_array_elements);
}

void nstrct_pack_value(nstrct_datatype_t * datatype, nstrct_argument_value_t * value, nstrct_write_buffer_t buffer, nstrct_cursor_t * cursor) {
  switch(*datatype) {
    case NSTRCT_DATATYPE_BOOLEAN: nstrct_write_buffer(buffer, &value->boolean, 1, cursor); break;
    case NSTRCT_DATATYPE_INT8: nstrct_write_buffer(buffer, &value->int8, 1, cursor); break;
    case NSTRCT_DATATYPE_INT16: {
      int16_t val = htons(value->int16);
      nstrct_write_buffer(buffer, &val, 2, cursor);
      break;
    }
    case NSTRCT_DATATYPE_INT32: {
      int32_t val = htonl(value->int32);
      nstrct_write_buffer(buffer, &val, 4, cursor);
      break;
    }
    case NSTRCT_DATATYPE_INT64: {
      int64_t val = htonq(value->int64);
      nstrct_write_buffer(buffer, &val, 8, cursor);
      break;
    }
    case NSTRCT_DATATYPE_UINT8: nstrct_write_buffer(buffer, &value->uint8, 1, cursor); break;
    case NSTRCT_DATATYPE_UINT16: {
      uint16_t val = htons(value->uint16);
      nstrct_write_buffer(buffer, &val, 2, cursor);
      break;
    }
    case NSTRCT_DATATYPE_UINT32: {
      uint32_t val = htonl(value->uint32);
      nstrct_write_buffer(buffer, &val, 4, cursor);
      break;
    }
    case NSTRCT_DATATYPE_UINT64: {
      uint64_t val = htonq(value->uint64);
      nstrct_write_buffer(buffer, &val, 8, cursor);
      break;
    }
    case NSTRCT_DATATYPE_FLOAT32: {
      float val = htonf(value->float32);
      nstrct_write_buffer(buffer, &val, 4, cursor);
      break;
    }
    case NSTRCT_DATATYPE_FLOAT64: {
      double val = htond(value->float64);
      nstrct_write_buffer(buffer, &val, 8, cursor);
      break;
    }
    case NSTRCT_DATATYPE_STRING: {
      nstrct_write_buffer(buffer, &value->string.size, 1, cursor);
      nstrct_write_buffer(buffer, value->string.ptr, value->string.size, cursor);
      break;
    }
    case NSTRCT_DATATYPE_ARRAY: break;
  }
}

void nstrct_unpack_value(nstrct_datatype_t * datatype, nstrct_argument_value_t * value, nstrct_read_buffer_t buffer, nstrct_cursor_t * cursor) {
  switch(*datatype) {
    case NSTRCT_DATATYPE_BOOLEAN: nstrct_read_buffer(buffer, &value->boolean, 1, cursor); break;
    case NSTRCT_DATATYPE_INT8: nstrct_read_buffer(buffer, &value->int8, 1, cursor); break;
    case NSTRCT_DATATYPE_INT16: {
      int16_t val;
      nstrct_read_buffer(buffer, &val, 2, cursor);
      value->int16 = ntohs(val);
      break;
    }
    case NSTRCT_DATATYPE_INT32: {
      int32_t val;
      nstrct_read_buffer(buffer, &val, 4, cursor);
      value->int32 = ntohl(val);
      break;
    }
    case NSTRCT_DATATYPE_INT64: {
      int64_t val;
      nstrct_read_buffer(buffer, &val, 8, cursor);
      value->int64 = ntohq(val);
      break;
    }
    case NSTRCT_DATATYPE_UINT8: nstrct_read_buffer(buffer, &value->uint8, 1, cursor); break;
    case NSTRCT_DATATYPE_UINT16: {
      uint16_t val;
      nstrct_read_buffer(buffer, &val, 2, cursor);
      value->uint16 = ntohs(val);
      break;
    }
    case NSTRCT_DATATYPE_UINT32: {
      uint32_t val;
      nstrct_read_buffer(buffer, &val, 4, cursor);
      value->uint32 = ntohl(val);
      break;
    }
    case NSTRCT_DATATYPE_UINT64: {
      uint64_t val;
      nstrct_read_buffer(buffer, &val, 8, cursor);
      value->uint64 = ntohq(val);
      break;
    }
    case NSTRCT_DATATYPE_FLOAT32: {
      float val;
      nstrct_read_buffer(buffer, &val, 4, cursor);
      value->float32 = ntohf(val);
      break;
    }
    case NSTRCT_DATATYPE_FLOAT64: {
      double val;
      nstrct_read_buffer(buffer, &val, 8, cursor);
      value->float64 = ntohd(val);
      break;
    }
    case NSTRCT_DATATYPE_STRING: {
      nstrct_read_buffer(buffer, &value->string.size, 1, cursor);
      value->string.ptr = buffer+*cursor;
      *cursor += value->string.size;
      break;
    }
    case NSTRCT_DATATYPE_ARRAY: break;
  }
}

void nstrct_pack_arguments(nstrct_argument_t * arguments, uint8_t * num_arguments, nstrct_write_buffer_t buffer, nstrct_cursor_t * cursor) {
  uint16_t i;
  for(i=0; i<*num_arguments; i++) {
    uint8_t id = arguments[i].type;
    nstrct_write_buffer(buffer, &id, 1, cursor);
    if(arguments[i].type == NSTRCT_DATATYPE_ARRAY) {
      uint8_t id2 = arguments[i].array_element_type;
      nstrct_write_buffer(buffer, &id2, 1, cursor);
      nstrct_write_buffer(buffer, &arguments[i].array_elements, 1, cursor);
      int ii;
      for(ii=0; ii<arguments[i].array_elements; ii++) {
        nstrct_pack_value(&arguments[i].array_element_type, &arguments[i].array_values[ii], buffer, cursor);
      }
    } else {
      nstrct_pack_value(&arguments[i].type, &arguments[i].value, buffer, cursor);
    }
  }
}

void nstrct_unpack_arguments(nstrct_argument_t * arguments, uint8_t * num_arguments, nstrct_argument_value_t * values, nstrct_read_buffer_t buffer, nstrct_cursor_t * cursor) {
  uint16_t i;
  uint16_t value_cursor = 0;
  for(i=0; i<*num_arguments; i++) {
    uint8_t id;
    nstrct_read_buffer(buffer, &id, 1, cursor);
    arguments[i].type = id;
    if(arguments[i].type == NSTRCT_DATATYPE_ARRAY) {
      uint8_t id2;
      nstrct_read_buffer(buffer, &id2, 1, cursor);
      arguments[i].array_element_type = id2;
      nstrct_read_buffer(buffer, &arguments[i].array_elements, 1, cursor);
      arguments[i].array_values = &values[value_cursor];
      int ii;
      for(ii=0; ii<arguments[i].array_elements; ii++) {
        nstrct_unpack_value(&arguments[i].array_element_type, &values[value_cursor], buffer, cursor);
        value_cursor++;
      }
    } else {
      nstrct_unpack_value(&arguments[i].type, &arguments[i].value, buffer, cursor);
    }
  }
}
