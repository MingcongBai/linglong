/*
 * Copyright (c) 2020-2021. Uniontech Software Ltd. All rights reserved.
 *
 * Author:     Iceyer <me@iceyer.net>
 *
 * Maintainer: Iceyer <me@iceyer.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <elf.h>
#include <sys/stat.h>
#include <cstring>
#include <unistd.h>

#include <QFile>
#include <QFileInfo>
#include <QString>
#include <QDir>

#include "module/util/result.h"
#include "module/util/fs.h"
#include "service/impl/dbus_retcode.h"
#include "module/util/httpclient.h"
#include "info.h"

using linglong::dbus::RetCode;
using namespace linglong::util;

namespace linglong {
namespace package {

// __LITTLE_ENDIAN or __BIG_ENDIAN
#if __BYTE_ORDER == __LITTLE_ENDIAN
#define ELFDATANATIVE ELFDATA2LSB
#elif __BYTE_ORDER == __BIG_ENDIAN
#define ELFDATANATIVE ELFDATA2MSB
#else
#error "Unknown machine endian"
#endif

// 16 bit system binary  swap
#define bswap16(value) ((((value)&0xff) << 8) | ((value) >> 8))
// 32 bit system binary  swap
#define bswap32(value) \
    (((uint32_t)bswap16((uint16_t)((value)&0xffff)) << 16) | (uint32_t)bswap16((uint16_t)((value) >> 16)))
// 64 bit system binary  swap
#define bswap64(value) \
    (((uint64_t)bswap32((uint32_t)((value)&0xffffffff)) << 32) | (uint64_t)bswap32((uint32_t)((value) >> 32)))

// FIXME: there is some problem that in module/util/runner.h, replace later
util::Result runner(const QString &program, const QStringList &args, int timeout = -1);

class BundlePrivate;

/*
 * Bundle
 * Create Bundle format file, An Bundle contains loader, and it's squashfs.
 *
 */
class Bundle : public QObject
{
    Q_OBJECT

public:
    explicit Bundle(QObject *parent = nullptr);
    ~Bundle();

    /**
     * Load Bundle from path, create parent if not exist
     * @param path
     * @return
     */
    util::Result load(const QString &path);

    /**
     * Save Bundle to path, create parent if not exist
     * @param path
     * @return
     */
    util::Result save(const QString &path);

    // Info info() const;

    /**
     * make Bundle
     * @param dataPath : data path
     * @param outputFilePath : output file path
     * @return Result
     */
    util::Result make(const QString &dataPath, const QString &outputFilePath);

    /**
     * push Bundle
     * @param uabFilePath : uab file path
     * @param force :  force to push
     * @return Result
     */
    util::Result push(const QString &bundleFilePath, bool force);

private:
    QScopedPointer<BundlePrivate> dd_ptr;
    Q_DECLARE_PRIVATE_D(qGetPtrHelper(dd_ptr), Bundle)
};

} // namespace package
} // namespace linglong