#include <tdme/network/httpclient/HTTPDownloadClient.h>

#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include <tdme/math/Math.h>
#include <tdme/network/httpclient/HTTPClientException.h>
#include <tdme/os/filesystem/FileSystem.h>
#include <tdme/os/filesystem/FileSystemInterface.h>
#include <tdme/os/network/Network.h>
#include <tdme/os/network/NIOIOSocketClosedException.h>
#include <tdme/os/network/NIOTCPSocket.h>
#include <tdme/os/threading/Mutex.h>
#include <tdme/os/threading/Thread.h>
#include <tdme/utils/Character.h>
#include <tdme/utils/Console.h>
#include <tdme/utils/Exception.h>
#include <tdme/utils/Integer.h>
#include <tdme/utils/StringTokenizer.h>
#include <tdme/utils/StringUtils.h>

using std::ifstream;
using std::ios;
using std::hex;
using std::nouppercase;
using std::ofstream;
using std::ostringstream;
using std::setw;
using std::string;
using std::stringstream;
using std::to_string;
using std::unordered_map;
using std::uppercase;
using std::vector;

using tdme::math::Math;
using tdme::network::httpclient::HTTPClientException;
using tdme::os::filesystem::FileSystem;
using tdme::os::filesystem::FileSystemInterface;
using tdme::os::network::Network;
using tdme::os::network::NIOIOSocketClosedException;
using tdme::os::network::NIOTCPSocket;
using tdme::os::threading::Mutex;
using tdme::os::threading::Thread;
using tdme::utils::Character;
using tdme::utils::Console;
using tdme::utils::Exception;
using tdme::utils::Integer;
using tdme::utils::StringTokenizer;
using tdme::utils::StringUtils;

using tdme::network::httpclient::HTTPDownloadClient;

HTTPDownloadClient::HTTPDownloadClient(): downloadThreadMutex("downloadthread-mutex") {
}

string HTTPDownloadClient::createHTTPRequestHeaders(const string& hostName, const string& relativeUrl) {
	auto request =
		string("GET " + relativeUrl + " HTTP/1.1\r\n") +
		string("User-Agent: tdme2-httpdownloadclient\r\n") +
		string("Host: " + hostName + "\r\n") +
		string("Connection: close\r\n") +
		string("\r\n");
	return request;
}

void HTTPDownloadClient::parseHTTPResponseHeaders(ifstream& rawResponse, int16_t& httpStatusCode, vector<string>& httpHeader) {
	string lastLine;
	string line;
	char lastChar = -1;
	char currentChar;
	while (rawResponse.eof() == false) {
		rawResponse.get(currentChar);
		if (lastChar == '\r' && currentChar == '\n') {
			if (line.size() != 0) {
				httpHeader.push_back(line);
			}
			lastLine = line;
			if (line.size() == 0 && lastLine.size() == 0) break;
			line.clear();
		} else
		if (currentChar != '\r' && currentChar != '\n') {
			line+= currentChar;
		}
		lastChar = currentChar;
	}
	if (httpHeader.size() > 0) {
		StringTokenizer t;
		t.tokenize(httpHeader[0], " ");
		for (auto i = 0; i < 3 && t.hasMoreTokens(); i++) {
			auto token = t.nextToken();
			if (i == 1) {
				httpStatusCode = Integer::parseInt(token);
			}
		}
	}
}

void HTTPDownloadClient::reset() {
	url.clear();
	file.clear();
}

void HTTPDownloadClient::execute() {
	class DownloadThread: public Thread {
		public:
			DownloadThread(HTTPDownloadClient* downloadClient): Thread("download-thread"), downloadClient(downloadClient) {
			}
			void run() {
				downloadClient->finished = false;
				downloadClient->progress = 0.0f;
				NIOTCPSocket socket;
				try {
					if (StringUtils::startsWith(downloadClient->url, "http://") == false) throw HTTPClientException("Invalid protocol");
					auto relativeUrl = StringUtils::substring(downloadClient->url, string("http://").size());
					if (relativeUrl.size() == 0) throw HTTPClientException("No URL given");
					auto slashIdx = relativeUrl.find('/');
					auto hostName = relativeUrl;
					if (slashIdx != -1) hostName = StringUtils::substring(relativeUrl, 0, slashIdx);
					relativeUrl = StringUtils::substring(relativeUrl, hostName.size());

					Console::println("HTTPDownloadClient::execute(): Hostname: " + hostName);
					Console::println("HTTPDownloadClient::execute(): RelativeUrl: " + relativeUrl);

					Console::print("HTTPDownloadClient::execute(): Resolving name to IP: " + hostName + ": ");
					auto ip = Network::getIpByHostName(hostName);
					if (ip.size() == 0) {
						Console::println("HTTPDownloadClient::execute(): Failed");
						throw HTTPClientException("Could not resolve host IP by host name");
					}
					Console::println(ip);

					// socket
					NIOTCPSocket::create(socket, NIOTCPSocket::determineIpVersion(ip));
					socket.connect(ip, 80);
					auto request = downloadClient->createHTTPRequestHeaders(hostName, relativeUrl);
					socket.write((void*)request.data(), request.length());

					{
						// output file stream
						ofstream ofs((downloadClient->file + ".download").c_str(), ofstream::binary);
						if (ofs.is_open() == false) {
							throw HTTPClientException("Unable to open file for writing(" + to_string(errno) + "): " + (downloadClient->file + ".download"));
						}

						// download
						char rawResponseBuf[16384];
						auto rawResponseBytesRead = 0;
						try {
							for (;true;) {
								auto rawResponseBytesRead = socket.read(rawResponseBuf, sizeof(rawResponseBuf));
								ofs.write(rawResponseBuf, rawResponseBytesRead);
							};
						} catch (NIOIOSocketClosedException& sce) {
							// end of stream
						}

						// close download file
						ofs.close();
					}

					{
						// input file stream
						ifstream ifs((downloadClient->file + ".download").c_str(), ofstream::binary);
						if (ifs.is_open() == false) {
							throw HTTPClientException("Unable to open file for reading(" + to_string(errno) + "): " + (downloadClient->file + ".download"));
						}

						downloadClient->parseHTTPResponseHeaders(ifs, downloadClient->httpStatusCode, downloadClient->httpHeader);

						auto ifsHeaderSize = ifs.tellg();
						ifs.seekg(0, ios::end);
						auto ifsSizeTotal = ifs.tellg();
						auto ifsSize = ifsSizeTotal - ifsHeaderSize;
						ifs.seekg(ifsHeaderSize, ios::beg);

						// output file stream
						ofstream ofs(downloadClient->file.c_str(), ofstream::binary);
						if (ofs.is_open() == false) {
							throw HTTPClientException("Unable to open file for writing(" + to_string(errno) + "): " + downloadClient->file);
						}

						//
						char buf[16384];
						auto ifsBytesToRead = 0;
						auto ifsBytesRead = 0;
						do {
							auto ifsBytesToRead = Math::min(ifsSize - ifsBytesRead, sizeof(buf));
							ifs.read(buf, ifsBytesToRead);
							ofs.write(buf, ifsBytesToRead);
							ifsBytesRead+= ifsBytesToRead;
						} while (ifsBytesRead < ifsSize);

						// close target file
						ofs.close();

						// close download file
						ifs.close();

						//
						FileSystem::getStandardFileSystem()->removeFile(".", downloadClient->file + ".download");
					}

					//
					socket.shutdown();

					//
					downloadClient->finished = true;
					downloadClient->progress = 1.0f;
				} catch (Exception& exception) {
					socket.shutdown();
					Console::println(string("HTTPDownloadClient::execute(): performed HTTP request: FAILED: ") + exception.what());
					// rethrow
					throw;
				}
			}
		private:
			HTTPDownloadClient* downloadClient;
	};
	downloadThreadMutex.lock();
	finished = false;
	this->downloadThread = new DownloadThread(this);
	downloadThreadMutex.unlock();
	this->downloadThread->start();
	downloadThreadMutex.unlock();
}

void HTTPDownloadClient::cancel() {
	downloadThreadMutex.lock();
	if (downloadThread != nullptr) downloadThread->stop();
	downloadThreadMutex.unlock();
}

void HTTPDownloadClient::join() {
	downloadThreadMutex.lock();
	if (downloadThread != nullptr) {
		downloadThread->join();
		delete this->downloadThread;
		this->downloadThread = nullptr;
	}
	downloadThreadMutex.unlock();
}