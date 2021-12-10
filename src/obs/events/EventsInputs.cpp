/*
** easyStream EIP, 2021
** easystream_main_plugin
** File description:
** eventInputs
*/

#include "../SourceTracker.hpp"
#include "../autoAudioLeveler/AutoAudioLeveler.hpp"

void es::obs::SourceTracker::handleInputCreated(obs_source_t *source)
{
	std::string inputKind = obs_source_get_id(source);
	OBSDataAutoRelease inputSettings = obs_source_get_settings(source);
	OBSDataAutoRelease defaultInputSettings = obs_get_source_defaults(inputKind.c_str());

	// if (!filterAudioSources("audio_input", source)) {
	// 	AutoAudioLeveler AutoAudioLeveler(source);
	// 	blog(LOG_INFO, "Instancing Audio Leveler for %s", obs_source_get_name(source));
	// }
    blog(LOG_INFO, "[SourceTracker::handleInputCreated]: %s, parent: %s", inputKind.c_str(), obs_source_get_name(source));
}

void es::obs::SourceTracker::handleInputRemoved(obs_source_t *source)
{
	std::string name(obs_source_get_name(source));
    blog(LOG_INFO, "[SourceTracker::handleInputRemoved]: %s ", name.c_str());
}

void es::obs::SourceTracker::handleInputNameChanged(obs_source_t *source, std::string oldInputName, std::string inputName)
{
    blog(LOG_INFO, "[SourceTracker::handleInputNameChanged]:");
	std::vector<json> j = es::utils::obs::listHelper::GetSceneList();
	OBSSourceAutoRelease scene = obs_get_source_by_name(j[rand() % j.size()]["sceneName"].get<std::string>().c_str());
	obs_frontend_set_current_scene(scene);
}

void es::obs::SourceTracker::handleInputActiveStateChanged(void *param, calldata_t *data)
{
	auto eventHandler = static_cast<es::obs::SourceTracker *>(param);

	obs_source_t *source = GetCalldataPointer<obs_source_t>(data, "source");
	if (!source)
		return;

	if (obs_source_get_type(source) != OBS_SOURCE_TYPE_INPUT)
		return;

	std::string name(obs_source_get_name(source));
	bool active = obs_source_active(source);
    blog(LOG_INFO, "[SourceTracker::handleInputActiveStateChanged]: %s", name.c_str());
}

void es::obs::SourceTracker::handleInputShowStateChanged(void *param, calldata_t *data)
{
	auto eventHandler = static_cast<es::obs::SourceTracker *>(param);

	obs_source_t *source = GetCalldataPointer<obs_source_t>(data, "source");
	if (!source)
		return;

	if (obs_source_get_type(source) != OBS_SOURCE_TYPE_INPUT)
		return;

	std::string name(obs_source_get_name(source));
	bool showing = obs_source_showing(source);
    blog(LOG_INFO, "[SourceTracker::handleInputShowStateChanged]: %s", name.c_str());
}

void es::obs::SourceTracker::handleInputMuteStateChanged(void *param, calldata_t *data)
{
	auto eventHandler = static_cast<es::obs::SourceTracker *>(param);

	obs_source_t *source = GetCalldataPointer<obs_source_t>(data, "source");
	if (!source)
		return;

	if (obs_source_get_type(source) != OBS_SOURCE_TYPE_INPUT)
		return;

	std::string name = obs_source_get_name(source);
	bool muted = obs_source_muted(source);
    blog(LOG_INFO, "[SourceTracker::handleInputMuteStateChanged]: %s", name.c_str());
}

void es::obs::SourceTracker::handleInputVolumeChanged(void *param, calldata_t *data)
{
	auto eventHandler = static_cast<es::obs::SourceTracker *>(param);

	obs_source_t *source = GetCalldataPointer<obs_source_t>(data, "source");
	if (!source)
		return;

	if (obs_source_get_type(source) != OBS_SOURCE_TYPE_INPUT)
		return;

	// Volume must be grabbed from the calldata. Running obs_source_get_volume() will return the previous value.
	double inputVolumeMul = calldata_float(data, "volume");

	double inputVolumeDb = obs_mul_to_db(inputVolumeMul);
	if (inputVolumeDb == -INFINITY)
		inputVolumeDb = -100;

	std::string name(obs_source_get_name(source));
    // blog(LOG_INFO, "[SourceTracker::handleInputVolumeChanged]: %s ", name.c_str());
}

void es::obs::SourceTracker::handleInputAudioSyncOffsetChanged(void *param, calldata_t *data)
{
	auto eventHandler = static_cast<es::obs::SourceTracker *>(param);

	obs_source_t *source = GetCalldataPointer<obs_source_t>(data, "source");
	if (!source)
		return;

	if (obs_source_get_type(source) != OBS_SOURCE_TYPE_INPUT)
		return;

	long long inputAudioSyncOffset = calldata_int(data, "offset");

	std::string name(obs_source_get_name(source));
	long offset = inputAudioSyncOffset / 1000000;
    blog(LOG_INFO, "[SourceTracker::handleInputAudioSyncOffsetChanged]: %s ", name.c_str());
}

void es::obs::SourceTracker::handleInputAudioTracksChanged(void *param, calldata_t *data)
{
	auto eventHandler = static_cast<es::obs::SourceTracker *>(param);

	obs_source_t *source = GetCalldataPointer<obs_source_t>(data, "source");
	if (!source)
		return;

	if (obs_source_get_type(source) != OBS_SOURCE_TYPE_INPUT)
		return;

	std::string name(obs_source_get_name(source));
    blog(LOG_INFO, "[SourceTracker::handleInputAudioTracksChanged]: %s ", name.c_str());
}

void es::obs::SourceTracker::handleInputAudioMonitorTypeChanged(void *param, calldata_t *data)
{
	auto eventHandler = static_cast<es::obs::SourceTracker *>(param);

	obs_source_t *source = GetCalldataPointer<obs_source_t>(data, "source");
	if (!source)
		return;

	if (obs_source_get_type(source) != OBS_SOURCE_TYPE_INPUT)
		return;

	std::string name(obs_source_get_name(source));
    blog(LOG_INFO, "[SourceTracker::handleInputAudioMonitorTypeChanged]: %s ", name.c_str());
}