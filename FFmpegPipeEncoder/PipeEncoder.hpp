#pragma once

#include <atomic>
#include <memory>
#include <thread>
#include <vector>

#include "EncoderSettings.hpp"
#include "LockFreeQueue.hpp"

/**
 * A class that uses an external FFmpeg process to encode raw frames into video.
 */
class PipeEncoder {
 public:
  PipeEncoder(const EncoderSettings& _settings);
  ~PipeEncoder();

  /**
   * Starts the FFmpeg process.
   */
  void start();

  /**
   * Marks the recording process as finished.
   */
  void stop();

  /**
   * Spawns a thread once in the beginning and pushes frames into the queue for
   * processing by FFmpeg.
   */
  void encode(unsigned char* pixelData);

  bool isReady() const;
  float getRecordedDuration() const;

 private:
  /**
   * Pulls frames from the queue and pipes it to FFmpeg until the recording
   * process is marked finished and there are no more frames in the queue.
   */
  void processFrame();
  void openPipe();
  void closePipe();
  void joinThread();

  EncoderSettings settings;
  std::string cmd = "ffmpeg";
  unsigned int numFramesAdded = 0;
  unsigned int numChannels;
  std::atomic<bool> isRecording;
  FILE* pipe = nullptr;
  std::thread piper;
  std::chrono::time_point<std::chrono::steady_clock> startTime, lastFrameTime;
  LockFreeQueue<std::shared_ptr<std::vector<unsigned char>>> frames;
};