/*
(c) 2009,2010,2011 Ian Clark

This file is part of Yasst.

Yasst is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Yasst is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Yasst.  If not, see <http://www.gnu.org/licenses/>.

*/
/* $Id: OptionsWindow.h,v 1.16 2010/07/20 20:08:15 ian Exp $
File:   OptionsWindow.h
(c) 2009 Ian Clark

*/
#ifndef OPTIONSWINDOW_H
#define OPTIONSWINDOW_H

#include <QtGui/QDialog>
#include "ui_OptionsWindow.h"
#include <QIntValidator>
#include <phonon/AudioOutput>
#include <phonon/MediaObject>
#include "backend/ImageCache.h"
#include "backend/fetchers/TweetFetcher.h"

class Ya_st;

class OptionsWindow : public QDialog
{
    Q_OBJECT

public:
    OptionsWindow(ImageCache *,QWidget *parent = 0);
    ~OptionsWindow();

    void showOpts();
	int initialTheme;
public slots:
	void setupFetcherWeights(TweetFetcher* =0);
signals:
	void optionsSaved();
protected:
	void closeEvent(QCloseEvent *);
private slots:
    void setOpts();
    void setFilePath();
    void setUserMainStylesheetPath();
    void setUserTextStylesheetPath();
    void clearFilePath();
    void testSound();
    void soundEnded();
    void handleThemeChange(int);
    void handleAccountClicked(QListWidgetItem*);
    void twitterUserChanged(QString);
    void twitterPassChanged(QString);
    void handleAddNewUser();
    void handleDeleteUser();
    void handleClearOAuth();
    void handleClearLang();
    void handleResetSettings();
    void handleRemoveAllSettings();
    void handleReloadUserStylesheet();
    void handleMinRefreshSliderMove(int);
private:
    Ui::OptionsWindowClass ui;
    QIntValidator *numValidator;
	Phonon::MediaObject *mediaObject;
	Phonon::AudioOutput *audioOutput;
	Phonon::Path path;
	bool playing;
	Ya_st *mainWindow;
	ImageCache *images;
	QList<QString> removedAccounts;
};

#endif // OPTIONSWINDOW_H
