#pragma once

#ifndef __BASE_LOGGER
#define __BASE_LOGGER

#include <memory>
#include <mutex>
#include <queue>
#include "../tstring.h"

enum LOG_LEVEL {
	lError = 1,
	lInfo = 2,
	lWarning = 4,
	lDebug = 8,
	lTrace = 16,
	lFull = 32
};
struct LogMsg
{
	LOG_LEVEL level;
	std::time_t time;
	tstring text;
	LogMsg() { };
	LogMsg(const tstring &text, LOG_LEVEL level) : level(level), text(text) {
		std::time(&this->time);
	}

	template<int index, typename... Args>
	struct iterator_tuple {
		static void next(tstring& str, std::tuple<Args...> &t) {
			iterator_tuple<index - 1, Args...>::next(str, t);
			size_t pos = str.find(_T("{") + to_tstring(index) + _T("}"));
			if (pos != tstring::npos) {
				tostringstream oss;
				oss << std::get<index>(t);
				str.replace(pos, 3, oss.str());
			}
		}
	};
	template<typename... Args>
	struct iterator_tuple<0, Args...> {
		static void next(tstring &str, std::tuple<Args...> &t) {
			size_t pos = str.find(_T("{0}"));
			if (pos != tstring::npos) {
				tostringstream oss;
				oss << std::get<0>(t);
				str.replace(pos, 3, oss.str());
			}
		}
	};
	template<typename... Args>
	struct iterator_tuple<-1, Args...> {
		static void next(tstring &str, std::tuple<Args...> &t) { }
	};
	template<typename... Args>
	void Create(tstring &str, std::tuple<Args...> &t, LOG_LEVEL level) {
		this->level = level;
		std::time(&this->time);
		int const t_size = std::tuple_size<std::tuple<Args...>>::value;
		iterator_tuple<t_size - 1, Args...>::next(str, t);
		this->text = str;
	}; 
};

#endif //__BASE_LOGGER