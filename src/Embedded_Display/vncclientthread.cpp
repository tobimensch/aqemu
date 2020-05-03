/****************************************************************************
**
** Copyright (C) 2007 - 2013 Urs Wolfer <uwolfer @ kde.org>
**
** This file is part of KDE.
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; see the file COPYING. If not, write to
** the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
** Boston, MA 02110-1301, USA.
**
****************************************************************************/

#include "vncclientthread.h"
#include "krdc_debug.h"

#include <cerrno>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <QMutexLocker>
#include <QThreadStorage>
#include <QTimer>

//for detecting intel AMT KVM vnc server
static const QString INTEL_AMT_KVM_STRING= QLatin1String("Intel(r) AMT KVM");
static QThreadStorage<VncClientThread **> instances;

// Dispatch from this static callback context to the member context.
rfbBool VncClientThread::newclientStatic(rfbClient *cl)
{
    VncClientThread *t = (VncClientThread *)rfbClientGetClientData(cl, nullptr);
    Q_ASSERT(t);

    return t->newclient();
}

// Dispatch from this static callback context to the member context.
void VncClientThread::updatefbStatic(rfbClient *cl, int x, int y, int w, int h)
{
    VncClientThread *t = (VncClientThread *)rfbClientGetClientData(cl, nullptr);
    Q_ASSERT(t);

    return t->updatefb(x, y, w, h);
}

// Dispatch from this static callback context to the member context.
void VncClientThread::cuttextStatic(rfbClient *cl, const char *text, int textlen)
{
    VncClientThread *t = (VncClientThread *)rfbClientGetClientData(cl, nullptr);
    Q_ASSERT(t);

    t->cuttext(text, textlen);
}

// Dispatch from this static callback context to the member context.
char *VncClientThread::passwdHandlerStatic(rfbClient *cl)
{
    VncClientThread *t = (VncClientThread *)rfbClientGetClientData(cl, nullptr);
    Q_ASSERT(t);

    return t->passwdHandler();
}

// Dispatch from this static callback context to the member context.
rfbCredential *VncClientThread::credentialHandlerStatic(rfbClient *cl, int credentialType)
{
    VncClientThread *t = (VncClientThread *)rfbClientGetClientData(cl, nullptr);
    Q_ASSERT(t);

    return t->credentialHandler(credentialType);
}

// Dispatch from this static callback context to the member context.
void VncClientThread::outputHandlerStatic(const char *format, ...)
{
    VncClientThread **t = instances.localData();

    va_list args;
    va_start(args, format);
    (*t)->outputHandler(format, args);
    va_end(args);
}

void VncClientThread::setClientColorDepth(rfbClient* cl, VncClientThread::ColorDepth cd)
{
    switch(cd) {
    case bpp8:
        if (m_colorTable.isEmpty()) {
            m_colorTable.resize(256);
            int r,g,b;
            for (int i = 0; i < 256; ++i) {
                //pick out the red (3 bits), green (3 bits) and blue (2 bits) bits and make them maximum significant in 8bits
                //this gives a colortable for 8bit true colors
                r= (i & 0x07) << 5;
                g= (i & 0x38) << 2;
                b= i & 0xc0;
                m_colorTable[i] = qRgb(r, g, b);
            }
        }
        cl->format.depth = 8;
        cl->format.bitsPerPixel = 8;
        cl->format.redShift = 0;
        cl->format.greenShift = 3;
        cl->format.blueShift = 6;
        cl->format.redMax = 7;
        cl->format.greenMax = 7;
        cl->format.blueMax = 3;
        break;
    case bpp16:
        cl->format.depth = 16;
        cl->format.bitsPerPixel = 16;
        cl->format.redShift = 11;
        cl->format.greenShift = 5;
        cl->format.blueShift = 0;
        cl->format.redMax = 0x1f;
        cl->format.greenMax = 0x3f;
        cl->format.blueMax = 0x1f;
        break;
    case bpp32:
    default:
        cl->format.depth = 24;
        cl->format.bitsPerPixel = 32;
        cl->format.redShift = 16;
        cl->format.greenShift = 8;
        cl->format.blueShift = 0;
        cl->format.redMax = 0xff;
        cl->format.greenMax = 0xff;
        cl->format.blueMax = 0xff;
    }
}

rfbBool VncClientThread::newclient()
{
    //8bit color hack for Intel(r) AMT KVM "classic vnc" = vnc server built in in Intel Vpro chipsets.
    if (INTEL_AMT_KVM_STRING == QLatin1String(cl->desktopName)) {
        qCDebug(KRDC) << "Intel(R) AMT KVM: switching to 8 bit color depth (workaround, recent libvncserver needed)";
        setColorDepth(bpp8);
    }
    setClientColorDepth(cl, colorDepth());

    const int width = cl->width, height = cl->height, depth = cl->format.bitsPerPixel;
    const int size = width * height * (depth / 8);
    if (frameBuffer)
        delete [] frameBuffer; // do not leak if we get a new framebuffer size
    frameBuffer = new uint8_t[size];
    cl->frameBuffer = frameBuffer;
    memset(cl->frameBuffer, '\0', size);

    switch (quality()) {
    case RemoteView::High:
        cl->appData.encodingsString = "copyrect zlib hextile raw";
        cl->appData.compressLevel = 0;
        cl->appData.qualityLevel = 9;
        break;
    case RemoteView::Medium:
        cl->appData.encodingsString = "copyrect tight zrle ultra zlib hextile corre rre raw";
        cl->appData.compressLevel = 5;
        cl->appData.qualityLevel = 7;
        break;
    case RemoteView::Low:
    case RemoteView::Unknown:
    default:
        cl->appData.encodingsString = "copyrect tight zrle ultra zlib hextile corre rre raw";
        cl->appData.compressLevel = 9;
        cl->appData.qualityLevel = 1;
    }

    SetFormatAndEncodings(cl);
    qCDebug(KRDC) << "Client created";
    return true;
}

void VncClientThread::updatefb(int x, int y, int w, int h)
{
//     qCDebug(KRDC) << "updated client: x: " << x << ", y: " << y << ", w: " << w << ", h: " << h;

    const int width = cl->width, height = cl->height;
    QImage img;
    switch(colorDepth()) {
    case bpp8:
        img = QImage(cl->frameBuffer, width, height, width, QImage::Format_Indexed8);
        img.setColorTable(m_colorTable);
        break;
    case bpp16:
        img = QImage(cl->frameBuffer, width, height, 2*width, QImage::Format_RGB16);
        break;
    case bpp32:
        img = QImage(cl->frameBuffer, width, height, 4*width, QImage::Format_RGB32);
        break;
    }

    if (img.isNull()) {
        qCDebug(KRDC) << "image not loaded";
    }

    if (m_stopped) {
        return; // sending data to a stopped thread is not a good idea
    }

    setImage(img);

    emitUpdated(x, y, w, h);
}

void VncClientThread::cuttext(const char *text, int textlen)
{
    const QString cutText = QString::fromUtf8(text, textlen);
    qCDebug(KRDC) << cutText;

    if (!cutText.isEmpty()) {
        emitGotCut(cutText);
    }
}

char *VncClientThread::passwdHandler()
{
    qCDebug(KRDC) << "password request";

    // Never request a password during a reconnect attempt.
    if (!m_keepalive.failed) {
        passwordRequest();
        m_passwordError = true;
    }
    return strdup(m_password.toUtf8().constData());
}

rfbCredential *VncClientThread::credentialHandler(int credentialType)
{
    qCDebug(KRDC) << "credential request" << credentialType;

    rfbCredential *cred = nullptr;

    switch (credentialType) {
        case rfbCredentialTypeUser:
            passwordRequest(true);
            m_passwordError = true;

            cred = new rfbCredential;
            cred->userCredential.username = strdup(username().toUtf8().constData());
            cred->userCredential.password = strdup(password().toUtf8().constData());
            break;
        default:
            qCritical(KRDC) << "credential request failed, unsupported credentialType:" << credentialType;
            outputErrorMessage(i18n("VNC authentication type is not supported."));
            break;
    }
    return cred;
}

void VncClientThread::outputHandler(const char *format, va_list args)
{
    QString message;
    message.vsprintf(format, args);

    message = message.trimmed();

    qCDebug(KRDC) << message;

    if ((message.contains(QLatin1String("Couldn't convert "))) ||
            (message.contains(QLatin1String("Unable to connect to VNC server")))) {
        // Don't show a dialog if a reconnection is needed. Never contemplate
        // reconnection if we don't have a password.
        QString tmp = i18n("Server not found.");
        if (m_keepalive.set && !m_password.isNull()) {
            m_keepalive.failed = true;
            if (m_previousDetails != tmp) {
                m_previousDetails = tmp;
                clientStateChange(RemoteView::Disconnected, tmp);
            }
        } else {
            outputErrorMessageString = tmp;
        }
    }

    // Process general authentication failures before more specific authentication
    // failures. All authentication failures cancel any auto-reconnection that
    // may be in progress.
    if (message.contains(QLatin1String("VNC connection failed: Authentication failed"))) {
        m_keepalive.failed = false;
        outputErrorMessageString = i18n("VNC authentication failed.");
    }
    if ((message.contains(QLatin1String("VNC connection failed: Authentication failed, too many tries"))) ||
            (message.contains(QLatin1String("VNC connection failed: Too many authentication failures")))) {
        m_keepalive.failed = false;
        outputErrorMessageString = i18n("VNC authentication failed because of too many authentication tries.");
    }

    if (message.contains(QLatin1String("VNC server closed connection")))
        outputErrorMessageString = i18n("VNC server closed connection.");

    // If we are not going to attempt a reconnection, at least tell the user
    // the connection went away.
    if (message.contains(QLatin1String("read ("))) {
        // Don't show a dialog if a reconnection is needed. Never contemplate
        // reconnection if we don't have a password.
        QString tmp = i18n("Disconnected: %1.", message.toLatin1());
        if (m_keepalive.set && !m_password.isNull()) {
            m_keepalive.failed = true;
            clientStateChange(RemoteView::Disconnected, tmp);
        } else {
            outputErrorMessageString = tmp;
        }
    }

    // internal messages, not displayed to user
    if (message.contains(QLatin1String("VNC server supports protocol version 3.889"))) // see https://bugs.kde.org/162640
        outputErrorMessageString = QLatin1String("INTERNAL:APPLE_VNC_COMPATIBILTY");
}

VncClientThread::VncClientThread(QObject *parent)
        : QThread(parent)
        , frameBuffer(nullptr)
        , cl(nullptr)
        , m_stopped(false)
{
    // We choose a small value for interval...after all if the connection is
    // supposed to sustain a VNC session, a reasonably frequent ping should
    // be perfectly supportable.
    m_keepalive.intervalSeconds = 1;
    m_keepalive.failedProbes = 3;
    m_keepalive.set = false;
    m_keepalive.failed = false;
    m_previousDetails = QString();
    outputErrorMessageString.clear(); //don't deliver error messages of old instances...
    QMutexLocker locker(&mutex);

    QTimer *outputErrorMessagesCheckTimer = new QTimer(this);
    outputErrorMessagesCheckTimer->setInterval(500);
    connect(outputErrorMessagesCheckTimer, SIGNAL(timeout()), this, SLOT(checkOutputErrorMessage()));
    outputErrorMessagesCheckTimer->start();
}

VncClientThread::~VncClientThread()
{
    if(isRunning()) {
        stop();
        terminate();
        const bool quitSuccess = wait(1000);
        qCDebug(KRDC) << "Attempting to stop in deconstructor, will crash if this fails:" << quitSuccess;
    }

    clientDestroy();

    delete [] frameBuffer;
}

void VncClientThread::checkOutputErrorMessage()
{
    if (!outputErrorMessageString.isEmpty()) {
        qCDebug(KRDC) << outputErrorMessageString;
        QString errorMessage = outputErrorMessageString;
        outputErrorMessageString.clear();
        // show authentication failure error only after the 3rd unsuccessful try
        if ((errorMessage != i18n("VNC authentication failed.")) || m_passwordError)
            outputErrorMessage(errorMessage);
    }
}

void VncClientThread::setHost(const QString &host)
{
    QMutexLocker locker(&mutex);
    m_host = host;
}

void VncClientThread::setPort(int port)
{
    QMutexLocker locker(&mutex);
    m_port = port;
}

void VncClientThread::setQuality(RemoteView::Quality quality)
{
    m_quality = quality;
    //set color depth dependent on quality
    switch(quality) {
    case RemoteView::Low:
        setColorDepth(bpp8);
        break;
    case RemoteView::High:
        setColorDepth(bpp32);
        break;
    case RemoteView::Medium:
    default:
        setColorDepth(bpp16);
    }
}

void VncClientThread::setColorDepth(ColorDepth colorDepth)
{
    m_colorDepth= colorDepth;
}

RemoteView::Quality VncClientThread::quality() const
{
    return m_quality;
}

VncClientThread::ColorDepth VncClientThread::colorDepth() const
{
    return m_colorDepth;
}

void VncClientThread::setImage(const QImage &img)
{
    QMutexLocker locker(&mutex);
    m_image = img;
}

const QImage VncClientThread::image(int x, int y, int w, int h)
{
    QMutexLocker locker(&mutex);

    if (w == 0) // full image requested
        return m_image;
    else
        return m_image.copy(x, y, w, h);
}

void VncClientThread::emitUpdated(int x, int y, int w, int h)
{
    emit imageUpdated(x, y, w, h);
}

void VncClientThread::emitGotCut(const QString &text)
{
    emit gotCut(text);
}

void VncClientThread::stop()
{
    QMutexLocker locker(&mutex);
    m_stopped = true;
}

void VncClientThread::run()
{
    QMutexLocker locker(&mutex);

    VncClientThread **threadTls = new VncClientThread *();
    *threadTls = this;
    instances.setLocalData(threadTls);
    while (!m_stopped) { // try to connect as long as the server allows
        locker.relock();
        m_passwordError = false;
        locker.unlock();

        if (clientCreate(false)) {
            // The initial connection attempt worked!
            break;
        }

        locker.relock();
        if (m_passwordError) {
            locker.unlock();
            // Try again.
            continue;
        }

        // The initial connection attempt failed, and not because of a
        // password problem. Bail out.
        m_stopped = true;
        locker.unlock();
    }

    locker.relock();
    qCDebug(KRDC) << "--------------------- Starting main VNC event loop ---------------------";
    while (!m_stopped) {
        locker.unlock();
        const int i = WaitForMessage(cl, 500);
        if (m_stopped || i < 0) {
            break;
        }
        if (i) {
            if (!HandleRFBServerMessage(cl)) {
                if (m_keepalive.failed) {
                    do {
                        // Reconnect after a short delay. That way, if the
                        // attempt fails very quickly, we don't sit in a very
                        // tight loop.
                        clientDestroy();
                        msleep(1000);
                        clientStateChange(RemoteView::Connecting, i18n("Reconnecting."));
                    } while (!clientCreate(true));
                    continue;
                }
                qCritical(KRDC) << "HandleRFBServerMessage failed";
                break;
            }
        }

        locker.relock();
        while (!m_eventQueue.isEmpty()) {
            ClientEvent* clientEvent = m_eventQueue.dequeue();
            locker.unlock();
            clientEvent->fire(cl);
            delete clientEvent;
            locker.relock();
        }
    }

    m_stopped = true;
}

/**
 * Factor out the initialisation of the VNC client library. Notice this has
 * both static parts as in @see rfbClientLog and @see rfbClientErr,
 * as well as instance local data @see rfbGetClient().
 *
 * On return from here, if cl is set, the connection will have been made else
 * cl will not be set.
 */
bool VncClientThread::clientCreate(bool reinitialising)
{
    rfbClientLog = outputHandlerStatic;
    rfbClientErr = outputHandlerStatic;

    //24bit color dept in 32 bits per pixel = default. Will change colordepth and bpp later if needed
    cl = rfbGetClient(8, 3, 4);
    setClientColorDepth(cl, this->colorDepth());
    cl->MallocFrameBuffer = newclientStatic;
    cl->canHandleNewFBSize = true;
    cl->GetPassword = passwdHandlerStatic;
    cl->GetCredential = credentialHandlerStatic;
    cl->GotFrameBufferUpdate = updatefbStatic;
    cl->GotXCutText = cuttextStatic;
    rfbClientSetClientData(cl, nullptr, this);

    cl->serverHost = strdup(m_host.toUtf8().constData());

    cl->serverPort = m_port;

    qCDebug(KRDC) << "--------------------- trying init ---------------------";

    if (!rfbInitClient(cl, nullptr, nullptr)) {
        if (!reinitialising) {
            // Don't whine on reconnection failure: presumably the network
            // is simply still down.
            qCritical(KRDC) << "rfbInitClient failed";
        }
        cl = nullptr;
        return false;
    }

    if (reinitialising) {
        clientStateChange(RemoteView::Connected, i18n("Reconnected."));
    } else {
        clientStateChange(RemoteView::Connected, i18n("Connected."));
    }
    clientSetKeepalive();
    return true;
}

/**
 * Undo @see clientCreate().
 */
void VncClientThread::clientDestroy()
{

    if (cl) {
        // Disconnect from vnc server & cleanup allocated resources
        rfbClientCleanup(cl);
        cl = nullptr;
    }
}

/**
 * The VNC client library does not make use of keepalives. We go behind its
 * back to set it up.
 */
void VncClientThread::clientSetKeepalive()
{
    // If keepalive is disabled, do nothing.
    m_keepalive.set = false;
    m_keepalive.failed = false;
    if (!m_keepalive.intervalSeconds) {
        return;
    }
    int optval;
    socklen_t optlen = sizeof(optval);

    // Try to set the option active
    optval = 1;
    if (setsockopt(cl->sock, SOL_SOCKET, SO_KEEPALIVE, &optval, optlen) < 0) {
        qCritical(KRDC) << "setsockopt(SO_KEEPALIVE)" << strerror(errno);
        return;
    }

    optval = m_keepalive.intervalSeconds;
    if (setsockopt(cl->sock, IPPROTO_TCP, TCP_KEEPIDLE, &optval, optlen) < 0) {
        qCritical(KRDC) << "setsockopt(TCP_KEEPIDLE)" << strerror(errno);
        return;
    }

    optval = m_keepalive.intervalSeconds;
    if (setsockopt(cl->sock, IPPROTO_TCP, TCP_KEEPINTVL, &optval, optlen) < 0) {
        qCritical(KRDC) << "setsockopt(TCP_KEEPINTVL)" << strerror(errno);
        return;
    }

    optval = m_keepalive.failedProbes;
    if(setsockopt(cl->sock, IPPROTO_TCP, TCP_KEEPCNT, &optval, optlen) < 0) {
        qCritical(KRDC) << "setsockopt(TCP_KEEPCNT)" << strerror(errno);
        return;
    }
    m_keepalive.set = true;
    qCDebug(KRDC) << "TCP keepalive set";
}

/**
 * The VNC client state changed.
 */
void VncClientThread::clientStateChange(RemoteView::RemoteStatus status, const QString &details)
{
    qCDebug(KRDC) << status << details << m_host << ":" << m_port;
    emit clientStateChanged(status, details);
}

ClientEvent::~ClientEvent()
{
}

void PointerClientEvent::fire(rfbClient* cl)
{
    SendPointerEvent(cl, m_x, m_y, m_buttonMask);
}

void KeyClientEvent::fire(rfbClient* cl)
{
    SendKeyEvent(cl, m_key, m_pressed);
}

void ClientCutEvent::fire(rfbClient* cl)
{
    QByteArray toUtf8Converted = text.toUtf8();
    SendClientCutText(cl, toUtf8Converted.data(), toUtf8Converted.length());
}

void VncClientThread::mouseEvent(int x, int y, int buttonMask)
{
    QMutexLocker lock(&mutex);
    if (m_stopped)
        return;

    m_eventQueue.enqueue(new PointerClientEvent(x, y, buttonMask));
}

void VncClientThread::keyEvent(int key, bool pressed)
{
    QMutexLocker lock(&mutex);
    if (m_stopped)
        return;

    m_eventQueue.enqueue(new KeyClientEvent(key, pressed));
}

void VncClientThread::clientCut(const QString &text)
{
    QMutexLocker lock(&mutex);
    if (m_stopped)
        return;

    m_eventQueue.enqueue(new ClientCutEvent(text));
}

//#include "moc_vncclientthread.cpp"
