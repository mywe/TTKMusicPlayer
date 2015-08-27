#ifndef MUSICMYDOWNLOADRECORDOBJECT_H
#define MUSICMYDOWNLOADRECORDOBJECT_H

/* =================================================
 * This file is part of the Music Player project
 * Copyright (c) 2014 - 2015 Greedysky Studio
 * All rights reserved!
 * Redistribution and use of the source code or any derivative
 * works are strictly forbiden.
   =================================================*/

#include "musicxmlabstract.h"

class MUSIC_CORE_EXPORT MusicMyDownloadRecordObject : public MusicXmlAbstract
{
public:
    explicit MusicMyDownloadRecordObject(QObject *parent = 0);

    inline bool readDownloadXMLConfig(){ return readConfig(DOWNLOADINFO); }
    void writeDownloadConfig(const QStringList &names,
                             const QStringList &paths);
    void readDownloadConfig(QStringList &names, QStringList &paths);

};

#endif // MUSICMYDOWNLOADRECORDOBJECT_H
