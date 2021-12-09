/*
** Easystream EIP, 2021
** easystream_main_plugin
** File description:
** obs
*/

#ifndef OBS_HPP_
#define OBS_HPP_

#include "Json.hpp"
#include "../plugin-main.hpp"

namespace es {
    namespace utils {
        namespace obs {
            namespace stringHelper {
                std::string GetObsVersion();
                std::string GetCurrentSceneCollection();
                std::string GetCurrentProfile();
                std::string GetCurrentProfilePath();
                std::string GetCurrentRecordOutputPath();
                std::string GetSourceType(obs_source_t *source);
                std::string GetInputMonitorType(obs_source_t *input);
                std::string GetMediaInputState(obs_source_t *input);
                std::string GetLastReplayBufferFilePath();
                std::string GetSceneItemBoundsType(enum obs_bounds_type type);
                std::string DurationToTimecode(uint64_t);
            }

            namespace enumHelper {
                enum obs_bounds_type GetSceneItemBoundsType(std::string boundsType);
            }

            namespace numberHelper {
                uint64_t GetOutputDuration(obs_output_t *output);
                size_t GetSceneCount();
            }

            namespace listHelper {
                std::vector<std::string> GetSceneCollectionList();
                std::vector<std::string> GetProfileList();
                std::vector<obs_hotkey_t *> GetHotkeyList();
                std::vector<std::string> GetHotkeyNameList();
                std::vector<json> GetSceneList();
                std::vector<json> GetSceneItemList(obs_scene_t *scene, bool basic = false);
                std::vector<json> GetTransitionList();
                std::vector<json> GetInputList(std::string inputKind = "");
                std::vector<std::string> GetInputKindList(bool unversioned = false, bool includeDisabled = false);
            }

            namespace dataHelper {
                json GetStats();
                json GetSceneItemTransform(obs_sceneitem_t *item);
            }

            namespace searchHelper {
                obs_hotkey_t *GetHotkeyByName(std::string name);
                obs_sceneitem_t *GetSceneItemByName(obs_scene_t *scene, std::string name); // Increments ref. Use OBSSceneItemAutoRelease
            }

            namespace actionHelper {
                obs_sceneitem_t *CreateSceneItem(obs_source_t *source, obs_scene_t *scene, bool sceneItemEnabled = true, obs_transform_info *sceneItemTransform = nullptr, obs_sceneitem_crop *sceneItemCrop = nullptr); // Increments ref. Use OBSSceneItemAutoRelease
                obs_sceneitem_t *CreateInput(std::string inputName, std::string inputKind, obs_data_t *inputSettings, obs_scene_t *scene, bool sceneItemEnabled = true); // Increments ref. Use OBSSceneItemAutoRelease
            }
        }
    }
}

#endif /* !OBS_HPP_ */