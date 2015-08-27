#ifndef MUSICSYSTEMTRAYMENU_H
#define MUSICSYSTEMTRAYMENU_H

/* =================================================
 * This file is part of the Music Player project
 * Copyright (c) 2014 - 2015 Greedysky Studio
 * All rights reserved!
 * Redistribution and use of the source code or any derivative
 * works are strictly forbiden.
   =================================================*/

#include <QMenu>
#include "musiclibexportglobal.h"

class QLabel;
class QWidgetAction;
class QToolButton;

class MUSIC_WIDGET_EXPORT MusicSystemTrayMenu : public QMenu
{
    Q_OBJECT
public:
    MusicSystemTrayMenu(QWidget *parent = 0);
    ~MusicSystemTrayMenu();

    void setLabelText(const QString &text) const;
    void showDesktopLrc(const QString &show) const;
    void showPlayStatus(bool status) const;

signals:
    void setShowDesktopLrc(bool status);
    void setWindowLockedChanged();

public slots:
    void showDesktopLrc();
    void lockDesktopLrc(bool lock);

protected:
    void createWidgetActions();

    QWidgetAction *m_widgetAction;
    QLabel *m_showText;
    QToolButton *m_previousPlay;
    QToolButton *m_nextPlay;
    QToolButton *m_PlayOrStop;
    QAction *m_showLrcAction;
    QAction *m_lockLrcAction;

};

#endif // MUSICSYSTEMTRAYMENU_H
