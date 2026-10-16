//===========================================================================
//! @file SphericalMaskTransition.cpp
//! @brief 球面トランジション
//===========================================================================
#include "SphericalMaskTransition.h"
#include <System/Component/ComponentModel.h>
#include <System/Component/ComponentCollisionModel.h>

//! @brief 初期化
void SphericalMaskTransition::Init()
{
    Super::Init();
    auto model = GetOwner()->GetComponent<ComponentModel>();
    // ピクセルシェーダー
    shader_ps_inner_ = std::make_shared<ShaderPs>("data/Shader/ps_sphericalmask_inner");
    shader_ps_outer_ = std::make_shared<ShaderPs>("data/Shader/ps_sphericalmask_outer");

    // 初期は内側表示
    model->SetShader(nullptr, shader_ps_inner_.get());

    spherical_info_ = CreateShaderConstantBuffer(sizeof(SpericalInfo));

    instance_count_++;
}

//! @brief 更新
void SphericalMaskTransition::Update()
{
    Super::Update();

    if(start_transition_)
    {
        // トランジション開始
        mask_radius_ += GetDeltaTime60() * speed_;
    }

    if(speed_ > 0.0f)
    {
        if(destroy_limit_ > 0.0f && mask_radius_ >= destroy_limit_)
        {
            // トランジション終了
            if(!destory_after_transition_)
            {
                GetOwner()->RemoveComponent(shared_from_this());
            }
            else
            {
                Scene::Object::Release(GetOwnerPtr());
            }
        }
    }
    else
    {
        if(mask_radius_ <= 0.0f)
        {
            // トランジション終了
            if(!destory_after_transition_)
            {
                GetOwner()->RemoveComponent(shared_from_this());
            }
            else
            {
                Scene::Object::Release(GetOwnerPtr());
            }
        }
    }
}

//! @brief 更新後処理
void SphericalMaskTransition::PostUpdate()
{
    Super::PostUpdate();

    // 定数バッファデータ設定
    void* p = GetBufferShaderConstantBuffer(spherical_info_);
    {
        auto* info                     = reinterpret_cast<SpericalInfo*>(p);
        info->mask_world_position_.xyz = mask_world_position_;
        info->mask_radius_             = mask_radius_;
        info->mask_softness_           = mask_softness_;
    }

    // メモリをGPU側へ転送
    UpdateShaderConstantBuffer(spherical_info_);

    // 定数バッファ設定
    SetShaderConstantBuffer(spherical_info_, DX_SHADERTYPE_PIXEL, 6);

    if(mask_inverse_ != last_inverse_)
    {
        auto model = GetOwner()->GetComponent<ComponentModel>();
        // シェーダー切り替え
        if(mask_inverse_)
        {
            model->SetShader(nullptr, shader_ps_inner_.get());
        }
        else
        {
            model->SetShader(nullptr, shader_ps_outer_.get());
        }
        last_inverse_ = mask_inverse_;
    }
    // 定数バッファ解除
    SetShaderConstantBuffer(-1, DX_SHADERTYPE_PIXEL, 6);
}

//! @brief 終了
void SphericalMaskTransition::Exit()
{
    Super::Exit();

    // シェーダー解除

    auto model = GetOwner()->GetComponent<ComponentModel>();
    if(model)
    {
        model->SetShader(nullptr, nullptr);
    }
    shader_ps_inner_.reset();
    shader_ps_outer_.reset();
    if(spherical_info_ != -1)
    {
        DeleteShaderConstantBuffer(spherical_info_);
    }

    // 全部のトランジションが終了したら初期化状態に戻す
    instance_count_--;
    if(instance_count_ <= 0)
    {
        instance_count_ = 0;
        mask_radius_    = 0.0f;
        mask_softness_  = 5.0f;
        speed_          = 3.0f;
    }
}

//! @brief GUI描画
void SphericalMaskTransition::GUI()
{
    Super::GUI();
    ImGui::Begin(GetOwner()->GetName().data());
    {
        ImGui::Separator();
        if(ImGui::TreeNode("SphericalMaskTransition"))
        {
            if(ImGui::Button(UNIQUE_TEXT(u8"削除")))
            {
                GetOwner()->RemoveComponent(shared_from_this());
            }
            ImGui::DragFloat(UNIQUE_TEXT(u8"半径"), &mask_radius_, 0.1f, 0.0f);
            ImGui::DragFloat(UNIQUE_TEXT(u8"ぼかし"), &mask_softness_, 0.1f, 0.0f, 10.0f);
            ImGui::Checkbox(UNIQUE_TEXT(u8"反転"), &mask_inverse_);
            ImGui::DragFloat(UNIQUE_TEXT(u8"速度"), &speed_, 0.1f, 0.0f, 1000.0f);
            ImGui::DragFloat3(UNIQUE_TEXT(u8"マスク球ワールド座標"), (float*)&mask_world_position_, 0.1f);
            ImGui::TreePop();
        }
    }
    ImGui::End();
}

//! @brief トランジション開始
//! @return 自身のポインタ
SphericalMaskTransitionPtr SphericalMaskTransition::StartTransition()
{
    start_transition_ = true;
    return std::dynamic_pointer_cast<SphericalMaskTransition>(shared_from_this());
}
