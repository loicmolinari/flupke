// Copyright © 2017-2018 Loïc Molinari <loicm@loicm.fr>
// Copyright © 2016 Canonical Ltd.
//
// This file is part of Quicken, licensed under the MIT license. See the license
// file at project root for full information.

#ifndef GPUTIMER_P_H
#define GPUTIMER_P_H

#include <QtGui/QOpenGLFunctions>

#if defined(QT_OPENGL_ES)
#include <EGL/egl.h>
#include <EGL/eglext.h>
#endif

#include <Quicken/private/quickenglobal_p.h>

// QuickenGPUTimer is used to measure the amount of time taken by the GPU to
// fully complete a set of graphics commands. As opposed to a basic timer which
// would determine the time taken by the graphics driver to push the graphics
// commands in the command buffer from the CPU, this timer pushes dedicated
// synchronization commands to the command buffer, which the GPU signals
// whenever completed. That allows to get accurate GPU timings.
class QUICKEN_PRIVATE_EXPORT QuickenGPUTimer
{
public:
    QuickenGPUTimer() :
#if !defined QT_NO_DEBUG
        m_context(nullptr), m_started(false),
#endif
        m_type(Unset) {}

    // Allocates/Deletes the OpenGL resources. finalize() is not called at
    // destruction, it must be explicitly called to free the resources at the
    // right time in a thread with the same OpenGL context bound than at
    // initialize().
    void initialize();
    void finalize();

    // Starts/Stops the timer. stop() returns the time elapsed in nanoseconds
    // since the call to start(). Calling start()/stop() two times in a row
    // triggers an assertion in debug builds and leads to undefined results in
    // non-debug builds. Must be called in a thread with the same OpenGL context
    // bound than at initialize().
    void start();
    quint64 stop();

private:
    enum Type {
        Unset,
        Finish,
#if defined(QT_OPENGL_ES)
        KHRFence,
        NVFence,
#else
        ARBTimerQuery,
        EXTTimerQuery
#endif
    };

#if !defined QT_NO_DEBUG
    QOpenGLContext* m_context;
    bool m_started;
#endif
    Type m_type;

#if defined(QT_OPENGL_ES)
    struct {
        void (QOPENGLF_APIENTRYP genFencesNV)(GLsizei n, GLuint* fences);
        void (QOPENGLF_APIENTRYP deleteFencesNV)(GLsizei n, const GLuint* fences);
        void (QOPENGLF_APIENTRYP setFenceNV)(GLuint fence, GLenum condition);
        void (QOPENGLF_APIENTRYP finishFenceNV)(GLuint fence);
    } m_fenceNV;
    GLuint m_fence[2];

    struct {
        EGLSyncKHR (QOPENGLF_APIENTRYP createSyncKHR)(EGLDisplay dpy, EGLenum type,
                                                      const EGLint* attrib_list);
        EGLBoolean (QOPENGLF_APIENTRYP destroySyncKHR)(EGLDisplay dpy, EGLSyncKHR sync);
        EGLint (QOPENGLF_APIENTRYP clientWaitSyncKHR)(EGLDisplay dpy, EGLSyncKHR sync, EGLint flags,
                                                      EGLTimeKHR timeout);
    } m_fenceSyncKHR;
    EGLSyncKHR m_beforeSync;

#else
    struct {
        void (QOPENGLF_APIENTRYP genQueries)(GLsizei n, GLuint* ids);
        void (QOPENGLF_APIENTRYP deleteQueries)(GLsizei n, const GLuint* ids);
        void (QOPENGLF_APIENTRYP beginQuery)(GLenum target, GLuint id);
        void (QOPENGLF_APIENTRYP endQuery)(GLenum target);
        void (QOPENGLF_APIENTRYP getQueryObjectui64v)(GLuint id, GLenum pname, GLuint64* params);
        void (QOPENGLF_APIENTRYP getQueryObjectui64vExt)(GLuint id, GLenum pname,
                                                         GLuint64EXT* params);
        void (QOPENGLF_APIENTRYP queryCounter)(GLuint id, GLenum target);
    } m_timerQuery;
    GLuint m_timer[2];
#endif
};

#endif  // GPUTIMER_P_H
