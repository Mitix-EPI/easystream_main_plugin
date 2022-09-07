/*
** Easystream EIP, 2021
** easystream_main_plugin
** File description:
** memory
*/

#ifndef MEMORY_HPP_
#define MEMORY_HPP_

#include <string>
#include <obs.hpp>

void ___source_dummy_addref(obs_source_t *);
void ___weak_source_dummy_addref(obs_weak_source_t *);
void ___scene_dummy_addref(obs_scene_t *);
void ___sceneitem_dummy_addref(obs_sceneitem_t *);
void ___data_dummy_addref(obs_data_t *);
void ___data_array_dummy_addref(obs_data_array_t *);
void ___output_dummy_addref(obs_output_t *);
void ___data_item_dummy_addref(obs_data_item_t *);
void ___data_item_release(obs_data_item_t *);
void ___properties_dummy_addref(obs_properties_t *);

// using OBSSourceAutoRelease = OBSRef<obs_source_t*, ___source_dummy_addref, obs_source_release>;
// using OBSWeakSourceAutoRelease = OBSRef<obs_weak_source_t*, ___weak_source_dummy_addref, obs_weak_source_release>;
// using OBSSceneAutoRelease = OBSRef<obs_scene_t*, ___scene_dummy_addref, obs_scene_release>;
// using OBSSceneItemAutoRelease = OBSRef<obs_sceneitem_t*, ___sceneitem_dummy_addref, obs_sceneitem_release>;
// using OBSDataAutoRelease = OBSRef<obs_data_t*, ___data_dummy_addref, obs_data_release>;
// using OBSDataArrayAutoRelease = OBSRef<obs_data_array_t*, ___data_array_dummy_addref, obs_data_array_release>;
// using OBSOutputAutoRelease = OBSRef<obs_output_t*, ___output_dummy_addref, obs_output_release>;
// using OBSDataItemAutoRelease = OBSRef<obs_data_item_t*, ___data_item_dummy_addref, ___data_item_release>;
// using OBSPropertiesAutoDestroy = OBSRef<obs_properties_t*, ___properties_dummy_addref, obs_properties_destroy>;

template <typename T>
T *GetCalldataPointer(const calldata_t *data, const char *name)
{
	void *ptr = nullptr;
	calldata_get_ptr(data, name, &ptr);
	return reinterpret_cast<T *>(ptr);
}

#endif /* !MEMORY_HPP_ */