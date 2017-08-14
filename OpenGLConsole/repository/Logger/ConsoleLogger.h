#pragma once

#ifndef __CONSOLE_LOGGER
#define __CONSOLE_LOGGER

#include "BaseLogger.h"

#define CONSOLE_LOGGER

class ConsoleLogger
{
private:
#ifdef CONSOLE_LOGGER
	static std::thread							m_thread;
	
	static std::queue<std::unique_ptr<LogMsg>>	m_msgQueue;
	static std::mutex							m_addMutex;
	static std::condition_variable				m_printCV;
	static std::mutex							m_printMutex;
	static int									threadFunc();
#endif // CONSOLE_LOGGER
protected:
	static bool									m_init;
	static bool									m_working;
	static bool									m_stop;
	static bool									m_interrupt;
	const static LOG_LEVEL						m_level = lFull;
#ifdef CONSOLE_LOGGER
	static inline void							print(std::unique_ptr<LogMsg> msg);
	static inline bool 							addMsg(std::unique_ptr<LogMsg> msg)
	{
		if (!m_init) return false;
		m_addMutex.lock();
		m_msgQueue.push(std::move(msg));
		m_addMutex.unlock();
		m_printCV.notify_one();
		return true;
	}
#endif // CONSOLE_LOGGER
public:
	template <typename... Args> static bool AddMsg(LOG_LEVEL level, tstring str, Args... args)
	{
		if (!m_init || level > m_level) return false;
#ifdef CONSOLE_LOGGER
		auto tuple = std::make_tuple(std::forward<Args>(args)...);
		LogMsg log;
		log.Create(std::move(str), tuple, level);
		return addMsg(std::make_unique<LogMsg>(std::move(log)));
#else
		return true;
#endif // CONSOLE_LOGGER
	}
	template <typename... Args> static bool AddError(tstring str, Args... args) { return AddMsg(lError, std::move(str), args...); }
	template <typename... Args> static bool AddInfo(tstring str, Args... args) { return AddMsg(lInfo, std::move(str), args...); }
	template <typename... Args> static bool AddWarning(tstring str, Args... args) { return AddMsg(lWarning, std::move(str), args...); }
	template <typename... Args> static bool AddDebug(tstring str, Args... args) { return AddMsg(lDebug, std::move(str), args...); }
	template <typename... Args> static bool AddTrace(tstring str, Args... args) { return AddMsg(lTrace, std::move(str), args...); }
	static bool AddMsg(const tstring &msg, LOG_LEVEL level = lInfo);
	static void InitThread();
	static void UninitThread();
	static void Interrupt();
	static bool Working() { return m_working; }
};

#endif //__CONSOLE_LOGGER