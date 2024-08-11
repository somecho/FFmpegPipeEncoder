#pragma once

#include <string>

struct EncoderSettings {
  float inputFps;
  float outputFps;
  std::string outputPath;
  std::string codec;
  std::string outputPixelFormat;
  std::string extraInputArgs;
  std::string extraOutputArgs;
  std::string inputPixelFormat;
  unsigned int width;
  unsigned int height;
};

class EncoderSettingsBuilder {
 public:
  EncoderSettingsBuilder() {
    settings.inputFps = 60.0;
    settings.outputFps = 60.0;
    settings.outputPath = "output.mp4";
    settings.codec = "libx264";
    settings.inputPixelFormat = "rgba";
    settings.outputPixelFormat = "yuv420p";
    settings.extraInputArgs = "";
    settings.extraOutputArgs = "";
    settings.width = 640;
    settings.height = 480;
  }

  EncoderSettingsBuilder& inputFps(float fps) {
    settings.inputFps = fps;
    return *this;
  }

  EncoderSettingsBuilder& outputFps(float fps) {
    settings.outputFps = fps;
    return *this;
  }

  EncoderSettingsBuilder& outputPath(std::string path) {
    settings.outputPath = path;
    return *this;
  }

  EncoderSettingsBuilder& codec(std::string codec) {
    settings.codec = codec;
    return *this;
  }

  EncoderSettingsBuilder& inputPixelFormat(std::string pixelFormat) {
    settings.inputPixelFormat = pixelFormat;
    return *this;
  }

  EncoderSettingsBuilder& outputPixelFormat(std::string pixelFormat) {
    settings.outputPixelFormat = pixelFormat;
    return *this;
  }

  EncoderSettingsBuilder& extraInputArgs(std::string args) {
    settings.extraInputArgs = args;
    return *this;
  }

  EncoderSettingsBuilder& extraOutputArgs(std::string args) {
    settings.extraOutputArgs = args;
    return *this;
  }

  EncoderSettingsBuilder& resolution(unsigned int width, unsigned int height) {
    settings.width = width;
    settings.height = height;
    return *this;
  }

  EncoderSettings build() { return settings; }

 private:
  EncoderSettings settings;
};