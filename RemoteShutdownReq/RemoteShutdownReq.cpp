// RemoteShutdownReq.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <zmq.hpp>
#include <string>

int main()
{
	const std::string addr = "tcp://192.168.0.104:5555";
	const size_t buffsize = 1024;

	zmq::message_t msg_buff(buffsize);
    zmq::context_t ctx;
	zmq::socket_t requester(ctx, zmq::socket_type::req);

	requester.connect(addr);

	requester.send(zmq::message_t(std::string("Shutdown")), zmq::send_flags::none);

	zmq_sleep(1);
	auto res = requester.recv(msg_buff);

	std::cout << msg_buff.to_string();

	return EXIT_SUCCESS;

}

