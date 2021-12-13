/*
obs-text-slideshow
Copyright (C) 2021 Joshua Wong jbwong05@gmail.com

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program. If not, see <https://www.gnu.org/licenses/>
*/

#include "plugin-main.hpp"
#include <Python.h>

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE(PLUGIN_NAME, "en-US")

std::shared_ptr<es::obs::SourceTracker> tracker = std::make_shared<es::obs::SourceTracker>();
std::shared_ptr<es::thread::ThreadPool> threadPool = std::make_shared<es::thread::ThreadPool>(10);
os_cpu_usage_info_t *cpuUsageInfo;

void test(std::shared_ptr<void>)
{
	blog(LOG_INFO, "[Thread::ThreadPool]: Thread start");
	std::this_thread::sleep_for(std::chrono::seconds(2));
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