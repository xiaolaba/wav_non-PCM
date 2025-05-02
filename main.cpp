/*
	author, xiao_laba_cn@yahoo.com, 2025-MAY-01
	dump wave header of IMA ADPCM MP3 recording files
	minGW, used compiler, win10 testing done. 
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>  // For uint16_t, uint32_t
#include <string.h>  // For memcmp
#include <iostream>  // for exit, key press


// 定义WAV文件头部结构, Non-PCM Data
struct WAVHeader {
    char chunkID[4];        // 文件标识，通常为"RIFF"
    uint32_t chunkSize;     // 文件大小
    char format[4];         // 文件格式，“WAVE”
    
	char subchunk1ID[4];    // 子块标识，“fmt ”
    uint32_t subchunk1Size; // 子块大小
    uint16_t audioFormat;   // 音频格式，1为PCM格式
    uint16_t numChannels;   // 声道数，1:单声道，2:双声道
    uint32_t sampleRate;    // 采样率
    uint32_t byteRate;      // 每秒的字节数
    uint16_t blockAlign;    // 块对齐
    uint16_t bitsPerSample; // 采样深度, 16bit
    uint32_t cbSize; // size of extension :0

	char subchunk2ID[4];    // 子块标识，“fact”
    uint32_t subchunk2Size; // 子块大小
    uint32_t sampleLength; // 子块大小
    
	char subchunk3ID[4];    // 子块标识，“data”
    uint32_t subchunk3Size; // 子块大小
};

// 音频数据结构
struct WAVData {
    struct WAVHeader header;  // 音频头部数据
    //uint8_t* sample;          // 音频数据
	uint32_t* sample;          // 音频数据
};

// 读取wav格式文件
struct WAVData* audioread(const char* filename);
// 写入wav格式文件
void audiowrite(const char* filename, struct WAVData* audio_data);

void printWAVHeader(struct WAVHeader* header);



int main() {
    //char filename[30] = "./inputs/test.wav";
	char filename[30] = "./inputs/1_trimmed.WAV";
    struct WAVData* audio_data = audioread(filename);
    if (audio_data == NULL) {
        //perror("文件打开失败!");
        perror("file open failed!");
        return 1;
    }

    char outfilename[30] = "./outputs/test.wav";
    audiowrite(outfilename, audio_data);



    std::cout << "Press Enter to exit...";
    std::cin.get();  // waits for Enter
    return 0;
	
    return 0;
}

struct WAVData* audioread(const char* filename) {
    // 打开文件
    FILE* inputFile = fopen(filename, "rb");
    struct WAVData* audio_data = NULL;
    if (inputFile == NULL) {
        //perror("文件代开失败!");
        perror("wav file open failed!");		
        return NULL;
    }

    // 读取文件头部信息
    struct WAVHeader header;
    fread(&header, sizeof(struct WAVHeader), 1, inputFile);

    // 验证文件格式
    if (strncmp(header.chunkID, "RIFF", 4) != 0 || strncmp(header.format, "WAVE", 4) != 0) {
        //perror("无效文件!");
		perror("wav format error!");
        fclose(inputFile);
        return NULL;
    }



	if ( header.audioFormat == 17 ) {
		printf("\nWAV file, header.audioFormat = 17\n");
		printf("IMA ADPCM (Interactive Multimedia Association Adaptive Differential Pulse Code Modulation) compression.\n\n");
	}


    // 读取wav文件数据
    audio_data = (struct WAVData*)malloc(sizeof(struct WAVData));
    if (audio_data == NULL) {
        //perror("内存申请失败!");
		perror("read wave file, memory allocation failed!");
        return NULL;
    }
	
    audio_data->header = header;


    // 输出wav文件信息
	printWAVHeader(&header);


    //audio_data->sample = (uint8_t*)malloc(header.subchunk2Size);
	audio_data->sample = (uint32_t*)malloc(header.subchunk2Size);
    if (audio_data->sample == NULL) {
        //perror("内存申请失败!");
		perror("read wave data, memory allocation failed!");		
        free(audio_data);
        return NULL;
    }
    fread(audio_data->sample, header.subchunk2Size, 1, inputFile);
    fclose(inputFile);

    return audio_data;
}

void audiowrite(const char* filename, struct WAVData* audio_data) {
    // 打开输出文件
    FILE* outputFile = fopen(filename, "wb");
    if (outputFile == NULL) {
        //perror("文件打开失败!");
		perror("output file, open failed!");	
        return;
    }

    // 写入文件
    fwrite(&audio_data->header, sizeof(struct WAVHeader), 1, outputFile);
    fwrite(audio_data->sample, audio_data->header.subchunk2Size, 1, outputFile);

    // 关闭文件
    fclose(outputFile);
}





void printWAVHeader(struct WAVHeader* header) {
    // Check if it's a valid WAV file
    if (memcmp(header->chunkID, "RIFF", 4) != 0 || 
        memcmp(header->format, "WAVE", 4) != 0) {
        printf("ERROR: Not a valid WAV file!\n");
        return;
    }

    printf("\n=== WAV Header ===\n");
	const unsigned char* data1 = (const unsigned char*) header;
	for (uint8_t i = 0; i < sizeof(WAVHeader); i++) {
	//for (uint8_t i = 0; i < 0x50; i++) {

		printf("%02X ", data1[i]);

		if (i == 4-1 ) printf("[4 bytes], ChunkID: %.4s\n", header->chunkID);     //printf("%#010X\n", *(header->chunkID));
		if (i == 8-1 ) printf("[4 bytes], ChunkSize: %#010X, %u\n", header->chunkSize, header->chunkSize);
		if (i == 12-1) printf("[4 bytes], Format: %.4s\n", header->format);
		if (i == 16-1) printf("[4 bytes], subchunk1ID: %.4s\n", header->subchunk1ID);
		if (i == 20-1) printf("[4 bytes], subchunk1Size: %#010X, %u\n", header->subchunk1Size, header->subchunk1Size);
		if (i == 22-1) printf("      [2 bytes], AudioFormat: %#06X, %u (%s)\n", header->audioFormat, header->audioFormat, header->audioFormat == 1 ? "PCM" : "Non-PCM");
		if (i == 24-1) printf("      [2 bytes], NumChannels: %#06X, %u\n", header->numChannels, header->numChannels);
		if (i == 28-1) printf("[4 bytes], SampleRate: %#010X, %u Hz\n", header->sampleRate, header->sampleRate);
		if (i == 32-1) printf("[4 bytes], ByteRate: %#010X, %u bytes/sec\n", header->byteRate, header->byteRate);
		if (i == 34-1) printf("      [2 bytes], BlockAlign: %#06X, %u bytes\n", header->blockAlign, header->blockAlign);
		if (i == 36-1) printf("      [2 bytes], BitsPerSample: %#06X, %u bits\n", header->bitsPerSample, header->bitsPerSample);
		if (i == 40-1) printf("[4 bytes], cbSize: %#06X, %u bits\n", header->cbSize, header->cbSize);
		if (i == 44-1) printf("[4 bytes], subchunk2ID: %.4s\n", header->subchunk2ID);
		if (i == 48-1) printf("[4 bytes], subchunk2Size: %#010X %u \n", header->subchunk2Size, header->subchunk2Size);
		if (i == 52-1) printf("[4 bytes], sampleLength: %#010X %u \n", header->sampleLength, header->sampleLength);
		if (i == 56-1) printf("[4 bytes], subchunk3ID: %.4s\n", header->subchunk3ID);
		if (i == 60-1) printf("[4 bytes], subchunk3Size: %#010X %u (audio data size)\n\n", header->subchunk3Size, header->subchunk3Size);
		
		// printf("\n");	//every 16 bytes, change to new line

	}
	
	
	printf("=== WAV Header hex dump for debug purpose ===\n");
	const unsigned char* data = (const unsigned char*) header;
	for (uint8_t i = 0; i < sizeof(WAVHeader); i++) {
		printf("%02X ", data[i]);
		if ((i + 1) % 16 == 0) printf("\n"); //every 16 bytes, change to new line
	}
	printf("\n\n");


	
	printf("=== WAV Header printable hex for debug purpose ===\n");	
	for (uint8_t i = 0; i < sizeof(WAVHeader); i++) {
		if (data[i] < 0x20 || data[i] > 0x7E) {
			printf(".");			// non-printable ASCII
		} else {
			printf("%c", data[i]);	// printable ASCII
		}
		if ((i + 1) % 16 == 0) printf("\n");	//every 16 bytes, change to new line
	}
	printf("\n\n");	
	
}


