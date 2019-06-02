/*
 * Invoke.h
 *
 *  Created on: Jun 1, 2019
 *      Author: chorm
 */

#ifndef INCLUDE_SMI_INVOKE_H__2019_06_01_11_30_58
#define INCLUDE_SMI_INVOKE_H__2019_06_01_11_30_58

typedef struct uuid{
	uint64_t hi;
	uint64_t lo;
#ifdef __cplusplus
	constexpr uuid():hi{},lo{}{}
	constexpr uuid(uint64_t hi,uint64_t lo):hi{hi},lo{lo}{}
	uuid(const char* rep);
	static uuid fromSHA1Hash(const char* in);
#endif
} uuid;

typedef struct version{
	uint8_t maj;
	uint8_t min;
} version;

typedef struct instant{
	uint64_t seconds;
	uint32_t nanos;
#ifdef __cplusplus
	static instant now();
#endif
} instant;

typedef struct{
	uint64_t seconds;
	uint32_t nanos;
} duration;

#ifdef __cplusplus
extern"C"{
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>



typedef struct SBuff SMIBuffer;

enum Type{
	Byte = 'B', SByte = 'K',
	Short = 'S', UShort = 'A',
	Int = 'I', UInt = 'U',
	Long = 'J', Uuid = 'P',
	Bool = 'Z', VERSION = 'V',
	INSTANT = 'Q', DURATION = 'T',
	Float = 'F', Double = 'D',
	String = 'R'
};

SMIBuffer* smibuff_new();

void smibuff_free(SMIBuffer* buff);

void smibuff_write_bool(SMIBuffer* buff,bool b);
bool smibuff_read_bool(SMIBuffer* buff);

void smibuff_write_byte(SMIBuffer* buff,uint8_t b);
uint8_t smibuff_read_byte(SMIBuffer* buff);

void smibuff_write_sbyte(SMIBuffer* buff,int8_t b);
int8_t smibuff_read_sbyte(SMIBuffer* buff);

void smibuff_write_short(SMIBuffer* buff,int16_t s);
int16_t smibuff_read_short(SMIBuffer* buff);

void smibuff_write_ushort(SMIBuffer* buff,uint16_t s);
uint16_t smibuff_read_ushort(SMIBuffer* buff);

void smibuff_write_int(SMIBuffer* buff,int32_t i);
int32_t smibuff_read_int(SMIBuffer* buff);

void smibuff_write_uint(SMIBuffer* buff,uint32_t i);
uint32_t smibuff_read_uint(SMIBuffer* buff);

void smibuff_write_long(SMIBuffer* buff,int64_t l);
int64_t smibuff_read_long(SMIBuffer* buff);

void smibuff_write_float(SMIBuffer* buff,float f);
float smibuff_read_float(SMIBuffer* buff);

double smibuff_write_double(SMIBuffer* buff,double d);
void smibuff_read_double(SMIBuffer* buff);

void smibuff_write_version(SMIBuffer* buff,version v);
version smibuff_read_version(SMIBuffer* buff);

void smibuff_write_uuid(SMIBuffer* buff,uuid id);
uuid smibuff_read_uuid(SMIBuffer* buff);

void smibuff_write_instant(SMIBuffer* buff,instant i);
instant smibuff_read_instant(SMIBuffer* buff);

void smibuff_write_duration(SMIBuffer* buff,duration d);
duration smibuff_read_duration(SMIBuffer* buff);

void smibuff_write_string(SMIBuffer* buff,const char* str);
/*
 * Reads a string from the buffer, allocating the storage for it using malloc.
 * The returned pointer is the read string, and must be deallocated using free.
 */
char* smibuff_read_string(SMIBuffer* buff);
/*
 * Reads a string from the buffer, allocating the storage for it using allocfn.
 * If Any Errors occur during reading, any allocated storage is returned using deallocfn, and a Null pointer is returned
 * The returned pointer is the read string.
 */
char* smibuff_read_string2(SMIBuffer* buff,void*(*allocfn)(size_t),void(*deallocfn)(void*));

bool smibuff_verify(SMIBuffer* buff,uint32_t hashcode);

uint32_t smibuff_get_param_count(SMIBuffer* buff);

enum Type smibuff_get_type_at(SMIBuffer* buff,uint32_t param);

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_SMI_INVOKE_H__2019_06_01_11_30_58 */
