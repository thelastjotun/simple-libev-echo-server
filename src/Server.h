#pragma once

#include <arpa/inet.h>
#include <cstdio>
#include <ev.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "Logger/Logger.h"

namespace Server {

static constexpr std::uint16_t RECEIVE_BUFF{256};

using EVLoop = struct ev_loop;
using EVIo = struct ev_io;

/**
 * @brief cleanup - функция для высвобождения ресурсов главного цикла.
 * @param loop - главный цикл.
 * @param watcher - наблюдатель за входящими подключениями.
 */
static void cleanup(EVLoop* loop, EVIo* watcher)
{
	Logger::instance().log("Cleanup server socket and accept watcher...");
	ev_io_stop(loop, watcher);

	close(watcher->fd);
	delete watcher;
}

/**
 * @brief on_signal - callback-функция обработчик сигнала для завершения.
 * @param loop - главный цикл.
 * @param watcher_signal - наблюдатель за входными сигналами.
 * @param revents - входящие события.
 */
static void on_signal(EVLoop* loop, ev_signal* watcher_signal, int revents)
{
	EVIo* watcher = static_cast<EVIo*>(watcher_signal->data);

	// Высвобождаем ресурсы
	cleanup(loop, watcher);

	// Завершаем цикл
	ev_break(loop, EVBREAK_ALL);
	Logger::instance().log("The server has stopped working...");
}
/**
 * @brief on_read - callback функция для чтения входящих данных.
 * @param loop - цикл обработки событий.
 * @param watcher_read - наблюдатель за входящими событиями.
 * @param events - входящие события.
 */
static void on_read(EVLoop* loop, EVIo* watcher_read, int events)
{
	char receive_buffer[RECEIVE_BUFF];

	// Вычитываем данные из сокета
	ssize_t bytes_received = recv(watcher_read->fd,
								  receive_buffer,
								  sizeof(receive_buffer) - 1, // Принимаем на 1 знак меньше
								  0);						  // чтобы последний всегда был концом строки

	if (bytes_received > 0) {
		// Обозначаем конец принятых данных
		receive_buffer[bytes_received] = '\0';

		Logger::instance().log("Readed data: %s(%d).", receive_buffer, bytes_received);

		// Отправляем принятые данные обратно
		send(watcher_read->fd, receive_buffer, bytes_received, MSG_DONTWAIT);
	} else if (bytes_received == 0) {
		ev_io_stop(loop, watcher_read);
		close(watcher_read->fd);
		Logger::instance().log("Closing socket %d.", watcher_read->fd);
		delete watcher_read;
	} else {
		ev_io_stop(loop, watcher_read);
		close(watcher_read->fd);
		Logger::instance().log("Recieve failed...");
		delete watcher_read;
	}
}

/**
 * @brief on_accept - callback функция для обработки входящих соединений.
 * @param loop - цикл обработки событий.
 * @param watcher_accept - наблюдатель за соединениями.
 * @param events - входящие события.
 */
static void on_accept(EVLoop* loop, EVIo* watcher_accept, int events)
{
	if (events & EV_READ) {
		Logger::instance().log("Waiting for accept incoming connection...");

		// Приниманием подключения с созданием сокета.
		struct sockaddr_in client_addr;
		socklen_t client_len = sizeof(client_addr);
		int sock = accept(watcher_accept->fd,
						  reinterpret_cast<struct sockaddr*>(&client_addr),
						  &client_len);
		if (sock < 0) {
			Logger::instance().log("ERROR accepting client connection.");
			return;
		}

		// Выводим информацию о подключившемся клиенте
		char accepted_buff[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &client_addr.sin_addr, accepted_buff, INET_ADDRSTRLEN);
		Logger::instance().log("Connection accepted from %s:%d.",
							   accepted_buff,
							   ntohs(client_addr.sin_port));

		// Инициируем обработку события приёма данных от клиента
		EVIo* watcher_read = new EVIo;
		ev_io_init(watcher_read, on_read, sock, EV_READ);
		ev_io_start(loop, watcher_read);
	}
}

/**
 * @brief start - запускает цикл обработки соединений с сервером.
 * @param addr - адрес сервера.
 * @param port - порт сервера.
 */
void start(const char* addr, int port)
{
	sockaddr_in addr_in;
	addr_in.sin_family = AF_INET;
	addr_in.sin_addr.s_addr = inet_addr(addr);
	addr_in.sin_port = htons(port);

	int server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (server_socket < 0) {
		Logger::instance().log("ERROR creating socket.");
	}

	// Настраиваем сокет для сервера
	Logger::instance().log("Binding socket...");
	if (bind(server_socket, reinterpret_cast<sockaddr*>(&addr_in), sizeof(addr_in)) < 0) {
		Logger::instance().log("ERROR binding socket.");
		close(server_socket);
	}

	// Настраиваем прослушивание
	Logger::instance().log("Listening socket...");
	if (listen(server_socket, SOMAXCONN) < 0) {
		Logger::instance().log("ERROR listening socket.");
		close(server_socket);
	}

	// Создаём обработчик подключений
	EVIo* accept_watcher = new EVIo();
	ev_io_init(accept_watcher, on_accept, server_socket, EV_READ);

	// Запускаем цикл в неблокирующем режиме
	EVLoop* loop = ev_default_loop(EVRUN_NOWAIT);
	ev_io_start(loop, accept_watcher);

	// Настраиваем обработчик для завершения
	ev_signal sig_watcher;
	ev_signal_init(&sig_watcher, on_signal, SIGINT);
	sig_watcher.data = accept_watcher;
	ev_signal_start(loop, &sig_watcher);

	// Запускаем главный цикл
	ev_run(loop, 0);
}
} // namespace Server
