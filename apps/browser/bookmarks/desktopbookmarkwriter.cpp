/****************************************************************************
**
** Copyright (c) 2014 Jolla Ltd.
** Contact: Raine Makelainen <raine.makelainen@jolla.com>
**
****************************************************************************/

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <QDir>
#include <QtConcurrent>

#include "desktopbookmarkwriter.h"
#include "browserpaths.h"

static bool dbw_testMode = false;

DesktopBookmarkWriter::DesktopBookmarkWriter(QObject *parent)
    : QObject(parent)
{
    connect(&m_writter, &QFutureWatcher<QString>::finished,
            this, &DesktopBookmarkWriter::desktopFileWritten);
}

DesktopBookmarkWriter::~DesktopBookmarkWriter()
{
    if (m_writter.isRunning()) {
        m_writter.waitForFinished();
    }
}

void DesktopBookmarkWriter::setTestModeEnabled(bool testMode)
{
    dbw_testMode = testMode;
}

bool DesktopBookmarkWriter::isTestModeEnabled()
{
    return dbw_testMode;
}

void DesktopBookmarkWriter::save(QString url, QString title, QString icon)
{
    if (url.trimmed().isEmpty() || title.trimmed().isEmpty()) {
        emit saved("");
        return;
    }

    if (icon.isEmpty()) {
        icon = DEFAULT_DESKTOP_BOOKMARK_ICON;
    }

    m_writter.setFuture(QtConcurrent::run(this, &DesktopBookmarkWriter::write, url, title, icon));
}

void DesktopBookmarkWriter::desktopFileWritten()
{
    QString path = m_writter.result();
    emit saved(path);
}

QString DesktopBookmarkWriter::uniqueDesktopFileName(QString title)
{
    QString filePath;
    if (!isTestModeEnabled()) {
        filePath = BrowserPaths::applicationsLocation();
    } else {
        filePath = BrowserPaths::dataLocation();
    }
    title = title.simplified().replace(QString(" "), QString("-"));

    QDir dir(filePath);
    dir.mkpath(filePath);

    dir.setNameFilters(QStringList() << QString("sailfish-browser-%2*").arg(title));
    QStringList similarlyNamedFiles = dir.entryList();
    int count = similarlyNamedFiles.count();

    QString fileName = QString(DESKTOP_FILE).arg(title).arg(count);
    while (similarlyNamedFiles.contains(fileName)) {
        ++count;
        fileName = QString(DESKTOP_FILE).arg(title).arg(count);
    }

    return QString(DESKTOP_FILE_PATTERN).arg(filePath, title).arg(count);
}

QString DesktopBookmarkWriter::write(const QString &url, const QString &title, const QString &icon)
{
    QString fileName = uniqueDesktopFileName(title);
    QString desktopFileData = QString("[Desktop Entry]\n" \
                                      "Type=Link\n" \
                                      "Name=%1\n" \
                                      "Icon=%2\n" \
                                      "URL=%3\n" \
                                      "Comment=%4\n").arg(title.trimmed(), icon,
                                                          url.trimmed(), title.trimmed());
    QFile desktopFile(fileName);
    if (desktopFile.open(QFile::WriteOnly)) {
        desktopFile.write(desktopFileData.toUtf8());
        desktopFile.flush();
        desktopFile.close();
        return fileName;
    } else {
        return "";
    }
}
