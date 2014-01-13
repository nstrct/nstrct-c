#include <stdio.h>
#include <string.h>

#include "nstrct.h"

/* Globals */

static const uint8_t __nstrct_frame_start = 0x55;
static const uint8_t __nstrct_frame_end = 0xAA;

static const uint32_t __nstrct_crc_table[16] = {
  0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac,
  0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
  0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
  0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c
};

uint32_t nstrct_checksum_update(uint32_t crc, uint8_t data) {
  uint8_t tbl_idx;
  tbl_idx = crc ^ (data >> (0 * 4));
  crc = __nstrct_crc_table[tbl_idx & 0x0f] ^ (crc >> 4);
  tbl_idx = crc ^ (data >> (1 * 4));
  crc = __nstrct_crc_table[tbl_idx & 0x0f] ^ (crc >> 4);
  return crc;
}

uint32_t nstrct_checksum(nstrct_read_buffer_t buffer, uint16_t length) {
  uint32_t crc = ~0L;
  while (length) {
    crc = nstrct_checksum_update(crc, *buffer++);
    length--;
  }
  crc = ~crc;
  return crc;
}

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

uint16_t nstrct_frame_length(nstrct_instruction_t* instruction) {
  return nstrct_instruction_length(instruction)+8;
}

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

nstrct_error_t nstrct_full_pack(nstrct_instruction_t * instruction, nstrct_write_buffer_t buffer, uint16_t length, nstrct_cursor_t * cursor) {
  uint16_t frame_length = nstrct_frame_length(instruction);
  
  if(frame_length > length) return NSTRCT_ERROR_BUFFER_OVERFLOW;

  nstrct_pack_frame_head(instruction, buffer, cursor);
  nstrct_pack_instruction_head(instruction, buffer, cursor);
  nstrct_pack_arguments(instruction->arguments, &instruction->num_arguments, buffer, cursor);
  nstrct_pack_frame_tail(instruction, buffer, cursor);
  
  return NSTRCT_ERROR_SUCCESS;
}

nstrct_error_t nstrct_preallocated_unpack(nstrct_preallocation_t * preallocation, nstrct_read_buffer_t buffer, uint16_t length, nstrct_cursor_t * cursor) {
  nstrct_error_t ret;
  
  ret = nstrct_frame_available(buffer, length);
  if(ret) return ret;

  ret = nstrct_frame_validate(buffer, length);
  if(ret) return ret;
  
  *cursor += 3; // frame head
  
  nstrct_unpack_instruction_head(&preallocation->instruction, buffer, cursor);
  
  if(preallocation->instruction.num_arguments > preallocation->max_arguments) return NSTRCT_ERROR_PREALLOCATION_OVERFLOW;
  if(preallocation->instruction.num_array_elements > preallocation->max_array_values) return NSTRCT_ERROR_PREALLOCATION_OVERFLOW;
  
  preallocation->instruction.arguments = preallocation->arguments;
  ret = nstrct_unpack_arguments(preallocation->arguments, &preallocation->instruction.num_arguments, preallocation->array_values, buffer, cursor);
  
  *cursor += 5; // frame tail
  
  if(ret) return ret;
  
  return NSTRCT_ERROR_SUCCESS;
}

void nstrct_pack_frame_head(nstrct_instruction_t * instruction, nstrct_write_buffer_t buffer, nstrct_cursor_t * cursor) {
  uint16_t instruction_length = nstrct_instruction_length(instruction);
  
  nstrct_write_buffer(buffer, &__nstrct_frame_start, 1, cursor);
  uint16_t length = htons(instruction_length);
  nstrct_write_buffer(buffer, &length, 2, cursor);
}

void nstrct_pack_frame_tail(nstrct_instruction_t * instruction, nstrct_write_buffer_t buffer, nstrct_cursor_t * cursor) {
  uint16_t instruction_length = nstrct_instruction_length(instruction);

  uint32_t checksum = htonl(nstrct_checksum(buffer+3, instruction_length));
  nstrct_write_buffer(buffer, &checksum, 4, cursor);
  nstrct_write_buffer(buffer, &__nstrct_frame_end, 1, cursor);
}

nstrct_error_t nstrct_frame_available(nstrct_read_buffer_t buffer, uint16_t length) {
  nstrct_cursor_t cursor = 0;
  
  if(length >= 1) {
    uint8_t frame_start;
    nstrct_read_buffer(buffer, &frame_start, 1, &cursor);
    
    if(frame_start != __nstrct_frame_start) {
      return NSTRCT_ERROR_FRAME_START_INVALID;
    }
    
    if(length >= 3) {
      uint16_t payload_length;
      nstrct_read_buffer(buffer, &payload_length, 2, &cursor);
      payload_length = ntohs(payload_length);
      
      if(length >= payload_length+8) {
        cursor += payload_length;
        cursor += 4; // checksum
        
        uint8_t frame_end;
        nstrct_read_buffer(buffer, &frame_end, 1, &cursor);
        
        if(frame_end != __nstrct_frame_end) {
          return NSTRCT_ERROR_FRAME_END_INVALID;
        }
        
        return NSTRCT_ERROR_SUCCESS;
      }
    }
  }
  
  return NSTRCT_ERROR_NO_FRAME_AVAILABLE;
}

nstrct_error_t nstrct_frame_validate(nstrct_read_buffer_t buffer, uint16_t length) {
  nstrct_error_t ret = NSTRCT_ERROR_SUCCESS;
  nstrct_cursor_t cursor = 1;
  
  uint16_t payload_length;
  nstrct_read_buffer(buffer, &payload_length, 2, &cursor);
  payload_length = ntohs(payload_length);
  
  cursor += payload_length;
  
  uint32_t checksum;
  nstrct_read_buffer(buffer, &checksum, 4, &cursor);
  checksum = ntohl(checksum);
  
  uint32_t payload_checksum = nstrct_checksum(buffer+3, payload_length);
  if(checksum != payload_checksum) {
    return NSTRCT_ERROR_CHECKSUM_INVALID;
  }
  
  return ret;
}

void nstrct_pack_instruction_head(nstrct_instruction_t * instruction, nstrct_write_buffer_t buffer, nstrct_cursor_t * cursor) {
  uint16_t code = htons(instruction->code);
  nstrct_write_buffer(buffer, &code, 2, cursor);
  nstrct_write_buffer(buffer, &instruction->num_arguments, 1, cursor);
  instruction->num_array_elements = nstrct_count_array_elements(instruction);
  uint16_t num_array_elements = htons(instruction->num_array_elements);
  nstrct_write_buffer(buffer, &num_array_elements, 2, cursor);
}

void nstrct_unpack_instruction_head(nstrct_instruction_t * instruction, nstrct_read_buffer_t buffer, nstrct_cursor_t * cursor) {
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

nstrct_error_t nstrct_unpack_value(nstrct_datatype_t * datatype, nstrct_argument_value_t * value, nstrct_read_buffer_t buffer, nstrct_cursor_t * cursor) {
  nstrct_error_t error = NSTRCT_ERROR_SUCCESS;
  
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
    default: return NSTRCT_ERROR_DATATYPE_INVALID;
  }
  
  return error;
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

nstrct_error_t nstrct_unpack_arguments(nstrct_argument_t * arguments, uint8_t * num_arguments, nstrct_argument_value_t * values, nstrct_read_buffer_t buffer, nstrct_cursor_t * cursor) {
  nstrct_error_t error = NSTRCT_ERROR_SUCCESS;

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
        error = nstrct_unpack_value(&arguments[i].array_element_type, &values[value_cursor], buffer, cursor);
        if(error) {
          break;
        }
        value_cursor++;
      }
    } else {
      error = nstrct_unpack_value(&arguments[i].type, &arguments[i].value, buffer, cursor);
    }
    if(error) {
      break;
    }
  }
  
  return error;
}
