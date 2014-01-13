#include <stdio.h>
#include <string.h>

#include "nstrct.h"

/* Globals */

uint8_t __nstrct_frame_start = 0x55;
uint8_t __nstrct_frame_end = 0xAA;

static uint32_t __nstrct_crc32_tab[] = {
	0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
	0xe963a535, 0x9e6495a3,	0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
	0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
	0xf3b97148, 0x84be41de,	0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
	0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec,	0x14015c4f, 0x63066cd9,
	0xfa0f3d63, 0x8d080df5,	0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
	0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,	0x35b5a8fa, 0x42b2986c,
	0xdbbbc9d6, 0xacbcf940,	0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
	0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
	0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
	0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,	0x76dc4190, 0x01db7106,
	0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
	0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
	0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
	0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
	0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
	0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
	0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
	0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
	0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
	0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
	0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
	0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
	0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
	0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
	0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
	0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
	0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
	0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
	0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
	0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
	0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
	0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
	0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
	0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
	0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
	0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
	0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
	0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
	0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
	0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
	0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
	0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};

uint32_t nstrct_crc32(uint32_t crc, const void *buf, size_t size) {
	const uint8_t *p;
	p = buf;
	crc = crc ^ ~0U;
	while (size--) {
		crc = __nstrct_crc32_tab[(crc ^ *p++) & 0xFF] ^ (crc >> 8);
  }
	return crc ^ ~0U;
}

uint32_t nstrct_checksum(nstrct_read_buffer_t buffer, uint16_t length) {
  uint32_t checksum = 0;
  return nstrct_crc32(checksum, buffer, length);
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
