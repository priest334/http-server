#pragma once
#ifndef UTILS_XTIME_H_
#define UTILS_XTIME_H_

#include <ctime>
#if defined(_WIN32)
#define LocalTime(t, r) localtime_s(t, r)
#else // OS_WIN
#include <sys/time.h>
#	define LocalTime(t, r) localtime_r(r, t)
#endif
#include <string>
#include <tuple>

struct timeval;
using std::string;
using std::tuple;

namespace simple {		
	class Time {
	public:
		enum class Unit {
			Second = 0,
			Minute,
			Hour,
			Day,
			Week,
			Month,
			Year
		};
		static Time Now();
		static int GetTimeOfDay(struct timeval* tv, void* tz);
		static unsigned long long GetTickCount();
		Time();
		Time(time_t t);
		string Format(const char* fmt);
		string Format();
		time_t Float() const;
		time_t Timestamp() const;
		string Print() const;
		int Year() const;
		int Month() const;
		int Day() const;
		int Hour() const;
		int Minute() const;
		int Second() const;
		Time Truncate(Time::Unit unit) const;
		Time Offset(int duration, Time::Unit unit) const;
		Time Date() const;
		Time operator+(size_t seconds) const;
		Time operator-(size_t seconds) const;
		Time& operator+=(size_t seconds);
		Time& operator-=(size_t seconds);
	private:
		time_t timestamp_;
		time_t ftimestamp_;
	};

	class TimeUnit {
	public:
		TimeUnit(int size, const string& unit);
		virtual ~TimeUnit();
		virtual std::tuple<int, int> Value(int now) const = 0;
		virtual string Label() const;
	protected:
		int size_;
		string unit_;
	};

	class Day : public TimeUnit {
	public:
		Day(int size);
		std::tuple<int, int> Value(int now) const override;
		string Label() const override;
	};

	class Week : public TimeUnit {
	public:
		Week(int size);
		std::tuple<int, int> Value(int now) const override;
	};

	class Month : public TimeUnit {
	public:
		Month(int size);
		std::tuple<int, int> Value(int now) const override;
	};

	class Year : public TimeUnit {
	public:
		Year(int size);
		std::tuple<int, int> Value(int now) const override;
	};
} // namespace simple

#endif // UTILS_XTIME_H_

