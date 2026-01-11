https://github.com/user-attachments/assets/8f17f04f-4033-4e97-9566-14b5e70ea156

**MSDAW** (short for _Mewiof's Silly Digital Audio Workstation_) is a effortless/bug-filled experiment in building an Ableton/FL Studio-like app

**Solo playground**: _pull requests (especially those introducing new features) will likely be rejected_

## Features

-   **plugin hosting:** support for VST 2 instruments/effects
-   **built-in plugins:** Eq Eight, OTT, Delay & Reverb, Bit Crusher, other
-   **timeline engine:** audio/MIDI clips (with linking)
-   **track management:** hierarchical track grouping and routing
-   **automation:** parameter automation with curved tension
-   **mixing:** per-track volume, panning, solo/mute, live peak metering
-   **export:** offline rendering to 16-bit WAV files
-   **UI:** hardware-accelerated interface
-   **input:** computer MIDI keyboard

## Community

Chat with me: [https://pony.land/](https://pony.land/)

## Building

The project uses **CMake** & configured for Clang/Windows

Install [LLVM](https://github.com/llvm/llvm-project/releases) & [CMake](https://cmake.org/download/) (+ [Ninja](https://ninja-build.org/) optionally)

1. Clone with submodules: `git clone --recursive https://github.com/mewiof/MSDAW.git`
2. Open in Visual Studio/VSCode or use CMake:
    ```bash
    mkdir build
    cd build
    cmake ..
    ```

## Notices & Licenses

MSDAW is released under the [MIT License](LICENSE) and relies on:

-   **[SDL3](https://github.com/libsdl-org/SDL):** platform window management & input (zlib License)
-   **[Dear ImGui](https://github.com/ocornut/imgui):** user interface (MIT License)
-   **[RtAudio](https://github.com/thestk/rtaudio):** audio I/O abstraction (MIT-style License)
-   **[FreeType](https://github.com/freetype/freetype):** font engine (FreeType License/GPL)
-   **VST 2.4 SDK:** Plugin interface logic (Proprietary/Legacy Steinberg license; contact `mewiof@gmail.com` for instant removal)
