// CLEANED
#ifndef DIRECTORY_HPP
#define DIRECTORY_HPP

#include <qglobal.h>
#ifdef Q_OS_ANDROID
#include <QCoreApplication>
#include <QtCore/private/qandroidextras_p.h>
#endif

#include <QObject>
#include <QString>
#include <QUrl>
#include <QDir>
#include <QStringList>
#include <QSettings>
#include <QStandardPaths>
#include <QTemporaryDir>
#include <QFileDialog>

#include "Media.hpp"

class Directory : virtual public Media
{
public:
    explicit Directory(QObject *parent = nullptr);
    virtual ~Directory();

    void        addDir(QUrl path) override;
    QStringList getAudioPaths() override;
    void        loadSavedPaths() override;

    void        preparePathsForPlay();
    void        addStartupAudiosOnEmptyStartupAudioListings();
    void        openDialogFromCpp();
    void        pickIosAudiosFromSandboxTmpDir();
    void        doAddDir();

private:
    QSettings   backups_;

};

#endif // DIRECTORY_HPP
