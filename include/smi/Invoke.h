/*
 * Invoke.h
 *
 *  Created on: Jun 1, 2019
 *      Author: chorm
 */

#ifndef INCLUDE_SMI_INVOKE_H__2019_06_01_11_30_58
#define INCLUDE_SMI_INVOKE_H__2019_06_01_11_30_58

#ifdef __cplusplus
extern"C"{
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct{
	uint64_t hi;
	uint64_t lo;
} uuid;

typedef struct{
	uint8_t maj;
	uint8_t min;
} version;

typedef struct{
	uint64_t seconds;
	uint32_t nanos;
} instant;

typedef struct{
	uint64_t seconds;
	uint32_t nanos;
} duration;

typedef struct SBuff SMIParameterList;

enum Type{
	Byte = 'B', SByte = 'K',
	Short = 'S', UShort = 'A',
	Int = 'I', UInt = 'U',
	Long = 'J', UUID = 'P',
	BOOL = 'Z', VERSION = 'V',
	INSTANT = 'Q', DURATION = 'T',
	FLOAT = 'F', DOUBLE = 'D',
	STRING = 'R'
};

SMIParameterList* smibuff_new();

void smibuff_free(SMIParameterList* buff);

void smibuff_write_bool(SMIParameterList* buff,bool b);
bool smibuff_read_bool(SMIParameterList* buff);

void smibuff_write_byte(SMIParameterList* buff,uint8_t b);
uint8_t smibuff_read_byte(SMIParameterList* buff);

void smibuff_write_sbyte(SMIParameterList* buff,int8_t b);
int8_t smibuff_read_sbyte(SMIParameterList* buff);

void smibuff_write_short(SMIParameterList* buff,int16_t s);
int16_t smibuff_read_short(SMIParameterList* buff);

void smibuff_write_ushort(SMIParameterList* buff,uint16_t s);
uint16_t smibuff_read_ushort(SMIParameterList* buff);

void smibuff_write_int(SMIParameterList* buff,int32_t i);
int32_t smibuff_read_int(SMIParameterList* buff);

void smibuff_write_uint(SMIParameterList* buff,uint32_t i);
uint32_t smibuff_read_uint(SMIParameterList* buff);

void smibuff_write_long(SMIParameterList* buff,int64_t l);
int64_t smibuff_read_long(SMIParameterList* buff);

void smibuff_write_float(SMIParameterList* buff,float f);
float smibuff_read_float(SMIParameterList* buff);

double smibuff_write_double(SMIParameterList* buff,double d);
void smibuff_read_double(SMIParameterList* buff);

void smibuff_write_version(SMIParameterList* buff,version v);
version smibuff_read_version(SMIParameterList* buff);

void smibuff_write_uuid(SMIParameterList* buff,uuid id);
uuid smibuff_read_uuid(SMIParameterList* buff);

void smibuff_write_instant(SMIParameterList* buff,instant i);
instant smibuff_read_instant(SMIParameterList* buff);

void smibuff_write_duration(SMIParameterList* buff,duration d);
duration smibuff_read_duration(SMIParameterList* buff);

void smibuff_write_string(SMIParameterList* buff,const char* str);
/*
 * Reads a string from the buffer, allocating the storage for it using malloc.
 * The returned pointer is the read string, and must be deallocated using free.
 */
char* smibuff_read_string(SMIParameterList* buff);
/*
 * Reads a string from the buffer, allocating the storage for it using allocfn.
 * If Any Errors occur during reading, any allocated storage is returned using deallocfn, and a Null pointer is returned
 * The returned pointer is the read string.
 */
char* smibuff_read_string2(SMIParameterList* buff,void*(*allocfn)(size_t),void(*deallocfn)(void*));

bool smibuff_verify(SMIParameterList* buff,uint32_t hashcode);

uint32_t smibuff_get_param_count(SMIParameterList* buff);

enum Type smibuff_get_type_at(SMIParameterList* buff,uint32_t param);

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_SMI_INVOKE_H__2019_06_01_11_30_58 */
