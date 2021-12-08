/*
** EasyStream EIP, 2021
** easystream_main_plugin
** File description:
** eventScenes
*/

#include "../sourceTracker.hpp"

void es::obs::SourceTracker::handleSceneCreated(obs_source_t *source)
{
	std::string name(obs_source_get_name(source));
	bool group = obs_source_is_group(source);

    blog(LOG_INFO, "Scene created: %s", name.c_str());
}

void es::obs::SourceTracker::handleSceneRemoved(obs_source_t *source)
{
	std::string name(obs_source_get_name(source));
	bool group = obs_source_is_group(source);

    blog(LOG_INFO, "Scene removed: %s", name.c_str());
}

void es::obs::SourceTracker::handleSceneNameChanged(obs_source_t *, std::string oldSceneName, std::string sceneName)
{
    blog(LOG_INFO, "Scene name changed: %s", sceneName.c_str());
}

void es::obs::SourceTracker::handleCurrentSceneChanged()
{
	obs_source_t *currentScene = obs_frontend_get_current_scene();

	std::string name(obs_source_get_name(currentScene));

    blog(LOG_INFO, "Current scene changed: %s", name.c_str());
}

void es::obs::SourceTracker::handleCurrentPreviewSceneChanged()
{
	obs_source_t *currentPreviewScene = obs_frontend_get_current_preview_scene();

	if (!currentPreviewScene)
		return;

	std::string name(obs_source_get_name(currentPreviewScene));

    blog(LOG_INFO, "Current preview scene changed: %s", name.c_str());
}

void es::obs::SourceTracker::handleSceneListChanged()
{
    blog(LOG_INFO, "Scene list changed");
}