#include "TeamworkCommands.hpp"
#include "MigrationHelper.hpp"

TeamworkSendCommand::TeamworkSendCommand () :
    CommandBase (CommonSchema::NotUsed)
{
}

GS::String TeamworkSendCommand::GetName () const
{
    return "TeamworkSend";
}

GS::ObjectState TeamworkSendCommand::Execute (const GS::ObjectState& /*parameters*/, GS::ProcessControl& /*processControl*/) const
{
    GSErrCode err = ACAPI_Teamwork_SendChanges ();

    if (err != NoError) {
        return CreateErrorResponse (err, "Failed to send changes.");
    }

    return {};
}

TeamworkReceiveCommand::TeamworkReceiveCommand () :
    CommandBase (CommonSchema::NotUsed)
{
}

GS::String TeamworkReceiveCommand::GetName () const
{
    return "TeamworkReceive";
}

GS::ObjectState TeamworkReceiveCommand::Execute (const GS::ObjectState& /*parameters*/, GS::ProcessControl& /*processControl*/) const
{
    GSErrCode err = ACAPI_Teamwork_ReceiveChanges ();

    if (err != NoError) {
        return CreateErrorResponse (err, "Failed to receive changes.");
    }

    return {};
}

TeamworkReserveElementsCommand::TeamworkReserveElementsCommand () :
    CommandBase (CommonSchema::Used)
{}

GS::String TeamworkReserveElementsCommand::GetName () const
{
    return "TeamworkReserveElements";
}

GS::Optional<GS::UniString> TeamworkReserveElementsCommand::GetInputParametersSchema () const
{
    return R"({
        "type": "object",
        "properties": {
            "elements": {
                "$ref": "#/Elements"
            },
            "enableDialogs": {
                "type": "boolean",
                "description": "Enable or suppress dialogs during process. Optional, by default the dialogs are disabled."
            },
        },
        "additionalProperties": false,
        "required": [
            "elements"
        ]
    })";
}

GS::Optional<GS::UniString> TeamworkReserveElementsCommand::GetResponseSchema () const
{
    return R"({
        "type": "object",
        "properties": {
            "ExecutionResultsOrError": {
                "$ref": "#/ExecutionResultsOrError",
                "description": "Result of the reservation attempt for each requested element or a general error"
            }
        },
        "additionalProperties": false,
        "required": [
            "ExecutionResultsOrError"
        ]
    })";
}

GS::ObjectState TeamworkReserveElementsCommand::Execute (const GS::ObjectState& parameters, GS::ProcessControl& /*processControl*/) const
{
    GS::Array<GS::ObjectState> elements;
    parameters.Get ("elements", elements);

    bool enableDialogs = false;
    parameters.Get ("enableDialogs", enableDialogs);

    GS::ObjectState response;
    const auto& executionResults = response.AddList<GS::ObjectState> ("executionResults");

    GS::HashTable<API_Guid, short> conflictsHashTable;
    GS::Array<API_Guid> guidsToReserve;

    for (const GS::ObjectState& element : elements) {
        const GS::ObjectState* elementId = element.Get ("elementId");
        if (elementId == nullptr) {
            executionResults (CreateErrorResponse (APIERR_BADPARS, "elementId is missing"));
            continue;
        }
        guidsToReserve.Push (GetGuidFromObjectState (*elementId));
    }

    GSErrCode err = ACAPI_Teamwork_ReserveElements (guidsToReserve, &conflictsHashTable, enableDialogs);

    if (err == APIERR_NOTEAMWORKPROJECT) { 
        return CreateFailedExecutionResult (APIERR_NOTEAMWORKPROJECT, "The current project is not Teamwork project");
    } else if (err != NoError){
        return CreateFailedExecutionResult (err, "Failed to reserve elements");
    }

    for (const API_Guid& inputGuid : guidsToReserve) {
        if (inputGuid == APINULLGuid) {
            // This was an invalid input from the start
            executionResults (CreateFailedExecutionResult (APIERR_BADPARS, "Invalid or missing elementId provided."));
        } else {
            // Check if this valid GUID resulted in a conflict
            if (conflictsHashTable.ContainsKey (inputGuid)) {
                executionResults (CreateFailedExecutionResult (APIERR_NOACCESSRIGHT, "Element reservation failed due to conflict."));
            } else {
                executionResults (CreateSuccessfulExecutionResult ());
            }
        }
    }
    return response;
}
