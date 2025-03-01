#pragma once

#include <array>
#include <fstream>
#include <iomanip>
#include <mutex>

class Logger final
{
	static constexpr unsigned BUFFLEN{1024};

public:
	/**
     * @brief instance - возвращает экземпляр логгера.
     * @return Экземпляр логгера.
     */
	static Logger& instance();

	/**
     * @brief log - логирует входные данные.
     * @param format - строка для логирования.
     */
	void log(const char* format, ...);

private:
	Logger();
	Logger(const Logger&) = delete;
	Logger& operator=(const Logger&) = delete;
	Logger(Logger&&) = delete;
	Logger& operator=(const Logger&&) = delete;
	~Logger() = default;

private:
	std::array<char, BUFFLEN> m_buffer; ///< Буффер для форматированной строки.
	std::mutex m_mutex; ///< Мьютекс для блокирования одновременного доступа к записи.
	std::ofstream m_file{"log.txt"}; ///< Файл для логирования.
};
