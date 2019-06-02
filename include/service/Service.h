/*
 * Service.h
 *
 *  Created on: Jun 1, 2019
 *      Author: chorm
 */

#ifndef INCLUDE_SERVICE_SERVICE_H__2019_06_01_11_27_52
#define INCLUDE_SERVICE_SERVICE_H__2019_06_01_11_27_52

#ifdef __cplusplus
extern"C"{
#endif

#include <smi/Invoke.h>

enum ErrorCode{
	Ok = 0,
	ServiceNotOpen = 'O',
	MethodErr = 'E',
	NoSuchMethod = 'M',
	FailedInvoke = 'F',
	SystemError = 'S',
	ServiceAlreadyOpen = 'A'
};

typedef struct SService Service;

typedef struct SMethod Method;

/**
 * Finds the service with the given name.
 * If that service does not exist, returns NULL.
 *
 * A Single Service may be opened for a given program with a given name at any given time or the behavior is undefined.
 * Additionally, Services are single-thread only. Only the thread which opens a service (not finds it) may operate on it, until it is closed (and at which point, may be reopened by a different thread).
 * The behavior is undefined if a thread which did not open a Service operates on it.
 */
Service* srv_find(const char* name);

void srv_release(Service* srv);

enum ErrorCode srv_open(Service* srv);

enum ErrorCode srv_close(Service* srv);



Method* srv_get_method(Service* srv, uuid methodid);
void srv_free_method(Service* owner,Method* method);

uint16_t method_get_parameter_count(Method* method);
enum Type method_get_parameter_type(Method* method,uint16_t idx);

uint16_t method_get_return_count(Method* method);
enum Type method_get_return_count(Method* method);

enum ErrorCode method_invoke(Method* method,SMIBuffer* inoutbuffer);

/*
 * Returns a read-only pointer to the last error reported by the service.
 * If no error has been reported yet, returns NULL.
 * Otherwise, returns a textual representation of the error message.
 * If a method is invoked on this service, and an error is returned, all pointers previously returned by this method are invalidated and cannot be safely dereferenced.
 */
const char* get_last_error(Service* srv);

#ifdef __cplusplus
}
#endif


#endif /* INCLUDE_SERVICE_SERVICE_H__2019_06_01_11_27_52 */
