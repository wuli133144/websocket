#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include "debug_log.h"
#include "network_interface.h"

Network_Interface *Network_Interface::m_network_interface = NULL;

Network_Interface::Network_Interface():
		epollfd_(0),
		listenfd_(0),
		websocket_handler_map_()
{
	if(0 != init())
		exit(1);
}

Network_Interface::~Network_Interface(){

}

int Network_Interface::init(){
    
	listenfd_ = socket(AF_INET, SOCK_STREAM, 0);
	if(listenfd_ == -1){
		DEBUG_LOG("error create socket fd!");
		return -1;
	}
    int optval = 1;  
    if (setsockopt(listenfd_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {  
    
      exit(-1);  
    }   
	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(sockaddr_in));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(g_serverinfo.ipaddr.c_str());
	server_addr.sin_port = htons(g_serverinfo.port);
	//DEBUG_LOG("sever ip port=[%s,%u]",g_serverinfo.ipaddr.c_str(),g_serverinfo.port);
	if(-1 == bind(listenfd_, (struct sockaddr *)(&server_addr), sizeof(server_addr))){
		DEBUG_LOG("bind socket failed!");
		return -1;
	}
	//setsockopt(listenfd_,SOL_SOCKET ,SO_REUSEADDR,(const char*)&bReuseaddr,sizeof(BOOL)); 
	if(-1 == listen(listenfd_, 5)){
		DEBUG_LOG("listen socket failed!");
		return -1;
	}
	epollfd_ = epoll_create(MAXEVENTSSIZE);

	ctl_event(listenfd_, true);	
	DEBUG_LOG("success start listen server!");
	return 0;
}

int Network_Interface::epoll_loop(){
	struct sockaddr_in client_addr;
	socklen_t clilen;
	int nfds = 0;
	int fd = 0;
	int bufflen = 0;
	struct epoll_event events[MAXEVENTSSIZE];
	while(true){
		nfds = epoll_wait(epollfd_, events, MAXEVENTSSIZE, TIMEWAIT);
		for(int i = 0; i < nfds; i++){
			if(events[i].data.fd == listenfd_){
				fd = accept(listenfd_, (struct sockaddr *)&client_addr, &clilen);
				ctl_event(fd, true);
			}
			else if(events[i].events & EPOLLIN){
				DEBUG_LOG("epollin......");
				if((fd = events[i].data.fd) < 0)
					continue;
				Websocket_Handler *handler = websocket_handler_map_[fd];
				if(handler == NULL)
					continue;
				if((bufflen = read(fd, handler->getbuff(), BUFFLEN)) <= 0){
					ctl_event(fd, false);
				}
				else{
					handler->process();
					//ctl_event1(fd,0);
					
				}
			}else if(events[i].events & EPOLLOUT){
			      
			}
		}
	}

	return 0;
}

int Network_Interface::set_noblock(int fd){
	int flags;
    if ((flags = fcntl(fd, F_GETFL, 0)) == -1)
        flags = 0;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

Network_Interface *Network_Interface::get_share_network_interface(){
	if(m_network_interface == NULL)
		m_network_interface = new Network_Interface();
	return m_network_interface;
}

void Network_Interface::ctl_event(int fd, bool flag){
	struct epoll_event ev;
	ev.data.fd = fd;
	ev.events = flag ? EPOLLIN : 0;
	epoll_ctl(epollfd_, flag ? EPOLL_CTL_ADD : EPOLL_CTL_DEL, fd, &ev);
	if(flag){
		set_noblock(fd);
		websocket_handler_map_[fd] = new Websocket_Handler(fd);
		if(fd != listenfd_)
			DEBUG_LOG("fd: %d add epoll loop", fd);
	}
	else{
		close(fd);
		delete websocket_handler_map_[fd];
		websocket_handler_map_.erase(fd);
		DEBUG_LOG("fd: %d exit loop", fd);
	}
}


void Network_Interface::ctl_event1(int fd,int flags)
{
   struct epoll_event ev;
   ev.data.fd=fd;
   ev.events=(flags==0)?EPOLLOUT:EPOLLIN;
   epoll_ctl(epollfd_, EPOLL_CTL_MOD, fd, &ev);
   
}


void Network_Interface::run(){
	epoll_loop();
}
