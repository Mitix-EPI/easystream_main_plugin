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

    public:
        AutoAudioLeveler(obs_source_t *input);
        ~AutoAudioLeveler();

        static void InputAudioCaptureCallback(void *priv_data, obs_source_t *, const struct audio_data *data, bool muted);

        bool ComputeAudioLevel(float audio_level, float audio_volume, float target_level, float *newAudioLevel, float min_detect_level, float margin_level);
        float CalculateAudioLevel(const struct audio_data *data, bool muted);
    };
}

#endif /* !AUTOAUDIOLEVEL_HPP_ */
