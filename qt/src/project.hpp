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

struct KnowledgeAttribute {
    QString key;
    QString value;

    [[nodiscard]] QJsonObject toJson() const
    {
        return QJsonObject {
            { QStringLiteral("key"), key },
            { QStringLiteral("value"), value },
        };
    }

    [[nodiscard]] static KnowledgeAttribute fromJson(const QJsonObject &object)
    {
        return KnowledgeAttribute {
            .key = object.value(QStringLiteral("key")).toString(),
            .value = object.value(QStringLiteral("value")).toString(),
        };
    }
};

struct SourceReference {
    QString id;
    QString sourceType;
    QString sourceTitle;
    QString sourceUri;
    QString excerpt;
    QString retrievalDate;
    QString trustClassification;

    [[nodiscard]] QJsonObject toJson() const
    {
        return QJsonObject {
            { QStringLiteral("id"), id },
            { QStringLiteral("sourceType"), sourceType },
            { QStringLiteral("sourceTitle"), sourceTitle },
            { QStringLiteral("sourceUri"), sourceUri },
            { QStringLiteral("excerpt"), excerpt },
            { QStringLiteral("retrievalDate"), retrievalDate },
            { QStringLiteral("trustClassification"), trustClassification },
        };
    }

    [[nodiscard]] static SourceReference fromJson(const QJsonObject &object)
    {
        return SourceReference {
            .id = object.value(QStringLiteral("id")).toString(),
            .sourceType = object.value(QStringLiteral("sourceType")).toString(),
            .sourceTitle = object.value(QStringLiteral("sourceTitle")).toString(),
            .sourceUri = object.value(QStringLiteral("sourceUri")).toString(),
            .excerpt = object.value(QStringLiteral("excerpt")).toString(),
            .retrievalDate = object.value(QStringLiteral("retrievalDate")).toString(),
            .trustClassification = object.value(QStringLiteral("trustClassification")).toString(),
        };
    }
};

struct ConflictRecord {
    QString id;
    QString conflictSummary;
    QString conflictType;
    QString severity;
    QString resolutionStatus;
    QString recommendedHandling;

    [[nodiscard]] QJsonObject toJson() const
    {
        return QJsonObject {
            { QStringLiteral("id"), id },
            { QStringLiteral("conflictSummary"), conflictSummary },
            { QStringLiteral("conflictType"), conflictType },
            { QStringLiteral("severity"), severity },
            { QStringLiteral("resolutionStatus"), resolutionStatus },
            { QStringLiteral("recommendedHandling"), recommendedHandling },
        };
    }

    [[nodiscard]] static ConflictRecord fromJson(const QJsonObject &object)
    {
        return ConflictRecord {
            .id = object.value(QStringLiteral("id")).toString(),
            .conflictSummary = object.value(QStringLiteral("conflictSummary")).toString(),
            .conflictType = object.value(QStringLiteral("conflictType")).toString(),
            .severity = object.value(QStringLiteral("severity")).toString(),
            .resolutionStatus = object.value(QStringLiteral("resolutionStatus")).toString(),
            .recommendedHandling = object.value(QStringLiteral("recommendedHandling")).toString(),
        };
    }
};

struct KnowledgeEntityDraft {
    QString canonicalName;
    QString category;
    QString visibility;
    QString scope;
    QString confidence;
    QStringList aliases;
    QString summary;
    QVector<KnowledgeAttribute> structuredAttributes;
    QVector<SourceReference> sourceRefs;
    QVector<ConflictRecord> conflictMarkers;
    QStringList versionTags;
};

struct KnowledgeEntity {
    QString id;
    QString projectId;
    QString canonicalName;
    QString category;
    QString visibility;
    QString scope;
    QString confidence;
    QStringList aliases;
    QString summary;
    QVector<KnowledgeAttribute> structuredAttributes;
    QVector<SourceReference> sourceRefs;
    QVector<ConflictRecord> conflictMarkers;
    QStringList versionTags;
    QString createdAt;
    QString updatedAt;

    [[nodiscard]] QString visibilityLabel() const
    {
        return visibility.trimmed() == QStringLiteral("hidden-codex")
            ? QStringLiteral("Hidden codex")
            : QStringLiteral("Visible guide");
    }

    [[nodiscard]] QString summaryLine() const
    {
        return QStringLiteral("%1 • %2 source%3 • %4 conflict%5 • %6")
            .arg(visibilityLabel())
            .arg(sourceRefs.size())
            .arg(sourceRefs.size() == 1 ? QString() : QStringLiteral("s"))
            .arg(conflictMarkers.size())
            .arg(conflictMarkers.size() == 1 ? QString() : QStringLiteral("s"))
            .arg(confidence.trimmed().isEmpty() ? QStringLiteral("medium") : confidence.trimmed());
    }

    [[nodiscard]] QString sourceTitleSummary() const
    {
        QStringList titles;
        titles.reserve(sourceRefs.size());
        for (const SourceReference &sourceRef : sourceRefs) {
            titles.push_back(sourceRef.sourceTitle.trimmed());
        }

        return titles.isEmpty() ? QStringLiteral("No source references") : titles.join(QStringLiteral(" • "));
    }

    [[nodiscard]] KnowledgeEntityDraft toDraft() const
    {
        return KnowledgeEntityDraft {
            .canonicalName = canonicalName,
            .category = category,
            .visibility = visibility,
            .scope = scope,
            .confidence = confidence,
            .aliases = aliases,
            .summary = summary,
            .structuredAttributes = structuredAttributes,
            .sourceRefs = sourceRefs,
            .conflictMarkers = conflictMarkers,
            .versionTags = versionTags,
        };
    }

    [[nodiscard]] QJsonObject toJson() const
    {
        QJsonArray attributeArray;
        for (const KnowledgeAttribute &attribute : structuredAttributes) {
            attributeArray.push_back(attribute.toJson());
        }

        QJsonArray sourceArray;
        for (const SourceReference &sourceRef : sourceRefs) {
            sourceArray.push_back(sourceRef.toJson());
        }

        QJsonArray conflictArray;
        for (const ConflictRecord &conflict : conflictMarkers) {
            conflictArray.push_back(conflict.toJson());
        }

        QJsonArray aliasArray;
        for (const QString &alias : aliases) {
            aliasArray.push_back(alias);
        }

        QJsonArray versionTagArray;
        for (const QString &versionTag : versionTags) {
            versionTagArray.push_back(versionTag);
        }

        return QJsonObject {
            { QStringLiteral("id"), id },
            { QStringLiteral("projectId"), projectId },
            { QStringLiteral("canonicalName"), canonicalName },
            { QStringLiteral("category"), category },
            { QStringLiteral("visibility"), visibility },
            { QStringLiteral("scope"), scope },
            { QStringLiteral("confidence"), confidence },
            { QStringLiteral("aliases"), aliasArray },
            { QStringLiteral("summary"), summary },
            { QStringLiteral("structuredAttributes"), attributeArray },
            { QStringLiteral("sourceRefs"), sourceArray },
            { QStringLiteral("conflictMarkers"), conflictArray },
            { QStringLiteral("versionTags"), versionTagArray },
            { QStringLiteral("createdAt"), createdAt },
            { QStringLiteral("updatedAt"), updatedAt },
        };
    }

    [[nodiscard]] static KnowledgeEntity fromJson(const QJsonObject &object)
    {
        KnowledgeEntity entity;
        entity.id = object.value(QStringLiteral("id")).toString();
        entity.projectId = object.value(QStringLiteral("projectId")).toString();
        entity.canonicalName = object.value(QStringLiteral("canonicalName")).toString();
        entity.category = object.value(QStringLiteral("category")).toString();
        entity.visibility = object.value(QStringLiteral("visibility")).toString();
        entity.scope = object.value(QStringLiteral("scope")).toString();
        entity.confidence = object.value(QStringLiteral("confidence")).toString();
        entity.summary = object.value(QStringLiteral("summary")).toString();
        entity.createdAt = object.value(QStringLiteral("createdAt")).toString();
        entity.updatedAt = object.value(QStringLiteral("updatedAt")).toString(entity.createdAt);

        const QJsonArray aliasArray = object.value(QStringLiteral("aliases")).toArray();
        for (const QJsonValue &value : aliasArray) {
            entity.aliases.push_back(value.toString());
        }

        const QJsonArray attributeArray = object.value(QStringLiteral("structuredAttributes")).toArray();
        for (const QJsonValue &value : attributeArray) {
            entity.structuredAttributes.push_back(KnowledgeAttribute::fromJson(value.toObject()));
        }

        const QJsonArray sourceArray = object.value(QStringLiteral("sourceRefs")).toArray();
        for (const QJsonValue &value : sourceArray) {
            entity.sourceRefs.push_back(SourceReference::fromJson(value.toObject()));
        }

        const QJsonArray conflictArray = object.value(QStringLiteral("conflictMarkers")).toArray();
        for (const QJsonValue &value : conflictArray) {
            entity.conflictMarkers.push_back(ConflictRecord::fromJson(value.toObject()));
        }

        const QJsonArray versionTagArray = object.value(QStringLiteral("versionTags")).toArray();
        for (const QJsonValue &value : versionTagArray) {
            entity.versionTags.push_back(value.toString());
        }

        return entity;
    }
};

struct PreviewSampleSection {
    QString title;
    QString kind;
    QString summary;
    QString body;

    [[nodiscard]] QJsonObject toJson() const
    {
        return QJsonObject {
            { QStringLiteral("title"), title },
            { QStringLiteral("kind"), kind },
            { QStringLiteral("summary"), summary },
            { QStringLiteral("body"), body },
        };
    }

    [[nodiscard]] static PreviewSampleSection fromJson(const QJsonObject &object)
    {
        return PreviewSampleSection {
            .title = object.value(QStringLiteral("title")).toString(),
            .kind = object.value(QStringLiteral("kind")).toString(),
            .summary = object.value(QStringLiteral("summary")).toString(),
            .body = object.value(QStringLiteral("body")).toString(),
        };
    }
};

struct PreviewPackage {
    QString id;
    QString projectId;
    int blueprintVersion = 1;
    int version = 1;
    QVector<PreviewSampleSection> sampleSections;
    QString reviewNotes;
    QString approvalState;
    QString generatedAt;
    QString updatedAt;

    [[nodiscard]] QString summaryLine() const
    {
        return QStringLiteral("v%1 • blueprint v%2 • %3 sample section%4")
            .arg(version)
            .arg(blueprintVersion)
            .arg(sampleSections.size())
            .arg(sampleSections.size() == 1 ? QString() : QStringLiteral("s"));
    }

    [[nodiscard]] QString reviewSummary() const
    {
        return reviewNotes.trimmed().isEmpty() ? QStringLiteral("No review notes recorded yet.") : reviewNotes.trimmed();
    }

    [[nodiscard]] QString sampleTitleSummary() const
    {
        QStringList titles;
        titles.reserve(sampleSections.size());
        for (const PreviewSampleSection &section : sampleSections) {
            titles.push_back(section.title.trimmed());
        }

        return titles.isEmpty() ? QStringLiteral("No preview sections") : titles.join(QStringLiteral(" • "));
    }

    [[nodiscard]] QJsonObject toJson() const
    {
        QJsonArray sampleSectionArray;
        for (const PreviewSampleSection &section : sampleSections) {
            sampleSectionArray.push_back(section.toJson());
        }

        return QJsonObject {
            { QStringLiteral("id"), id },
            { QStringLiteral("projectId"), projectId },
            { QStringLiteral("blueprintVersion"), blueprintVersion },
            { QStringLiteral("version"), version },
            { QStringLiteral("sampleSections"), sampleSectionArray },
            { QStringLiteral("reviewNotes"), reviewNotes },
            { QStringLiteral("approvalState"), approvalState },
            { QStringLiteral("generatedAt"), generatedAt },
            { QStringLiteral("updatedAt"), updatedAt },
        };
    }

    [[nodiscard]] static PreviewPackage fromJson(const QJsonObject &object)
    {
        PreviewPackage preview;
        preview.id = object.value(QStringLiteral("id")).toString();
        preview.projectId = object.value(QStringLiteral("projectId")).toString();
        preview.blueprintVersion = object.value(QStringLiteral("blueprintVersion")).toInt(1);
        preview.version = object.value(QStringLiteral("version")).toInt(1);

        const QJsonArray sampleSectionArray = object.value(QStringLiteral("sampleSections")).toArray();
        for (const QJsonValue &value : sampleSectionArray) {
            preview.sampleSections.push_back(PreviewSampleSection::fromJson(value.toObject()));
        }

        preview.reviewNotes = object.value(QStringLiteral("reviewNotes")).toString();
        preview.approvalState = object.value(QStringLiteral("approvalState")).toString(QStringLiteral("requires-review"));
        preview.generatedAt = object.value(QStringLiteral("generatedAt")).toString();
        preview.updatedAt = object.value(QStringLiteral("updatedAt")).toString(preview.generatedAt);
        return preview;
    }
};

struct GuideBuildUnit {
    QString id;
    QString title;
    QString kind;
    QString purpose;
    QString body;
    QStringList sourceKnowledgeIds;
    QStringList blockingFindings;
    QStringList warningFindings;

    [[nodiscard]] QJsonObject toJson() const
    {
        QJsonArray sourceKnowledgeArray;
        for (const QString &sourceKnowledgeId : sourceKnowledgeIds) {
            sourceKnowledgeArray.push_back(sourceKnowledgeId);
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
            { QStringLiteral("title"), title },
            { QStringLiteral("kind"), kind },
            { QStringLiteral("purpose"), purpose },
            { QStringLiteral("body"), body },
            { QStringLiteral("sourceKnowledgeIds"), sourceKnowledgeArray },
            { QStringLiteral("blockingFindings"), blockingArray },
            { QStringLiteral("warningFindings"), warningArray },
        };
    }

    [[nodiscard]] static GuideBuildUnit fromJson(const QJsonObject &object)
    {
        GuideBuildUnit unit;
        unit.id = object.value(QStringLiteral("id")).toString();
        unit.title = object.value(QStringLiteral("title")).toString();
        unit.kind = object.value(QStringLiteral("kind")).toString();
        unit.purpose = object.value(QStringLiteral("purpose")).toString();
        unit.body = object.value(QStringLiteral("body")).toString();

        const QJsonArray sourceKnowledgeArray = object.value(QStringLiteral("sourceKnowledgeIds")).toArray();
        for (const QJsonValue &value : sourceKnowledgeArray) {
            unit.sourceKnowledgeIds.push_back(value.toString());
        }

        const QJsonArray blockingArray = object.value(QStringLiteral("blockingFindings")).toArray();
        for (const QJsonValue &value : blockingArray) {
            unit.blockingFindings.push_back(value.toString());
        }

        const QJsonArray warningArray = object.value(QStringLiteral("warningFindings")).toArray();
        for (const QJsonValue &value : warningArray) {
            unit.warningFindings.push_back(value.toString());
        }

        return unit;
    }
};

struct GuideBuildBundle {
    QString id;
    QString projectId;
    QString previewId;
    int blueprintVersion = 1;
    int version = 1;
    QString validationState;
    QStringList visibleKnowledgeIds;
    QStringList hiddenCodexIds;
    QVector<GuideBuildUnit> units;
    QString generatedAt;
    QString summary;

    [[nodiscard]] QString summaryLine() const
    {
        return QStringLiteral("v%1 • blueprint v%2 • %3 unit%4 • %5")
            .arg(version)
            .arg(blueprintVersion)
            .arg(units.size())
            .arg(units.size() == 1 ? QString() : QStringLiteral("s"))
            .arg(validationState.trimmed().isEmpty() ? QStringLiteral("freeze-pending") : validationState.trimmed());
    }

    [[nodiscard]] QString unitTitleSummary() const
    {
        QStringList titles;
        titles.reserve(units.size());
        for (const GuideBuildUnit &unit : units) {
            titles.push_back(unit.title.trimmed());
        }

        return titles.isEmpty() ? QStringLiteral("No build units") : titles.join(QStringLiteral(" • "));
    }

    [[nodiscard]] QJsonObject toJson() const
    {
        QJsonArray visibleKnowledgeArray;
        for (const QString &knowledgeId : visibleKnowledgeIds) {
            visibleKnowledgeArray.push_back(knowledgeId);
        }

        QJsonArray hiddenCodexArray;
        for (const QString &knowledgeId : hiddenCodexIds) {
            hiddenCodexArray.push_back(knowledgeId);
        }

        QJsonArray unitsArray;
        for (const GuideBuildUnit &unit : units) {
            unitsArray.push_back(unit.toJson());
        }

        return QJsonObject {
            { QStringLiteral("id"), id },
            { QStringLiteral("projectId"), projectId },
            { QStringLiteral("previewId"), previewId },
            { QStringLiteral("blueprintVersion"), blueprintVersion },
            { QStringLiteral("version"), version },
            { QStringLiteral("validationState"), validationState },
            { QStringLiteral("visibleKnowledgeIds"), visibleKnowledgeArray },
            { QStringLiteral("hiddenCodexIds"), hiddenCodexArray },
            { QStringLiteral("units"), unitsArray },
            { QStringLiteral("generatedAt"), generatedAt },
            { QStringLiteral("summary"), summary },
        };
    }

    [[nodiscard]] static GuideBuildBundle fromJson(const QJsonObject &object)
    {
        GuideBuildBundle bundle;
        bundle.id = object.value(QStringLiteral("id")).toString();
        bundle.projectId = object.value(QStringLiteral("projectId")).toString();
        bundle.previewId = object.value(QStringLiteral("previewId")).toString();
        bundle.blueprintVersion = object.value(QStringLiteral("blueprintVersion")).toInt(1);
        bundle.version = object.value(QStringLiteral("version")).toInt(1);
        bundle.validationState = object.value(QStringLiteral("validationState")).toString(QStringLiteral("freeze-pending"));
        bundle.generatedAt = object.value(QStringLiteral("generatedAt")).toString();
        bundle.summary = object.value(QStringLiteral("summary")).toString();

        const QJsonArray visibleKnowledgeArray = object.value(QStringLiteral("visibleKnowledgeIds")).toArray();
        for (const QJsonValue &value : visibleKnowledgeArray) {
            bundle.visibleKnowledgeIds.push_back(value.toString());
        }

        const QJsonArray hiddenCodexArray = object.value(QStringLiteral("hiddenCodexIds")).toArray();
        for (const QJsonValue &value : hiddenCodexArray) {
            bundle.hiddenCodexIds.push_back(value.toString());
        }

        const QJsonArray unitsArray = object.value(QStringLiteral("units")).toArray();
        for (const QJsonValue &value : unitsArray) {
            bundle.units.push_back(GuideBuildUnit::fromJson(value.toObject()));
        }

        return bundle;
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
