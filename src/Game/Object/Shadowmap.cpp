//---------------------------------------------------------------------------
//! @file   Shadowmap.cpp
//! @brief  シャドウマップ管理
//---------------------------------------------------------------------------
#include "Shadowmap.h"
#include "System/Graphics/Render.h"
#include "System/SystemMain.h"

static constexpr int resolution_ = 2048;   // シャドウマップの解像度

// 2点間の距離を求める
float distance(const float3& a, const float3& b)
{
    float3 d = a - b;

    float lengthSq = dot(d, d);
    if(lengthSq < FLT_EPSILON)
    {
        return 0.0f;
    }

    return sqrtf(lengthSq);
}

// 8点から外接球を求める
void ComputeBoundingSphere(const float3* points, int count, float3& outCenter, float& outRadius)
{
    // 8点の重心を初期中心とする
    float3 center = float3(0.0f, 0.0f, 0.0f);
    for(int i = 0; i < count; i++)
    {
        center += points[i];
    }
    center /= float(count);

    // 最も遠い点までの距離を半径とする
    float radius = 0.0f;
    for(int i = 0; i < count; i++)
    {
        float d = distance(center, points[i]);
        if(d > radius)
        {
            radius = d;
        }
    }

    // 収束まで中心を調整（数回繰り返す）
    for(int iter = 0; iter < 8; iter++)
    {
        int farthest  = 0;
        float maxDist = 0.0f;
        for(int i = 0; i < count; i++)
        {
            float d = distance(center, points[i]);
            if(d > maxDist)
            {
                maxDist  = d;
                farthest = i;
            }
        }
        if(maxDist <= radius + 1e-5f)
        {
            break;
        }
        float3 dir  = normalize(points[farthest] - center);
        center     += dir * (maxDist - radius) * 0.5f;
        radius      = (radius + maxDist) * 0.5f;
    }

    outCenter = center;
    outRadius = radius;
}

struct ShadowInfo
{
    //       [4]+------ --+[5]
    //        ／|  　  ／ |
    //      ／  |6  ／    |
    // [0] +-----+ [1]   +[7]
    //     |     |   ／
    // [2] +-----+ [3]
    float3 frustum_vertices_[8];

    float near_z_;
    float far_z_;
    float aspect_ratio_;
    float fov_y_;
    matrix mat_camera_world_;   // シーンのカメラのワールド行列

    float4 bounding_sphere_;   // バウンディング球  [xyz_]:中心座標 [_w]:半径

public:
    void update()
    {
        // シャドウマップのフラスタム頂点を計算
        {
            auto& v = frustum_vertices_;

            float3 right    = normalize(mat_camera_world_[0].xyz);
            float3 up       = normalize(mat_camera_world_[1].xyz);
            float3 front    = normalize(mat_camera_world_[2].xyz);
            float3 position = mat_camera_world_[3].xyz;

            // nearの高さを求める
            // h0 / near_z_ = tan(fov_y_)

            float nz = near_z_;
            float fz = far_z_;

            float h0 = tanf(fov_y_ * 0.5f) * nz;
            float h1 = tanf(fov_y_ * 0.5f) * fz;

            v[0] = position - (right * h0 * aspect_ratio_) + (up * h0) + (front * nz);
            v[1] = position + (right * h0 * aspect_ratio_) + (up * h0) + (front * nz);
            v[2] = position - (right * h0 * aspect_ratio_) - (up * h0) + (front * nz);
            v[3] = position + (right * h0 * aspect_ratio_) - (up * h0) + (front * nz);

            v[4] = position - (right * h1 * aspect_ratio_) + (up * h1) + (front * fz);
            v[5] = position + (right * h1 * aspect_ratio_) + (up * h1) + (front * fz);
            v[6] = position - (right * h1 * aspect_ratio_) - (up * h1) + (front * fz);
            v[7] = position + (right * h1 * aspect_ratio_) - (up * h1) + (front * fz);
        }

        // 8点の中心座標と最大半径を求める
        // 8点が外接する球を計算する
        float3 center;
        float radius;
        ComputeBoundingSphere(frustum_vertices_, 8, center, radius);
        bounding_sphere_ = float4(center, radius);
    }

    void draw()
    {
        DxLib::SetUseLighting(false);

        // near
        for(int i = 0; i < 4; i++)
        {
            auto& v = frustum_vertices_[i];
            DxLib::DrawSphere3D(cast(v), 0.0001f, 64, GetColor(255, 255, 0), 0, 0);
        }
        // far
        for(int i = 4; i < 8; i++)
        {
            auto& v = frustum_vertices_[i];
            DxLib::DrawSphere3D(cast(v), 250.0f / 100, 64, GetColor(255, 0, 255), 0, 0);
        }

        DxLib::SetUseLighting(true);
    }
};

ShadowInfo shadow_info_[Shadowmap::CASCADE_NUM];   // シャドウ情報

//! @brief 初期化
//! @return 初期化済み
bool Shadowmap::Init()
{
    SetName("Shadowmap");
    // シャドウマップのテクスチャを生成
    shadowmap_color_       = std::make_shared<Texture>(resolution_, resolution_, DXGI_FORMAT_R32_FLOAT);
    shadowmap_color_atlas_ = std::make_shared<Texture>(resolution_ * CASCADE_NUM, resolution_, DXGI_FORMAT_R32_FLOAT);
    shadowmap_depth_       = std::make_shared<Texture>(resolution_, resolution_, DXGI_FORMAT_D32_FLOAT);

    if(!shadowmap_color_ || !shadowmap_depth_)
    {
        return false;   // テクスチャの生成に失敗
    }

    // シャドウサンプラーを作成 (DirectX11)
    {
        ID3D11Device* d3dDevice = GetD3DDevice();

        D3D11_SAMPLER_DESC desc{
            .Filter         = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT, // ピクセルサンプリング動作モード
            .AddressU       = D3D11_TEXTURE_ADDRESS_CLAMP, // テクスチャのアドレスモードU
            .AddressV       = D3D11_TEXTURE_ADDRESS_CLAMP, // テクスチャのアドレスモードV
            .AddressW       = D3D11_TEXTURE_ADDRESS_CLAMP, // テクスチャのアドレスモードW
            .MipLODBias     = 0.0f, // MIPレベルのバイアス値
            .MaxAnisotropy  = 1, // 異方性サンプリングの品質 [1,16]
            .ComparisonFunc = D3D11_COMPARISON_LESS, // 数値比較の場合の大小関係
            .BorderColor    = {1.0f, 1.0f, 1.0f, 1.0f}, // 範囲外の色(ボーダーカラー)
            .MinLOD         = -FLT_MAX, // 最小のLOD
            .MaxLOD         = +FLT_MAX, // 最大のLOD
        };

        d3dDevice->CreateSamplerState(&desc, &d3d_shadow_sampler_);
    }

    // シャドウ開始描画
    {
        auto begin = [this](int cascade_index) {
            // シャドウのレンダリング情報を更新
            auto camera = Scene::GetCurrentCamera().lock();
            if(!camera)
            {
                return;   // カメラが無効な場合は何もしない
            }

            auto& info = shadow_info_[cascade_index];

            float camera_near_z = camera->GetNearZ();

            // カスケードごとの分割距離設定
            const float split_distance[CASCADE_NUM]{75.0f, 200.0f, 1000.0f, 1800.0f};

            if(cascade_index == 0)
            {
                info.near_z_ = camera_near_z;
            }
            else
            {
                info.near_z_ = split_distance[cascade_index - 1];
            }

            info.far_z_ = split_distance[cascade_index];

            info.aspect_ratio_     = camera->GetAspectRatio();
            info.fov_y_            = camera->GetPerspective() * DegToRad;
            info.mat_camera_world_ = camera->GetWorldMatrix();   // シーンのカメラのワールド行列

            info.update();   // シャドウマップのフラスタム頂点を計算

            float3 light_direction = shadow_direction_;   // ライトの方向

            float3 shadow_center = info.bounding_sphere_.xyz;   // シャドウマップの中心位置
            float shadow_size    = info.bounding_sphere_.w;     // シャドウマップの適用範囲

            // シャドウマップのビュー行列と投影行列を更新
            float3 position = shadow_center + light_direction * shadow_height_;   // シャドウマップのカメラ位置
            float3 look_at  = position - light_direction;                         // シャドウマップのカメラの注視点

            shadowmap_view_ = matrix::lookAtLH(position, look_at);

            // シャドウマップの上下左右の映る範囲を計算

            float left   = -shadow_size;
            float right  = +shadow_size;
            float bottom = -shadow_size;
            float top    = +shadow_size;

            float near_z = 0.1f;      // シャドウマップの近くのクリッピング面
            float far_z  = 2000.0f;   // シャドウマップの遠くのクリッピング面

            shadowmap_proj_                     = matrix::orthographicOffCenterLH(left, right, bottom, top, near_z, far_z);
            shadowmap_view_proj_[cascade_index] = mul(shadowmap_view_, shadowmap_proj_);

            // シャドウマップのレンダリングターゲットを設定
            SetRenderTarget(shadowmap_color_.get(), shadowmap_depth_.get());

            // シャドウデプス値をクリア
            ClearDepth(shadowmap_depth_.get(), 1.0f);

            // シャドウ用カメラ行列を設定
            DxLib::SetCameraViewMatrix(shadowmap_view_);
            DxLib::SetupCamera_ProjectionMatrix(shadowmap_proj_);
        };

        for(int i = 0; i < CASCADE_NUM; i++)
        {
            std::string name = "Shadowmap::begin" + std::to_string(i);

            int block_size = 65536 / CASCADE_NUM;   // 1ブロック分のサイズ
            int priority   = i * block_size + (0);

            SetProc(name, [=]() { begin(i); }, ProcTiming::Shadow, static_cast<ProcPriority>(priority));
        }
    }

    // シャドウ終了描画
    {
        auto end = [this](int cascade_index) {
            // レンダリングターゲットをデフォルトに戻す
            SetRenderTarget(GetHdrBuffer(), GetDepthStencil());

            // シャドウデプスバッファをカラーバッファへコピーする
            // DxLibの制限で、デプスバッファをレンダリングターゲットに設定できないため
            // DirectX11直接カラーバッファにコピー
            auto d3d_context = static_cast<ID3D11DeviceContext*>(const_cast<void*>(DxLib::GetUseDirect3D11DeviceContext()));

            // デプスからカラーのコピー
            d3d_context->CopyResource(shadowmap_color_->d3dResource(), shadowmap_depth_->d3dResource());

            int x = resolution_ * cascade_index;
            int y = 0;
            int z = 0;

            // アトラスへコピー
            d3d_context->CopySubresourceRegion(shadowmap_color_atlas_->d3dResource(),   // 転送先のテクスチャ
                                               0,                                       // サブリソース番号
                                               x,                                       //
                                               y,                                       //
                                               z,                                       //
                                               shadowmap_color_->d3dResource(),         // 転送元のテクスチャ
                                               0,                                       // 転送元サブリソース番号
                                               nullptr);                                // 転送元の範囲

            // シーン用カメラを設定
            if(auto camera = Scene::GetCurrentCamera().lock())
            {
                camera->SetCameraTransform();
            }

            // シャドウ用比較サンプラーを13番に指定
            d3d_context->PSSetSamplers(13, 1, &d3d_shadow_sampler_);
        };

        for(int i = 0; i < CASCADE_NUM; i++)
        {
            std::string name = "Shadowmap::end" + std::to_string(i);

            int block_size = 65535 / CASCADE_NUM;   // 1ブロック分のサイズ
            int priority   = i * block_size + (block_size - 1);

            SetProc(name, [=]() { end(i); }, ProcTiming::Shadow, static_cast<ProcPriority>(priority));
        }
    }

    return Super::Init();
}

//! @brief 更新
void Shadowmap::PostUpdate() {}

//! @brief 描画
void Shadowmap::Draw() {}

//! @brief 終了
void Shadowmap::Exit()
{
    if(d3d_shadow_sampler_)
    {
        d3d_shadow_sampler_->Release();
        d3d_shadow_sampler_ = nullptr;
    }
}
