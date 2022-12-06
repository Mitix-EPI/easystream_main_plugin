/*
** EPITECH PROJECT, 2021
** easystream_main_plugin
** File description:
** SpeechRecognition
*/

#include "SpeechRecognition.hpp"
#include <obs.h>

struct WaveHeaderChunk
{
    char RIFF_marker[4];
    uint32_t file_size;
    char filetype_header[4];
};

struct WaveFormatChunk
{
    char format_marker[4];       // 4
    uint32_t data_header_length; // 4
    uint16_t format_type;        // 2
    uint16_t number_of_channels; // 2
    uint32_t sample_rate;        // 4
    uint32_t bytes_per_second;   // 4
    uint16_t block_align;        // 2
    uint16_t bits_per_sample;    // 2
};

struct WaveDataChunk
{
    char data[4];
    uint32_t size;
};

struct WaveFileMetadata
{
    WaveHeaderChunk header_chunk;
    WaveFormatChunk format_chunk;
    WaveDataChunk data_chunk;
};

WaveFileMetadata *generateWAVMetadata(int sample_rate)
{
    WaveFileMetadata *md = new WaveFileMetadata;

    // TMP
    const int BIT_DEPTH = 16;

    // ...HEADER CHUNK
    memcpy(md->header_chunk.RIFF_marker, "RIFF", 4);
    md->header_chunk.file_size = 0;
    memcpy(md->header_chunk.filetype_header, "WAVE", 4);

    // ...FORMAT CHUNK
    memcpy(md->format_chunk.format_marker, "fmt ", 4);
    md->format_chunk.data_header_length = 16;
    md->format_chunk.format_type = 1;                                                                      // Uncompressed
    md->format_chunk.number_of_channels = 1;                                                               // MONO
    md->format_chunk.sample_rate = sample_rate;                                                            // 44100 Hz ?
    md->format_chunk.bytes_per_second = sample_rate * BIT_DEPTH * md->format_chunk.number_of_channels / 8; // div per 8 since 1 byte is 8 bits
    md->format_chunk.block_align = BIT_DEPTH / 8;                                                          // Block Align
    md->format_chunk.bits_per_sample = BIT_DEPTH;                                                          // bits per sample = bit depth

    // ...DATA CHUNK
    memcpy(md->data_chunk.data, "data", 4);
    md->data_chunk.size = 0;

    return md;
}

void writeDataToFile(std::ofstream &file, int value, int size)
{
    file.write(reinterpret_cast<const char *>(&value), size);
    file << std::flush;
}

es::obs::SpeechRecognition::SpeechRecognition(obs_source_t *input) : _source(input), last_caption_at(std::chrono::steady_clock::now())
{
    const struct audio_output_info *obs_audio = audio_output_get_info(obs_get_audio());
    if (!obs_audio || !input)
    {
        blog(LOG_INFO, "[es::Obs::SpeechRecognition] Error");
        return;
    }

    blog(LOG_INFO, "[es::Obs::SpeechRecognition] - name : %s", obs_audio->name);
    blog(LOG_INFO, "[es::Obs::SpeechRecognition] - format : %d", obs_audio->format);
    blog(LOG_INFO, "[es::Obs::SpeechRecognition] - speakers : %s", obs_audio->speakers);

    // Opening file
    this->myfile.open("audio_file.wav", std::ios_base::binary);

    WaveFileMetadata *wav_metadata = generateWAVMetadata(44100);
    this->myfile.write(
        reinterpret_cast<const char *>(wav_metadata),
        sizeof(WaveFileMetadata));
    delete wav_metadata;

    // resample_info resample_to = {16000, AUDIO_FORMAT_16BIT, SPEAKERS_MONO};
    // bytes_per_channel = get_audio_bytes_per_channel(resample_to.format);

    // if (obs_audio->samples_per_sec != resample_to.samples_per_sec || obs_audio->format != resample_to.format || obs_audio->speakers != resample_to.speakers)
    // {
    //     resample_info src = {
    //         obs_audio->samples_per_sec,
    //         obs_audio->format,
    //         obs_audio->speakers};

    //     this->resampler = audio_resampler_create(&resample_to, &src);
    //     if (!this->resampler)
    //         throw std::string("Failed to create audio resampler");
    // }

    blog(LOG_INFO, "[es::Obs::SpeechRecognition] Speech recognition for input: %s", obs_source_get_name(_source));
    obs_source_add_audio_capture_callback(_source, InputAudioCaptureCallback, this);
}

es::obs::SpeechRecognition::~SpeechRecognition()
{
    this->myfile.close();
}

void es::obs::SpeechRecognition::InputAudioCaptureCallback(void *priv_data, obs_source_t *source, const struct audio_data *data, bool muted)
{
    static int count = 0;
    SpeechRecognition *self = static_cast<es::obs::SpeechRecognition *>(priv_data);

    if (muted)
    {
        blog(LOG_INFO, "[es::Obs::SpeechRecognition] SOUND IS MUTED");
        return;
    }
    if (!data || !data->frames)
    {
        blog(LOG_INFO, "[es::Obs::SpeechRecognition] ERROR WITH DATA OR DATAFRAMES");
        return;
    }

    unsigned int size = 0;
    std::string *str = nullptr;

    // if (!self->resampler)
    // {
    //     uint8_t *out[MAX_AV_PLANES];
    //     memset(out, 0, sizeof(out));

    //     uint32_t out_frames;
    //     uint64_t ts_offset;

    //     bool success = audio_resampler_resample(self->resampler, out, &out_frames, &ts_offset, (const uint8_t *const *)data->data, data->frames);
    //     if (!success || !out[0])
    //     {
    //         blog(LOG_INFO, "failed resampling audio data");
    //         return;
    //     }

    //     size = out_frames * get_audio_bytes_per_channel((audio_format)self->bytes_per_channel);
    //     str = new std::string((char *)out[0], size);
    // }
    // else
    // {
    size = data->frames * get_audio_bytes_per_channel((audio_format)self->bytes_per_channel);
    str = new std::string((char *)data->data[0], size);

    // }
    self->output.push_back(str);
    double secs_since_last_caption = std::chrono::duration_cast<std::chrono::duration<double>>(
                                         std::chrono::steady_clock::now() - self->last_caption_at)
                                         .count();

    // Writes captured audio to file.
    if (secs_since_last_caption >= 5)
    {
        blog(LOG_INFO, "[es::Obs::SpeechRecognition]---------");

        // WaveFileMetadata *wav_metadata = generateWAVMetadata(16000);
        // self->myfile.write(reinterpret_cast<const char *>(wav_metadata), sizeof(WaveFileMetadata));

        uint32_t preAudioPos = self->myfile.tellp();
        blog(LOG_INFO, "[FILE WRITING] - Position before writing audio : %d", preAudioPos);

        self->last_caption_at = std::chrono::steady_clock::now();
        // int s = 0;

        // for (auto &n : self->output)
        //     s += n->size();
        // self->myfile << std::hex << s << "\r\n";
        for (auto &n : self->output)
            self->myfile << *n;

        uint32_t postAudioPos = self->myfile.tellp();
        blog(LOG_INFO, "[FILE WRITING] - Position after writing audio : %d", postAudioPos);

        self->myfile.seekp(preAudioPos - 4); // Setting stream pos to the audio size field
        writeDataToFile(self->myfile, postAudioPos - preAudioPos, 4);

        self->myfile.seekp(4, std::ios::beg);
        writeDataToFile(self->myfile, postAudioPos - 8, 4);

        self->output.clear();
        blog(LOG_INFO, "---------[es::Obs::SpeechRecognition]");
    }

    // blog(LOG_INFO, "[es::Obs::SpeechRecognition] Size: %d", size);
}