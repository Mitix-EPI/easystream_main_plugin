#include "plugin-main.hpp"
#include "obs/speechRecognition/SpeechRecognition.hpp"

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE(PLUGIN_NAME, "en-US")

std::shared_ptr<es::obs::SourceTracker> tracker = std::make_shared<es::obs::SourceTracker>();
std::shared_ptr<es::thread::ThreadPool> threadPool = std::make_shared<es::thread::ThreadPool>(10);
os_cpu_usage_info_t *cpuUsageInfo;

void test(std::shared_ptr<void>)
{
	blog(LOG_INFO, "[Thread::ThreadPool]: Thread start");
	std::this_thread::sleep_for(std::chrono::seconds(2));
	std::vector<json> j = es::utils::obs::listHelper::GetMicsList();
	obs_source_t *source = obs_get_source_by_name(j[0]["micName"].get<std::string>().c_str());
	if (source)
	{
		es::obs::SpeechRecognition r(source);
		while (1);
	}
	blog(LOG_INFO, "[Thread::ThreadPool]: Thread finish");
}

bool obs_module_load(void)
{
	blog(LOG_INFO, "plugin loaded successfully (version %s)", PLUGIN_VERSION);

	blog(LOG_INFO, "-----------------------------------------");
	tracker->init();
	threadPool->push(std::function(test), nullptr);
	cpuUsageInfo = os_cpu_usage_info_start();
	blog(LOG_INFO, "-----------------------------------------");
	return true;
}

void obs_module_unload()
{
	tracker.reset();
	threadPool.reset();
	os_cpu_usage_info_destroy(cpuUsageInfo);

	blog(LOG_INFO, "plugin unloaded");
}

os_cpu_usage_info_t* GetCpuUsageInfo()
{
	return cpuUsageInfo;
}