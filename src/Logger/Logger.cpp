#include "Logger.h"

#include <cassert>
#include <chrono>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <iostream>

Logger& Logger::instance()
{
	// Lazy & thread-safety реализация синглтона.
	static Logger instance;
	return instance;
}

void Logger::log(const char* format, ...)
{
	if (!format)
		return;

	std::lock_guard lock(m_mutex);

	va_list args;
	va_start(args, format);
	vsprintf(m_buffer.data(), format, args);
	va_end(args);

	auto now = std::chrono::system_clock::now();
	auto currentTime = std::chrono::system_clock::to_time_t(now);
	auto puttedTime = std::put_time(std::localtime(&currentTime), "%d-%m-%Y / %X");

	// Проводим логирование в файл.
	m_file << puttedTime << " | " << m_buffer.data() << std::endl;

	// Проводим логирование в Command Line.
	std::cout << puttedTime << " | " << m_buffer.data() << std::endl;

	std::fill(m_buffer.begin(), m_buffer.end(), 0);
}

Logger::Logger()
{
	assert(m_file.is_open());
}
