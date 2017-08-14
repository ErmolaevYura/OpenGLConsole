#pragma once

#ifndef __FILE_LOGGER
#define __FILE_LOGGER

#include "BaseLogger.h"

class FileLogger
{
private:
	std::thread							m_thread;
	std::queue<std::unique_ptr<LogMsg>> m_msgQueue;
	std::mutex							m_addMutex;
	std::condition_variable				m_printCV;
	std::mutex							m_printMutex;
	LOG_LEVEL							m_level = lFull;
	static int							threadFunc(FileLogger*);
	bool								needNewFile = true;
protected:
	tostringstream						m_oss;

	bool								m_init = false;
	bool								m_working = false;
	bool								m_stop = false;
	bool								m_interrupt = false;

	tofstream m_ofstream;
	tstring m_path;
	tstring m_prefix;
	std::tm m_timeName;

	inline void checkFile(const std::tm* checkTime);
	inline void write(std::unique_ptr<LogMsg> msg);
	inline bool addMsg(std::unique_ptr<LogMsg> msg);
public:
	FileLogger(tstring prefix = _T("app"), tstring path = _T(".//"));
	~FileLogger();
	void SetPath(const tstring &path);
	void SetPrefix(const tstring &prefix);
	void SetLocale(const tstring &locale);
	tstring GetFileName();
	void SetLevel(LOG_LEVEL level);

	template<typename... Args> bool AddMsg(LOG_LEVEL level, tstring str, Args... args) {
		if (!m_init || level > m_level) return false;
		auto tuple = std::make_tuple(std::forward<Args>(args)...);
		LogMsg log;
		log.Create(std::move(str), tuple, level);
		return addMsg(std::make_unique<LogMsg>(std::move(log)));
	}
	template<typename... Args> inline bool AddError(tstring str, Args... args) { return AddMsg(lError, std::move(str), args...); }
	template<typename... Args> inline bool AddInfo(tstring str, Args... args) { return AddMsg(lInfo, std::move(str), args...); }
	template<typename... Args> inline bool AddWarning(tstring str, Args... args) { return AddMsg(lWarning, std::move(str), args...); }
	template<typename... Args> inline bool AddDebug(tstring str, Args... args) { return AddMsg(lDebug, std::move(str), args...); }
	template<typename... Args> inline bool AddTrace(tstring str, Args... args) { return AddMsg(lTrace, std::move(str), args...); }
	template<typename... Args> inline bool AddMsg(tstring str, Args... args) { return AddMsg(lInfo, std::move(str), args...); }

	template<typename T> FileLogger& operator<<(const T& arg) { m_oss << arg; return *this; }
	FileLogger& log(LOG_LEVEL = lTrace);
	inline FileLogger& error() { log(lError); }
	inline FileLogger& info() { log(lInfo); }
	inline FileLogger& warning() { log(lWarning); }
	inline FileLogger& debug() { log(lDebug); }
	inline FileLogger&  trace() { log(lTrace); }

	bool AddMsg(const tstring &msg, LOG_LEVEL level = lTrace);
	void InitThread();
	void UninitThread();
	void Interrupt();
	bool Working() { return m_working; }
};

#endif __FILE_LOGGER