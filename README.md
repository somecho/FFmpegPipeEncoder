# FFmpegPipeEncoder

A C++ Class to encode raw frames with an external FFmpeg process on a separate
thread. Useful for creative coding applications where high quality output is
desired.

## Installation 

FFmpegPipeEncoder has no dependencies.

### CMake

```
cmake -S . -B build -DCMAKE_INSTALL_PREFIX=/desired/install/path
cmake --build build --target install
```

In your `CMakeLists.txt`:

```cmake
find_package(FFmpegPipeEncoder)
target_link_libraries(YOUR_PROJECT PRIVATE FFmpegPipeEncoder)
```

## Example Usage

There is only one header to include.

```cpp
#include <FFmpegPipeEncoder/PipeEncoder.hpp>

EncoderSettings settings = EncoderSettingsBuilder().resolution(w, h).build();
PipeEncoder encoder(settings);
encoder.start();

unsigned char* data = GET_DATA_FROM_SOMEWHERE();
encoder.encode(data);

// Note: doesn't immediately finish encoding, only tells the encoder not to
// accept anymore frames.
encoder.stop(); 
```

### Raylib example

With [rlFastPixelReader](https://github.com/somecho/rlFastPixelReader):

```cpp
#include <raylib.h>
#include <rlgl.h>

#include <FFmpegPipeEncoder/PipeEncoder.hpp>
#include <rlFastPixelReader.hpp>

static int w = 1000;
static int h = 1000;

int main() {
  InitWindow(w, h, "Pipe Encoding");

  EncoderSettings settings = EncoderSettingsBuilder().resolution(w, h).build();
  PipeEncoder encoder(settings);

  RenderTexture2D fbo = LoadRenderTexture(w, h);
  rlFastPixelReader reader(w, h);
  unsigned int frameNum = 0;

  SetTargetFPS(60);
  encoder.start();

  while (!WindowShouldClose()) {
    BeginDrawing();

    BeginTextureMode(fbo);
    ClearBackground(BLACK);
    rlTranslatef(w * .5, h * .5, 0);
    rlRotatef(GetTime() * 20.0, 0, 0, 1);
    DrawPoly({0, 0}, 4, 300, 0, RED);
    EndTextureMode();

    if (frameNum < (60 * 10)) {
      reader.readPixels(fbo);
      auto data = reader.getData();
      encoder.encode(data);
    }
    if (frameNum > (60 * 10)) {
      encoder.stop();
    }

    DrawTextureRec(fbo.texture, {0, 0, (float)w, (float)h}, {0, 0}, WHITE);

    EndDrawing();
    frameNum++;
  }
}
```

### OpenFrameworks

With [`ofxFastFboReader`](https://github.com/satoruhiga/ofxFastFboReader):

```cpp
#include <FFmpegPipeEncoder/PipeEncoder.hpp>

EncoderSettings settings = EncoderSettingsBuilder().resolution(w, h).build();
PipeEncoder encoder(settings);
ofFbo fbo;
ofPixels pix;
ofxFastFboReader reader;

void setup(){
    fbo.allocate(ofGetWidth(), ofGetHeight());
    encoder.start();
}

void draw(){
    fbo.begin();
    ofClear(0,0,0,0);
    ofDrawCircle(ofGetWidth()*0.5, ofGetHeight()*0.5, 300);
    fbo.end();

    if(ofGetFrameNum() < (60 * 10)){
        reader.readToPixels(fbo, pix);
        encoder.encode(pix.getData());
    }
    if(ofGetFrameNum() > (60 * 10)){
        encoder.stop();
    }
}

```

--- 

MIT License, Copyright © 2024 Somē Cho