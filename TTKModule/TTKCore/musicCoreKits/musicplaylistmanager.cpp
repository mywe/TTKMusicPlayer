#include "musicplaylistmanager.h"
#include "musiclistconfigmanager.h"
#include "musictime.h"
#include "musicversion.h"
#include "musicmessagebox.h"

#include <QTextCodec>

MusicWPLConfigManager::MusicWPLConfigManager(QObject *parent)
    : MusicAbstractXml(parent)
{

}

void MusicWPLConfigManager::writeWPLXMLConfig(const MusicSongItems &musics, const QString &path)
{
    //Open wirte file
    if(musics.isEmpty() || !writeConfig(path))
    {
        return;
    }
    ///////////////////////////////////////////////////////
    const QDomNode &node = m_document->createProcessingInstruction("wpl", "version='1.0' encoding='UTF-8'");
    m_document->appendChild( node );
    ///////////////////////////////////////////////////////
    QDomElement musicPlayerDom = createRoot("smil");
    //Class A
    QDomElement headSettingDom = writeDom(musicPlayerDom, "head");
    QDomElement bodySettingDom = writeDom(musicPlayerDom, "body");
    //Class B
    writeDomElementMutil(headSettingDom, "meta", MusicXmlAttributes() << MusicXmlAttribute("name", "Generator") <<
                         MusicXmlAttribute("content", QString("%1 %2").arg(APP_NAME).arg(TTKMUSIC_VERSION_STR)));
//    writeDomElementMutil(headSettingDom, "meta", MusicXmlAttributes() << MusicXmlAttribute("name", "ItemCount") <<
//                         MusicXmlAttribute("content", QString("%1").arg(musics.count())));
    for(int i=0; i<musics.count(); ++i)
    {
        const MusicSongItem &item = musics[i];
        //Class C
        QDomElement seqDom = writeDomElementMutil(bodySettingDom, "seq", MusicXmlAttributes()
                                              << MusicXmlAttribute("name", item.m_itemName) << MusicXmlAttribute("index", i)
                                              << MusicXmlAttribute("count", item.m_songs.count())
                                              << MusicXmlAttribute("sortIndex", item.m_sort.m_index)
                                              << MusicXmlAttribute("sortType", item.m_sort.m_sortType));
        foreach(const MusicSong &song, musics[i].m_songs)
        {
            writeDomElementMutil(seqDom, "media", MusicXmlAttributes() << MusicXmlAttribute("name", song.getMusicName())
                                 << MusicXmlAttribute("playCount", song.getMusicPlayCount())
                                 << MusicXmlAttribute("time", song.getMusicPlayTime())
                                 << MusicXmlAttribute("src", song.getMusicPath()));
        }
    }

    //Write to file
    QTextStream out(m_file);
    m_document->save(out, 4);
}

void MusicWPLConfigManager::readWPLXMLConfig(MusicSongItems &musics)
{
    bool state = false;
    const QDomNodeList &nodes = m_document->elementsByTagName("head");
    for(int i=0; i<nodes.count(); ++i)
    {
        const QDomNodeList &nodelist = nodes.at(i).childNodes();
        for(int i=0; i<nodelist.count(); ++i)
        {
            const QDomElement &element = nodelist.at(i).toElement();
            state = element.attribute("content").contains(APP_NAME);
            break;
        }
    }

    if(!state)
    {
        MusicPlayListManager::messageAlert();
        return;
    }

    const QDomNodeList &sepNodes = m_document->elementsByTagName("seq");
    for(int i=0; i<sepNodes.count(); ++i)
    {
        const QDomNode &node = sepNodes.at(i);
        MusicSongItem item;
        item.m_songs = readMusicFilePath(node);

        const QDomElement &element = node.toElement();
        item.m_itemIndex = element.attribute("index").toInt();
        item.m_itemName = element.attribute("name");

        const QString &string = element.attribute("sortIndex");
        item.m_sort.m_index = string.isEmpty() ? -1 : string.toInt();
        item.m_sort.m_sortType = MStatic_cast(Qt::SortOrder, element.attribute("sortType").toInt());
        musics << item;
    }
}

MusicSongs MusicWPLConfigManager::readMusicFilePath(const QDomNode &node) const
{
    const QDomNodeList &nodelist = node.childNodes();

    MusicSongs songs;
    for(int i=0; i<nodelist.count(); i++)
    {
        const QDomElement &element = nodelist.at(i).toElement();
        songs << MusicSong(element.attribute("src"),
                           element.attribute("playCount").toInt(),
                           element.attribute("time"),
                           element.attribute("name"));
    }
    return songs;
}



MusicXSPFConfigManager::MusicXSPFConfigManager(QObject *parent)
    : MusicAbstractXml(parent)
{

}

void MusicXSPFConfigManager::writeXSPFXMLConfig(const MusicSongItems &musics, const QString &path)
{
    //Open wirte file
    if(musics.isEmpty() || !writeConfig(path))
    {
        return;
    }
    ///////////////////////////////////////////////////////
    createProcessingInstruction();
    ///////////////////////////////////////////////////////
    QDomElement musicPlayerDom = createRoot("playlist");
    //Class A
    writeDomText(musicPlayerDom, "creator", APP_NAME);
    for(int i=0; i<musics.count(); ++i)
    {
        const MusicSongItem &item = musics[i];
        //Class A
        QDomElement trackListDom = writeDomElementMutil(musicPlayerDom, "trackList", MusicXmlAttributes()
                                                       << MusicXmlAttribute("name", item.m_itemName) << MusicXmlAttribute("index", i)
                                                       << MusicXmlAttribute("count", item.m_songs.count())
                                                       << MusicXmlAttribute("sortIndex", item.m_sort.m_index)
                                                       << MusicXmlAttribute("sortType", item.m_sort.m_sortType));

        foreach(const MusicSong &song, musics[i].m_songs)
        {
            //Class B
            QDomElement trackDom = writeDomElementMutil(trackListDom, "track", MusicXmlAttributes()
                                                        << MusicXmlAttribute("name", song.getMusicName())
                                                        << MusicXmlAttribute("playCount", song.getMusicPlayCount())
                                                        << MusicXmlAttribute("time", song.getMusicPlayTime())
                                                        << MusicXmlAttribute("src", song.getMusicPath()));
            writeDomText(trackDom, "location", song.getMusicPath());
            writeDomText(trackDom, "title", song.getMusicArtistBack());
            writeDomText(trackDom, "creator", song.getMusicArtistFront());
            writeDomText(trackDom, "annotation", QString());
            writeDomText(trackDom, "album", QString());
            writeDomText(trackDom, "trackNum", QString());
            writeDomElementText(trackDom, "meta", MusicXmlAttribute("rel", "year"), QString());
        }
    }

    //Write to file
    QTextStream out(m_file);
    m_document->save(out, 4);
}

void MusicXSPFConfigManager::readXSPFXMLConfig(MusicSongItems &musics)
{
    bool state = false;
    const QDomNodeList &nodes = m_document->elementsByTagName("playlist");
    for(int i=0; i<nodes.count(); ++i)
    {
        const QDomNodeList &nodelist = nodes.at(i).childNodes();
        for(int i=0; i<nodelist.count(); ++i)
        {
            const QDomNode &node = nodelist.at(i);
            if(node.nodeName() == "creator")
            {
                const QDomElement &element = node.toElement();
                state = element.text().contains(APP_NAME);
                break;
            }
        }
    }

    if(!state)
    {
        MusicPlayListManager::messageAlert();
        return;
    }

    const QDomNodeList &trackNodes = m_document->elementsByTagName("trackList");
    for(int i=0; i<trackNodes.count(); ++i)
    {
        const QDomNode &node = trackNodes.at(i);
        MusicSongItem item;
        item.m_songs = readMusicFilePath(node);

        const QDomElement &element = node.toElement();
        item.m_itemIndex = element.attribute("index").toInt();
        item.m_itemName = element.attribute("name");

        const QString &string = element.attribute("sortIndex");
        item.m_sort.m_index = string.isEmpty() ? -1 : string.toInt();
        item.m_sort.m_sortType = MStatic_cast(Qt::SortOrder, element.attribute("sortType").toInt());
        musics << item;
    }
}

MusicSongs MusicXSPFConfigManager::readMusicFilePath(const QDomNode &node) const
{
    const QDomNodeList &nodelist = node.childNodes();

    MusicSongs songs;
    for(int i=0; i<nodelist.count(); i++)
    {
        const QDomElement &element = nodelist.at(i).toElement();
        songs << MusicSong(element.attribute("src"),
                           element.attribute("playCount").toInt(),
                           element.attribute("time"),
                           element.attribute("name"));
    }
    return songs;
}



MusicASXConfigManager::MusicASXConfigManager(QObject *parent)
    : MusicAbstractXml(parent)
{

}

void MusicASXConfigManager::writeASXXMLConfig(const MusicSongItems &musics, const QString &path)
{
    //Open wirte file
    if(musics.isEmpty() || !writeConfig(path))
    {
        return;
    }
    ///////////////////////////////////////////////////////
    QDomElement musicPlayerDom = createRoot("asx", MusicXmlAttribute("version ", "3.0"));
    //Class A
    for(int i=0; i<musics.count(); ++i)
    {
        const MusicSongItem &item = musics[i];

        writeDomText(musicPlayerDom, "title", item.m_itemName);

        foreach(const MusicSong &song, musics[i].m_songs)
        {
            //Class B
            QDomElement trackDom = writeDom(musicPlayerDom, "entry");

            writeDomText(trackDom, "title", song.getMusicArtistBack());
            writeDomElement(trackDom, "ref", MusicXmlAttribute("href", song.getMusicPath()));

            writeDomText(trackDom, "author", APP_NAME);
            writeDomElementMutil(trackDom, "ttkitem", MusicXmlAttributes()
                                 << MusicXmlAttribute("name", song.getMusicName())
                                 << MusicXmlAttribute("playCount", song.getMusicPlayCount())
                                 << MusicXmlAttribute("time", song.getMusicPlayTime())
                                 << MusicXmlAttribute("src", song.getMusicPath()));

            writeDomElementMutil(trackDom, "ttklist", MusicXmlAttributes()
                                 << MusicXmlAttribute("name", item.m_itemName) << MusicXmlAttribute("index", i)
                                 << MusicXmlAttribute("count", item.m_songs.count())
                                 << MusicXmlAttribute("sortIndex", item.m_sort.m_index)
                                 << MusicXmlAttribute("sortType", item.m_sort.m_sortType));
        }
    }

    //Write to file
    QTextStream out(m_file);
    m_document->save(out, 4);
}

void MusicASXConfigManager::readASXXMLConfig(MusicSongItems &musics)
{
    bool state = false;
    const QDomNodeList &nodes = m_document->elementsByTagName("author");
    for(int i=0; i<nodes.count(); ++i)
    {
        const QDomNode &node = nodes.at(i);
        if(node.nodeName() == "author")
        {
            const QDomElement &element = node.toElement();
            state = element.text().contains(APP_NAME);
            break;
        }
    }

    if(!state)
    {
        MusicPlayListManager::messageAlert();
        return;
    }

    MusicSongItem item;
    const QDomNodeList &itemNodes = m_document->elementsByTagName("ttkitem");
    for(int i=0; i<itemNodes.count(); ++i)
    {
        const QDomElement &element = itemNodes.at(i).toElement();
        item.m_songs << MusicSong(element.attribute("src"),
                                  element.attribute("playCount").toInt(),
                                  element.attribute("time"),
                                  element.attribute("name"));
    }

    const QDomNodeList &listNodes = m_document->elementsByTagName("ttklist");
    if(!listNodes.isEmpty())
    {
        const QDomElement &element = listNodes.at(0).toElement();
        item.m_itemIndex = element.attribute("index").toInt();
        item.m_itemName = element.attribute("name");

        const QString &string = element.attribute("sortIndex");
        item.m_sort.m_index = string.isEmpty() ? -1 : string.toInt();
        item.m_sort.m_sortType = MStatic_cast(Qt::SortOrder, element.attribute("sortType").toInt());
        musics << item;
    }
}



MusicKWLConfigManager::MusicKWLConfigManager(QObject *parent)
    : MusicAbstractXml(parent)
{

}

bool MusicKWLConfigManager::readConfig(const QString &name)
{
    delete m_file;
    m_file = new QFile( name );
    if(!m_file->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return false;
    }

    QByteArray data(m_file->readAll());
    data.insert(0, "<?xml version=\"1.0\" encoding=\"gb2312\"?>\r\n");
    m_file->close();

    if(!m_file->open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return false;
    }
    m_file->write(data);
    m_file->close();

    return MusicAbstractXml::readConfig(name);
}

void MusicKWLConfigManager::readKWLXMLConfig(MusicSongItems &musics)
{
    MusicSongItem item;
    item.m_itemName = QFileInfo(m_file->fileName()).baseName();

    const QDomNodeList &nodes = m_document->elementsByTagName("so");
    for(int i=0; i<nodes.count(); ++i)
    {
        if(i == 0) //Skip root node
        {
            continue;
        }

        const QDomElement &element = nodes.at(i).toElement();
        const QDomNodeList &reslist = nodes.at(i).childNodes();
        for(int i=0; i<reslist.count(); i++)
        {
            const QDomElement &resElement = reslist.at(i).toElement();
            item.m_songs << MusicSong(resElement.attribute("p2pcachepath"),
                                      element.attribute("playedtimes").toInt(), "00:00",
                                      element.attribute("artist") + " - " +
                                      element.attribute("name"));
            break;
        }
    }

    if(!item.m_songs.isEmpty())
    {
        musics << item;
    }
}



MusicKGLConfigManager::MusicKGLConfigManager(QObject *parent)
    : MusicAbstractXml(parent)
{

}

void MusicKGLConfigManager::readKGLXMLConfig(MusicSongItems &musics)
{
    MusicSongItem item;
    item.m_itemName = QFileInfo(m_file->fileName()).baseName();

    QTextCodec *codec = QTextCodec::codecForName("windows-1252");
    const QDomNodeList &nodes = m_document->elementsByTagName("File");
    for(int i=0; i<nodes.count(); ++i)
    {
        MusicSong song;
        const QDomNodeList &cNodes = nodes.at(i).childNodes();
        for(int i=0; i<cNodes.count(); ++i)
        {
            const QDomNode &cNode = cNodes.at(i);
            if(cNode.nodeName() == "Duration")
            {
                song.setMusicPlayTime(MusicTime::msecTime2LabelJustified(cNode.toElement().text().toULongLong()));
            }
            else if(cNode.nodeName() == "FileName")
            {
                const QFileInfo info(codec->fromUnicode(cNode.toElement().text()));
                song.setMusicName(info.baseName());
                song.setMusicType(info.suffix());

                if(song.getMusicPath().isEmpty())
                {
                    song.setMusicPath(info.fileName());
                }
                else
                {
                    song.setMusicPath(song.getMusicPath() + info.fileName());
                }
            }
            else if(cNode.nodeName() == "FilePath")
            {
                const QString &path = codec->fromUnicode(cNode.toElement().text());
                if(song.getMusicName().isEmpty())
                {
                    song.setMusicPath(path);
                }
                else
                {
                    song.setMusicPath(path + song.getMusicName() + '.' + song.getMusicType());
                }
            }
            else if(cNode.nodeName() == "FileSize")
            {
                song.setMusicSize(cNode.toElement().text().toLongLong());
            }
        }
        item.m_songs << song;
    }

    if(!item.m_songs.isEmpty())
    {
        musics << item;
    }
}



void MusicPlayListManager::messageAlert()
{
    MusicMessageBox message;
    message.setText(QObject::tr("Unrecognized PlayList File!"));
    message.exec();
}

void MusicPlayListManager::setMusicSongItem(const QString &save, const MusicSongItem &item)
{
    const QFileInfo info(save);
    const QString &suffix = info.suffix().toLower();
    if(suffix == "lis")
    {
        writeLisList(save, item);
    }
    else if(suffix == "m3u" || suffix == "m3u8")
    {
        writeM3UList(save, item);
    }
    else if(suffix == "pls")
    {
        writePLSList(save, item);
    }
    else if(suffix == "wpl")
    {
        writeWPLList(save, item);
    }
    else if(suffix == "xspf")
    {
        writeXSPFList(save, item);
    }
    else if(suffix == "asx")
    {
        writeASXList(save, item);
    }
}

void MusicPlayListManager::getMusicSongItems(const QStringList &open, MusicSongItems &items)
{
    foreach(const QString &path, open)
    {
        const QFileInfo info(path);
        const QString &suffix = info.suffix().toLower();
        if(suffix == "lis")
        {
            readLisList(path, items);
        }
        else if(suffix == "m3u" || suffix == "m3u8")
        {
            readM3UList(path, items);
        }
        else if(suffix == "pls")
        {
            readPLSList(path, items);
        }
        else if(suffix == "wpl")
        {
            readWPLList(path, items);
        }
        else if(suffix == "xspf")
        {
            readXSPFList(path, items);
        }
        else if(suffix == "asx")
        {
            readASXList(path, items);
        }
        else if(suffix == "kwl")
        {
            readKWLList(path, items);
        }
        else if(suffix == "kgl")
        {
            readKGLList(path, items);
        }
    }
}

void MusicPlayListManager::readLisList(const QString &path, MusicSongItems &items)
{
    MusicListConfigManager manager;
    if(manager.readConfig(path))
    {
        manager.readMusicSongsConfig(items);
    }
}

void MusicPlayListManager::writeLisList(const QString &path, const MusicSongItem &item)
{
    MusicListConfigManager manager;
    manager.writeMusicSongsConfig(MusicSongItems() << item, path);
}

void MusicPlayListManager::readM3UList(const QString &path, MusicSongItems &items)
{
    QFile file(path);
    if(file.open(QFile::ReadOnly))
    {
        bool state = false;
        MusicSongItem item;
        const QStringList data(QString(file.readAll()).split("\n"));
        foreach(QString str, data)
        {
            str = str.trimmed();
            if(str.isEmpty())
            {
                continue;
            }
            else if(str.startsWith("#TTKM3U"))
            {
                state = true;
                continue;
            }
            else if(str.startsWith("#TTKNAME:"))
            {
                item.m_itemName = str.remove("#TTKNAME:");
            }
            else if(str.startsWith("#TTKTIT:"))
            {
                str = str.remove("#TTKTIT:");
                const QStringList &dds = str.split(TTK_STR_SPLITER);
                if(dds.count() == 3)
                {
                    item.m_itemIndex = dds[0].toInt();
                    item.m_sort.m_index = dds[1].toInt();
                    item.m_sort.m_sortType = MStatic_cast(Qt::SortOrder, dds[2].toInt());
                }
            }
            else if(str.startsWith("#TTKINF:"))
            {
                str = str.remove("#TTKINF:");
                const QStringList &dds = str.split(TTK_STR_SPLITER);
                if(dds.count() == 4)
                {
                    item.m_songs << MusicSong(dds[3], dds[0].toInt(), dds[2], dds[1]);
                }
            }
        }
        file.close();

        if(state)
        {
            items << item;
        }
        else
        {
            MusicPlayListManager::messageAlert();
        }
    }
    else
    {
        MusicPlayListManager::messageAlert();
    }
}

void MusicPlayListManager::writeM3UList(const QString &path, const MusicSongItem &item)
{
    QStringList data;
    data << QString("#TTKM3U");
    data << QString("#TTKNAME:%1").arg(item.m_itemName);
    data << QString("#TTKTIT:%2%1%3%1%4").arg(TTK_STR_SPLITER).arg(item.m_itemIndex)
                                         .arg(item.m_sort.m_index).arg(item.m_sort.m_sortType);
    foreach(const MusicSong &song, item.m_songs)
    {
        data.append(QString("#TTKINF:%2%1%3%1%4%1%5").arg(TTK_STR_SPLITER).arg(song.getMusicPlayCount())
                                                     .arg(song.getMusicName()).arg(song.getMusicPlayTime())
                                                     .arg(song.getMusicPath()));
        data.append(song.getMusicPath());
    }

    QFile file(path);
    if(file.open(QFile::WriteOnly))
    {
        file.write(data.join("\n").toUtf8());
        file.close();
    }
}

void MusicPlayListManager::readPLSList(const QString &path, MusicSongItems &items)
{
    QFile file(path);
    if(file.open(QFile::ReadOnly))
    {
        bool state = false;
        MusicSongItem item;
        QStringList data(QString(file.readAll()).split("\n"));
        if(!data.isEmpty() && data.takeFirst().toLower().contains("[playlist]"))
        {
            foreach(QString str, data)
            {
                str = str.trimmed();
                if(str.isEmpty())
                {
                    continue;
                }
                else if(str.startsWith("#TTKPLS"))
                {
                    state = true;
                    continue;
                }
                else if(str.startsWith("#TTKNAME:"))
                {
                    item.m_itemName = str.remove("#TTKNAME:");
                }
                else if(str.startsWith("#TTKTIT:"))
                {
                    str = str.remove("#TTKTIT:");
                    const QStringList &dds = str.split(TTK_STR_SPLITER);
                    if(dds.count() == 3)
                    {
                        item.m_itemIndex = dds[0].toInt();
                        item.m_sort.m_index = dds[1].toInt();
                        item.m_sort.m_sortType = MStatic_cast(Qt::SortOrder, dds[2].toInt());
                    }
                }
                else if(str.startsWith("#TTKINF:"))
                {
                    str = str.remove("#TTKINF:");
                    const QStringList &dds = str.split(TTK_STR_SPLITER);
                    if(dds.count() == 4)
                    {
                        item.m_songs << MusicSong(dds[3], dds[0].toInt(), dds[2], dds[1]);
                    }
                }
            }
        }
        file.close();

        if(state)
        {
            items << item;
        }
        else
        {
            MusicPlayListManager::messageAlert();
        }
    }
    else
    {
        MusicPlayListManager::messageAlert();
    }
}

void MusicPlayListManager::writePLSList(const QString &path, const MusicSongItem &item)
{
    QStringList data;
    data << QString("[playlist]");
    data << QString("#TTKPLS");
    data << QString("#TTKNAME:%1").arg(item.m_itemName);
    data << QString("#TTKTIT:%2%1%3%1%4").arg(TTK_STR_SPLITER).arg(item.m_itemIndex)
                                         .arg(item.m_sort.m_index).arg(item.m_sort.m_sortType);
    int count = 1;
    foreach(const MusicSong &song, item.m_songs)
    {
        data.append(QString("#TTKINF:%2%1%3%1%4%1%5").arg(TTK_STR_SPLITER).arg(song.getMusicPlayCount())
                                                     .arg(song.getMusicName()).arg(song.getMusicPlayTime())
                                                     .arg(song.getMusicPath()));
        data.append(QString("File%1=%2").arg(count).arg(song.getMusicPath()));
        data.append(QString("Title%1=%2").arg(count).arg(song.getMusicName()));
        data.append(QString("Length%1=%2").arg(count).arg(MusicTime::fromString(song.getMusicPlayTime(), "mm:ss").getTimeStamp(MusicTime::All_Sec)));
        ++count;
    }
    data << "NumberOfEntries=" + QString::number(item.m_songs.count());

    QFile file(path);
    if(file.open(QFile::WriteOnly))
    {
        file.write(data.join("\n").toUtf8());
        file.close();
    }
}

void MusicPlayListManager::readWPLList(const QString &path, MusicSongItems &items)
{
    MusicWPLConfigManager manager;
    if(manager.readConfig(path))
    {
        manager.readWPLXMLConfig(items);
    }
}

void MusicPlayListManager::writeWPLList(const QString &path, const MusicSongItem &item)
{
    MusicWPLConfigManager manager;
    manager.writeWPLXMLConfig(MusicSongItems() << item, path);
}

void MusicPlayListManager::readXSPFList(const QString &path, MusicSongItems &items)
{
    MusicXSPFConfigManager manager;
    if(manager.readConfig(path))
    {
        manager.readXSPFXMLConfig(items);
    }
}

void MusicPlayListManager::writeXSPFList(const QString &path, const MusicSongItem &item)
{
    MusicXSPFConfigManager manager;
    manager.writeXSPFXMLConfig(MusicSongItems() << item, path);
}

void MusicPlayListManager::readASXList(const QString &path, MusicSongItems &items)
{
    MusicASXConfigManager manager;
    if(manager.readConfig(path))
    {
        manager.readASXXMLConfig(items);
    }
}

void MusicPlayListManager::writeASXList(const QString &path, const MusicSongItem &item)
{
    MusicASXConfigManager manager;
    manager.writeASXXMLConfig(MusicSongItems() << item, path);
}

void MusicPlayListManager::readKWLList(const QString &path, MusicSongItems &items)
{
    MusicKWLConfigManager manager;
    if(manager.readConfig(path))
    {
        manager.readKWLXMLConfig(items);
    }
}

void MusicPlayListManager::readKGLList(const QString &path, MusicSongItems &items)
{
    MusicKGLConfigManager manager;
    if(manager.readConfig(path))
    {
        manager.readKGLXMLConfig(items);
    }
}
