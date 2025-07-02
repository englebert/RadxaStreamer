// streamer.cpp
#include <gst/gst.h>
#include <iostream>
#include <sstream>
#include <cstring>
#include <cstdlib>

static void print_usage(const char *prog) {
    std::cerr <<
        "Usage: " << prog << " [options]\n"
        "  -cd   H264|H265         (default: H264)\n"
        "  -w    <width>           (default: 1280)\n"
        "  -h    <height>          (default: 720)\n"
        "  -fps  <framerate>       (default: 30)\n"
        "  -b    <bps>             (default: 6000000)\n";
    std::exit(1);
}

int main(int argc, char *argv[]) {
    // defaults
    std::string codec = "H264";
    int width        = 1280;
    int height       = 720;
    int fps          = 100;
    int bps          = 6000000;

    // parse args
    for (int i = 1; i < argc; ++i) {
        if (!std::strcmp(argv[i], "-cd")) {
            if (i + 1 < argc &&
                (!std::strcmp(argv[i+1], "H264") || !std::strcmp(argv[i+1], "H265")))
            {
                codec = argv[++i];
            } else {
                std::cerr << "Error: -cd must be H264 or H265\n";
                print_usage(argv[0]);
            }
        }
        else if (!std::strcmp(argv[i], "-w")) {
            if (i + 1 < argc) width = std::atoi(argv[++i]);
            else print_usage(argv[0]);
        }
        else if (!std::strcmp(argv[i], "-h")) {
            if (i + 1 < argc) height = std::atoi(argv[++i]);
            else print_usage(argv[0]);
        }
        else if (!std::strcmp(argv[i], "-fps")) {
            if (i + 1 < argc) fps = std::atoi(argv[++i]);
            else print_usage(argv[0]);
        }
        else if (!std::strcmp(argv[i], "-b")) {
            if (i + 1 < argc) bps = std::atoi(argv[++i]);
            else print_usage(argv[0]);
        }
    }

    // choose encoder & parser
    std::string encoder, parser;
    if (codec == "H264") {
        encoder = "mpph264enc";
        parser  = "h264parse config-interval=1";
    } else {
        encoder = "mpph265enc";
        parser  = "h265parse config-interval=1";
    }

    // build pipeline description
    // gst-launch-1.0   v4l2src device=/dev/video0 io-mode=2     ! video/x-raw,width=1280,height=720,framerate=30/1   ! videoconvert                   ! video/x-raw,format=NV12,width=1280,height=720   ! queue   ! mpph264enc rc-mode=vbr bps=6000000 bps-max=8000000  ! h264parse     ! filesink location=/tmp/ruby/fifocam1 sync=false
    std::ostringstream ss;
    ss << "v4l2src device=/dev/video0 io-mode=2 ! "
       << "video/x-raw,format=NV12,width=" << width
       << ",height=" << height
       << ",framerate=" << fps << "/1 ! "
       //// << "videoconvert ! video/x-raw,format=NV12,width=1280,height=720 !"
       << encoder
       << " rc-mode=vbr bps=1500000"
       << " bps-max=" << bps << " ! "
       // << " ! " << parser
       // << " ! mpegtsmux ! "
       << "queue max-size-time=200000000 leaky=downstream ! "
       << "filesink location=/tmp/ruby/fifocam1 sync=false";
       // << "filesink location=/tmp/ruby/fifocam1 sync=true";

    std::string pipeline_desc = ss.str();

    gst_init(&argc, &argv);
    GError *error = nullptr;
    GstElement *pipeline = gst_parse_launch(pipeline_desc.c_str(), &error);
    if (!pipeline) {
        std::cerr << "Failed to create pipeline: "
                  << (error ? error->message : "unknown error") << "\n";
        return -1;
    }

    std::cout << "Launching:\n  " << pipeline_desc << "\n";
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    // wait for EOS or error
    GstBus *bus = gst_element_get_bus(pipeline);
    gst_bus_timed_pop_filtered(
        bus,
        GST_CLOCK_TIME_NONE,
        static_cast<GstMessageType>(GST_MESSAGE_ERROR | GST_MESSAGE_EOS)
    );

    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(bus);
    gst_object_unref(pipeline);
    return 0;
}

