//===========================================================================
//! @file OutLine.cpp
//! @brief 輪郭線描画コンポーネント
//===========================================================================
#include "OutLine.h"
#include <System/Component/ComponentModel.h>
#include <System/Object.h>
#include <Game/Pause.h>

//! @brief 初期化
void OutLine::Init()
{
    Super::Init();

    shader_vs_ = std::make_shared<ShaderVs>("data/Shader/vs_outline", 8);
    shader_ps_ = std::make_shared<ShaderPs>("data/Shader/ps_outline");

    // 定数バッファの作成
    outline_cb_ = CreateShaderConstantBuffer(sizeof(OutlineWidth));
    color_cb_   = CreateShaderConstantBuffer(sizeof(OutlineColor));

    auto draw_proc = [this]() { DrawOutline(); };
    SetProc("OutlinePreDraw", draw_proc, ProcTiming::PostDraw, ProcPriority::NORMAL);
}

//! @brief 描画
void OutLine::Draw()
{
    Super::Draw();
}

//! @brief 更新
void OutLine::Update()
{
    Super::Update();
    if(!IsPause() && !Scene::IsPause())
    {
        scan_seconds_ = std::max(0.0f, scan_seconds_ - GetDeltaTime());
    }
}

//! @brief 終了
void OutLine::Exit()
{
    Super::Exit();

    // シェーダーの解放
    shader_vs_.reset();
    shader_ps_.reset();

    DeleteShaderConstantBuffer(outline_cb_);
    outline_cb_ = -1;

    DeleteShaderConstantBuffer(color_cb_);
    color_cb_ = -1;
}

//! @brief GUI表示処理
void OutLine::GUI()
{
    Super::GUI();

    auto owner = GetOwner();
    if(!owner)
    {
        return;
    }

    auto obj_name = owner->GetName();
    ImGui::Begin(obj_name.data());
    {
        ImGui::Separator();

        auto name = std::string(u8"OutLine");
        if(ImGui::TreeNode(UNIQUE_TEXT(name)))
        {
            if(ImGui::Button(UNIQUE_TEXT(u8"削除")))
            {
                GetOwner()->RemoveComponent(shared_from_this());
            }

            // 有効/無効の切り替え
            ImGui::Checkbox(UNIQUE_TEXT(u8"有効"), &enabled_);

            // 輪郭線の太さ
            ImGui::DragFloat(UNIQUE_TEXT(u8"太さ"), &outline_width_, 0.001f, 0.0f, 1.0f, "%.3f");

            ImGui::TreePop();
        }
    }
    ImGui::End();
}

//! @brief 輪郭線描画
void OutLine::DrawOutline()
{
    if(!enabled_ && scan_seconds_ <= 0.0f)
    {
        return;
    }

    auto model = GetOwner()->GetComponent<ComponentModel>();
    if(!model)
    {
        return;
    }

    // https://game.watch.impress.co.jp/img/gmw/docs/1617/901/html/11_o.jpg.html
    auto camera = Scene::GetCurrentCamera().lock();
    if(!camera)
    {
        return;
    }
    auto camera_owner = camera->GetOwner();
    float distance    = length(GetOwner()->GetTranslate() - camera_owner->GetTranslate());
    float fov         = camera->GetPerspective();
    float width       = distance * tanf(fov * DegToRad / 2.0f) * outline_width_;
    // 定数バッファの更新
    {
        auto* p = reinterpret_cast<OutlineWidth*>(GetBufferShaderConstantBuffer(outline_cb_));
        if(p)
        {
            p->outline_width_ = width;
            p->pos_[0]        = GetOwner()->GetTranslate().x;
            p->pos_[1]        = GetOwner()->GetTranslate().y;
            p->pos_[2]        = GetOwner()->GetTranslate().z;
        }
        UpdateShaderConstantBuffer(outline_cb_);
        SetShaderConstantBuffer(outline_cb_, DX_SHADERTYPE_VERTEX, 12);
    }

    // 定数バッファの更新
    {
        auto* p = reinterpret_cast<OutlineColor*>(GetBufferShaderConstantBuffer(color_cb_));
        if(p)
        {
            p->outline_color_ = outline_color_;
        }
        UpdateShaderConstantBuffer(color_cb_);
        SetShaderConstantBuffer(color_cb_, DX_SHADERTYPE_PIXEL, 12);
    }

    int mv1_handle = model->GetModel();
    auto mesh_num  = MV1GetMeshNum(mv1_handle);
    for(int i = 0; i < mesh_num; i++)
    {
        MV1SetMeshBackCulling(mv1_handle, i, DX_CULLING_RIGHT);
    }

    // カスタムシェーダーで描画
    model->GetModelClass()->render(shader_vs_.get(), shader_ps_.get());

    SetShaderConstantBuffer(-1, DX_SHADERTYPE_VERTEX, 12);
    SetShaderConstantBuffer(-1, DX_SHADERTYPE_PIXEL, 12);
    for(int i = 0; i < mesh_num; i++)
    {
        MV1SetMeshBackCulling(mv1_handle, i, DX_CULLING_LEFT);
    }
}
