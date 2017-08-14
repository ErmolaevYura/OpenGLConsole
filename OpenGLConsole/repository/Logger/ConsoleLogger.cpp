#include "stdafx.h"
#include "ConsoleLogger.h"
#if _WIN32 || _WIN64
	#include <Windows.h>
#endif // #if _WIN32 || _WIN64

bool						ConsoleLogger::m_init = false;
bool						ConsoleLogger::m_working = false;
bool						ConsoleLogger::m_stop = false;
bool						ConsoleLogger::m_interrupt = false;
#ifdef CONSOLE_LOGGER						
std::thread					ConsoleLogger::m_thread;

std::queue<std::unique_ptr<LogMsg>>		ConsoleLogger::m_msgQueue;
std::mutex					ConsoleLogger::m_addMutex;
std::condition_variable		ConsoleLogger::m_printCV;
std::mutex					ConsoleLogger::m_printMutex;
	
inline void ConsoleLogger::print(std::unique_ptr<LogMsg> msg) {
	std::unique_ptr<std::tm> time;
	TCHAR str_time[21];
#if _WIN32 || _WIN64
	auto console = GetStdHandle(STD_OUTPUT_HANDLE);
	switch (msg->level) {
		case lInfo: SetConsoleTextAttribute(console, FOREGROUND_GREEN); break;
		case lError: SetConsoleTextAttribute(console, FOREGROUND_RED); break;
		case lWarning: SetConsoleTextAttribute(console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);	break;
		case lDebug: SetConsoleTextAttribute(console, FOREGROUND_BLUE); break;
		case lTrace: SetConsoleTextAttribute(console, FOREGROUND_INTENSITY); break;
		default: SetConsoleTextAttribute(console, FOREGROUND_INTENSITY); break;
	}
	time = std::make_unique<std::tm>();
	localtime_s(time.get(), &msg->time);
#else
	time = std::make_unique<std::tm>(std::localtime(&msg->time));
#endif
	tstrftime(str_time, sizeof(str_time), _T("%H:%M:%S %Y.%m.%d "), time.get());
	tcout << str_time << msg->text << "\n" << std::flush;
}
int ConsoleLogger::threadFunc() {
	int result = 0;
	std::unique_ptr<LogMsg> msg;
	std::unique_lock<std::mutex> printLock(m_printMutex);
start:
	try {
		m_working = true;
		while (!m_stop && !m_interrupt) {
			m_printCV.wait(printLock);
			while (m_msgQueue.size() > 0 && !m_interrupt) {
				m_addMutex.lock();
				msg = std::move(m_msgQueue.front());
				m_msgQueue.pop();
				m_addMutex.unlock();
				print(std::move(msg));
			}
			while (!m_msgQueue.empty()) m_msgQueue.pop();
		}
	}
	catch (std::exception* ex) {
		AddError(_T("Exeption in ConsoleLogger: {0}"), GetString(ex->what()));
		goto start;
	}
	m_working = false;
	return result;
}
#endif
void ConsoleLogger::InitThread() {
	if (m_init) return;
	m_stop = false;
	m_interrupt = false;
#ifdef CONSOLE_LOGGER
	m_thread = std::thread(threadFunc);
#endif // CONSOLE_LOGGER
	m_init = true;
}
void ConsoleLogger::UninitThread() {
	if (!m_init) return;
	m_init = false;
	m_stop = true;
#ifdef CONSOLE_LOGGER
	m_printCV.notify_one();
	m_thread.join();
#endif // CONSOLE_LOGGER
}
bool ConsoleLogger::AddMsg(const tstring &msg, LOG_LEVEL level) {
	if (!m_init || level > m_level) return false;
#ifdef CONSOLE_LOGGER
	return addMsg(std::make_unique<LogMsg>(LogMsg(std::move(msg), level)));
#else 
	return true;
#endif // CONSOLE_LOGGER
}
void ConsoleLogger::Interrupt() {
	m_init = false;
	m_interrupt = true;
	m_printCV.notify_one();
}
