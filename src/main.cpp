#include <atomic>
#include <raylib.h>
#include <thread>
#include <string>
#include <plf_nanotimer.h>
#include <portable-file-dialogs.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_io.h>
#include <fmt/format.h>
#include <fmt/color.h>

#include <detect.c>
using namespace detect;

using namespace std;
using namespace dlib;
using namespace fmt;

#define NOFILE_TEXT "Press E to open an image!"
#define VERYDARKGRAY CLITERAL(Color){16, 16, 16, 255}
#define IDLE_TEXT "Opening..."

int main()
{
    env.state.store(NOFILE);
#ifndef DEBUG
    SetTraceLogLevel(LOG_NONE);
#endif

    InitWindow(env.window_width, env.window_height, "DLib Visualizer");
    env.detfn = detect::detect;

    while(!WindowShouldClose())
    {
        if(IsKeyDown(KEY_E))
        {
            env.state.store(NOFILE);
            SpawnDialog();
            env.image = LoadTextureFromImage(LoadImage(env.fpath.c_str()));

            env.window_height = env.image.height;
            env.window_width = env.image.width;
            SetWindowSize(env.window_width, env.window_height);

            env.state.store(DETECTING);

            thread detection_thread(env.detfn, env.fpath);
            detection_thread.detach();
        }

        BeginDrawing();
        {
            ClearBackground(VERYDARKGRAY);

            switch(env.state.load()) {
                case NOFILE:
                    BENCH(
                        "drawing 'NOFILE' text to window",
                        DrawText(
                            NOFILE_TEXT,
                            (env.window_width / 2.0) - (MeasureText(NOFILE_TEXT, 20) / 2.0),
                            env.window_height / 2.0,
                            20,
                            LIGHTGRAY
                        ), "render-nofile"
                    );
                break;

                case FINISHED:
                case DETECTING:
                {
                    BENCH(
                        "drawing texture",
                        DrawTexture(
                            env.image,
                            0, 0,
                            CLITERAL(Color) {255, 255, 255, 255}
                        ), "texture"
                    );

                    // Prevent catching trash from previous runs
                    if(env.state.load() == FINISHED)
                    {
                        if(env.is_empty)
                            DrawText("No faces detected", 10, 10, 20, RED);
                        else
                            for(rectangle r : env.hits)
                                DrawRectangleLines(r.left() / 2, r.top() / 2, r.width() / 2, r.height() / 2, RED);
                    }
                    else
                    {
                        DrawText(
                            "Detecting...",
                            10, 10, 20,
                            LIGHTGRAY
                        );
                    }
                } break;
            }
        }
        EndDrawing();
    }
}
