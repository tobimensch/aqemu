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

#ifndef VNCVIEW_H
#define VNCVIEW_H

#include "remoteview.h"
#include "vncclientthread.h"

#ifdef QTONLY
    class KConfigGroup{};
#else
    #include "vnchostpreferences.h"
#endif

#ifdef LIBSSH_FOUND
    #include "vncsshtunnelthread.h"
#endif


#include <QClipboard>
#include <QMap>

extern "C" {
#include <rfb/rfbclient.h>
}

class VncView: public RemoteView
{
    Q_OBJECT

public:
    explicit VncView(QWidget *parent = nullptr, const QUrl &url = QUrl(), KConfigGroup configGroup = KConfigGroup());
    ~VncView() override;

    QSize framebufferSize() override;
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;
    void startQuitting() override;
    bool isQuitting() override;
    bool start() override;
    bool supportsScaling() const override;
    bool supportsLocalCursor() const override;
    bool supportsViewOnly() const override;
    
#ifndef QTONLY
    HostPreferences* hostPreferences() override;
#endif

    void setViewOnly(bool viewOnly) override;
    void showDotCursor(DotCursorState state) override;
    void enableScaling(bool scale) override;

    void updateConfiguration() override;

public Q_SLOTS:
    void scaleResize(int w, int h) override;

protected:
    void paintEvent(QPaintEvent *event) override;
    bool event(QEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    VncClientThread vncThread;
    QClipboard *m_clipboard;
    bool m_initDone;
    int m_buttonMask;
    QMap<unsigned int, bool> m_mods;
    bool m_quitFlag;
    bool m_firstPasswordTry;
    bool m_dontSendClipboard;
    qreal m_horizontalFactor;
    qreal m_verticalFactor;
#ifndef QTONLY
    VncHostPreferences *m_hostPreferences;
#endif
    QImage m_frame;
    bool m_forceLocalCursor;
#ifdef LIBSSH_FOUND
    VncSshTunnelThread *m_sshTunnelThread;

    QString readWalletSshPassword();
    void saveWalletSshPassword();
#endif

    void keyEventHandler(QKeyEvent *e);
    void unpressModifiers();
    void wheelEventHandler(QWheelEvent *event);
    void mouseEventHandler(QMouseEvent *event);

private Q_SLOTS:
    void updateImage(int x, int y, int w, int h);
    void setCut(const QString &text);
    void requestPassword(bool includingUsername);
#ifdef LIBSSH_FOUND
    void sshRequestPassword(VncSshTunnelThread::PasswordRequestFlags flags);
#endif
    void outputErrorMessage(const QString &message);
    void sshErrorMessage(const QString &message);
    void clipboardDataChanged();
};

#endif
