#include <iostream>
#include <fmt/core.h>
#include <dlib/pixel.h>
#include <dlib/image_io.h>
#include <dlib/image_loader/load_image.h>
#include <dlib/image_processing/frontal_face_detector.h>

#define USAGE \
    "Usage: {} <filename>"

using namespace std;
using namespace dlib;

int main(int argc, char** argv) {
    if(argc == 1) {
        cout << fmt::format(USAGE, argv[0]) << endl;
        return 0;
    }
    
    array2d<rgb_pixel> img;
    load_image(img, argv[1]);
    auto detector = get_frontal_face_detector();
    auto hits = detector(img);
    
    if(hits.size() == 0) {
        cout << "No faces found" << endl;
        return 0;
    }

    cout << "Detections are in order of: TOP, LEFT | BOTTOM, RIGHT" << endl;

    int iteration = 0;
    for(auto hit : hits) {
        cout << fmt::format("[{}]: {}, {} | {}, {}", iteration++, hit.top(), hit.left(), hit.bottom(), hit.right()) << endl;
    }    
}
