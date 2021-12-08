/*
** EPITECH PROJECT, 2021
** easystream_main_plugin
** File description:
** autoAudioLeveler
*/

#include "AutoAudioLeveler.hpp"

es::obs::AutoAudioLeveler::AutoAudioLeveler(obs_source_t *source) : _source(source)
{
    obs_source_add_audio_capture_callback(_source, AutoAudioLeveler::InputAudioCaptureCallback, this);

    blog(LOG_INFO, "[es::Obs::autoAudioLeveler] Auto audio leveler created for input: %s", obs_source_get_name(_source));
}

es::obs::AutoAudioLeveler::~AutoAudioLeveler()
{
	// Delete audio callback
}


void es::obs::AutoAudioLeveler::InputAudioCaptureCallback(void *priv_data, obs_source_t *source, const struct audio_data *data, bool muted)
{
    AutoAudioLeveler *autoAudioLeveler = static_cast<es::obs::AutoAudioLeveler *>(priv_data);

    float inputAudioLevel = autoAudioLeveler->CalculateAudioLevel(data, muted);

    // float inputAudioLevelInDb = obs_mul_to_db(inputAudioLevel);

    float desiredAudioLevelInDb = -20.0;
    float desiredAudioLevel = obs_db_to_mul(desiredAudioLevelInDb);

    float newAudioLevel = 0.0;
    if (autoAudioLeveler->ComputeAudioLevel(inputAudioLevel, obs_source_get_volume(source), desiredAudioLevel, &newAudioLevel, 0.05, 0.01))
    {
        blog(LOG_INFO, "newAudioLevel: %f", newAudioLevel);
        obs_source_set_volume(source, newAudioLevel);
    }
}

bool es::obs::AutoAudioLeveler::ComputeAudioLevel(float audio_level, float audio_volume, float target_level, float *newAudioLevel, float min_detect_level, float margin_level)
{
	float level_change = target_level - (audio_level * audio_volume);
	// blog(LOG_INFO, "level_change: %f", level_change);
	// blog(LOG_INFO, "target_level: %f", target_level);
	// blog(LOG_INFO, "audio_level: %f", audio_level);
	// blog(LOG_INFO, "min_detect_level: %f", min_detect_level);
	// if absolute value of level change is more than smoothing level,
	// then we need to change the level
	if (fabsf(level_change) < margin_level)
		return false;
	// Level is too low
	if (level_change > 0.0)
	{
		// *newAudioLevel = target_level - level_change;
		*newAudioLevel = CLAMP(audio_volume + 0.1, min_detect_level, 1.0f);
		return true;
	}
	// Level is too high
	else
	{
		// *newAudioLevel = target_level - level_change;
		// if ((audio_level * audio_volume) < min_detect_level && ) {
		// 	return false;
		// }
		*newAudioLevel = CLAMP(audio_volume - 0.1, min_detect_level, 1.0f);
		return true;
	}
	return false;
}

float es::obs::AutoAudioLeveler::CalculateAudioLevel(const struct audio_data *data, bool muted)
{
	float audio_level = 0.0;

	if (muted)
	{
		return 0.0;
	}

	size_t nr_samples = data->frames;

	float *samples = (float *)data->data[0];
	if (!samples)
	{
		return 0.0;
	}
	float sum = 0.0;
	for (size_t i = 0; i < nr_samples; i++)
	{
		float sample = samples[i];
		sum += sample * sample;
	}
	audio_level = (float)(sqrtf(sum / nr_samples));
	return audio_level;
}