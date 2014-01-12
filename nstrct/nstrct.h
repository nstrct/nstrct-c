#ifndef NSTRCT_H__
#define NSTRCT_H__

#include <stdint.h>

#ifndef NSTRCT_DISABLE_ARPA_INET
#include <arpa/inet.h>
#endif

#ifdef NSTRCT_FORCE_BYTE_SWAPPING

#define htons(x) ( ((x)<<8) | (((x)>>8)&0xFF) )
#define ntohs(x) htons(x)

#define htonl(x) ( ((x)<<24 & 0xFF000000UL) | \
((x)<< 8 & 0x00FF0000UL) | \
((x)>> 8 & 0x0000FF00UL) | \
((x)>>24 & 0x000000FFUL) )
#define ntohl(x) htonl(x)

#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Buffer Manipulation */

#define nstrct_read_buffer(BUFFER, TARGET, BYTES, CURSOR); \
  memcpy(TARGET, BUFFER+*CURSOR, BYTES); \
  *CURSOR += BYTES;

#define nstrct_write_buffer(BUFFER, SOURCE, BYTES, CURSOR); \
  memcpy(BUFFER+*CURSOR, SOURCE, BYTES); \
  *CURSOR += BYTES;

typedef uint16_t nstrct_cursor_t;
typedef const char * nstrct_read_buffer_t;
typedef char * nstrct_write_buffer_t;

/* Definitions */

/**
 * Boolean Values
 */
#define NSTRCT_TRUE 1
#define NSTRCT_FALSE 0

/**
 * The available datatypes.
 */
typedef enum {
  NSTRCT_DATATYPE_BOOLEAN = 1,
  NSTRCT_DATATYPE_INT8 = 10,
  NSTRCT_DATATYPE_INT16 = 11,
  NSTRCT_DATATYPE_INT32 = 12,
  NSTRCT_DATATYPE_INT64 = 13,
  NSTRCT_DATATYPE_UINT8 = 14,
  NSTRCT_DATATYPE_UINT16 = 15,
  NSTRCT_DATATYPE_UINT32 = 16,
  NSTRCT_DATATYPE_UINT64 = 17,
  NSTRCT_DATATYPE_FLOAT32 = 21,
  NSTRCT_DATATYPE_FLOAT64 = 22,
  NSTRCT_DATATYPE_STRING = 31,
  NSTRCT_DATATYPE_ARRAY = 32
} nstrct_datatype_t;

/**
 * The string datatype.
 */
typedef struct {
  uint8_t size;
  const char * ptr;
} nstrct_string_t;

/**
 * The value of an argument;
 */
typedef union {
  int8_t boolean;
  int8_t int8;
  int16_t int16;
  int32_t int32;
  int64_t int64;
  uint8_t uint8;
  uint16_t uint16;
  uint32_t uint32;
  uint64_t uint64;
  float float32;
  double float64;
  nstrct_string_t string;
} nstrct_argument_value_t;

/**
 * An argument holding different datatypes.
 */
typedef struct {
  nstrct_datatype_t type;
  nstrct_argument_value_t value;
  nstrct_datatype_t array_element_type;
  uint8_t array_elements;
  nstrct_argument_value_t * array_values;
} nstrct_argument_t;

/**
 * The instruction object.
 */
typedef struct {
  uint16_t code;
  uint8_t num_arguments;
  uint16_t num_array_elements;
  nstrct_argument_t * arguments;
} nstrct_instruction_t;

/**
 * Return Values
 */
typedef enum {
  NSTRCT_ERROR_SUCCESS = 0,
  NSTRCT_ERROR_NO_FRAME_AVAILABLE,
  NSTRCT_ERROR_FRAME_START_INVALID,
  NSTRCT_ERROR_FRAME_END_INVALID,
  NSTRCT_ERROR_CHECKSUM_INVALID,
  NSTRCT_ERROR_DATATYPE_INVALID
} nstrct_error_t;

/* API Helpers */

uint64_t htonq(uint64_t v);
uint64_t ntohq(uint64_t v);

float htonf(float v);
float ntohf(float v);

double htond(double v);
double ntohd(double v);

/**
 * Generate Checksum (crc32)
 *
 * @param buffer
 * @param length
 */
uint32_t nstrct_checksum(nstrct_read_buffer_t buffer, uint16_t length);

/**
 * Convert a string to a nstrct string
 *
 * @param str the string to convert
 * @return a nstrct string
 */
nstrct_string_t nstrct_to_string(const char * str);
  
/* API Length Calculation */

/**
 * Computer length of a frame
 *
 * @param instruction
 */
uint16_t nstrct_frame_length(nstrct_instruction_t* instruction);
  
/**
 * Comnpute the length of an instruction.
 *
 * @param instruction
 */
uint16_t nstrct_instruction_length(nstrct_instruction_t * instruction);

/**
 * Compute the length of a datytype.
 *
 * @param datatype
 * @param value
 */
uint8_t nstrct_datatype_length(nstrct_datatype_t * datatype, nstrct_argument_value_t * value);
  
/**
 * Compute the lengths of an argument array.
 *
 * @param arguments
 * @param num arguments
 */
uint16_t nstrct_arguments_length(nstrct_argument_t * arguments, uint8_t * num_arguments);
  
/**
 * Count the total array elements of an instruction.
 *
 * @param instruction
 */
uint16_t nstrct_count_array_elements(nstrct_instruction_t * instruction);
  
/* Packing & Unpacking */

/**
 *
 *
 * @param instruction
 * @param buffer
 * @param cursor
*/
void nstrct_pack_frame(nstrct_instruction_t * instruction, nstrct_write_buffer_t buffer, nstrct_cursor_t * cursor);

/**
 * Check for buffer an available frame
 *
 * @param buffer
 * @param length
 * @return error
 */
nstrct_error_t nstrct_frame_available(nstrct_read_buffer_t buffer, uint16_t length);

/**
 * Unpack instruction from a frame
 *
 * @param instruction
 * @param buffer
 * @param cursor
*/
nstrct_error_t nstrct_unpack_frame(nstrct_instruction_t * instruction, nstrct_read_buffer_t buffer, nstrct_cursor_t * cursor);
  
/**
 * Pack an instruction into a buffer
 *
 * @param instruction the instruction
 * @param buffer the buffer
 * @param cursor the cursor to the buffer
 */
void nstrct_pack_instruction(nstrct_instruction_t * instruction, nstrct_write_buffer_t buffer, nstrct_cursor_t * cursor);
  
/**
 * Unpack an instruction from a buffer
 *
 * @param instruction the instruction to fill
 * @param buffer the buffer to take from
 * @param cursor the read cursor to the buffer
 */
void nstrct_unpack_instruction(nstrct_instruction_t * instruction, nstrct_read_buffer_t buffer, nstrct_cursor_t * cursor);

/**
 * Pack a value in a buffer
 *
 * @param datatype
 * @param value
 * @param cursor
 */
void nstrct_pack_value(nstrct_datatype_t * datatype, nstrct_argument_value_t * value, nstrct_write_buffer_t buffer, nstrct_cursor_t * cursor);

/**
 * Unpack a value from a buffer.
 *
 * @param datatype
 * @param value
 * @param cursor
 * @return error
 */
nstrct_error_t nstrct_unpack_value(nstrct_datatype_t * datatype, nstrct_argument_value_t * value, nstrct_read_buffer_t buffer, nstrct_cursor_t * cursor);

/**
 * Pack arguments in a buffer
 *
 * @param arguments
 * @param num_arguments
 * @param buffer
 * @param cursor
 */
void nstrct_pack_arguments(nstrct_argument_t * arguments, uint8_t * num_arguments, nstrct_write_buffer_t buffer, nstrct_cursor_t * cursor);

/**
 * Unpack arguments from a buffer
 *
 * @param arguments
 * @param num arguments
 * @param buffer
 * @param cursor
 * @return error
 */
nstrct_error_t nstrct_unpack_arguments(nstrct_argument_t * arguments, uint8_t * num_arguments, nstrct_argument_value_t * values, nstrct_read_buffer_t buffer, nstrct_cursor_t * cursor);


#ifdef __cplusplus
}
#endif

#endif
