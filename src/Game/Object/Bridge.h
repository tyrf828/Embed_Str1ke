#pragma once

USING_PTR(Bridge);

class Bridge : public Object
{
public:
    BP_OBJECT_DECL(Bridge, "Bridge");

    bool Init() override;

    void Update() override;

    void Exit() override;

private:
};
