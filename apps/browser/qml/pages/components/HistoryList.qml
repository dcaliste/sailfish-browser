/****************************************************************************
**
** Copyright (c) 2014 Jolla Ltd.
** Copyright (c) 2021 Open Mobile Platform LLC.
** Contact: Vesa-Matti Hartikainen <vesa-matti.hartikainen@jolla.com>
**
****************************************************************************/

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

import QtQuick 2.1
import Sailfish.Silica 1.0
import Sailfish.WebView.Popups 1.0

SilicaListView {
    id: view
    property string search
    property bool showDeleteButton
    property bool menuClosed

    signal load(string url, string title)

    // To prevent model to steal focus
    currentIndex: -1

    delegate: HistoryItem {
        id: historyDelegate
        menu: contextMenuComponent
        search: view.search
        showDeleteButton: view.showDeleteButton
        onMenuOpenChanged: view.menuClosed = !menuOpen

        Component {
            id: contextMenuComponent

            ContextMenu {
                MenuItem {
                    //: Share link from browser history pulley menu
                    //% "Share"
                    text: qsTrId("sailfish_browser-me-share-link")
                    onClicked: webShareAction.shareLink(model.url, model.title)
                }
                MenuItem {
                    //% "Copy to clipboard"
                    text: qsTrId("sailfish_browser-me-copy-to-clipboard")
                    onClicked: Clipboard.text = model.url
                }

                MenuItem {
                    //: Delete history entry
                    //% "Delete"
                    text: qsTrId("sailfish_browser-me-delete")
                    onClicked: historyDelegate.remove(model.url)
                }
            }
        }
    }

    WebShareAction {
        id: webShareAction
    }

    VerticalScrollDecorator {}
}
