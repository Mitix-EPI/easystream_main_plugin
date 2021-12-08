/*
** Easystream EIP, 2021
** easystream_main_plugin
** File description:
** eventsMediaInputs
*/

#include "../sourceTracker.hpp"

void es::obs::SourceTracker::sourceMediaPauseMultiHandler(void *param, calldata_t *data)
{
	obs_source_t *source = GetCalldataPointer<obs_source_t>(data, "source");
	if (!source)
		return;

	if (obs_source_get_type(source) != OBS_SOURCE_TYPE_INPUT)
		return;
    blog(LOG_INFO, "sourceMediaPauseMultiHandler");
}

void es::obs::SourceTracker::sourceMediaPlayMultiHandler(void *param, calldata_t *data)
{
	obs_source_t *source = GetCalldataPointer<obs_source_t>(data, "source");
	if (!source)
		return;

	if (obs_source_get_type(source) != OBS_SOURCE_TYPE_INPUT)
		return;
    blog(LOG_INFO, "sourceMediaPlayMultiHandler");
}

void es::obs::SourceTracker::sourceMediaRestartMultiHandler(void *param, calldata_t *data)
{
	obs_source_t *source = GetCalldataPointer<obs_source_t>(data, "source");
	if (!source)
		return;

	if (obs_source_get_type(source) != OBS_SOURCE_TYPE_INPUT)
		return;
    blog(LOG_INFO, "sourceMediaRestartMultiHandler");
}

void es::obs::SourceTracker::sourceMediaStopMultiHandler(void *param, calldata_t *data)
{
	obs_source_t *source = GetCalldataPointer<obs_source_t>(data, "source");
	if (!source)
		return;

	if (obs_source_get_type(source) != OBS_SOURCE_TYPE_INPUT)
		return;
    blog(LOG_INFO, "sourceMediaStopMultiHandler");
}

void es::obs::SourceTracker::sourceMediaNextMultiHandler(void *param, calldata_t *data)
{
	obs_source_t *source = GetCalldataPointer<obs_source_t>(data, "source");
	if (!source)
		return;

	if (obs_source_get_type(source) != OBS_SOURCE_TYPE_INPUT)
		return;
    blog(LOG_INFO, "sourceMediaNextMultiHandler");
}

void es::obs::SourceTracker::sourceMediaPreviousMultiHandler(void *param, calldata_t *data)
{
	obs_source_t *source = GetCalldataPointer<obs_source_t>(data, "source");
	if (!source)
		return;

	if (obs_source_get_type(source) != OBS_SOURCE_TYPE_INPUT)
		return;
    blog(LOG_INFO, "sourceMediaPreviousMultiHandler");
}

void es::obs::SourceTracker::handleMediaInputPlaybackStarted(void *param, calldata_t *data)
{
	obs_source_t *source = GetCalldataPointer<obs_source_t>(data, "source");
	if (!source)
		return;

	if (obs_source_get_type(source) != OBS_SOURCE_TYPE_INPUT)
		return;

	std::string name(obs_source_get_name(source));
    blog(LOG_INFO, "name:: %s", name.c_str());
}

void es::obs::SourceTracker::handleMediaInputPlaybackEnded(void *param, calldata_t *data)
{
	obs_source_t *source = GetCalldataPointer<obs_source_t>(data, "source");
	if (!source)
		return;

	if (obs_source_get_type(source) != OBS_SOURCE_TYPE_INPUT)
		return;

	std::string name(obs_source_get_name(source));
    blog(LOG_INFO, "name:: %s", name.c_str());
}