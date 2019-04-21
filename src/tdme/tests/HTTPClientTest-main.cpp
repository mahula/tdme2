#include <string>

#include <tdme/utils/Console.h>
#include <tdme/utils/Exception.h>

#include <tdme/os/network/NIOTCPSocket.h>

using std::string;
using std::to_string;

using tdme::os::network::NIOTCPSocket;
using tdme::utils::Console;
using tdme::utils::Exception;

int main(int argc, char *argv[]) {
	try {
		NIOTCPSocket socket;
		NIOTCPSocket::create(socket, NIOTCPSocket::determineIpVersion("2a02:2350:5:101:5140:0:3b22:c515"));
		socket.connect("2a02:2350:5:101:5140:0:3b22:c515", /*"46.30.213.69",*/ 80);
		string req =
			string("GET /dnwp/ HTTP/1.1\r\n") +
			string("User-Agent: tdme2-httpclient\r\n") +
			string("Host: drewke.net\r\n") +
			string("Connection: close\r\n\r\n");
		socket.write((void*)req.c_str(), req.length());
		char c[4096];
		auto bytesRead = socket.read(c, 4096);
		Console::print(string(c, bytesRead));
		socket.shutdown();
	} catch (Exception& exception) {
		Console::println(string("Fail: ") + exception.what());
	}
}

