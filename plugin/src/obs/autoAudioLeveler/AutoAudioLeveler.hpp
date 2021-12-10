/*
** easyStream EIP, 2021
** easystream_main_plugin
** File description:
** autoAudioLeveler
*/

#ifndef AUTOAUDIOLEVEL_HPP_
#define AUTOAUDIOLEVEL_HPP_

#include "../../Common.hpp"

namespace es::obs
{
    class AutoAudioLeveler
    {
    private:
        obs_source_t *_source;
        float _levelToGo;
        std::clock_t _startTime;
        std::list<float> _levels;

        float _desiredAudioLevel;
        float _minDetectLevel;

    public:
        AutoAudioLeveler(obs_source_t *input);
        ~AutoAudioLeveler();

        static void InputAudioCaptureCallback(void *priv_data, obs_source_t *, const struct audio_data *data, bool muted);

        float computeLerp(float audioVolume);
        void ComputeAudioLevel(float audioLevelMul, float audioVolume);
        float CalculateAudioLevel(const struct audio_data *data, bool muted);
    };
}

#endif /* !AUTOAUDIOLEVEL_HPP_ */
