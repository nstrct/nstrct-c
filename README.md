# nstrct-c

**a multi-purpose binary protocol for instruction interchange**

Interchange formats like json or xml are great to keep data visible, but due to their parse and pack complexity they aren't used in embedded applications. There are alternatives like msgpack or Google's protocol buffer, which allow a more binary representation of data, but these protcols are still heavy and developers tend to rather implement their own 'simple' binary protocols instead of porting or using the big ones. 

The protcol **nstrct** is designed to be an alternative in those situations where a tiny and versatile protocol is needed. The main transportable unit in nstrct are **instructions** which carry a dynamic amount of data in the form of **arguments**. Each instruction is identified by a code between 0-65535 which can be used by two communicating applications to identify the blueprint of the message. Arguments support all standard types of integers(boolean, int8-64, uint8-64, float32/64), strings, and arrays of integers or strings. There is no support for hashes by design to keep the pack and unpacking functions as small as possible. [Check the main repository for the binary layout of the instructions](http://github.com/nstrct/nstrct).

**Start using nstrct by getting the library for your favorite programming language:**

* [C/C++ (nstrct-c)](http://github.com/nstrct/nstrct-c)
* [Ruby (nstrct-ruby)](http://github.com/nstrct/nstrct-ruby)
* [Obj-C (nstrct-objc)](http://github.com/nstrct/nstrct-objc)
* [Java (nstrct-c)](http://github.com/nstrct/nstrct-java)

_This software has been open sourced by [ElectricFeel Mobility Systems Gmbh](http://electricfeel.com) and is further maintained by [Joël Gähwiler](http://github.com/256dpi)._

## Instruction Composing

```c
/* Preparing */

// allocate memory for 4 arguments
nstrct_argument_t arguments[4];

// set the values of the first 3 arguments
arguments[0].type = NSTRCT_DATATYPE_BOOLEAN;
arguments[0].value.boolean = NSTRCT_FALSE;
arguments[1].type = NSTRCT_DATATYPE_INT8;
arguments[1].value.int8 = 11;
arguments[2].type = NSTRCT_DATATYPE_UINT16;
arguments[2].value.uint16 = -9242;

// allocate memory for 3 values
nstrct_argument_value_t values[3];

// set the 3 values
values[0].boolean = NSTRCT_TRUE;
values[1].boolean = NSTRCT_FALSE;
values[2].boolean = NSTRCT_TRUE;

// add the array to the last argument
arguments[3].type = NSTRCT_DATATYPE_ARRAY;
arguments[3].array_element_type = NSTRCT_DATATYPE_BOOLEAN;
arguments[3].array_elements = 3;
arguments[3].array_values = values;

// allocate memory for the instruction
nstrct_instruction_t instruction;

// set code and arguments
instruction.code = 245;
instruction.num_arguments = 13;
instruction.arguments = arguments;

/* Packing */

// allocate a buffer of the instructions resulting size
char buffer[255];

// allocate a cursor used by the pack function
uint16_t write_cursor = 0;

// pack the frame
nstrct_full_pack(&instruction, buffer, 255, &write_cursor);

// send the buffer away
send_buffer(buffer, write_cursor);
```

## Instruction Processing

```c
// the received buffer of a complete frame
const char * buffer;
uint16_t length;

// the read cursor used by the unpack function
uint16_t read_cursor = 0;

// preallocate memory for unpacking
nstrct_preallocation_t preallocation;
nstrct_argument_t arguments[20];
nstrct_argument_value_t values[10];
preallocation.arguments = arguments;
preallocation.max_arguments = 20;
preallocation.array_values = values;
preallocation.max_array_values = 10;

// unpack the frame
nstrct_preallocated_unpack(&preallocation, buffer, length, &read_cursor);

// process the instruction
process_instruction(&instruction);

/* The buffer used to parse the instructions has to be alive until the instruction has been finished processing.
   This is necessary because parsed strings are passed as reference to spare memory) */
```

## Additions

You can unload the `arpa/inet.h` defined functions for byte swapping by defining the constant `NSTRCT_DISABLE_ARPA_INET`. If you know that your on a little endian system enforce byte swapping by defining `NSTRCT_FORCE_BYTE_SWAPPING`.

