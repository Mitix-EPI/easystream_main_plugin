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

#include <obs-module.h>
#include <iostream>
#include <string>
#include <vector>

#include "plugin-macros.generated.h"

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE(PLUGIN_NAME, "en-US")

struct InputInfo {
    std::string name;
    std::string id;
    std::string type;
    std::string description;
    std::string value;
};

struct EnumInputInfo {
	std::string inputKind;
	std::vector<InputInfo> inputs;
};

std::vector<InputInfo> GetInputList(std::string inputKind)
{
	EnumInputInfo inputInfo;
	inputInfo.inputKind = inputKind;

	auto inputEnumProc = [](void *param, obs_source_t *input) {
		if (obs_source_get_type(input) != OBS_SOURCE_TYPE_INPUT)
			return true;

		auto inputInfo = reinterpret_cast<EnumInputInfo*>(param);

		std::string inputKind = obs_source_get_id(input);

		if (inputInfo->inputKind.empty() || inputInfo->inputKind != inputKind)
			return true;

		blog(LOG_INFO, "volume: %f",  obs_mul_to_db(obs_source_get_volume(input)));
		obs_source_set_volume(input, 0.0f);

		InputInfo value;
		value.name = obs_source_get_name(input);
		value.type = inputKind;

		inputInfo->inputs.push_back(value);
		return true;
	};
	obs_enum_sources(inputEnumProc, &inputInfo);

	return inputInfo.inputs;
}

std::vector<std::string> GetInputKindList(bool unversioned, bool includeDisabled)
{
	std::vector<std::string> ret;

	size_t idx = 0;
	const char *kind;
	const char *unversioned_kind;
	while (obs_enum_input_types2(idx++, &kind, &unversioned_kind)) {
		uint32_t caps = obs_get_source_output_flags(kind);

		if (!includeDisabled && (caps & OBS_SOURCE_CAP_DISABLED) != 0)
			continue;

		if (unversioned)
			ret.push_back(unversioned_kind);
		else
			ret.push_back(kind);
	}
	return ret;
}

void source_created_signal(void *param, calldata_t *data)
{
	std::vector<std::string> inputKinds = GetInputKindList(false, true);
    blog(LOG_INFO, "-----------------------------------------");
    for (auto &inputKind : inputKinds) {
        blog(LOG_INFO, "input kind: %s", inputKind.c_str());
        std::vector<InputInfo> values = GetInputList(inputKind);
        for (auto &value : values) {
            blog(LOG_INFO, "  name: %s", value.name.c_str());
        }
    }
    blog(LOG_INFO, "-----------------------------------------");
}


bool obs_module_load(void)
{
    blog(LOG_INFO, "plugin loaded successfully (version %s)", PLUGIN_VERSION);

	auto iso = obs_get_signal_handler();

	signal_handler_connect(iso, "source_create", source_created_signal, nullptr);

    return true;
}


void obs_module_unload()
{
    blog(LOG_INFO, "plugin unloaded");
}

