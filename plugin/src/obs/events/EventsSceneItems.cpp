/*
** Easystream EIP, 2021
** easystream_main_plugin
** File description:
** eventsSceneItems
*/

#include "../SourceTracker.hpp"

void es::obs::SourceTracker::handleSceneItemCreated(void *param, calldata_t *data)
{
	obs_scene_t *scene = GetCalldataPointer<obs_scene_t>(data, "scene");
	if (!scene)
		return;

	obs_sceneitem_t *sceneItem = GetCalldataPointer<obs_sceneitem_t>(data, "item");
	if (!sceneItem)
		return;

	std::string sceneName(obs_source_get_name(obs_scene_get_source(scene)));
	std::string inputName(obs_source_get_name(obs_sceneitem_get_source(sceneItem)));
	int64_t sceneItemId = obs_sceneitem_get_id(sceneItem);
	int sceneItemIndex = obs_sceneitem_get_order_position(sceneItem);
	blog(LOG_INFO, "[SourceTracker::handleSceneItemCreated]: %s", sceneName.c_str());
}

void es::obs::SourceTracker::handleSceneItemRemoved(void *param, calldata_t *data)
{
	obs_scene_t *scene = GetCalldataPointer<obs_scene_t>(data, "scene");
	if (!scene)
		return;

	obs_sceneitem_t *sceneItem = GetCalldataPointer<obs_sceneitem_t>(data, "item");
	if (!sceneItem)
		return;

	std::string sceneName(obs_source_get_name(obs_scene_get_source(scene)));
	std::string inputName(obs_source_get_name(obs_sceneitem_get_source(sceneItem)));
	int64_t sceneItemId = obs_sceneitem_get_id(sceneItem);
	int sceneItemIndex = obs_sceneitem_get_order_position(sceneItem);
	blog(LOG_INFO, "[SourceTracker::handleSceneItemRemoved]: %s", sceneName.c_str());
}

void es::obs::SourceTracker::handleSceneItemListReindexed(void *param, calldata_t *data)
{
	obs_scene_t *scene = GetCalldataPointer<obs_scene_t>(data, "scene");
	if (!scene)
		return;

	std::string name(obs_source_get_name(obs_scene_get_source(scene)));
    blog(LOG_INFO, "[SourceTracker::handleSceneItemListReindexed]: %s", name.c_str());
}

void es::obs::SourceTracker::handleSceneItemEnableStateChanged(void *param, calldata_t *data)
{
	obs_scene_t *scene = GetCalldataPointer<obs_scene_t>(data, "scene");
	if (!scene)
		return;

	obs_sceneitem_t *sceneItem = GetCalldataPointer<obs_sceneitem_t>(data, "item");
	if (!sceneItem)
		return;

	bool sceneItemEnabled = calldata_bool(data, "visible");

	std::string name(obs_source_get_name(obs_scene_get_source(scene)));
	int64_t sceneItemId = obs_sceneitem_get_id(sceneItem);
    blog(LOG_INFO, "[SourceTracker::handleSceneItemEnableStateChanged]: %s", name.c_str());
}

void es::obs::SourceTracker::handleSceneItemLockStateChanged(void *param, calldata_t *data)
{
	obs_scene_t *scene = GetCalldataPointer<obs_scene_t>(data, "scene");
	if (!scene)
		return;

	obs_sceneitem_t *sceneItem = GetCalldataPointer<obs_sceneitem_t>(data, "item");
	if (!sceneItem)
		return;

	bool sceneItemLocked = calldata_bool(data, "locked");

	std::string name(obs_source_get_name(obs_scene_get_source(scene)));
	int64_t sceneItemId = obs_sceneitem_get_id(sceneItem);
    blog(LOG_INFO, "[SourceTracker::handleSceneItemLockStateChanged]: %s", name.c_str());
}

void es::obs::SourceTracker::handleSceneItemTransformChanged(void *param, calldata_t *data)
{
	obs_scene_t *scene = GetCalldataPointer<obs_scene_t>(data, "scene");
	if (!scene)
		return;

	obs_sceneitem_t *sceneItem = GetCalldataPointer<obs_sceneitem_t>(data, "item");
	if (!sceneItem)
		return;

	std::string name(obs_source_get_name(obs_scene_get_source(scene)));
	int64_t sceneItemId = obs_sceneitem_get_id(sceneItem);
    blog(LOG_INFO, "[SourceTracker::handleMediaInputPlaybackEnded]: %s", name.c_str());
}