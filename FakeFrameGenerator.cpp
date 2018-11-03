#include "FakeFrameGenerator.h"
#include <QTimer>
#include <cstdlib>
#include "Util.h"

FakeFrameGenerator::FakeFrameGenerator(int w_in, int h_in, double fps, int nuniq)
    : w(w_in), h(h_in), t(nullptr), lastTime(-1.0)
{
    setObjectName("Fake Frame Generator");
    if (fps <= 0.0) fps = 1.0;
    if (fps > 1000.0) fps = 1000.0;
    if (w <= 0) w = 1;
    if (h <= 0) h = 1;
    if (nuniq <= 0) nuniq = 1;

    frames.reserve(nuniq);

    postLambdaSync([this, fps] {
        // run in thread...
        t = new QTimer(this);
        t->setSingleShot(false);
        t->setInterval(int(1000.0/fps));
        connect(t, SIGNAL(timeout()), this, SLOT(genFrame()));
        t->start();
    });
}

FakeFrameGenerator::~FakeFrameGenerator()
{
    postLambdaSync([this]{
        if (t) delete t;
        t = nullptr;
    });
}

void FakeFrameGenerator::genFrame()
{
    const double tNow = Util::getTimeSecs();
    QImage img2Send;

    if (frames.size() < frames.capacity()) {
        QImage img(w, h, QImage::Format_RGB444);

        for (int r = 0; r < h; ++r) {
            if (img.format() == QImage::Format_RGB444) {

                quint16 *line = reinterpret_cast<quint16 *>(img.scanLine(r));
                for (int c = 0; c < w; ++c) {
                    //                double r[3];
                    //                for (int i = 0; i < 3; ++i)
                    //                    r[i] = double(qrand())/double(RAND_MAX);
                    //                line[c] = qRgb(int(r[0]*255.0), int(r[1]*255.0), int(r[2]*255.0));
                    qint8 intensity = qint8((double(qrand())/double(RAND_MAX))*4.0);
                    line[c] = quint16(
                                  ( (qMax(intensity-1, 0) & 0xf) << 0) // blue
                                | ( (qMax(intensity-2, 0) & 0xf) << 4) // green
                                | ( ( intensity & 0xf )          << 8) // red
                                | (0x0<<12) ); // unused
                }

            } else if (img.format() == QImage::Format_ARGB32) {

                QRgb *line = reinterpret_cast<QRgb *>(img.scanLine(r));
                for (int c = 0; c < w; ++c) {
                    //                double r[3];
                    //                for (int i = 0; i < 3; ++i)
                    //                    r[i] = double(qrand())/double(RAND_MAX);
                    //                line[c] = qRgb(int(r[0]*255.0), int(r[1]*255.0), int(r[2]*255.0));
                    int intensity = int((double(qrand())/double(RAND_MAX))*48.0);
                    line[c] = qRgb(intensity, qMax(intensity-16, 0), qMax(intensity-8, 0));
                }

            }
        }
        frames.push_back(img);
        img2Send = img; // shallow copy
    } else {
        int which = int(double(qrand())/double(RAND_MAX) * frames.size());
        if (which < 0) which = 0;
        if (which >= frames.size()) which = frames.size()-1;
        img2Send = frames[which];
    }
    emit generatedFrame(img2Send);

    /*if (lastTime > 0.0) {
        double _fps = 1.0/(tNow-lastTime);
        emit fps(_fps);
        Debug() << "FPS=" << _fps;
    }*/
    lastTime = tNow;
}
