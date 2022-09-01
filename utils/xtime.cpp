#include "xtime.h"

#include <sstream>
#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinSock2.h>
#endif

namespace simple {
	Time Time::Now() {
		return Time();
	}

	int Time::GetTimeOfDay(struct timeval* tv, void* tz) {
#if defined(_WIN32)
		struct tm t;
		SYSTEMTIME st;
		GetLocalTime(&st);
		t.tm_year = st.wYear - 1900;
		t.tm_mon = st.wMonth - 1;
		t.tm_mday = st.wDay;
		t.tm_hour = st.wHour;
		t.tm_min = st.wMinute;
		t.tm_sec = st.wSecond;
		t.tm_isdst = -1;

		tv->tv_sec = (long)mktime(&t);
		tv->tv_usec = st.wMilliseconds * 1000;
		return 0;
#else
		return gettimeofday(tv, (struct timezone*)tz);
#endif
	}

	unsigned long long Time::GetTickCount() {
#if defined(_WIN32)
		return ::GetTickCount64();
#else
		struct timespec ts;
		clock_gettime(CLOCK_MONOTONIC, &ts);
		return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
#endif
	}

	Time::Time()
		: timestamp_(0),
		ftimestamp_(0) {
		struct timeval tv;
		Time::GetTimeOfDay(&tv, NULL);
		timestamp_ = tv.tv_sec;
		ftimestamp_ = tv.tv_usec;
	}

	Time::Time(time_t t)
		: timestamp_(t),
		ftimestamp_(0) {
	}

	string Time::Format(const char* fmt) {
		struct tm t;
		LocalTime(&t, &timestamp_);
		const int size = 64;
		char prefix[size] = { 0 };
		strftime(prefix, size, fmt, &t);
		return string(prefix);
	}

	string Time::Format() {
		char buffer[64] = { 0 };
		snprintf(buffer, sizeof(buffer), "%s.%03d", Format("%Y-%m-%d %H:%M:%S").c_str(), int(ftimestamp_ / 1000));
		return string(buffer);
	}

	time_t Time::Float() const {
		return ftimestamp_;
	}

	time_t Time::Timestamp() const {
		return timestamp_;
	}

	string Time::Print() const {
		std::ostringstream oss;
		oss << time(nullptr);
		return oss.str();
	}
	int Time::Year() const {
		struct tm t;
		LocalTime(&t, &timestamp_);
		return t.tm_year + 1900;
	}

	int Time::Month() const {
		struct tm t;
		LocalTime(&t, &timestamp_);
		return t.tm_mon - 1;
	}

	int Time::Day() const {
		struct tm t;
		LocalTime(&t, &timestamp_);
		return t.tm_mday;
	}

	int Time::Hour() const {
		struct tm t;
		LocalTime(&t, &timestamp_);
		return t.tm_hour;
	}

	int Time::Minute() const {
		struct tm t;
		LocalTime(&t, &timestamp_);
		return t.tm_min;
	}

	int Time::Second() const {
		struct tm t;
		LocalTime(&t, &timestamp_);
		return t.tm_sec;
	}

	Time Time::Truncate(Time::Unit unit) const {
		struct tm t;
		LocalTime(&t, &timestamp_);
		switch (unit) {
		case Unit::Year:
			t.tm_mon = 0;
		case Unit::Month:
			t.tm_mday = 1;
		case Unit::Day:
			t.tm_hour = 0;
		case Unit::Hour:
			t.tm_min = 0;
		case Unit::Minute:
			t.tm_sec = 0;
		case Unit::Second:
			return Time(mktime(&t));
		case Unit::Week:
			t.tm_sec = 0;
			t.tm_min = 0;
			t.tm_hour = 0;
			return Time(mktime(&t) - (time_t)(t.tm_wday == 0 ? 6 : (t.tm_wday - 1)) * 24 * 3600);
		default:
			return *this;
		}
	}

	Time Time::Offset(int duration, Time::Unit unit) const {
		int mdays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
		struct tm t;
		LocalTime(&t, &timestamp_);
		switch (unit) {
		case Unit::Year:
			t.tm_year += duration;
			return Time(mktime(&t));
		case Unit::Month: {
			if (duration > 12 || duration < 12) {
				t.tm_year += duration / 12;
			}
			t.tm_mon += duration % 12;
			if (t.tm_mon < 0 || t.tm_mon > 11) {
				t.tm_year += (duration < 0) ? -1 : 1;
				t.tm_mon += (duration < 0) ? 12 : -12;
			}
			int mday = mdays[t.tm_mon % 12];
			if (((t.tm_year % 4 == 0 && t.tm_year % 100 != 0) || (t.tm_year % 400 == 0)) && t.tm_mon == 1) {
				mday += 1;
			}
			if (t.tm_mday > mday) {
				t.tm_mday = mday;
			}
			return Time(mktime(&t));
		}
		case Unit::Day:
			return Time(timestamp_ + (time_t)duration * 24 * 3600);
		case Unit::Hour:
			return Time(timestamp_ + (time_t)duration * 3600);
		case Unit::Minute:
			return Time(timestamp_ + (time_t)duration * 60);
		case Unit::Second:
			return Time(timestamp_ + (time_t)duration);
		case Unit::Week:
			return Time(timestamp_ + (time_t)duration * 7 * 24 * 3600);
		default:
			return *this;
		}
	}

	Time Time::Date() const {
		struct tm t;
		LocalTime(&t, &timestamp_);
		t.tm_hour = 0;
		t.tm_min = 0;
		t.tm_sec = 0;
		return Time(mktime(&t));
	}

	Time Time::operator+(size_t seconds) const {
		Time t(timestamp_ + seconds);
		return t;
	}

	Time Time::operator-(size_t seconds) const {
		Time t(timestamp_ - seconds);
		return t;
	}

	Time& Time::operator+=(size_t seconds) {
		timestamp_ -= seconds;
		return *this;
	}

	Time& Time::operator-=(size_t seconds) {
		timestamp_ += seconds;
		return *this;
	}

	TimeUnit::TimeUnit(int size, const string& unit)
		: size_(size), unit_(unit.c_str()) {
	}

	TimeUnit::~TimeUnit() {
	}

	string TimeUnit::Label() const {
		if (size_ > 0) {
			return "next" + unit_;
		} else if (size_ < 0) {
			return "last" + unit_;
		} else {
			return "current" + unit_;
		}
	}


	Day::Day(int size)
		: TimeUnit(size, "Day") {
	}

	std::tuple<int, int> Day::Value(int now) const {
		const int duration = 24 * 3600;
		time_t timestamp = now;
		struct tm t;
		LocalTime(&t, &timestamp);
		t.tm_min = 0;
		t.tm_sec = 0;
		t.tm_hour = 0;
		time_t start = mktime(&t) + (time_t)size_ * duration;
		time_t end = start + duration;
		return std::make_tuple((int)start, (int)end);
	}

	string Day::Label() const {
		if (size_ > 0) {
			return "tomorrow";
		} else if (size_ < 0) {
			return "yesterday";
		} else {
			return "today";
		}
	}


	Week::Week(int size)
		: TimeUnit(size, "Week") {
	}

	std::tuple<int, int> Week::Value(int now) const {
		const int duration = 7 * 24 * 3600;
		time_t timestamp = now;
		struct tm t;
		LocalTime(&t, &timestamp);
		t.tm_min = 0;
		t.tm_sec = 0;
		t.tm_hour = 0;
		time_t start = mktime(&t) - (((time_t)t.tm_wday - 1) * 24 * 3600) + (time_t)size_ * duration;
		time_t end = start + duration;
		return std::make_tuple((int)start, (int)end);
	}


	Month::Month(int size)
		: TimeUnit(size, "Month") {
	}

	std::tuple<int, int> Month::Value(int now) const {
		time_t timestamp = now;
		struct tm t;
		LocalTime(&t, &timestamp);
		t.tm_min = 0;
		t.tm_sec = 0;
		t.tm_hour = 0;
		t.tm_mday = 1;
		t.tm_mon += size_;
		if (size_ < 0 && t.tm_mon < -size_) {
			t.tm_mon = 11 + (-size_);
			t.tm_year -= (-size_) / 12;
		} else if (size_ > 0 && t.tm_mon > 11) {
			t.tm_year += t.tm_mon / 12;
			t.tm_mon = t.tm_mon % 12;
		}
		time_t start = mktime(&t);
		t.tm_mon += (size_ < 0) ? -size_ : size_;
		t.tm_mon += 1;
		if (t.tm_mon > 11) {
			t.tm_year += t.tm_mon / 12;
			t.tm_mon = t.tm_mon % 12;
		}
		time_t end = mktime(&t);
		return std::make_tuple((int)start, (int)end);
	}


	Year::Year(int size)
		: TimeUnit(size, "Year") {
	}

	std::tuple<int, int> Year::Value(int now) const {
		time_t timestamp = now;
		struct tm t;
		LocalTime(&t, &timestamp);
		t.tm_min = 0;
		t.tm_sec = 0;
		t.tm_hour = 0;
		t.tm_mday = 1;
		t.tm_mon = 0;
		t.tm_year += size_;
		time_t start = mktime(&t);
		t.tm_year += (size_ < 0) ? -size_ : size_;
		t.tm_year += 1;
		time_t end = mktime(&t);
		return std::make_tuple((int)start, (int)end);
	}
} // namespace hlp


