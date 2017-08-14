#include "stdafx.h"
#include "FileLogger.h"
#include "ConsoleLogger.h"
#include <filesystem>

inline void FileLogger::checkFile(const std::tm* checkTime) {
	if (m_timeName.tm_year != checkTime->tm_year && m_timeName.tm_mon != checkTime->tm_mon && m_timeName.tm_mday != checkTime->tm_mday) {
		m_timeName = std::tm(*checkTime);
		if (m_ofstream.is_open()) m_ofstream.close();
		needNewFile = true;
	}
	if (needNewFile) {
		tstring fileName = GetFileName();
		m_ofstream.open(fileName, std::ios_base::out | std::ios_base::app);
		needNewFile = false;
	}
}
inline void FileLogger::write(std::unique_ptr<LogMsg> msg) {
	std::unique_ptr<std::tm> time;
	TCHAR str_time[21];
#ifdef _WIN32 || _WIN64
	time = std::make_unique<std::tm>();
	localtime_s(time.get(), &msg->time);
#else
	time = std::make_unique<std::tm>(std::localtime(&msg->time));
#endif
	checkFile(time.get());
	if (m_ofstream.is_open()) {
		tstrftime(str_time, sizeof(str_time), _T("%d.%m.%Y %H:%M:%S "), time.get());
		m_ofstream << str_time << (
			msg->level == lError	? _T("ERR") : msg->level == lInfo	? _T("INF") :
			msg->level == lWarning	? _T("WAR") : msg->level == lDebug	? _T("DBG") :
			msg->level == lTrace	? _T("TRC") : _T("UNK"))
			<< _T(" ") << msg->text << "\n" << std::flush;
	}
}
inline bool FileLogger::addMsg(std::unique_ptr<LogMsg> msg) {
	if (!m_init) return false;
	m_addMutex.lock();
	m_msgQueue.push(std::move(msg));
	m_addMutex.unlock();
	m_printCV.notify_one();
	return true;
}
int FileLogger::threadFunc(FileLogger *lg) {
	int result = 0;
	int k = 0;
	std::unique_ptr<LogMsg> msg;
	std::unique_lock<std::mutex> printLock(lg->m_printMutex);
start:
	try {
		lg->m_working = true;
		while (!lg->m_stop && !lg->m_interrupt) {
			lg->m_printCV.wait(printLock);
			while (lg->m_msgQueue.size() > 0 && !lg->m_interrupt) {
				lg->m_addMutex.lock();
				msg = std::move(lg->m_msgQueue.front());
				lg->m_msgQueue.pop();
				lg->m_addMutex.unlock();
				lg->write(std::move(msg));
			}
		}
		while (!lg->m_msgQueue.empty()) lg->m_msgQueue.pop();
		lg->m_ofstream.close();
	}
	catch (std::exception* ex) {
		lg->AddError(_T("Exeption in Logger {0}, {1}"), lg->GetFileName(), GetString(ex->what()));
		ConsoleLogger::AddError(_T("Exeption in Logger {0}, {1}"), lg->GetFileName(), GetString(ex->what()));
		goto start;
	}
	lg->m_working = false;
	return result;
}
FileLogger::FileLogger(tstring prefix, tstring path) : m_prefix(prefix), m_path(path) {
	std::time_t time;
	std::time(&time);
#ifdef _WIN32 || _WIN64
	localtime_s(&m_timeName, &time);
#else
	std::localtime(&m_timeName)
#endif
	std::experimental::filesystem::create_directory(path.c_str());
}
FileLogger::~FileLogger() { UninitThread(); }
void FileLogger::SetPath(const tstring &path) {
	if (m_path != path) {
		m_path = path;
		std::experimental::filesystem::create_directory(path);
	}
}
void FileLogger::SetPrefix(const tstring &prefix) {
	if (m_prefix != prefix) m_prefix = prefix;
}
void FileLogger::SetLocale(const tstring &locale) {
	m_ofstream.imbue(std::locale(GetC(locale.c_str())));
}
void FileLogger::SetLevel(LOG_LEVEL level) {
	m_level = level;
}
tstring FileLogger::GetFileName() {
	TCHAR str_time[11];
	tstrftime(str_time, sizeof(str_time), _T("%Y.%m.%d"), &m_timeName);
	return m_path + m_prefix + _T("_") + str_time + _T(".log");
}
void FileLogger::InitThread() {
	if (m_init) return;
	m_stop = false;
	m_interrupt = false;
	m_thread = std::thread(threadFunc, this);
	m_init = true;
}
void FileLogger::UninitThread() {
	if (!m_init) return;
	m_init = false;
	m_stop = true;
	m_printCV.notify_one();
	m_thread.join();
}
bool FileLogger::AddMsg(const tstring &msg, LOG_LEVEL level) {
	if (!m_init || level > m_level) return false;
	return addMsg(std::make_unique<LogMsg>(LogMsg(std::move(msg), level)));
}
FileLogger& FileLogger::log(LOG_LEVEL level) {
	if (m_level >= level) {
		LogMsg log(m_oss.str(), level);
		addMsg(std::make_unique<LogMsg>(std::move(log)));
		m_oss.str(_T(""));
	}
	return *this;
}
void FileLogger::Interrupt() {
	m_init = false;
	m_interrupt = true;
	m_printCV.notify_one();
	ConsoleLogger::AddInfo(_T("Interrupt FileLogger for {0}"), GetFileName());
}
