#include "Server.h"

static constexpr unsigned short PORT{5000};
static const std::string ADDRESS{"127.0.0.1"};

int main(int argc, char* argv[])
{
	Server::start(ADDRESS.c_str(), PORT);
	return 0;
}
