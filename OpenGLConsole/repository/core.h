#pragma once
#include "Logger/ConsoleLogger.h"
#include "Logger/FileLogger.h"

namespace core
{
	const static tstring  LOG_PATH = _T("");
	extern FileLogger *logger;
	extern bool close;
	const inline void Init() {
		ConsoleLogger::InitThread();
		core::logger = new FileLogger(_T("app"), _T(".//logs//"));
		core::logger->InitThread();
	}
	const inline int Exit(int code = 0) {
		core::close = true;
		ConsoleLogger::UninitThread();
		logger->UninitThread();
		while (ConsoleLogger::Working() || logger->Working());
		delete logger; 
		return code;
	}
	const inline void Deinit() { Exit(); }
	const inline void AddMsg(tstring msg, LOG_LEVEL level = lInfo) {
		ConsoleLogger::AddMsg(std::move(msg), level);
		logger->AddMsg(std::move(msg), level);
	}
	const inline void AddError(tstring msg) { AddMsg(std::move(msg), lError); }
	const inline void AddInfo(tstring msg) { AddMsg(std::move(msg), lInfo); }
	const inline void AddWarning(tstring msg) { AddMsg(std::move(msg), lWarning); }
	const inline void AddDebug(tstring msg) { AddMsg(std::move(msg), lDebug); }
	const inline void AddTrace(tstring msg) { AddMsg(std::move(msg), lTrace); }
	
	template <typename... Args> static bool AddMsg(LOG_LEVEL level, tstring str, Args... args) {
		return ConsoleLogger::AddMsg(level, str, args...) || logger->AddMsg(level, str, args...);
	}
	template <typename... Args> static bool AddError(tstring str, Args... args) { return AddMsg(lError, std::move(str), args...); }
	template <typename... Args> static bool AddInfo(tstring str, Args... args) { return AddMsg(lInfo, std::move(str), args...); }
	template <typename... Args> static bool AddWarning(tstring str, Args... args) { return AddMsg(lWarning, std::move(str), args...); }
	template <typename... Args> static bool AddDebug(tstring str, Args... args) { return AddMsg(lDebug, std::move(str), args...); }
	template <typename... Args> static bool AddTrace(tstring str, Args... args) { return AddMsg(lTrace, std::move(str), args...); }
};