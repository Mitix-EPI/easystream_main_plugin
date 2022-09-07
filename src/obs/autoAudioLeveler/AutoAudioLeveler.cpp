/*
** EPITECH PROJECT, 2021
** easystream_main_plugin
** File description:
** autoAudioLeveler
*/

#include "AutoAudioLeveler.hpp"

es::obs::AutoAudioLeveler::AutoAudioLeveler(obs_source_t *source) : _source(source), _levelToGo(0), _startTime(std::clock())
{
	obs_source_add_audio_capture_callback(_source, InputAudioCaptureCallback, this);

	_desiredAudioLevel = -40.0;

	_minDetectLevel = -56.0;

	_levels = std::list(60, _desiredAudioLevel);

	blog(LOG_INFO, "[es::Obs::autoAudioLeveler] Auto audio leveler created for input: %s", obs_source_get_name(_source));
}

es::obs::AutoAudioLeveler::~AutoAudioLeveler()
{
}

void es::obs::AutoAudioLeveler::InputAudioCaptureCallback(void *priv_data, obs_source_t *source, const struct audio_data *data, bool muted)
{
	static int count = 0;
	AutoAudioLeveler *autoAudioLeveler = static_cast<es::obs::AutoAudioLeveler *>(priv_data);

	float inputAudioLevel = autoAudioLeveler->CalculateAudioLevel(data, muted);

	float audioVolume = obs_source_get_volume(source);

	autoAudioLeveler->ComputeAudioLevel(inputAudioLevel, audioVolume);

	audioVolume = autoAudioLeveler->computeLerp(audioVolume);
	obs_source_set_volume(source, audioVolume);
}

float es::obs::AutoAudioLeveler::computeLerp(float audioVolume)
{
	float levelChange = obs_mul_to_db(audioVolume) - _levelToGo;

	if (levelChange > 0.0)
	{
		return CLAMP(audioVolume - 0.01, 0.0f, 1.0f);
	}
	else if (levelChange < 0.0)
	{
		return CLAMP(audioVolume + 0.01, 0.0f, 1.0f);
	}
	return audioVolume;
}

void es::obs::AutoAudioLeveler::ComputeAudioLevel(float audioLevelMul, float audioVolume)
{
	float curAudioLevel = obs_mul_to_db(audioLevelMul * audioVolume);
	float average = std::accumulate(_levels.begin(), _levels.end(), 0.0) / _levels.size();
	float levelChange = _desiredAudioLevel - average;

	std::clock_t curTime = std::clock();
	double elapsedTime = (curTime - _startTime) / (double)CLOCKS_PER_SEC;

	if (elapsedTime > 0.01)
	{
		_startTime = curTime;
	}
	else
	{
		return;
	}
	if (curAudioLevel > _minDetectLevel)
	{
		_levels.push_front(curAudioLevel);
		_levels.pop_back();
	}

	_levelToGo = levelChange;
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