#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

struct WaveHeader
{
        char RIFF_marker[4];
        uint32_t file_size;
        char filetype_header[4];
        char format_marker[4];
        uint32_t data_header_length;
        uint16_t format_type;
        uint16_t number_of_channels;
        uint32_t sample_rate;
        uint32_t bytes_per_second;
        uint16_t bytes_per_frame;
        uint16_t bits_per_sample;
};

struct WaveHeader *genericWAVHeader(uint32_t sample_rate, uint16_t bit_depth, uint16_t channels)
{
    struct WaveHeader *hdr;
    hdr = (struct WaveHeader*) malloc(sizeof(*hdr));
    if (!hdr)
        return NULL;

    memcpy(&hdr->RIFF_marker, "RIFF", 4);
    memcpy(&hdr->filetype_header, "WAVE", 4);
    memcpy(&hdr->format_marker, "fmt ", 4);
    hdr->data_header_length = 16;
    hdr->format_type = 1;
    hdr->number_of_channels = channels;
    hdr->sample_rate = sample_rate;
    hdr->bytes_per_second = sample_rate * channels * bit_depth / 8;
    hdr->bytes_per_frame = channels * bit_depth / 8;
    hdr->bits_per_sample = bit_depth;

    return hdr;
}

int writeWAVHeader(int fd, struct WaveHeader *hdr)
{
    if (!hdr)
        return -1;

    write(fd, &hdr->RIFF_marker, 4);
    write(fd, &hdr->file_size, 4);
    write(fd, &hdr->filetype_header, 4);
    write(fd, &hdr->format_marker, 4);
    write(fd, &hdr->data_header_length, 4);
    write(fd, &hdr->format_type, 2);
    write(fd, &hdr->number_of_channels, 2);
    write(fd, &hdr->sample_rate, 4);
    write(fd, &hdr->bytes_per_second, 4);
    write(fd, &hdr->bytes_per_frame, 2);
    write(fd, &hdr->bits_per_sample, 2);
    write(fd, "data", 4);

    uint32_t data_size = hdr->file_size + 8 - 44;
    write(fd, &data_size, 4);

    return 0;
}

int main(int argc, char *argv[]) {

    struct WaveHeader *hdr;

    char wavFile[L_tmpnam + 5];
    char *tempFilenameStub = tmpnam(NULL);
    sprintf(wavFile, "%s.wav", tempFilenameStub);
    printf("%s\n", wavFile);
    hdr = genericWAVHeader(16000, 16, 2);
    if (!hdr)
    {
        fprintf(stderr, "Error allocating WAV header.n");
        return -1;
    }

    int filedesc = open(wavFile, O_WRONLY | O_CREAT, 0644);
    uint32_t pcm_data_size = hdr->sample_rate * hdr->bytes_per_frame * 3;
    hdr->file_size = pcm_data_size + 44 - 8;
    writeWAVHeader(filedesc, hdr);

    free(hdr);
    return 0;
}