/*
 * SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "linglong/builder/depend_fetcher.h"

#include "builder_config.h"
#include "module/repo/ostree_repo.h"
#include "project.h"

#include <QDir>

namespace linglong {
namespace builder {

class DependFetcherPrivate
{
public:
    explicit DependFetcherPrivate(QSharedPointer<const BuildDepend> bd, Project *parent)
        : ref(fuzzyRef(bd))
        , project(parent)
        , buildDepend(bd)
        , dependType(bd->type)
    {
    }

    // TODO: dependType should be removed, buildDepend include it
    package::Ref ref;
    Project *project;
    QSharedPointer<const BuildDepend> buildDepend;
    QString dependType;
};

DependFetcher::DependFetcher(QSharedPointer<const BuildDepend> bd, Project *parent)
    : QObject(parent)
    , dd_ptr(new DependFetcherPrivate(bd, parent))
{
}

DependFetcher::~DependFetcher() = default;

linglong::util::Error DependFetcher::fetch(const QString &subPath, const QString &targetPath)
{
    repo::OSTreeRepo ostree(BuilderConfig::instance()->repoPath(),
                            BuilderConfig::instance()->remoteRepoEndpoint,
                            BuilderConfig::instance()->remoteRepoName);

    // depends with source > depends from remote > depends from local
    auto dependRef = package::Ref("", dd_ptr->ref.appId, dd_ptr->ref.version, dd_ptr->ref.arch);

    if (!dd_ptr->buildDepend->source) {
        dependRef = package::Ref(BuilderConfig::instance()->remoteRepoName,
                                 "linglong",
                                 dd_ptr->ref.appId,
                                 dd_ptr->ref.version,
                                 dd_ptr->ref.arch,
                                 "");

        // FIXME(black_desk):
        // 1. We should not use ostree repo in ll-builder, we should use the
        //    repo interface;
        // 2. Offline should not only be an option of builder, but also a work
        //    mode argument passed to repo, which prevent all network request.
        // 3. For now we just leave these code here, we will refactor them later.
        if (BuilderConfig::instance()->getOffline()) {
            dependRef = ostree.localLatestRef(dependRef);

            qInfo() << QString("offline dependency: %1 %2")
                               .arg(dependRef.appId)
                               .arg(dependRef.version);
        } else {
            dependRef = ostree.remoteLatestRef(dependRef);

            qInfo() << QString("fetching dependency: %1 %2")
                               .arg(dependRef.appId)
                               .arg(dependRef.version);
            auto err = ostree.pullAll(dependRef, true);
            if (err) {
                return WrapError(err, "pull " + dependRef.toString() + " failed");
            }
        }
    }

    QDir targetParentDir(targetPath);
    targetParentDir.cdUp();
    targetParentDir.mkpath(".");

    {
        auto err = ostree.checkoutAll(dependRef, subPath, targetPath);

        if (err) {
            return WrapError(
                    err,
                    QString("ostree checkout %1 failed").arg(dependRef.toLocalRefString()));
        }
    }

    // for app,lib. if the dependType match runtime, should be submitted together.
    if (dd_ptr->dependType == DependTypeRuntime) {
        auto targetInstallPath = dd_ptr->project->config().cacheAbsoluteFilePath(
                { "overlayfs", "up", dd_ptr->project->config().targetInstallPath("") });
        {
            auto err = ostree.checkoutAll(dependRef, subPath, targetInstallPath);
            if (err) {
                return WrapError(err,
                                 QString("ostree checkout %1 with subpath '%2' to %3")
                                         .arg(dependRef.toLocalRefString())
                                         .arg(subPath)
                                         .arg(targetPath));
            }
        }
    }

    return {};
}

} // namespace builder
} // namespace linglong