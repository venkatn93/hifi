//
//  ScriptCache.cpp
//  libraries/script-engine/src
//
//  Created by Brad Hefta-Gaub on 2015-03-30
//  Copyright 2015 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#include <QCoreApplication>
#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkConfiguration>
#include <QNetworkReply>
#include <QObject>

#include <assert.h>
#include <NetworkAccessManager.h>
#include <SharedUtil.h>

#include "ScriptCache.h"
#include "ScriptEngineLogging.h"

ScriptCache::ScriptCache(QObject* parent) {
    // nothing to do here...
}

QString ScriptCache::getScript(const QUrl& url, ScriptUser* scriptUser, bool& isPending, bool reload) {
    assert(!_scriptCache.contains(url) || !reload);

    QString scriptContents;
    if (_scriptCache.contains(url) && !reload) {
        qCDebug(scriptengine) << "Found script in cache:" << url.toString();
        scriptContents = _scriptCache[url];
        scriptUser->scriptContentsAvailable(url, scriptContents);
        isPending = false;
    } else {
        isPending = true;
        bool alreadyWaiting = _scriptUsers.contains(url);
        _scriptUsers.insert(url, scriptUser);

        if (alreadyWaiting) {
            qCDebug(scriptengine) << "Already downloading script at:" << url.toString();
        } else {
            QNetworkAccessManager& networkAccessManager = NetworkAccessManager::getInstance();
            QNetworkRequest networkRequest = QNetworkRequest(url);
            networkRequest.setHeader(QNetworkRequest::UserAgentHeader, HIGH_FIDELITY_USER_AGENT);
            if (reload) {
                networkRequest.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysNetwork);
                qCDebug(scriptengine) << "Redownloading script at:" << url.toString();
            } else {
                qCDebug(scriptengine) << "Downloading script at:" << url.toString();
            }

            QNetworkReply* reply = networkAccessManager.get(networkRequest);
            connect(reply, &QNetworkReply::finished, this, &ScriptCache::scriptDownloaded);
        }
    }
    return scriptContents;
}

void ScriptCache::deleteScript(const QUrl& url) {
    if (_scriptCache.contains(url)) {
        qCDebug(scriptengine) << "Delete script from cache:" << url.toString();
        _scriptCache.remove(url);
    }
}

void ScriptCache::scriptDownloaded() {
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    QUrl url = reply->url();
    QList<ScriptUser*> scriptUsers = _scriptUsers.values(url);
    _scriptUsers.remove(url);

    if (reply->error() == QNetworkReply::NoError && reply->attribute(QNetworkRequest::HttpStatusCodeAttribute) == 200) {
        _scriptCache[url] = reply->readAll();
        qCDebug(scriptengine) << "Done downloading script at:" << url.toString();

        foreach(ScriptUser* user, scriptUsers) {
            user->scriptContentsAvailable(url, _scriptCache[url]);
        }
    } else {
        qCWarning(scriptengine) << "Error loading script from URL " << reply->url().toString()
            << "- HTTP status code is" << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt()
            << "and error from QNetworkReply is" << reply->errorString();
        foreach(ScriptUser* user, scriptUsers) {
            user->errorInLoadingScript(url);
        }
    }
    reply->deleteLater();
}


