#pragma once
/*
如果只在debug模式（需要预定义宏 _DEBUG)下输出，使用 LOG->debug/trace（LOGD, LOGT)
使用info及以上的，将会在release下也输出
*/

#ifdef LOG_OUT	//是否启用保存日志
#include <spdlog/spdlog.h>
#include <memory>
#include <vector>
#include <string>
#ifdef _MSC_VER
#include <direct.h>
#include <io.h>
#else
#include <stdio.h>
#include <unistd.h>
#endif

namespace LabelMeWin {
	//配置区
	//是否输出到控件台
#define log2console true
	//是否输出到文件
#define log2file true
	//保存的日志文件名
#define logname "LabelMeWin.log"
	//日志保存的路径
#define logdir "./log"
	//每个日志文件最大为5M
#define max_buffer_size (5 * 1024 * 1024)
	//最多保存5个日志文件（存满后将会rotated的方式继续 存）
#define max_file_size (5)						

	class spdhelper
	{
	public:
		static spdhelper& instance()
		{
			static spdhelper logger{};
			return logger;
		}
		~spdhelper()
		{
			spdlog::apply_all([&](std::shared_ptr<spdlog::logger> l) { l->info("...log End"); });
			spdlog::drop_all();
		}

		//获得日志变量
		std::shared_ptr<spdlog::logger> operator->()
		{
			return spdlog::get(logname);
		}
		std::shared_ptr<spdlog::logger> getLogger()
		{
			return spdlog::get(logname);
		}

	protected:
		explicit spdhelper()
		{
			std::vector<spdlog::sink_ptr> sinks;
			std::string strname = logname;
			//默认输出到控件台
			if (log2console)
				sinks.push_back(std::make_shared<spdlog::sinks::stdout_sink_mt>());
			if (log2file)
			{
				std::string strdir = logdir;
#ifdef _MSC_VER
				if (0 != _access(logdir, 0))
#else
				if (0 != access(logdir, 0))
#endif
				{
#ifdef _MSC_VER
					_mkdir(logdir);
#else
					mkdir(logdir, 0755);
#endif
				}
				if (strdir[strdir.length() - 1] != '/')
					strdir = strdir + "/";
				strdir = strdir + strname;
				sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>(strdir.c_str(), max_buffer_size, max_file_size));
			}

			//创建实例
			spdlog::details::registry::instance().create(logname, sinks.begin(), sinks.end());
#ifdef _DEBUG
			spdlog::details::registry::instance().get(logname)->set_level(spdlog::level::trace);
#else
			spdlog::details::registry::instance().get(logname)->set_level(spdlog::level::trace);
#endif
			//设置格式
			/*
			[2018-05-08 15:16:43.342055] [tid] [info] this is info
			*/
			std::string pt = R"(%^[%Y-%m-%d %X.%f] [t:%t] [%l] %v %$)";
			spdlog::set_pattern(pt);
			spdlog::apply_all([&](std::shared_ptr<spdlog::logger> l) { l->info("log start..."); });
		}
	};
};
using LabelMeWin::spdhelper;
#define LOG (spdhelper::instance())
#define LOGT LOG->trace
#define LOGD LOG->debug
#define LOGI LOG->info
#define LOGW LOG->warn
#define LOGE LOG->error
#define LOGC LOG->critical
#define ENTER_FUNC (LOGD("enter function: {0}", __FUNCTION__))
#define LEAVE_FUNC (LOGD("leave function: {0}", __FUNCTION__))

#else
#define ENTER_FUNC /##/
#define LEAVE_FUNC /##/
#define LOG /##/
#define LOGT /##/
#define LOGI /##/
#define LOGD /##/
#define LOGW /##/
#define LOGE /##/
#define LOGC /##/
#endif
