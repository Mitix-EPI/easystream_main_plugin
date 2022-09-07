#include <iostream>
#include <cmath>
#include <cstring>
#include <fstream>
using namespace std;

const int sampleRate = 44100;
const int bitDepth = 16;

class SineOscillator
{
    float frequency, amplitude, angle = 0.0f, offset = 0.0f;

public:
    SineOscillator(float freq, float amp) : frequency(freq), amplitude(amp)
    {
        offset = 2 * M_PI * frequency / sampleRate;
    }
    float process()
    {
        auto sample = amplitude * sin(angle);
        angle += offset;
        return sample;
        // Asin(2pif/sr)
    }
};

struct WaveHeaderChunk
{
    char RIFF_marker[4];
    uint32_t file_size;
    char filetype_header[4];
};

struct WaveFormatChunk
{
    char format_marker[4];
    uint32_t data_header_length;
    uint16_t format_type;
    uint16_t number_of_channels;
    uint32_t sample_rate;
    uint32_t bytes_per_second;
    uint16_t block_align;
    uint16_t bits_per_sample;
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
    md->format_chunk.sample_rate = sample_rate;                                                            // 44100 Hz ? idk
    md->format_chunk.bytes_per_second = sample_rate * BIT_DEPTH * md->format_chunk.number_of_channels / 8; // div per 8 since 1 byte is 8 bits
    md->format_chunk.block_align = BIT_DEPTH / 8;                                                          // Block Align
    md->format_chunk.bits_per_sample = BIT_DEPTH;                                                          // bits per sample = bit depth

    // ...DATA CHUNK
    memcpy(md->data_chunk.data, "data", 4);
    md->data_chunk.size = 0;

    return md;
}

void writeToFile(ofstream &file, int value, int size)
{
    file.write(reinterpret_cast<const char *>(&value), size);
}

int main()
{
    int duration = 2;
    ofstream audioFile;
    audioFile.open("waveform2.wav", ios::binary);
    SineOscillator sineOscillator(440, 0.5);

    auto *md = generateWAVMetadata(44100);
    // Header chunk
    // audioFile.write(reinterpret_cast<char *>(&md->header_chunk), sizeof(WaveHeaderChunk));

    // Format chunk
    // audioFile.write(reinterpret_cast<char *>(&md->format_chunk), sizeof(WaveFormatChunk));

    // Data chunk
    // audioFile.write(reinterpret_cast<char *>(&md->data_chunk), sizeof(WaveDataChunk));
    // audioFile << "data";
    // audioFile << "----";

    audioFile.write(reinterpret_cast<char *>(md), sizeof(WaveFileMetadata));

    int preAudioPosition = audioFile.tellp();

    auto maxAmplitude = pow(2, bitDepth - 1) - 1;
    for (int i = 0; i < sampleRate * duration; i++)
    {
        auto sample = sineOscillator.process();
        int intSample = static_cast<int>(sample * maxAmplitude);
        writeToFile(audioFile, intSample, 2);
    }
    int postAudioPosition = audioFile.tellp();

    audioFile.seekp(preAudioPosition - 4);
    writeToFile(audioFile, postAudioPosition - preAudioPosition, 4);

    audioFile.seekp(4, ios::beg);
    writeToFile(audioFile, postAudioPosition - 8, 4);

    audioFile.close();
    return 0;
}