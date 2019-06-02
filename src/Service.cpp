/*
 * Service.c
 *
 *  Created on: Jun 1, 2019
 *      Author: chorm
 */


#include <service/Service.h>

#include <string>

#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

#include <unordered_map>

struct SService{
	int pipefd;
	char* errMsgCache;
	const char* serviceName;
	int exposedFd;
};

static bool consume_dir_exists(){
	DIR* d = opendir("/proc/self/consrv");
	if(d){
		closedir(d);
		return true;
	}else if(ENOENT == errno)
		return false;
	else
		return true;

}


struct ServiceDirectoryGuard{
public:
	int consumeFd;
	int provideFd;
	ServiceDirectoryGuard(){
		if(!consume_dir_exists())
			mkdir("/proc/self/consrv",S_IRWXU|S_IRWXG|S_IROTH);
		consumeFd = open("/proc/self/consrv",O_PATH|O_DIRECTORY);
		provideFd = open("/proc/services",O_PATH|O_DIRECTORY);
	}
	~ServiceDirectoryGuard(){
		close(consumeFd);
		close(provideFd);
	}
};



static const ServiceDirectoryGuard guard{};

static constexpr mode_t mode{0666};

static void set_error(Service* srv,const char* msg){
	if(srv->errMsgCache)
		delete[] srv->errMsgCache;
	srv->errMsgCache = new char[strlen(msg)+1];
	strcpy(srv->errMsgCache,msg);
}

static bool check_open(Service* srv){
	if(srv->pipefd==0){
		set_error(srv,"Service Operation Failed, Service not currently open");
		return false;
	}
	return true;
}

static bool check_closed(Service* srv){
	if(srv->pipefd==0)
		return true;
	set_error(srv,"Cannot Open Service, Service Already open");
	return false;
}

ErrorCode srv_open(Service* srv){
	if(!check_closed(srv))
		return ServiceAlreadyOpen;
	int provideFd = openat(guard.provideFd,srv->serviceName,O_RDWR);
	if(!provideFd){
		set_error(srv,"Failed to open service, service source not found or cannot be opened");
		return SystemError;
	}
	mkfifoat(guard.consumeFd,srv->serviceName,mode)
	int consumeFd = openat(guard.consumeFd,srv->serviceName,O_RDWR);
	if(!consumeFd){
		set_error(srv,"Failed to open service, consumer pipe could not be created");
		return SystemError;
	}
	pid_t pid = getpid();
	uint8_t buffer[128]{0xAA,'S','R','V'};
	uint8_t sz = strlen(srv->serviceName);
	memcpy(buffer+4,&pid,sizeof(pid));
	memcpy(buffer+5+sizeof(pid),&sz,1);
	memcpy(buffer+6+sizeof(pid),srv->serviceName,sz);
	if(write(provideFd,buffer,6+sizeof(pid)+sz)==-1){
		set_error(srv,"Failed to open service, write to source pipe failed");
		return SystemError;
	}
	srv->exposedFd = provideFd;
	srv->pipefd = provideFd;
	return Ok;
}

const std::string consumeDir{"/proc/services"};

Service* serv_find(const char* service){
	if(access((consumeDir+service).c_str(),R_OK|W_OK))
		return NULL;
	else{
		Service* srv = new Service;
		srv->serviceName = service;
		srv->exposedFd = -1;
		srv->pipefd = -1;
		srv->errMsgCache = NULL;
		return srv;
	}
}

