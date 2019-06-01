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
	ServiceNotOpen = 1,
	MethodErr = 2,
	NoSuchMethod = 3,
	FailedInvoke = -1,
	SystemError = 4
};

typedef struct SService Service;

typedef struct SMethodInfo Method;

/**
 * Finds the service with the given name.
 * If that service does not exist, returns NULL.
 */
Service* srv_find(const char* name);

enum ErrorCode srv_release(Service* srv);

enum ErrorCode srv_open(Service* srv);

enum ErrorCode srv_close(Service* srv);

enum ErrorCode srv_invoke(Service* srv ,uuid method, SMIParameterList* buff, SMIParameterList** out);





#ifdef __cplusplus
}
#endif


#endif /* INCLUDE_SERVICE_SERVICE_H__2019_06_01_11_27_52 */
