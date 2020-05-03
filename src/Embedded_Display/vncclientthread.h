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

#ifndef VNCCLIENTTHREAD_H
#define VNCCLIENTTHREAD_H

#ifdef QTONLY
    #define i18n tr
#else
    #include <KLocalizedString>
#endif

#include "remoteview.h"

#include <QQueue>
#include <QThread>
#include <QImage>
#include <QMutex>

extern "C" {
#include <rfb/rfbclient.h>
}

class ClientEvent
{
public:
    virtual ~ClientEvent();

    virtual void fire(rfbClient*) = 0;
};

class KeyClientEvent : public ClientEvent
{
public:
    KeyClientEvent(int key, int pressed)
            : m_key(key), m_pressed(pressed) {}

    void fire(rfbClient*) override;

private:
    int m_key;
    int m_pressed;
};

class PointerClientEvent : public ClientEvent
{
public:
    PointerClientEvent(int x, int y, int buttonMask)
            : m_x(x), m_y(y), m_buttonMask(buttonMask) {}

    void fire(rfbClient*) override;

private:
    int m_x;
    int m_y;
    int m_buttonMask;
};

class ClientCutEvent : public ClientEvent
{
public:
    explicit ClientCutEvent(const QString &text)
            : text(text) {}

    void fire(rfbClient*) override;

private:
    QString text;
};

class VncClientThread: public QThread
{
    Q_OBJECT

public:
    enum ColorDepth {
        bpp32,
        bpp16,
        bpp8
    };
    Q_ENUM(ColorDepth)

    explicit VncClientThread(QObject *parent = nullptr);
    ~VncClientThread() override;
    const QImage image(int x = 0, int y = 0, int w = 0, int h = 0);
    void setImage(const QImage &img);
    void emitUpdated(int x, int y, int w, int h);
    void emitGotCut(const QString &text);
    void stop();
    void setHost(const QString &host);
    void setPort(int port);
    void setQuality(RemoteView::Quality quality);
    void setPassword(const QString &password) {
        m_password = password;
    }
    const QString password() const {
        return m_password;
    }
    void setUsername(const QString &username) {
        m_username = username;
    }
    const QString username() const {
        return m_username;
    }

    RemoteView::Quality quality() const;
    ColorDepth colorDepth() const;
    uint8_t *frameBuffer;

Q_SIGNALS:
    void imageUpdated(int x, int y, int w, int h);
    void gotCut(const QString &text);
    void passwordRequest(bool includingUsername = false);
    void outputErrorMessage(const QString &message);

    /**
     * When we connect/disconnect/reconnect/etc., this signal will be emitted.
     *
     * @param status            Is the client connected?
     * @param details           A sentence describing what happened.
     */
    void clientStateChanged(RemoteView::RemoteStatus status, const QString &details);

public Q_SLOTS:
    void mouseEvent(int x, int y, int buttonMask);
    void keyEvent(int key, bool pressed);
    void clientCut(const QString &text);

protected:
    void run() override;

private:
    void setClientColorDepth(rfbClient *cl, ColorDepth cd);
    void setColorDepth(ColorDepth colorDepth);

    // These static methods are callback functions for libvncclient. Each
    // of them calls back into the corresponding member function via some
    // TLS-based logic.
    static rfbBool newclientStatic(rfbClient *cl);
    static void updatefbStatic(rfbClient *cl, int x, int y, int w, int h);
    static void cuttextStatic(rfbClient *cl, const char *text, int textlen);
    static char *passwdHandlerStatic(rfbClient *cl);
    static rfbCredential *credentialHandlerStatic(rfbClient *cl, int credentialType);
    static void outputHandlerStatic(const char *format, ...);

    // Member functions corresponding to the above static methods.
    rfbBool newclient();
    void updatefb(int x, int y, int w, int h);
    void cuttext(const char *text, int textlen);
    char *passwdHandler();
    rfbCredential *credentialHandler(int credentialType);
    void outputHandler(const char *format, va_list args);

    QImage m_image;
    rfbClient *cl;
    QString m_host;
    QString m_password;
    QString m_username;
    int m_port;
    QMutex mutex;
    RemoteView::Quality m_quality;
    ColorDepth m_colorDepth;
    QQueue<ClientEvent* > m_eventQueue;
    //color table for 8bit indexed colors
    QVector<QRgb> m_colorTable;
    QString outputErrorMessageString;

    volatile bool m_stopped;
    volatile bool m_passwordError;

    /**
     * Connection keepalive/reconnection support.
     */
    struct {
        /**
         * Number of seconds between probes. If zero, we will not attempt
         * to enable it.
         */
        int intervalSeconds;
        /**
         * Number of failed probes required to recognise a disconnect.
         */
        int failedProbes;
        /**
         * Was keepalive successfully set?
         */
        bool set;
        /**
         * Did keepalive detect a disconnect?
         */
        volatile bool failed;
    } m_keepalive;

    // Initialise the VNC client library object.
    bool clientCreate(bool reinitialising);

    // Uninitialise the VNC client library object.
    void clientDestroy();

    // Turn on keepalive support.
    void clientSetKeepalive();

    // Record a state change.
    void clientStateChange(RemoteView::RemoteStatus status, const QString &details);
    QString m_previousDetails;

private Q_SLOTS:
    void checkOutputErrorMessage();
};

#endif
