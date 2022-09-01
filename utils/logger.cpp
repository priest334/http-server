#include "logger.h"

#include <ctime>
#include <cstdlib>
#include <iostream>
#include <thread>
#include "xtime.h"


namespace simple {
	const char* const SeverityNames[(unsigned int)Severity::MaxValue] = { "Debug", "Info", "Warning", "Error", "Fatal" };
	inline unsigned int SeverityIndex(Severity severity) {
		return (unsigned int)severity % (unsigned int)Severity::MaxValue;
	}

	string SeverityName(Severity severity) {
		return SeverityNames[SeverityIndex(severity)];
	}

	LogWriter::~LogWriter() {
	}

	Console::Console() {
	}

	void Console::Write(const string& message) {
		std::lock_guard<std::mutex> lock(mutex_);
		std::cout << message << std::endl;
	}

	SingleFile::SingleFile(const string& filename) {
		ofs_.open(filename.c_str(), std::ios_base::app);
	}

	void SingleFile::Write(const string& message) {
		std::lock_guard<std::mutex> lock(mutex_);
		ofs_ << message << std::endl;
		ofs_.flush();
	}

	Message::Message(Severity severity, shared_ptr<Logger> logger)
		: severity_(severity), logger_(logger) {
		(*this) << "[" << Time::Now().Format() << "]-" << SeverityName(severity) << " ";
	}

	Message::~Message() {
		if (logger_) {
			logger_->Write(severity_, *this);
		}
	}

	Message& Message::Format(const char* fmt, ...) {
		const int max_buffer_size = 1024;
		char buffer[max_buffer_size + 32] = { 0 };
		va_list args;
		va_start(args, fmt);
		int wr = vsnprintf(buffer, max_buffer_size, fmt, args);
		if (wr > 0) {
			if (wr > 1024) {
				*(buffer + max_buffer_size) = '\0';
				int more = wr - max_buffer_size;
				(*this) << buffer << "..." << more << "more bytes";
			}
			else {
				(*this) << buffer;
			}
		}
		va_end(args);
		return *this;
	}

	Logger::Logger(Severity severity/* = Severity::Debug*/)
		: severity_(severity), writer_(std::make_shared<Console>()) {
		methods_[(unsigned int)Severity::Debug] = &Logger::Debug;
		methods_[(unsigned int)Severity::Info] = &Logger::Info;
		methods_[(unsigned int)Severity::Warning] = &Logger::Warning;
		methods_[(unsigned int)Severity::Error] = &Logger::Error;
		methods_[(unsigned int)Severity::Fatal] = &Logger::Fatal;
	}

	Logger::~Logger() {
	}

	Logger& Logger::SetLevel(Severity severity) {
		severity_ = severity;
		return *this;
	}

	Logger& Logger::SetWriter(shared_ptr<LogWriter> writer) {
		writer_ = writer;
		return *this;
	}

	Logger& Logger::Debug(const Message& message) {
		if (Severity::Debug >= severity_) {
			writer_->Write(message.str());
		}
		return *this;
	}

	Logger& Logger::Info(const Message& message) {
		if (Severity::Info >= severity_) {
			writer_->Write(message.str());
		}
		return *this;
	}

	Logger& Logger::Warning(const Message& message) {
		if (Severity::Warning >= severity_) {
			writer_->Write(message.str());
		}
		return *this;
	}

	Logger& Logger::Error(const Message& message) {
		if (Severity::Error >= severity_) {
			writer_->Write(message.str());
		}
		return *this;
	}

	Logger& Logger::Fatal(const Message& message) {
		if (Severity::Fatal >= severity_) {
			writer_->Write(message.str());
		}
		return *this;
	}

	Logger& Logger::Write(Severity severity, const Message& message) {
		if (writer_) {
			(this->*methods_[SeverityIndex(severity)])(message);
		}
		return *this;
	}

	namespace logger {
		std::once_flag logger_initliazer_flag;
		shared_ptr<Logger> default_logger = nullptr;

		shared_ptr<Logger> GetDefault() {
			std::call_once(logger_initliazer_flag, []() {
				default_logger = std::make_shared<Logger>();
			});
			return default_logger;
		}

		void SetWriter(shared_ptr<LogWriter> writer) {
			GetDefault()->SetWriter(writer);
		}

		Debug::Debug() : Message(Severity::Debug, GetDefault()) {}
		Debug::Debug(shared_ptr<Logger> logger) : Message(Severity::Debug, logger) {}
		Info::Info() : Message(Severity::Info, GetDefault()) {}
		Info::Info(shared_ptr<Logger> logger) : Message(Severity::Info, logger) {}
		Warning::Warning() : Message(Severity::Warning, GetDefault()) {}
		Warning::Warning(shared_ptr<Logger> logger) : Message(Severity::Warning, logger) {}
		Error::Error() : Message(Severity::Error, GetDefault()) {}
		Error::Error(shared_ptr<Logger> logger) : Message(Severity::Error, logger) {}
		Fatal::Fatal() : Message(Severity::Fatal, GetDefault()) {}
		Fatal::Fatal(shared_ptr<Logger> logger) : Message(Severity::Fatal, logger) {}
	} // namespace logger
} // namespace simple


