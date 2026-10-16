//===========================================================================
//! @file ScanWave.cpp
//! @brief スキャン波
//===========================================================================
#include "ScanWave.h"
#include <Game/Object/Weapon/AttachWeapon/AttachWeaponBase.h>
#include <Game/Component/Weapon/AttachWeapon.h>
#include <Game/Component/Weapon/AttachWeaponStandby.h>
#include <Game/Component/OutLine.h>
#include <Game/Pause.h>

//! @brief 初期化
bool ScanWave::Init()
{
    Super::Init();
    // テクスチャ作成
    texture_      = std::make_shared<Texture>(WINDOW_W, WINDOW_H, DXGI_FORMAT_R16G16B16A16_FLOAT);
    depth_        = std::make_shared<Texture>(WINDOW_W, WINDOW_H, DXGI_FORMAT_R32_FLOAT);
    weapon_mask_  = std::make_shared<Texture>(WINDOW_W, WINDOW_H, DXGI_FORMAT_R16G16B16A16_FLOAT);
    weapon_depth_ = std::make_shared<Texture>(WINDOW_W, WINDOW_H, DXGI_FORMAT_D32_FLOAT);

    // シェーダー初期化
    shader_           = std::make_shared<ShaderPs>("data/Shader/ps_scan_wave");
    weapon_shader_    = std::make_shared<ShaderPs>("data/Shader/ps_outline");
    composite_shader_ = std::make_shared<ShaderPs>("data/Shader/ps_scan_composite");

    buffer_cb_ = CreateShaderConstantBuffer(sizeof(ScanInfo));
    weapon_cb_ = CreateShaderConstantBuffer(sizeof(ColorInfo));

    info_.color_     = {0.1f, 1.8f, 2.5f};
    info_.width_     = 10.0f;
    info_.intensity_ = 1.0f;

    SetProc("ScanOccludedWeapons", [this]() { DrawOccludedWeapons(); }, ProcTiming::PostDraw, static_cast<ProcPriority>(65534));

    auto execute = [this]() {
        if(!active_)
        {
            return;
        }

        auto* p = reinterpret_cast<ScanInfo*>(GetBufferShaderConstantBuffer(buffer_cb_));
        *p      = info_;

        UpdateShaderConstantBuffer(buffer_cb_);

        CopyToRenderTarget(texture_.get(), GetHdrBuffer());
        SetShaderConstantBuffer(buffer_cb_, DX_SHADERTYPE_PIXEL, 13);
        CopyToRenderTarget(GetHdrBuffer(), texture_.get(), *shader_);
        SetShaderConstantBuffer(-1, DX_SHADERTYPE_PIXEL, 13);
    };

    SetProc("ScanWave", execute, ProcTiming::Filter, static_cast<ProcPriority>(5));
    return true;
}

//! @brief 更新
void ScanWave::Update()
{
    Super::Update();
    if(!active_ || IsPause() || Scene::IsPause())
    {
        return;
    }
    info_.radius_ += speed_ * GetDeltaTime();
    for(auto it = weapon_list.begin(); it != weapon_list.end();)
    {
        auto weapon = it->lock();
        if(!CanScan(weapon))
        {
            it = weapon_list.erase(it);
            continue;
        }
        float distance = length(weapon->GetTranslate() - info_.world_position_);
        // スキャン波の半径内に入った武器はを表示
        if(distance <= max_radius_ && distance <= info_.radius_)
        {
            auto outline = weapon->GetComponent<OutLine>();
            if(!outline)
            {
                outline = weapon->AddComponent<OutLine>();
                outline->SetEnabled(false);
            }
            outline->SetScanTime(highlight_seconds_);
            it = weapon_list.erase(it);
        }
        else
        {
            it++;
        }
    }

    if(info_.radius_ >= max_radius_ + info_.width_)
    {
        active_ = false;
        weapon_list.clear();
    }
}

//! @brief 解放
void ScanWave::Exit()
{
    Super::Exit();

    weapon_list.clear();

    DeleteShaderConstantBuffer(buffer_cb_);
    buffer_cb_ = -1;

    DeleteShaderConstantBuffer(weapon_cb_);
    weapon_cb_ = -1;

    texture_.reset();
    depth_.reset();
    weapon_mask_.reset();
    weapon_depth_.reset();
    shader_.reset();
    weapon_shader_.reset();
    composite_shader_.reset();
}

//! @brief GUI
void ScanWave::GUI()
{
    Super::GUI();
    if(ImGui::Begin(GetName().data()))
    {
        ImGui::SliderFloat("Speed", &speed_, 20.0f, 1000.0f);
        ImGui::SliderFloat("Range", &max_radius_, 50.0f, 1000.0f);
        ImGui::SliderFloat("Wave width", &info_.width_, 1.0f, 30.0f);
        ImGui::SliderFloat("Highlight seconds", &highlight_seconds_, 1.0f, 15.0f);
    }
    ImGui::End();
}

//! @brief スキャン開始
//! @param position プレイヤーの座標
void ScanWave::Start(const float3& position)
{
    if(active_)
    {
        return;
    }
    info_.world_position_ = position;
    info_.radius_         = 0.0f;
    info_.intensity_      = 1.0f;
    weapon_list.clear();

    for(auto& weapon : Scene::Object::GetArray<AttachWeaponBase>())
    {
        if(CanScan(weapon))
        {
            weapon_list.push_back(weapon);
        }
    }
    active_ = true;
}

//! @brief 隠れた武器を描画
void ScanWave::DrawOccludedWeapons()
{
    std::vector<std::shared_ptr<AttachWeaponBase>> highlighted;
    for(auto& weapon : Scene::Object::GetArray<AttachWeaponBase>())
    {
        auto outline = weapon->GetComponent<OutLine>();
        if(CanScan(weapon) && outline && outline->GetScanTime() > 0.0f)
        {
            highlighted.push_back(weapon);
        }
    }
    if(highlighted.empty())
    {
        return;
    }
    auto* p = reinterpret_cast<ColorInfo*>(GetBufferShaderConstantBuffer(weapon_cb_));

    const auto target = GetRenderTarget();
    GetD3DDeviceContext()->CopyResource(depth_->d3dResource(), GetDepthStencil()->d3dResource());
    ClearColor(weapon_mask_.get(), float4(0.0f, 0.0f, 0.0f, 0.0f));
    ClearDepth(weapon_depth_.get(), 1.0f);
    SetRenderTarget(weapon_mask_.get(), weapon_depth_.get());
    for(auto& weapon : highlighted)
    {
        const auto outline   = weapon->GetComponent<OutLine>();
        float4 outline_color = outline->GetOutlineColor();
        p->color_            = outline_color;
        p->color_.w          = 0.65f * outline_color.w * outline->GetScanTime();
        UpdateShaderConstantBuffer(weapon_cb_);
        SetShaderConstantBuffer(weapon_cb_, DX_SHADERTYPE_PIXEL, 12);
        if(auto model = weapon->GetComponent<ComponentModel>())
        {
            model->GetModelClass()->render(nullptr, weapon_shader_.get());
        }
    }
    SetShaderConstantBuffer(-1, DX_SHADERTYPE_PIXEL, 12);
    SetRenderTarget(target);

    CopyToRenderTarget(texture_.get(), GetHdrBuffer());

    SetTexture(24, weapon_depth_.get());
    SetTexture(25, depth_.get());
    SetTexture(26, weapon_mask_.get());

    CopyToRenderTarget(GetHdrBuffer(), texture_.get(), *composite_shader_);

    SetTexture(24, nullptr);
    SetTexture(25, nullptr);
    SetTexture(26, nullptr);
}

//! @brief スキャン対象か
//! @param weapon 武器
bool ScanWave::CanScan(const std::shared_ptr<AttachWeaponBase>& weapon)
{
    return weapon && weapon->GetStatus(Object::StatusBit::Alive) && !weapon->GetStatus(Object::StatusBit::NoDraw) &&
           !weapon->GetComponent<AttachWeaponStandby>() && !weapon->GetComponent<AttachWeapon>();
}
