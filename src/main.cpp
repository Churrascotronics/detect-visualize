#include <atomic>
#include <raylib.h>
#include <thread>
#include <string>
#include <vector>
#include <plf_nanotimer.h>
#include <portable-file-dialogs.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_io.h>
#include <fmt/format.h>
#include <fmt/color.h>

using namespace std;
using namespace dlib;
using namespace fmt;

#define NOFILE_TEXT "Press E to open an image!"
#define VERYDARKGRAY CLITERAL(Color){16, 16, 16, 255}
#define IDLE_TEXT "Opening..."

enum AppState
{
    NOFILE,
    DETECTING,
    FINISHED
};

struct Environment
{
    bool is_empty = false;
    std::atomic<AppState> state;

    plf::nanotimer shared_timer;

    std::vector<rectangle> hits = {};
    frontal_face_detector detector = get_frontal_face_detector(); // Rather get this at startup than runtime :3

    Texture2D image;
    string fpath;
    int window_height = 400;
    int window_width = 600;

    void (*detfn)(string);
} env;

void bench_end(std::string passage, std::string id, plf::nanotimer& tim )
{
    static std::vector<std::string> past = {};

    if(id != "none")
    {
        // if ID was already benchmarked
        // here to prevent recalling this function in repetitive contexts
        // like rendering images and such
        if(count(past.begin(), past.end(), id) != 0) return;
        past.push_back(id);
    }

    auto timepass = tim.get_elapsed_ms();
    std::string timescale("ms");

    if(timepass > 1000)
    {
        timepass /= 1000;
        timescale = "S";
        cout << format(emphasis::bold, format("[!] BENCH: Spent {}{} {}\n", timepass, timescale, passage)) << endl;
        return;
    }

    cout << format("BENCH: Spent {}{} {}", timepass, timescale, passage) << endl;
}

template<typename F>
void benchfn(std::string passage, F&& fn, std::string id = "none", plf::nanotimer& t = env.shared_timer)
{
#ifdef BENCH
    t.start();
    fn();
    bench_end(passage, id, t);
#endif
}

#define BENCH(pass, exp, ...) do { \
    benchfn(pass, [&]{exp;}, __VA_ARGS__);    \
} while(0);


void SpawnDialog()
{
    auto select = pfd::open_file("Select an image").result();
    if(select.empty()) return;

    // Sketchy ass pointer might get dropped
    env.fpath = select[0];
}

void detect(string path_copy)
{
    plf::nanotimer local_timer;
    array2d<unsigned char> img;

    BENCH("load_image",
        load_image(img, path_copy.c_str()),
        "none", local_timer
    );

    pyramid_up(img);

    BENCH("detector",
        env.hits = env.detector(img),
        "none", local_timer
    );

    if(env.hits.size() == 0) env.is_empty = true;
    env.state.store(FINISHED);
}

int main()
{
    env.state.store(NOFILE);
#ifndef DEBUG
    SetTraceLogLevel(LOG_NONE);
#endif

    InitWindow(env.window_width, env.window_height, "DLib Visualizer");
    env.detfn = detect;

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

            thread detection_thread(detect, env.fpath);
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
