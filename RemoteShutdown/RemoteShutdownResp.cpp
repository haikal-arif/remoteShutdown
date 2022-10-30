// RemoteShutdown.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define RECV_TIMEOUT 1000 // timeout in millisecond
#define WIN32_LEAN_AND_MEAN 

#include <iostream>
#include <zmq.hpp>
#include <chrono>
#include <string>
#include <Windows.h>

void SystemShutdown(UINT nSDType);

int main()
{
	using namespace std::chrono;

	const size_t buffsize = 1024;
	const std::string addr = "tcp://*:5555";
	std::string str_recvd_msg;

	steady_clock::time_point timer = steady_clock::now();

	zmq::message_t msg_buff(buffsize);
	zmq::message_t acc_resp(std::string("Command received, shutting down computer...\n"));
	zmq::message_t dec_resp(std::string("Command not found.\n"));
	zmq::context_t ctx;
	zmq::socket_t responder(ctx, zmq::socket_type::rep);
	


	responder.bind(addr);
	std::cout << "Responder has succesfully bind to " << addr << "\n";
	

	while (true)
	{
		auto interval = duration_cast<milliseconds>(steady_clock::now() - timer);
		if (interval.count() < RECV_TIMEOUT)
			continue;

		auto res = responder.recv(msg_buff);
		str_recvd_msg = msg_buff.to_string();

		if (str_recvd_msg.compare("Shutdown")) // return 1 if fail
		{
			responder.send(dec_resp, zmq::send_flags::none);
			continue;
		} 
			
		responder.send(acc_resp, zmq::send_flags::none);
		break;
	}


	responder.close();
	ctx.close();

#ifdef _DEBUG
	std::cout << "Shutting Down...\n";
#endif // DEBUG

#ifdef NDEBUG
	SystemShutdown(0);
#endif // NDEBUG

	

	return EXIT_SUCCESS;
}



// ==========================================================================
// system shutdown
// nSDType: 0 - Shutdown the system
//          1 - Shutdown the system and turn off the power (if supported)
//          2 - Shutdown the system and then restart the system
void SystemShutdown(UINT nSDType)
{
	HANDLE           hToken;
	TOKEN_PRIVILEGES tkp;

	::OpenProcessToken(::GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
	::LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);

	tkp.PrivilegeCount = 1; // set 1 privilege
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	// get the shutdown privilege for this process
	::AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);

	switch (nSDType)
	{
	case 0: ::ExitWindowsEx(EWX_HYBRID_SHUTDOWN | EWX_SHUTDOWN | EWX_FORCE, 0); break;
	case 1: ::ExitWindowsEx(EWX_POWEROFF | EWX_FORCE, 0); break;
	case 2: ::ExitWindowsEx(EWX_REBOOT | EWX_FORCE, 0); break;
	}
}


