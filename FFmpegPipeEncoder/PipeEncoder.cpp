#include "PipeEncoder.hpp"

#include <iostream>

PipeEncoder::PipeEncoder(const EncoderSettings& _settings)
    : settings(_settings) {
  std::string fmt = settings.inputPixelFormat;
  numChannels = fmt == "rgba" || fmt == "rgb32" ? 4 : 3;
  std::vector<std::string> args = {
      "-y",   // overwrite
      "-an",  // disable audio
      "-framerate " + std::to_string(settings.inputFps),
      "-s " + std::to_string(settings.width) + "x" +
          std::to_string(settings.height),
      "-f rawvideo",  // input codec
      "-pix_fmt " + settings.inputPixelFormat,
      settings.extraInputArgs,
      "-i pipe:",  // input source
      "-c:v " + settings.codec,
      "-r " + std::to_string(settings.outputFps),
      settings.extraOutputArgs,
      "-pix_fmt " + settings.outputPixelFormat,
      settings.outputPath};
  for (const std::string& arg : args) {
    if (!arg.empty()) {
      cmd += " " + arg;
    }
  }
}

PipeEncoder::~PipeEncoder() {
  stop();
  joinThread();
  closePipe();
}

void PipeEncoder::start() {
  if (isRecording.load()) {
    std::cerr << "Can't start recording - already started.\n";
    return;
  }
  if (!isReady()) {
    std::cerr
        << "Can't start recording - previous recording is still processing.\n";
    return;
  }
  if (settings.outputPath.empty()) {
    std::cerr << "Can't start recording - output path is empty.";
    return;
  }
  numFramesAdded = 0;
  closePipe();
  openPipe();
  isRecording = true;
}

void PipeEncoder::stop() { isRecording = false; }

void PipeEncoder::encode(unsigned char* pixelData) {
  if (!isRecording) {
    std::cerr << "Can't add frame - recording not started.\n";
    return;
  }

  if (!pipe) {
    std::cerr << "Can't add frame - Ffmpeg pipe is invalid.\n";
    return;
  }

  using Clock = std::chrono::steady_clock;
  using Seconds = std::chrono::duration<float>;

  if (numFramesAdded == 0) {
    joinThread();
    piper = std::thread(&PipeEncoder::processFrame, this);
    startTime = Clock::now();
    lastFrameTime = startTime;
  }

  const float delta =
      Seconds(Clock::now() - startTime).count() - getRecordedDuration();
  const std::size_t framesToWrite = delta * settings.outputFps;
  std::size_t written = 0;

  auto frameData = std::make_shared<std::vector<unsigned char>>(
      pixelData, pixelData + settings.width * settings.height * 4);

  while (numFramesAdded == 0 || framesToWrite > written) {
    frames.produce(frameData);
    numFramesAdded++;
    written++;
    lastFrameTime = Clock::now();
  }
}

void PipeEncoder::processFrame() {
  using Clock = std::chrono::steady_clock;
  while (isRecording.load()) {
    std::chrono::time_point<Clock> t = Clock::now();
    const float frameDur = 1.f / settings.outputFps;
    while (frames.size()) {
      float delta = std::chrono::duration<float>(Clock::now() - t).count();
      if (delta >= frameDur) {
        if (!isRecording.load()) {
          std::cout << "Recording stopped, but finishing frame queue - "
                    << frames.size() << " remaining frames at "
                    << settings.outputFps << " fps\n";
        }

        std::shared_ptr<std::vector<unsigned char>> data;
        if (frames.consume(data) && data) {
          using u = std::size_t;
          const u len = settings.width * settings.height * numChannels;
          const u written = fwrite(data->data(), sizeof(char), len, pipe);
          if (written <= 0) {
            std::cerr << "Unable to write the frame.\n";
          }
          t = Clock::now();
        }
      }
    }
  }
  closePipe();
  numFramesAdded = 0;
}

bool PipeEncoder::isReady() const {
  return isRecording.load() == false && frames.size() == 0;
}

float PipeEncoder::getRecordedDuration() const {
  return numFramesAdded / settings.outputFps;
}

void PipeEncoder::openPipe() {
  std::cout << "Starting FfmpegRecorder with command...\n\t" << cmd
            << std::endl;
  pipe = popen(cmd.c_str(), "w");
  if (!pipe) {
    std::cerr << "Unable to open pipe.\n";
    return;
  }
  setvbuf(pipe, nullptr, _IONBF, 0);
};

void PipeEncoder::closePipe() {
  if (pipe) {
    int res = pclose(pipe);
    if (res < 0) {
      std::cerr << "Error closing FFmpeg pipe.\n";
      pipe = nullptr;
      return;
    }
  }
}

void PipeEncoder::joinThread() {
  if (piper.joinable()) {
    piper.join();
  }
}