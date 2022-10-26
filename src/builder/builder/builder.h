/*
 * Copyright (c) 2021. Uniontech Software Ltd. All rights reserved.
 *
 * Author:     Iceyer <me@iceyer.net>
 *
 * Maintainer: Iceyer <me@iceyer.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QObject>

#include "module/util/result.h"

namespace linglong {
namespace builder {

class Builder
{
public:
    virtual linglong::util::Error config(const QString &userName, const QString &password) = 0;

    virtual linglong::util::Error create(const QString &projectName) = 0;

    virtual linglong::util::Error build() = 0;

    virtual linglong::util::Error exportBundle(const QString &outputFilepath, bool useLocalDir) = 0;

    virtual util::Error push(const QString &repoUrl, const QString &repoName, const QString &channel,
                             bool pushWithDevel) = 0;

    virtual linglong::util::Error import() = 0;

    virtual linglong::util::Error run() = 0;
};

void registerAllMetaType();

} // namespace builder
} // namespace linglong
