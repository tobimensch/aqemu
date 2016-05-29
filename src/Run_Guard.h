// this code was found on stackoverflow and later (slightly) modified
// http://stackoverflow.com/a/28172162
// Author: Dmitry Sazonov Date: edited Oct 29 '15 at 11:11 answered Jan 27 '15 at 13:53
// the license (for code posted at stackoverflow at that time) is CC BY-SA 3.0
// that ought to be gplv2 compatible according to the license text
// TODO: contact Dmitry Sazonov for explicit relicensing

#ifndef RUN_GUARD_H
#define RUN_GUARD_H

#include <QObject>
#include <QSharedMemory>
#include <QSystemSemaphore>


class Run_Guard
{

public:
    Run_Guard( const QString& key );
    ~Run_Guard();

    bool isAnotherRunning();
    bool tryToRun();
    void release();

private:
    QString generateKeyHash( const QString& key, const QString& salt );

    const QString key;
    const QString memLockKey;
    const QString sharedmemKey;

    QSharedMemory sharedMem;
    QSystemSemaphore memLock;

    Q_DISABLE_COPY( Run_Guard )
};


#endif // RUN_GUARD_H
