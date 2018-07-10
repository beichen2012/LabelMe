#pragma once
/*
���ֻ��debugģʽ����ҪԤ����� _DEBUG)�������ʹ�� LOG->debug/trace��LOGD, LOGT)
ʹ��info�����ϵģ�������release��Ҳ���
*/

#ifdef LOG_OUT	//�Ƿ����ñ�����־
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
	//������
	//�Ƿ�������ؼ�̨
#define log2console true
	//�Ƿ�������ļ�
#define log2file true
	//�������־�ļ���
#define logname "LabelMeWin.log"
	//��־�����·��
#define logdir "./log"
	//ÿ����־�ļ����Ϊ5M
#define max_buffer_size (5 * 1024 * 1024)
	//��ౣ��5����־�ļ��������󽫻�rotated�ķ�ʽ���� �棩
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

		//�����־����
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
			//Ĭ��������ؼ�̨
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

			//����ʵ��
			spdlog::details::registry::instance().create(logname, sinks.begin(), sinks.end());
#ifdef _DEBUG
			spdlog::details::registry::instance().get(logname)->set_level(spdlog::level::trace);
#else
			spdlog::details::registry::instance().get(logname)->set_level(spdlog::level::trace);
#endif
			//���ø�ʽ
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
