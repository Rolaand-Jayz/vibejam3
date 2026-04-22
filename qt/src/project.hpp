#pragma once

#include <QDateTime>
#include <QJsonArray>
#include <QJsonObject>
#include <QString>
#include <QStringList>
#include <QVector>
#include <QUuid>

struct ProjectDraft {
    QString gameTitle;
    QString platform;
    QString guideIntent;
    QString playerGoal;
    QString runStyle;
    QString spoilerPolicy;
    QString depthPreference;
    QString sharedThemeCss;
};

struct Project {
    QString id;
    QString projectState;
    QString gameTitle;
    QString platform;
    QString guideIntent;
    QString playerGoal;
    QString runStyle;
    QString spoilerPolicy;
    QString depthPreference;
    QString sharedThemeCss;
    QString createdAt;

    [[nodiscard]] QString displayTitle() const
    {
        return gameTitle.trimmed().isEmpty() ? QStringLiteral("Untitled project") : gameTitle.trimmed();
    }

    [[nodiscard]] QString stateLabel() const
    {
        return projectState.trimmed().isEmpty() ? QStringLiteral("intake") : projectState.trimmed();
    }

    [[nodiscard]] QString summaryLine() const
    {
        return QStringLiteral("%1 • %2 • %3 spoilers")
            .arg(platform.trimmed().isEmpty() ? QStringLiteral("unspecified platform") : platform.trimmed(),
                 depthPreference.trimmed().isEmpty() ? QStringLiteral("standard") : depthPreference.trimmed(),
                 spoilerPolicy.trimmed().isEmpty() ? QStringLiteral("minimal") : spoilerPolicy.trimmed());
    }

    [[nodiscard]] QStringList unresolvedFields() const
    {
        QStringList unresolved;
        if (platform.trimmed().isEmpty()) {
            unresolved << QStringLiteral("platform / version");
        }
        if (playerGoal.trimmed().isEmpty()) {
            unresolved << QStringLiteral("player goal");
        }
        return unresolved;
    }

    [[nodiscard]] QString unresolvedSummary() const
    {
        const QStringList unresolved = unresolvedFields();
        return unresolved.isEmpty() ? QStringLiteral("none") : unresolved.join(QStringLiteral(", "));
    }

    [[nodiscard]] QString sharedThemeSummary() const
    {
        return sharedThemeCss.trimmed().isEmpty()
            ? QStringLiteral("default theme")
            : QStringLiteral("custom CSS defined");
    }

    [[nodiscard]] ProjectDraft toDraft() const
    {
        return ProjectDraft {
            .gameTitle = gameTitle,
            .platform = platform,
            .guideIntent = guideIntent,
            .playerGoal = playerGoal,
            .runStyle = runStyle,
            .spoilerPolicy = spoilerPolicy,
            .depthPreference = depthPreference,
            .sharedThemeCss = sharedThemeCss,
        };
    }

    [[nodiscard]] QJsonObject toJson() const
    {
        return QJsonObject {
            { QStringLiteral("id"), id },
            { QStringLiteral("projectState"), stateLabel() },
            { QStringLiteral("gameTitle"), gameTitle },
            { QStringLiteral("platform"), platform },
            { QStringLiteral("guideIntent"), guideIntent },
            { QStringLiteral("playerGoal"), playerGoal },
            { QStringLiteral("runStyle"), runStyle },
            { QStringLiteral("spoilerPolicy"), spoilerPolicy },
            { QStringLiteral("depthPreference"), depthPreference },
            { QStringLiteral("sharedThemeCss"), sharedThemeCss },
            { QStringLiteral("createdAt"), createdAt },
        };
    }

    [[nodiscard]] static Project fromJson(const QJsonObject &object)
    {
        Project project;
        project.id = object.value(QStringLiteral("id")).toString();
        project.projectState = object.value(QStringLiteral("projectState")).toString(QStringLiteral("intake"));
        project.gameTitle = object.value(QStringLiteral("gameTitle")).toString();
        project.platform = object.value(QStringLiteral("platform")).toString();
        project.guideIntent = object.value(QStringLiteral("guideIntent")).toString();
        project.playerGoal = object.value(QStringLiteral("playerGoal")).toString();
        project.runStyle = object.value(QStringLiteral("runStyle")).toString();
        project.spoilerPolicy = object.value(QStringLiteral("spoilerPolicy")).toString();
        project.depthPreference = object.value(QStringLiteral("depthPreference")).toString();
        project.sharedThemeCss = object.value(QStringLiteral("sharedThemeCss")).toString();
        project.createdAt = object.value(QStringLiteral("createdAt")).toString();
        return project;
    }

    [[nodiscard]] static Project fromDraft(const ProjectDraft &draft)
    {
        Project project;
        project.id = QUuid::createUuid().toString(QUuid::WithoutBraces);
        project.projectState = QStringLiteral("intake");
        project.gameTitle = draft.gameTitle.trimmed();
        project.platform = draft.platform.trimmed();
        project.guideIntent = draft.guideIntent.trimmed();
        project.playerGoal = draft.playerGoal.trimmed();
        project.runStyle = draft.runStyle.trimmed();
        project.spoilerPolicy = draft.spoilerPolicy.trimmed();
        project.depthPreference = draft.depthPreference.trimmed();
        project.sharedThemeCss = draft.sharedThemeCss;
        project.createdAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
        return project;
    }
};

struct BuildJob {
    QString id;
    QString projectId;
    QString jobType;
    QString status;
    int progress = 0;
    QString startedAt;
    QString endedAt;
    QString humanSummary;
    QString validationReportId;

    [[nodiscard]] QJsonObject toJson() const
    {
        return QJsonObject {
            { QStringLiteral("id"), id },
            { QStringLiteral("projectId"), projectId },
            { QStringLiteral("jobType"), jobType },
            { QStringLiteral("status"), status },
            { QStringLiteral("progress"), progress },
            { QStringLiteral("startedAt"), startedAt },
            { QStringLiteral("endedAt"), endedAt },
            { QStringLiteral("humanSummary"), humanSummary },
            { QStringLiteral("validationReportId"), validationReportId },
        };
    }

    [[nodiscard]] static BuildJob fromJson(const QJsonObject &object)
    {
        BuildJob job;
        job.id = object.value(QStringLiteral("id")).toString();
        job.projectId = object.value(QStringLiteral("projectId")).toString();
        job.jobType = object.value(QStringLiteral("jobType")).toString();
        job.status = object.value(QStringLiteral("status")).toString();
        job.progress = object.value(QStringLiteral("progress")).toInt();
        job.startedAt = object.value(QStringLiteral("startedAt")).toString();
        job.endedAt = object.value(QStringLiteral("endedAt")).toString();
        job.humanSummary = object.value(QStringLiteral("humanSummary")).toString();
        job.validationReportId = object.value(QStringLiteral("validationReportId")).toString();
        return job;
    }
};

struct BlueprintChapterPlan {
    QString title;
    QString purpose;

    [[nodiscard]] QJsonObject toJson() const
    {
        return QJsonObject {
            { QStringLiteral("title"), title },
            { QStringLiteral("purpose"), purpose },
        };
    }

    [[nodiscard]] static BlueprintChapterPlan fromJson(const QJsonObject &object)
    {
        return BlueprintChapterPlan {
            .title = object.value(QStringLiteral("title")).toString(),
            .purpose = object.value(QStringLiteral("purpose")).toString(),
        };
    }
};

struct BlueprintDraft {
    QVector<BlueprintChapterPlan> chapterPlans;
    QString terminologyRules;
    QString styleBible;
    QString crossReferencePlan;
    QString checklistPlan;
    QString visualPackIntent;
    QString spoilerGuardrails;
};

struct EditorialBlueprint {
    QString id;
    QString projectId;
    int version = 1;
    QVector<BlueprintChapterPlan> chapterPlans;
    QString terminologyRules;
    QString styleBible;
    QString crossReferencePlan;
    QString checklistPlan;
    QString visualPackIntent;
    QString spoilerGuardrails;
    QString createdAt;
    QString updatedAt;

    [[nodiscard]] QString summaryLine() const
    {
        return QStringLiteral("v%1 • %2 chapter plan%3")
            .arg(version)
            .arg(chapterPlans.size())
            .arg(chapterPlans.size() == 1 ? QString() : QStringLiteral("s"));
    }

    [[nodiscard]] QString chapterTitleSummary() const
    {
        QStringList titles;
        titles.reserve(chapterPlans.size());
        for (const BlueprintChapterPlan &chapterPlan : chapterPlans) {
            titles.push_back(chapterPlan.title.trimmed());
        }

        return titles.isEmpty() ? QStringLiteral("No chapter plans") : titles.join(QStringLiteral(" • "));
    }

    [[nodiscard]] BlueprintDraft toDraft() const
    {
        return BlueprintDraft {
            .chapterPlans = chapterPlans,
            .terminologyRules = terminologyRules,
            .styleBible = styleBible,
            .crossReferencePlan = crossReferencePlan,
            .checklistPlan = checklistPlan,
            .visualPackIntent = visualPackIntent,
            .spoilerGuardrails = spoilerGuardrails,
        };
    }

    [[nodiscard]] QJsonObject toJson() const
    {
        QJsonArray chapterPlanArray;
        for (const BlueprintChapterPlan &chapterPlan : chapterPlans) {
            chapterPlanArray.push_back(chapterPlan.toJson());
        }

        return QJsonObject {
            { QStringLiteral("id"), id },
            { QStringLiteral("projectId"), projectId },
            { QStringLiteral("version"), version },
            { QStringLiteral("chapterPlans"), chapterPlanArray },
            { QStringLiteral("terminologyRules"), terminologyRules },
            { QStringLiteral("styleBible"), styleBible },
            { QStringLiteral("crossReferencePlan"), crossReferencePlan },
            { QStringLiteral("checklistPlan"), checklistPlan },
            { QStringLiteral("visualPackIntent"), visualPackIntent },
            { QStringLiteral("spoilerGuardrails"), spoilerGuardrails },
            { QStringLiteral("createdAt"), createdAt },
            { QStringLiteral("updatedAt"), updatedAt },
        };
    }

    [[nodiscard]] static EditorialBlueprint fromJson(const QJsonObject &object)
    {
        EditorialBlueprint blueprint;
        blueprint.id = object.value(QStringLiteral("id")).toString();
        blueprint.projectId = object.value(QStringLiteral("projectId")).toString();
        blueprint.version = object.value(QStringLiteral("version")).toInt(1);

        const QJsonArray chapterPlanArray = object.value(QStringLiteral("chapterPlans")).toArray();
        for (const QJsonValue &value : chapterPlanArray) {
            blueprint.chapterPlans.push_back(BlueprintChapterPlan::fromJson(value.toObject()));
        }

        blueprint.terminologyRules = object.value(QStringLiteral("terminologyRules")).toString();
        blueprint.styleBible = object.value(QStringLiteral("styleBible")).toString();
        blueprint.crossReferencePlan = object.value(QStringLiteral("crossReferencePlan")).toString();
        blueprint.checklistPlan = object.value(QStringLiteral("checklistPlan")).toString();
        blueprint.visualPackIntent = object.value(QStringLiteral("visualPackIntent")).toString();
        blueprint.spoilerGuardrails = object.value(QStringLiteral("spoilerGuardrails")).toString();
        blueprint.createdAt = object.value(QStringLiteral("createdAt")).toString();
        blueprint.updatedAt = object.value(QStringLiteral("updatedAt")).toString();
        return blueprint;
    }
};

struct ValidationReport {
    QString id;
    QString projectId;
    QString targetType;
    QString targetId;
    QStringList checksRun;
    QStringList blockingFindings;
    QStringList warningFindings;
    int blockingCount = 0;
    int warningCount = 0;
    QString createdAt;
    QString summary;

    [[nodiscard]] QJsonObject toJson() const
    {
        QJsonArray checksArray;
        for (const QString &check : checksRun) {
            checksArray.push_back(check);
        }

        QJsonArray blockingArray;
        for (const QString &finding : blockingFindings) {
            blockingArray.push_back(finding);
        }

        QJsonArray warningArray;
        for (const QString &finding : warningFindings) {
            warningArray.push_back(finding);
        }

        return QJsonObject {
            { QStringLiteral("id"), id },
            { QStringLiteral("projectId"), projectId },
            { QStringLiteral("targetType"), targetType },
            { QStringLiteral("targetId"), targetId },
            { QStringLiteral("checksRun"), checksArray },
            { QStringLiteral("blockingFindings"), blockingArray },
            { QStringLiteral("warningFindings"), warningArray },
            { QStringLiteral("blockingCount"), blockingCount },
            { QStringLiteral("warningCount"), warningCount },
            { QStringLiteral("createdAt"), createdAt },
            { QStringLiteral("summary"), summary },
        };
    }

    [[nodiscard]] static ValidationReport fromJson(const QJsonObject &object)
    {
        ValidationReport report;
        report.id = object.value(QStringLiteral("id")).toString();
        report.projectId = object.value(QStringLiteral("projectId")).toString();
        report.targetType = object.value(QStringLiteral("targetType")).toString();
        report.targetId = object.value(QStringLiteral("targetId")).toString();

        const QJsonArray checksArray = object.value(QStringLiteral("checksRun")).toArray();
        for (const QJsonValue &value : checksArray) {
            report.checksRun.push_back(value.toString());
        }

        const QJsonArray blockingArray = object.value(QStringLiteral("blockingFindings")).toArray();
        for (const QJsonValue &value : blockingArray) {
            report.blockingFindings.push_back(value.toString());
        }

        const QJsonArray warningArray = object.value(QStringLiteral("warningFindings")).toArray();
        for (const QJsonValue &value : warningArray) {
            report.warningFindings.push_back(value.toString());
        }

        report.blockingCount = object.value(QStringLiteral("blockingCount")).toInt(report.blockingFindings.size());
        report.warningCount = object.value(QStringLiteral("warningCount")).toInt(report.warningFindings.size());
        report.createdAt = object.value(QStringLiteral("createdAt")).toString();
        report.summary = object.value(QStringLiteral("summary")).toString();
        return report;
    }
};
