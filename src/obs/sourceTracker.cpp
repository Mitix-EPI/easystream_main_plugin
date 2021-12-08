/*
** EasyStream EIP, 2021
** easystream_main_plugin
** File description:
** sourceTracker
*/

#include "sourceTracker.hpp"

void es::obs::SourceTracker::init()
{
    blog(LOG_INFO, "SourceTracker::SourceTracker()");
    blog(LOG_INFO, "[SourceTracker::SourceTracker] Setting up...");

    obs_frontend_add_event_callback(onFrontendEvent, this);

    signal_handler_t *coreHandler = obs_get_signal_handler();
    if (coreHandler) {
	    signal_handler_connect(coreHandler, "source_create", createHandler, this);
	    signal_handler_connect(coreHandler, "source_destroy", destroyHandler, this);
	    signal_handler_connect(coreHandler, "source_rename", renameHandler, this);
    } else {
        blog(LOG_ERROR,  "[SourceTracker::SourceTracker] Unable to get libobs signal handler!");
    }

    blog(LOG_INFO, "[SourceTracker::SourceTracker] Finished.");
}

es::obs::SourceTracker::SourceTracker(): _obsLoaded(false)
{
}

es::obs::SourceTracker::~SourceTracker()
{
    blog(LOG_INFO, "[SourceTracker::~SourceTracker] Shutting down...");

    obs_frontend_remove_event_callback(onFrontendEvent, this);

    signal_handler_t *coreHandler = obs_get_signal_handler();
	if (coreHandler) {
		signal_handler_disconnect(coreHandler, "source_create", createHandler, this);
	    signal_handler_disconnect(coreHandler, "source_destroy", destroyHandler, this);
	    signal_handler_disconnect(coreHandler, "source_rename", renameHandler, this);
	} else {
        blog(LOG_ERROR,  "[SourceTracker::~SourceTracker] Unable to get libobs signal handler!");
    }

	_sources.clear();
    blog(LOG_INFO, "[SourceTracker::~SourceTracker] Finished.");
}

void es::obs::SourceTracker::onFrontendEvent(enum obs_frontend_event event, void *privateData)
{
    es::obs::SourceTracker *self = static_cast<es::obs::SourceTracker*>(privateData);

    if (!self->_obsLoaded) {
		if (event == OBS_FRONTEND_EVENT_FINISHED_LOADING) {
			blog(LOG_INFO, "[EventHandler::OnFrontendEvent] OBS has finished loading. Connecting final handlers and enabling events...");
			// Connect source signals and enable events only after OBS has fully loaded (to reduce extra logging).
			self->_obsLoaded = true;

			// In the case that plugins become hotloadable, this will have to go back into `EventHandler::EventHandler()`
			// Enumerate inputs and connect each one
			obs_enum_sources([](void* param, obs_source_t* source) {
				es::obs::SourceTracker *sourceTracker = static_cast<es::obs::SourceTracker *>(param);
				sourceTracker->connectSourceSignals(source);
				return true;
			}, privateData);

			// Enumerate scenes and connect each one
			obs_enum_scenes([](void* param, obs_source_t* source) {
				es::obs::SourceTracker *sourceTracker = static_cast<es::obs::SourceTracker *>(param);
				sourceTracker->connectSourceSignals(source);
				return true;
			}, privateData);

			blog(LOG_INFO, "[EventHandler::OnFrontendEvent] Finished.");
		} else {
			return;
		}
	}

    switch (event) {
		case OBS_FRONTEND_EVENT_EXIT:
        {
			blog(LOG_INFO, "[SourceTracker::OnFrontendEvent] OBS is unloading. Disabling events...");
			// Disconnect source signals and disable events when OBS starts unloading (to reduce extra logging).
			self->_obsLoaded = false;

			// In the case that plugins become hotloadable, this will have to go back into `EventHandler::~EventHandler()`
			// Enumerate inputs and disconnect each one
			obs_enum_sources([](void* param, obs_source_t* source) {
				es::obs::SourceTracker *sourceTracker = static_cast<es::obs::SourceTracker *>(param);
				sourceTracker->disconnectSourceSignals(source);
				return true;
			}, privateData);

			// Enumerate scenes and disconnect each one
			obs_enum_scenes([](void* param, obs_source_t* source) {
				es::obs::SourceTracker *sourceTracker = static_cast<es::obs::SourceTracker *>(param);
				sourceTracker->disconnectSourceSignals(source);
				return true;
			}, privateData);

			blog(LOG_INFO, "[SourceTracker::OnFrontendEvent] Finished.");

			break;
        }
        default:
            break;
    }

}

void es::obs::SourceTracker::createHandler(void* ptr, calldata_t* data)
try {
	es::obs::SourceTracker *self = static_cast<es::obs::SourceTracker*>(ptr);

    if (!self->_obsLoaded) {
        return;
    }
	obs_source_t *target = GetCalldataPointer<obs_source_t>(data, "source");

	if (!target || !self) {
		return;
	}

	const char *name = obs_source_get_name(target);
	if (!name) {
		return;
	}

	obs_weak_source_t *weak = obs_source_get_weak_source(target);
	if (!weak) {
		return;
	}

    self->connectSourceSignals(target);

    switch (obs_source_get_type(target)) {
		case OBS_SOURCE_TYPE_INPUT:
			self->handleInputCreated(target);
			break;
		case OBS_SOURCE_TYPE_FILTER:
			break;
		case OBS_SOURCE_TYPE_TRANSITION:
			break;
		case OBS_SOURCE_TYPE_SCENE:
			self->handleSceneCreated(target);
			break;
		default:
			break;
	}

	std::unique_lock<std::mutex> ul(self->_lock);
	self->_sources.insert({std::string(name), {weak, obs_weak_source_release}});
} catch (...) {
	blog(LOG_ERROR,  "Unexpected exception in function '%s'.", __FUNCTION_NAME__);
}

void es::obs::SourceTracker::destroyHandler(void* ptr, calldata_t* data)
try {
	es::obs::SourceTracker *self = static_cast<es::obs::SourceTracker*>(ptr);

	obs_source_t *target = GetCalldataPointer<obs_source_t>(data, "source");

	if (!target || !self) {
		return;
	}

    self->disconnectSourceSignals(target);

    if (!self->_obsLoaded)
		return;
    
    switch (obs_source_get_type(target)) {
		case OBS_SOURCE_TYPE_INPUT:
			self->handleInputRemoved(target);
			break;
		case OBS_SOURCE_TYPE_FILTER:
			break;
		case OBS_SOURCE_TYPE_TRANSITION:
			break;
		case OBS_SOURCE_TYPE_SCENE:
			break;
		default:
			break;
	}

	const char *name = obs_source_get_name(target);
	if (!name) {
		return;
	}

	std::unique_lock<std::mutex> ul(self->_lock);
	auto found = self->_sources.find(std::string(name));
	if (found == self->_sources.end()) {
		return;
	}
	self->_sources.erase(found);
} catch (...) {
	blog(LOG_ERROR,  "Unexpected exception in function '%s'.", __FUNCTION_NAME__);
}

void es::obs::SourceTracker::renameHandler(void* ptr, calldata_t* data)
try {
	es::obs::SourceTracker* self = static_cast<es::obs::SourceTracker *>(ptr);

    if (!self->_obsLoaded) {
        return;
    }

	obs_source_t *target = GetCalldataPointer<obs_source_t>(data, "source");
    if (!self)
        return;

	std::string prev_name = calldata_string(data, "prev_name");
	std::string new_name = calldata_string(data, "new_name");
	if (prev_name.empty() || new_name.empty()) {
		return;
	}

    switch (obs_source_get_type(target)) {
		case OBS_SOURCE_TYPE_INPUT:
			self->handleInputNameChanged(target, prev_name, new_name);
			break;
		case OBS_SOURCE_TYPE_FILTER:
			break;
		case OBS_SOURCE_TYPE_TRANSITION:
			break;
		case OBS_SOURCE_TYPE_SCENE:
			self->handleSceneNameChanged(target, prev_name, new_name);
			break;
		default:
			break;
	}

	std::unique_lock<std::mutex> ul(self->_lock);
	auto found = self->_sources.find(std::string(prev_name));
	if (found == self->_sources.end()) {
		obs_weak_source_t* weak = obs_source_get_weak_source(target);
		if (!weak) {
			return;
		}
		self->_sources.insert({new_name, {weak, obs_weak_source_release}});
		return;
	}

	self->_sources.insert({new_name, found->second});
	self->_sources.erase(found);
} catch (...) {
	blog(LOG_ERROR,  "Unexpected exception in function '%s'.", __FUNCTION_NAME__);
}

void es::obs::SourceTracker::enumerate(enumerate_cb_t ecb, filter_cb_t fcb)
{
	decltype(_sources) _clone;
	{
		std::unique_lock<std::mutex> ul(_lock);
		_clone = _sources;
	}

	//for (auto kv : _clone) {
	//	auto source = std::shared_ptr<obs_source_t>(obs_weak_source_get_source(kv.second.get()), obs_weak_source_release);
	//	if (!source) {
	//		continue;
	//	}
//
	//	if (fcb) {
	//		if (fcb(kv.first, source.get())) {
	//			continue;
	//		}
	//	}
//
	//	if (ecb) {
	//		if (ecb(kv.first, source.get())) {
	//			break;
	//		}
	//	}
	//}
}

void es::obs::SourceTracker::connectSourceSignals(obs_source_t *source)
{
	if (!source || obs_source_removed(source))
		return;

	// Disconnect all existing signals from the source to prevent multiple connections
	disconnectSourceSignals(source);

	signal_handler_t* sh = obs_source_get_signal_handler(source);

	obs_source_type sourceType = obs_source_get_type(source);

	// Inputs
	signal_handler_connect(sh, "activate", handleInputActiveStateChanged, this);
	signal_handler_connect(sh, "deactivate", handleInputActiveStateChanged, this);
	signal_handler_connect(sh, "show", handleInputShowStateChanged, this);
	signal_handler_connect(sh, "hide", handleInputShowStateChanged, this);
	signal_handler_connect(sh, "mute", handleInputMuteStateChanged, this);
	signal_handler_connect(sh, "volume", handleInputVolumeChanged, this);
	signal_handler_connect(sh, "audio_sync", handleInputAudioSyncOffsetChanged, this);
	signal_handler_connect(sh, "audio_mixers", handleInputAudioTracksChanged, this);

	if (sourceType == OBS_SOURCE_TYPE_INPUT) {
		signal_handler_connect(sh, "media_started", handleMediaInputPlaybackStarted, this);
		signal_handler_connect(sh, "media_ended", handleMediaInputPlaybackEnded, this);
		signal_handler_connect(sh, "media_pause", sourceMediaPauseMultiHandler, this);
		signal_handler_connect(sh, "media_play", sourceMediaPlayMultiHandler, this);
		signal_handler_connect(sh, "media_restart", sourceMediaRestartMultiHandler, this);
		signal_handler_connect(sh, "media_stopped", sourceMediaStopMultiHandler, this);
		signal_handler_connect(sh, "media_next", sourceMediaNextMultiHandler, this);
		signal_handler_connect(sh, "media_previous", sourceMediaPreviousMultiHandler, this);
	}

	// Scenes
	if (sourceType == OBS_SOURCE_TYPE_SCENE) {
		signal_handler_connect(sh, "item_add", handleSceneItemCreated, this);
		signal_handler_connect(sh, "item_remove", handleSceneItemRemoved, this);
		signal_handler_connect(sh, "reorder", handleSceneItemListReindexed, this);
		signal_handler_connect(sh, "item_visible", handleSceneItemEnableStateChanged, this);
		signal_handler_connect(sh, "item_locked", handleSceneItemLockStateChanged, this);
		signal_handler_connect(sh, "item_transform", handleSceneItemTransformChanged, this);
	}
}

void es::obs::SourceTracker::disconnectSourceSignals(obs_source_t *source)
{
	if (!source)
		return;

	signal_handler_t* sh = obs_source_get_signal_handler(source);

	// Inputs
	signal_handler_disconnect(sh, "activate", handleInputActiveStateChanged, this);
	signal_handler_disconnect(sh, "deactivate", handleInputActiveStateChanged, this);
	signal_handler_disconnect(sh, "show", handleInputShowStateChanged, this);
	signal_handler_disconnect(sh, "hide", handleInputShowStateChanged, this);
	signal_handler_disconnect(sh, "mute", handleInputMuteStateChanged, this);
	signal_handler_disconnect(sh, "volume", handleInputVolumeChanged, this);
	signal_handler_disconnect(sh, "audio_sync", handleInputAudioSyncOffsetChanged, this);
	signal_handler_disconnect(sh, "audio_mixers", handleInputAudioTracksChanged, this);
	//signal_handler_disconnect(sh, "audio_monitoring", handleInputAudioMonitorTypeChanged, this);
	signal_handler_disconnect(sh, "media_started", handleMediaInputPlaybackStarted, this);
	signal_handler_disconnect(sh, "media_ended", handleMediaInputPlaybackEnded, this);
	signal_handler_disconnect(sh, "media_pause", sourceMediaPauseMultiHandler, this);
	signal_handler_disconnect(sh, "media_play", sourceMediaPlayMultiHandler, this);
	signal_handler_disconnect(sh, "media_restart", sourceMediaRestartMultiHandler, this);
	signal_handler_disconnect(sh, "media_stopped", sourceMediaStopMultiHandler, this);
	signal_handler_disconnect(sh, "media_next", sourceMediaNextMultiHandler, this);
	signal_handler_disconnect(sh, "media_previous", sourceMediaPreviousMultiHandler, this);

	// Scenes
	signal_handler_disconnect(sh, "item_add", handleSceneItemCreated, this);
	signal_handler_disconnect(sh, "item_remove", handleSceneItemRemoved, this);
	signal_handler_disconnect(sh, "reorder", handleSceneItemListReindexed, this);
	signal_handler_disconnect(sh, "item_visible", handleSceneItemEnableStateChanged, this);
	signal_handler_disconnect(sh, "item_locked", handleSceneItemLockStateChanged, this);
	signal_handler_disconnect(sh, "item_transform", handleSceneItemTransformChanged, this);
}

bool es::obs::SourceTracker::filterSources(std::string, obs_source_t* source)
{
	return (obs_source_get_type(source) != OBS_SOURCE_TYPE_INPUT);
}

bool es::obs::SourceTracker::filterAudioSources(std::string, obs_source_t* source)
{
	uint32_t flags = obs_source_get_output_flags(source);
	return !(flags & OBS_SOURCE_AUDIO) || (obs_source_get_type(source) != OBS_SOURCE_TYPE_INPUT);
}

bool es::obs::SourceTracker::filterVideoSources(std::string, obs_source_t* source)
{
	uint32_t flags = obs_source_get_output_flags(source);
	return !(flags & OBS_SOURCE_VIDEO) || (obs_source_get_type(source) != OBS_SOURCE_TYPE_INPUT);
}

bool es::obs::SourceTracker::filterTransitions(std::string, obs_source_t* source)
{
	return (obs_source_get_type(source) != OBS_SOURCE_TYPE_TRANSITION);
}

bool es::obs::SourceTracker::filterScenes(std::string, obs_source_t* source)
{
	return (obs_source_get_type(source) != OBS_SOURCE_TYPE_SCENE);
}