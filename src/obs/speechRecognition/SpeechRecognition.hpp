/*
** EPITECH PROJECT, 2021
** easystream_main_plugin
** File description:
** SpeechRecognition
*/

#ifndef SPEECHRECOGNITION_HPP_
#define SPEECHRECOGNITION_HPP_

#include "../../Common.hpp"

namespace es::obs {
    class SpeechRecognition
    {
        public:
            SpeechRecognition(obs_source_t *input);
            ~SpeechRecognition();

            static void InputAudioCaptureCallback(void *priv_data, obs_source_t *, const struct audio_data *data, bool muted);

        private:


            obs_source_t *_source;
            audio_resampler_t *resampler = nullptr;
            std::string text;
            std::vector<std::string *> output;
            std::chrono::_V2::steady_clock::time_point last_caption_at;
            std::ofstream myfile;
            int bytes_per_channel;
    };
}

#endif /* !SPEECHRECOGNITION_HPP_ */
