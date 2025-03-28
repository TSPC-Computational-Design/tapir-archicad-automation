#pragma once

#include "CommandBase.hpp"

class TeamworkSendCommand : public CommandBase
{
public:
    TeamworkSendCommand ();
    virtual GS::String GetName () const override;
    virtual GS::ObjectState Execute (const GS::ObjectState& parameters, GS::ProcessControl& processControl) const override;
};

class TeamworkReceiveCommand : public CommandBase
{
public:
    TeamworkReceiveCommand ();
    virtual GS::String GetName () const override;
    virtual GS::ObjectState Execute (const GS::ObjectState& parameters, GS::ProcessControl& processControl) const override;
};

class TeamworkReserveElementsCommand : public CommandBase
{
public:
    TeamworkReserveElementsCommand ();
    virtual GS::String GetName () const override;
    virtual GS::Optional<GS::UniString> GetInputParametersSchema () const override;
    virtual GS::Optional<GS::UniString> GetResponseSchema () const override;
    virtual GS::ObjectState Execute (const GS::ObjectState& parameters, GS::ProcessControl& processControl) const override;
};

class TeamworkReleaseElementsCommand : public CommandBase
{
public:
    TeamworkReleaseElementsCommand ();
    virtual GS::String GetName () const override;
    virtual GS::Optional<GS::UniString> GetInputParametersSchema () const override;
    virtual GS::Optional<GS::UniString> GetResponseSchema () const override;
    virtual GS::ObjectState Execute (const GS::ObjectState& parameters, GS::ProcessControl& processControl) const override;
};
