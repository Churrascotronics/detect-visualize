#include <atomic>
#include <raylib.h>
#include <thread>
#include <string>
#include <vector>
#include <plf_nanotimer.h>
#include <portable-file-dialogs.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_io.h>
#include <fmt/core.h>

using namespace std;
using namespace dlib;
using namespace fmt;

#define NOFILE_TEXT "Press E to open an image!"
#define VERYDARKGRAY CLITERAL(Color){16, 16, 16, 255}
#define IDLE_TEXT "Opening..."

std::ostream& bold_on(std::ostream& os)  { return os << "\e[1m"; }
std::ostream& bold_off(std::ostream& os) { return os << "\e[0m"; }

enum AppState
{
    NOFILE,
    WAIT_DIALOG,
    DETECTING,
    FINISHED
};

struct Environment
{
    bool is_empty = false;
    std::atomic<AppState> state;

    plf::nanotimer shared_timer;

    std::vector<rectangle> hits = {};
    frontal_face_detector detector = get_frontal_face_detector();

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
        cout << bold_on << format("[!] BENCH: Spent {}{} {}", timepass, timescale, passage) << bold_off << endl;
        return;
    }

    cout << format("BENCH: Spent {}{} {}", timepass, timescale, passage) << endl;
}

template<typename F>
void benchfn(std::string passage, F&& fn, std::string id = "none", plf::nanotimer& t = env.shared_timer)
{
    t.start();
    fn();
    bench_end(passage, id, t);
}

#define BENCH(pass, exp) do { \
    bencfn(pass, [&]{exp;});    \
} while(0);


void SpawnDialog()
{
    auto select = pfd::open_file("Select an image").result();
    if(select.empty()) return;

    // Sketchy ass pointer might get dropped
    env.fpath = select[0];

    //TODO: RlImgui with mama lizs chilli oil?
}

void detect(string path_copy)
{
    plf::nanotimer local_timer;
    array2d<unsigned char> img;

    benchfn("load_image",
            [&]{load_image(img, path_copy.c_str());}, "none", local_timer);

    pyramid_up(img);

    benchfn("detector",
            [&]{
                env.hits = env.detector(img);
            }, "none", local_timer);

    if(env.hits.size() == 0) env.is_empty = true;
    env.state.store(FINISHED);
}

int main()
{
    env.state.store(NOFILE);
    SetTraceLogLevel(LOG_NONE);

    InitWindow(env.window_width, env.window_height, "DLib Visualizer");
    env.detfn = detect;

    while(!WindowShouldClose())
    {
        if(IsKeyDown(KEY_ESCAPE)) CloseWindow();
        else if(IsKeyDown(KEY_E))
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
                    benchfn("drawing 'NOFILE' text to window",
                        []{
                            DrawText(
                                NOFILE_TEXT,
                                (env.window_width / 2.0) - (MeasureText(NOFILE_TEXT, 20) / 2.0),
                                env.window_height / 2.0,
                                20,
                                LIGHTGRAY
                            );
                        },
                    "render-nofile");
                break;

                case WAIT_DIALOG:
                    DrawText(
                        "Opening...",
                        (env.window_width / 2.0) - (MeasureText("Opening...", 20) / 2.0),
                        env.window_height / 2.0,
                        20,
                        LIGHTGRAY
                    );
                break;

                case FINISHED:
                case DETECTING:
                {
                    benchfn("drawing texture",
                        [] {
                            DrawTexture(
                            env.image,
                            0, 0,
                            CLITERAL(Color) {255, 255, 255, 255});
                        },
                    "texture");

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
