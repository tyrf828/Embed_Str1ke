//---------------------------------------------------------------------------
//! @file   ComponentModel.h
//! @brief  モデルコンポーネント
//---------------------------------------------------------------------------
#include <System/Component/ComponentModel.h>
#include <System/Component/ComponentTransform.h>
#include <System/Object.h>
#include <Game/Object/Gbuffer.h>
#include <Game/Object/Shadowmap.h>

namespace
{
std::string null_name = "  ";

// string用のCombo
static int Combo(const std::string& caption, std::string& current_item, const std::vector<std::string_view>& items)
{
    int select_index = -1;
    for(int i = 0; i < items.size(); i++)
    {
        auto& item = items[i];
        if(current_item == item)
        {
            select_index = i;
            break;
        }
    }

    if(ImGui::BeginCombo(caption.c_str(), current_item.c_str()))
    {
        for(int i = 0; i < items.size(); i++)
        {
            auto& item = items[i];

            bool is_selected = (current_item == item);
            if(ImGui::Selectable(item.data(), is_selected))
            {
                current_item = item;
                select_index = i;
            }
            if(is_selected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    return select_index;
}

static bool Items_ArrayGetter(void* data, int idx, const char** out_text)
{
    const char* const* items = (const char* const*)data;
    if(out_text)
    {
        *out_text = items[idx];
    }
    return true;
}

bool ListBox(const char* label, int* current_item, const std::vector<std::string_view> items, int height_items)
{
    bool value_changed = false;

    const size_t items_count = items.size();
    const char** tbl         = (const char**)malloc(items_count * sizeof(char*));
    if(tbl != nullptr)
    {
        for(int i = 0; i < items_count; i++)
        {
            tbl[i] = items[i].data();
        }

        value_changed = ImGui::ListBox(label, current_item, Items_ArrayGetter, (void*)tbl, (int)items_count, height_items);
        free(tbl);
    }
    return value_changed;
}

}   // namespace

//! @brief モデルロード
//! @param path ロードするモデル(.MV1/.MQO/.Xなど)
void ComponentModel::Load(std::string_view path)
{
    model_status_.off(ModelBit::ErrorFileNotFound);
    model_status_.on(ModelBit::SetFileAlready);

    path_ = path;

    // ファイルが存在しているか?
    if(!HelperLib::File::CheckFileExistence(path_))
    {
        // ロードできなかった
        model_status_.on(ModelBit::ErrorFileNotFound);
        model_status_.off(ModelBit::Initialized);
        nodes_name_.clear();
        return;
    }

    bool result = model_->load(path_);
    if(!result)
    {
        // ロードできなかった
        model_status_.on(ModelBit::ErrorFileNotFound);
    }
    else
    {
        // ロード済み
        model_status_.on(ModelBit::Initialized);
    }

    // ノード名を初期化しておく
    nodes_name_.clear();
}

//---------------------------------------------------------------------------
//! 初期化
//---------------------------------------------------------------------------
void ComponentModel::Init()
{
    // 親クラス
    Super::Init();

    // シャドウの描画をDraw()を呼ぶようにProcTiming::Shadowへ追加
    // Updateを違う処理に変更します
    {
        for(int i = 0; i < Shadowmap::CASCADE_NUM; i++)
        {
            std::string name = "Shadowmap::Draw" + std::to_string(i);

            int block_size = 65536 / Shadowmap::CASCADE_NUM;   // 1ブロック分のサイズ
            int priority   = i * block_size + (block_size / 2);

            SetProc(name, [this]() { DrawModel(); }, ProcTiming::Shadow, static_cast<ProcPriority>(priority));
        }
    }

    // GBufferが有効の場合はGBufferパスで描画。それ以外はHDRパスで描画
    auto gbuffer = Scene::GetObjectPtr<GBuffer>();
    if(gbuffer && !use_forward_)
    {
        SetProc("Draw", [this]() { DrawModel(); }, ProcTiming::Gbuffer, static_cast<ProcPriority>(32768));
    }
    else
    {
        // HDRパスで描画
        SetProc("Draw", [this]() { DrawModel(); }, ProcTiming::HDR, static_cast<ProcPriority>(32768));
    }

    model_cb_ = CreateShaderConstantBuffer(sizeof(ModelInfo));
}

//! @brief モデル更新
//! @param delta 1フレームの秒数
void ComponentModel::Update()
{
    if(hit_stop_time_ > 0.0f)
    {
        hit_stop_time_ -= GetDeltaTime();
        if(hit_stop_time_ <= 0.0f)
        {
            hit_stop_time_ = 0.0f;
        }
        return;
    }

    float delta = GetDeltaTime() * time_scale_;

    // モデルが存在しているならばTransform設定を行う
    if(IsValid())
    {
        auto mat  = model_transform_;
        auto trns = GetOwner()->GetComponent<ComponentTransform>();
        if(trns)
        {
            mat = mul(mat, trns->GetWorldMatrix());
        }

        if(model_)
        {
            // ワールド行列を設定
            model_->setWorldMatrix(mat);

            //------------------------------------------------------
            // アニメーション更新
            //------------------------------------------------------
            // アニメーションがあり再生している?
            if(animation_ && animation_->isValid() && animation_->isPlaying())
            {
                // [DxLib] MV1モデルハンドルを取得
                int mv1_handle                 = GetModel();
                int move_animation_frame_index = DxLib::MV1SearchFrame(mv1_handle, "mixamorig:Hips");

                // ユーザーカスタムのワールド行列情報をリセット消去
                // 一旦リセットしないと、1フレーム前の上書き情報がそのまま利用されてupdate()されてしまう
                DxLib::MV1ResetFrameUserLocalWorldMatrix(mv1_handle, move_animation_frame_index);

                animation_->update(delta * 2);

                //------------------------------------------------------
                // DxLibアニメーションの関節(フレーム)のワールド座標を上書き
                //------------------------------------------------------

                // ワールド行列を取得
                auto m = DxLib::MV1GetFrameLocalWorldMatrix(mv1_handle, move_animation_frame_index);
                // 座標のXZをプレイヤー座標で固定する
                if(on_place_xz)
                {
                    m.m[3][0] = GetOwner()->Matrix().translate().x;
                    m.m[3][2] = GetOwner()->Matrix().translate().z;
                }
                // 座標のYをプレイヤー座標で固定する
                if(on_place_y)
                {
                    m.m[3][1] = GetOwner()->Matrix().translate().y;
                }
                // DxLibに上書き再設定
                // ※一度上書きされると上書き情報が残り続けるため、次回のupdate()の時にアニメーションの座標が更新されなくなる。
                //   update()の前にMV1ResetFrameUserLocalWorldMatrix()でリセットする必要がある
                DxLib::MV1SetFrameUserLocalWorldMatrix(mv1_handle, move_animation_frame_index, m);

                animation_time_ += delta * 2;

                // 進めた結果終了した場合はOldとして確保
                if(!animation_->isPlaying())
                {
                    old_animation_name_ = current_animation_name_;
                }
            }

            // モデル更新
            model_->update(delta);
        }
    }
}

//! @brief モデル描画
void ComponentModel::DrawModel()
{
    if(!model_status_.is(ModelBit::Initialized))
    {
        return;
    }

    // Drawはここで抑えておく
    if(GetStatus(Component::StatusBit::NoDraw))
    {
        return;
    }

    if(model_ == nullptr)
    {
        return;
    }

    // 完全透明の場合は描画をスキップ
    if(model_->GetHandle() != -1)
    {
        float opacity = MV1GetMeshOpacityRate(model_->GetHandle(), 0);
        if(opacity <= 0.0f)
        {
            return;
        }
    }

    // ワールド行列を設定(コリジョン移動分)
    model_->setWorldMatrix(GetWorldMatrix());

    // シェーダーを利用するかどうかを設定
    model_->useShader(UseShader());

    // Forward Renderingを利用するかどうかを設定
    model_->useForwardRendering(use_forward_);

    // 定数バッファの更新
    {
        auto* p = reinterpret_cast<ModelInfo*>(GetBufferShaderConstantBuffer(model_cb_));
        if(p)
        {
            p->receives_decal_ = is_receives_decal_;
        }
        SetShaderConstantBuffer(model_cb_, DX_SHADERTYPE_PIXEL, 11);
        UpdateShaderConstantBuffer(model_cb_);
    }
    SetShaderConstantBuffer(-1, DX_SHADERTYPE_PIXEL, 11);

    std::shared_ptr<Texture> empty = nullptr;

    auto drawFrame = [this, &empty](int i) {
        if(materials_.find(i) != materials_.end())
        {
            // マテリアルが存在した
            model_->overrideTexture(Model::TextureType::Diffuse, materials_[i].diffuse_);
            model_->overrideTexture(Model::TextureType::Normal, materials_[i].normal_);
            model_->overrideTexture(Model::TextureType::Roughness, materials_[i].roughness_);
            model_->overrideTexture(Model::TextureType::Metalness, materials_[i].metalness_);
            model_->overrideTexture(Model::TextureType::AO, materials_[i].AO_);
            model_->overrideTexture(Model::TextureType::Specular, materials_[i].specular_);
        }
        else
        {
            // 存在せず
            model_->overrideTexture(Model::TextureType::Diffuse, nullptr);
            model_->overrideTexture(Model::TextureType::Normal, nullptr);
            model_->overrideTexture(Model::TextureType::Roughness, nullptr);
            model_->overrideTexture(Model::TextureType::Metalness, nullptr);
            model_->overrideTexture(Model::TextureType::AO, nullptr);
            model_->overrideTexture(Model::TextureType::Specular, nullptr);
        }

        //// IBLテクスチャを設定
        //DxLib::SetUseTextureToShader(14, SceneShader::getIBLDiffuse() ? static_cast<int>(*SceneShader::getIBLDiffuse()) : -1);
        //DxLib::SetUseTextureToShader(15, SceneShader::getIBLSpecular() ? static_cast<int>(*SceneShader::getIBLSpecular()) : -1);

        // シャドウマップのビュー行列と投影行列を取得
        auto shadowmap = Scene::GetObjectPtr<Shadowmap>();
        if(shadowmap)
        {
            auto* shadowmap_texture = shadowmap->GetShadowmapTexture();
            auto shadowmap_handle   = static_cast<int>(*shadowmap_texture);
            DxLib::SetUseTextureToShader(13, shadowmap_handle ? shadowmap_handle : -1);
        }

        // モデル描画
        model_->renderByFrame(i, overrided_shader_vs_, overrided_shader_ps_);

        DxLib::SetUseTextureToShader(13, -1);
        // IBLテクスチャを解除
        DxLib::SetUseTextureToShader(14, -1);
        DxLib::SetUseTextureToShader(15, -1);
    };

    // 非同期読み込みが完了していない場合は通常描画するとワイヤーフレーム表示になる
    if(!model_->isActive())
    {
        model_->render();
        return;
    }

    // モデル描画
    if(draw_meshes_.size() > 0 && draw_meshes_[0] == -1)
    {
        if(materials_.find(-1) != materials_.end())
        {
            model_->overrideTexture(Model::TextureType::Diffuse, materials_[-1].diffuse_);
            model_->overrideTexture(Model::TextureType::Normal, materials_[-1].normal_);
            model_->overrideTexture(Model::TextureType::Roughness, materials_[-1].roughness_);
            model_->overrideTexture(Model::TextureType::Metalness, materials_[-1].metalness_);
            model_->overrideTexture(Model::TextureType::AO, materials_[-1].AO_);
            model_->overrideTexture(Model::TextureType::Specular, materials_[-1].specular_);
            model_->render();
        }
        else
        {
            for(int i = 0; i < model_->frameCount(); i++)
            {
                drawFrame(i);
            }
        }
        return;
    }

    for(int i : draw_meshes_)
    {
        drawFrame(i);
    }
}

//! @brief 終了処理
void ComponentModel::Exit()
{
    __super::Exit();
    // 解放必要なし
}

//! @brief GUI処理
void ComponentModel::GUI()
{
    // オーナーの取得
    assert(GetOwner());
    auto obj_name = GetOwner()->GetName();

    ImGui::Begin(obj_name.data());
    {
        ImGui::Separator();

        if(!ImGui::IsWindowFocused())
        {
            node_manipulate_ = false;
        }

        // モデルコンポーネント表示
        auto name = std::string(u8"Model 【") + std::string(GetName()) + std::string(u8"】");
        if(ImGui::TreeNode(UNIQUE_TEXT(name)))
        {
            if(ImGui::Button(UNIQUE_TEXT(u8"削除")))
            {
                GetOwner()->RemoveComponent(shared_from_this());
            }

            // シェーダー利用設定
            bool shader = UseShader();
            if(ImGui::Checkbox(UNIQUE_TEXT(u8"UseShader"), &shader))
            {
                model_status_.set(ModelBit::UseShader, shader);
            }

            // ロード完了チェックフラグ
            bool loaded = IsValid();

            ImGui::BeginDisabled(true);   // UI上の編集不可(ReadOnly)
            {
                if(loaded)
                {
                    ImGui::Checkbox(u8"【LoadOK】", &loaded);
                }
                else
                {
                    ImGui::TextColored({1, 0, 0, 1}, u8"【LoadNG】");
                }
            }
            ImGui::EndDisabled();

            ImGui::SameLine();

            // モデルファイル名
            char file_name[1024]{};
            sprintf_s(file_name, "%s", path_.c_str());
            if(ImGui::InputText(UNIQUE_TEXT(u8"File"), file_name, 1024))
            {
                path_ = file_name;
                model_status_.off(ModelBit::Initialized);
                Load(path_);
            }
            ImGui::Separator();

            auto GUIAnimation = [this]() {
                constexpr int item_count = 5;
                auto flags               = ImGuiTableFlags_Resizable | ImGuiTableFlags_Borders | ImGuiTableFlags_Sortable |   //
                                           ImGuiTableFlags_SizingFixedSame | ImGuiTableFlags_ScrollY;

                bool change = false;

                if(animations_desc_.empty())
                {
                    if(ImGui::Button(UNIQUE_TEXT(u8"アニメーション追加")))
                    {
                        animations_desc_.push_back({});
                    }
                }
                else
                {
                    if(ImGui::TreeNode(UNIQUE_TEXT(u8"アニメーション")))
                    {
                        if(ImGui::Button(UNIQUE_TEXT(u8"Play"), ImVec2(100, 20)))
                        {
                            if(!current_animation_name_.empty())
                            {
                                PlayAnimation(current_animation_name_, anim_loop_);
                            }
                        }
                        ImGui::SameLine();
                        ImGui::Checkbox(UNIQUE_TEXT(u8"ループ"), &anim_loop_);

                        if(ImGui::BeginTable(UNIQUE_TEXT("table1"), item_count, flags))
                        {
                            for(int row = 0; row < animations_desc_.size();)
                            {
                                auto& desc = animations_desc_[row];
                                ImGui::TableNextRow();

                                {
                                    ImGui::SetNextItemWidth(0);
                                    ImGui::TableSetColumnIndex(0);
                                    std::string radio =
                                        std::string(u8"##radio_anm_row") + std::to_string(row) + std::string(".") + std::to_string((size_t)this);
                                    if(ImGui::RadioButton(radio.c_str(), current_animation_name_ == desc.name_))
                                    {
                                        current_animation_name_ = desc.name_;
                                    }
                                    ImGui::SameLine();
                                    ImGui::SetNextItemWidth(0);
                                    ImGui::TableSetColumnIndex(0);
                                    std::string id = std::string(u8"削除##anm_row") + std::to_string(row) + std::string(".") + std::to_string((size_t)this);
                                    if(ImGui::Button(id.c_str()))
                                    {
                                        animations_desc_.erase(animations_desc_.begin() + row);
                                        continue;
                                    }
                                    ImGui::SameLine();

                                    ImGui::SetNextItemWidth(100);
                                    ImGui::TableSetColumnIndex(1);
                                    ImGui::PushID(row * item_count + 0);   // assign unique id
                                    change |= ImGui::InputText("##v1", &desc.name_, ImGuiInputTextFlags_EnterReturnsTrue);
                                    ImGui::PopID();

                                    ImGui::SetNextItemWidth(300);
                                    ImGui::TableSetColumnIndex(2);
                                    ImGui::PushID(row * item_count + 1);   // assign unique id
                                    change |= ImGui::InputText("##v2", &desc.file_path_);
                                    ImGui::PopID();

                                    ImGui::SetNextItemWidth(80);
                                    ImGui::TableSetColumnIndex(3);
                                    ImGui::PushID(row * item_count + 2);   // assign unique id
                                    change |= ImGui::InputInt("##v3", (int*)&desc.animation_index_);
                                    ImGui::PopID();

                                    ImGui::SetNextItemWidth(80);
                                    ImGui::TableSetColumnIndex(4);
                                    ImGui::PushID(row * item_count + 3);   // assign unique id
                                    change |= ImGui::InputFloat("##v4", &desc.animation_speed_);
                                    ImGui::PopID();
                                }
                                row++;
                            }
                            ImGui::EndTable();
                        }
                        if(ImGui::Button(UNIQUE_TEXT(u8"アニメーション追加")))
                        {
                            animations_desc_.push_back({});
                        }

                        ImGui::TreePop();
                    }
                }

                // 再登録
                if(change)
                {
                    SetAnimation(animations_desc_);
                }
            };

            GUIAnimation();

            // アニメーション名
            if(IsPlaying())
            {
                ImGui::TextColored({0.5, 1, 0.5, 1}, u8"アニメーション再生中");
                ImGui::Text(u8"[%3.2f]%s", animation_time_, GetPlayAnimationName().data());
                ImGui::Separator();
            }

            // モデル姿勢
            if(ImGui::TreeNode(UNIQUE_TEXT(u8"モデル姿勢(matrix)")))
            {
                ImGui::DragFloat4(UNIQUE_TEXT(u8"Ｘ軸"), model_transform_.f32_128_0, 0.01f, -10000.0f, 10000.0f, "%.2f");
                ImGui::DragFloat4(UNIQUE_TEXT(u8"Ｙ軸"), model_transform_.f32_128_1, 0.01f, -10000.0f, 10000.0f, "%.2f");
                ImGui::DragFloat4(UNIQUE_TEXT(u8"Ｚ軸"), model_transform_.f32_128_2, 0.01f, -10000.0f, 10000.0f, "%.2f");
                ImGui::DragFloat4(UNIQUE_TEXT(u8"座標"), model_transform_.f32_128_3, 0.01f, -10000.0f, 10000.0f, "%.2f");
                ImGui::Separator();
                ImGui::TreePop();
            }

            // 姿勢を TRSで変更できるように設定
            float* mat = model_transform_.f32_128_0;
            float matrixTranslation[3], matrixRotation[3], matrixScale[3];
            DecomposeMatrixToComponents(mat, matrixTranslation, matrixRotation, matrixScale);
            ImGui::DragFloat3(UNIQUE_TEXT(u8"座標(T)"), matrixTranslation, 0.01f, -100000.00f, 100000.0f, "%.2f");
            ImGui::DragFloat3(UNIQUE_TEXT(u8"回転(R)"), matrixRotation, 0.1f, -360.0f, 360.0f, "%.2f");
            ImGui::DragFloat3(UNIQUE_TEXT(u8"スケール(S)"), matrixScale, 0.01f, 0.00f, 1000.0f, "%.2f");
            RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, mat);

            if(ImGui::TreeNode(UNIQUE_TEXT("Model:Nodes")))
            {
                node_manipulate_ = false;
                auto list        = GetNodesNamePChar();
                ImGui::ListBox(UNIQUE_TEXT(u8"ノード名"), &select_node_index_, list.data(), (int)list.size(), 10);
                if(select_node_index_ < list.size())
                {
                    node_manipulate_ = true;
                }

                ImGui::TreePop();
            }
            else
            {
                node_manipulate_ = false;
            }

            ImGui::Separator();

            // 自分以外のコンポーネントを取得する
            auto owner  = GetOwner();
            auto models = owner->GetComponents<ComponentModel>();
            std::vector<ComponentModel*> model_vec;
            std::vector<std::string_view> model_name_vec;
            for(int i = (int)models.size() - 1; i >= 0; --i)
            {
                if(models[i].get() == this)
                {
                    models.erase(models.begin() + i);
                }
            }
            for(auto model : models)
            {
                model_vec.push_back(model.get());
                model_name_vec.push_back(model.get()->GetName());
            }

            if(!models.empty())
            {
                editor_attach_node_manipulate_ = false;

                // Attach Node
                if(ImGui::TreeNode(UNIQUE_TEXT("Attach Node")))
                {
                    if(editor_attach_model_index_ >= 0 && editor_attach_node_index_ >= 0)
                    {
                        auto node_list = model_vec[editor_attach_model_index_]->GetNodesName();

                        bool attached = model_status_.is(ModelBit::AttachedOtherModel);
                        if(ImGui::Checkbox(UNIQUE_TEXT(u8"モデルに接続"), &attached))
                        {
                            model_status_.set(ModelBit::AttachedOtherModel, attached);
                            if(attached)
                            {
                                auto model_name = model_name_vec[editor_attach_model_index_];
                                Attach(model_name, node_list[editor_attach_node_index_]);
                            }
                            else
                            {
                                Detach();
                            }
                        }
                    }
                    std::string model_name = "";
                    if(editor_attach_model_index_ >= 0 && editor_attach_model_index_ < model_name_vec.size())
                    {
                        model_name = model_name_vec[editor_attach_model_index_];
                    }

                    //ImGui::ListBox( UNIQUE_TEXT( u8"target model" ), &editor_attach_model_index_, model_name_vec, 5 );
                    editor_attach_model_index_ = Combo(UNIQUE_TEXT(u8"target model"), model_name, model_name_vec);
                    if(editor_attach_model_index_ >= 0)
                    {
                        editor_attach_model_           = model_vec[editor_attach_model_index_];
                        auto node_list                 = editor_attach_model_->GetNodesName();
                        editor_attach_node_manipulate_ = true;
                        if(ListBox(UNIQUE_TEXT(u8"ノード名"), &editor_attach_node_index_, node_list, 4))
                        {
                            auto _name = model_name_vec[editor_attach_model_index_];
                            Attach(_name, node_list[editor_attach_node_index_]);
                        }
                    }
                    else
                    {
                        editor_attach_model_ = nullptr;
                    }

                    ImGui::TreePop();
                }
            }

            ImGui::TreePop();
        }
    }
    ImGui::End();

    if(node_manipulate_)
    {
        matrix matx = MV1GetFrameLocalWorldMatrix(GetModel(), select_node_index_);
        auto trns   = GetOwner()->GetComponent<ComponentTransform>();

        float* mat_float = (float*)matx.f32_128_0;
        ShowGizmo(mat_float, gizmo_operation_, gizmo_mode_, reinterpret_cast<uint64_t>(this));
    }
    if(editor_attach_node_manipulate_)
    {
        matrix matx = MV1GetFrameLocalWorldMatrix(editor_attach_model_->GetModel(), editor_attach_node_index_);
        auto trns   = GetOwner()->GetComponent<ComponentTransform>();

        float* mat_float = (float*)matx.f32_128_0;
        ShowGizmo(mat_float, gizmo_operation_, gizmo_mode_, reinterpret_cast<uint64_t>(this));
    }
}

ComponentModelPtr ComponentModel::SetAnimation(const std::vector<Animation::Desc>& anims)
{
    if(model_ && model_->isValid())
    {
        // エディター用アニメーション設定用
        animations_desc_ = anims;

        // アニメーションクラスの作成
        animation_ = std::make_unique<Animation>(anims.data(), anims.size());

        //  モデルにアニメーションを設定
        model_->bindAnimation(animation_.get());
    }

    return std::dynamic_pointer_cast<ComponentModel>(shared_from_this());
}

void ComponentModel::PlayAnimationNoSame(std::string_view name, bool loop, float blend_time, float start_time)
{
    if(current_animation_name_ == name)
    {
        return;
    }

    PlayAnimation(name, loop, blend_time, start_time);
}

void ComponentModel::PlayAnimation(std::string_view name, bool loop, float blend_time, float start_time)
{
    if(animation_)
    {
        animation_->play(name, loop, blend_time, start_time);
        current_animation_name_ = name;
        animation_time_         = 0.0f;
    }
}

void ComponentModel::PlayAnimationEx(std::string_view name, std::string_view node, bool loop, float blend_time, float start_time)
{
    if(!animation_modifier_.contains(node.data()))
    {
        // 登録されていない場合
        int frame_index                  = DxLib::MV1SearchFrame(*model_, node.data());   // 分割ポイント
        animation_modifier_[node.data()] = animation_->createAnimationModifier(frame_index);
    }

    auto& node_point = animation_modifier_[node.data()];

    node_point->play(name, loop, blend_time, start_time);
}

bool ComponentModel::IsPlaying()
{
    if(animation_)
    {
        return animation_->isPlaying();
    }

    return false;
}

bool ComponentModel::IsExPlaying(std::string_view node)
{
    if(animation_modifier_.contains(node.data()))
    {
        return animation_modifier_[node.data()]->isPlaying();
    }

    return false;
}

void ComponentModel::PlayPause(bool is_pause)
{
    if(animation_)
    {
        animation_->pause(is_pause);
    }
}

bool ComponentModel::IsPaused()
{
    if(animation_)
    {
        return animation_->isPaused();
    }

    return false;
}

bool ComponentModel::IsAnimationValid()
{
    if(animation_)
    {
        return animation_->isValid();
    }

    return false;
}

const std::string_view ComponentModel::GetPlayAnimationName()
{
    if(IsPlaying())
    {
        return current_animation_name_;
    }

    // 何も再生されていない
    return "";
}

const std::string_view ComponentModel::GetOldPlayAnimationName()
{
    if(IsPlaying())
    {
        // 既に再生されている場合は前のアニメーション名を返す
        return old_animation_name_;
    }

    // 何も再生されていないときは再生されていたものを返す
    return current_animation_name_;
}

const float ComponentModel::GetAnimationTime()
{
    if(IsPlaying())
    {
        return animation_time_;
    }

    return 0.0f;
}

const float ComponentModel::GetAnimationSpeed()
{
    if(IsPlaying())
    {
        auto index = animation_->animationIndex(current_animation_name_);
        auto desc  = animation_->desc(index);
        return desc->animation_speed_;
    }
    return 1.0f;
}

std::vector<std::string_view> ComponentModel::GetNodesName()
{
    if(nodes_name_.empty())
    {
        int num = MV1GetFrameNum(GetModel());
        if(num <= 0)
        {
            return nodes_name_;
        }

        nodes_name_.reserve(num);
        for(int i = 0; i < num; i++)
        {
            nodes_name_.emplace_back(MV1GetFrameName(GetModel(), i));
        }
    }

    return nodes_name_;
}

std::vector<const char*> ComponentModel::GetNodesNamePChar()
{
    std::vector<const char*> listbox;
    auto names = GetNodesName();

    listbox.reserve(names.size());

    for(auto& name : names)
    {
        listbox.emplace_back(name.data());
    }

    return listbox;
}

int ComponentModel::GetNodeIndex(std::string_view name)
{
    auto names = GetNodesName();
    for(int i = 0; i < names.size(); i++)
    {
        if(names[i] == name)
        {
            return i;
        }
    }
    // 存在せず
    return -1;
}

float3 ComponentModel::GetNodePosition(std::string_view name, bool local)
{
    int index = GetNodeIndex(name);
    if(index >= 0)
    {
        return GetNodePosition(index, local);
    }
    // 存在せず(初期位置)
    return {0, 0, 0};
}

float3 ComponentModel::GetNodePosition(int no, bool local)
{
    if(local)
    {
        // ローカル座標で取得
        auto mat = MV1GetFrameLocalMatrix(GetModel(), no);
        return {mat.m[3][0], mat.m[3][1], mat.m[3][2]};
    }
    // ワールド座標で取得
    auto mat = MV1GetFrameLocalWorldMatrix(GetModel(), no);
    return {mat.m[3][0], mat.m[3][1], mat.m[3][2]};
}

matrix ComponentModel::GetNodeMatrix(std::string_view name, bool local)
{
    int index = GetNodeIndex(name);
    if(index >= 0)
    {
        return GetNodeMatrix(index, local);
    }
    // 存在せず(初期位置)
    return matrix::identity();
}

matrix ComponentModel::GetNodeMatrix(int no, bool local)
{
    if(local)
    {
        // ローカル座標で取得
        return MV1GetFrameLocalMatrix(GetModel(), no);
    }

    // ワールド座標で取得
    return MV1GetFrameLocalWorldMatrix(GetModel(), no);
}

void ComponentModel::SetMaterial(int index, Material material)
{
    materials_[index] = material;
}

void ComponentModel::ResetMaterial(int index)
{
    materials_.erase(index);
}

void ComponentModel::ResetMaterialAll()
{
    materials_.clear();
}

void ComponentModel::SetDrawMeshIndex(const std::vector<int>& meshes)
{
    draw_meshes_.clear();
    auto check_all = std::any_of(meshes.begin(), meshes.end(), [this](int x) {
        if(x == -1)
        {
            return true;
        }
        else if(x < MV1GetMeshNum(GetModel()))
        {
            draw_meshes_.push_back(x);
        }

        return false;
    });

    if(check_all)
    {
        draw_meshes_ = {-1};
    }
}

void ComponentModel::UseForwardRendering(bool use)
{
    prev_use_forward_ = use_forward_;
    use_forward_      = use;

    if(prev_use_forward_ != use_forward_)
    {
        // GBufferが有効の場合はGBufferパスで描画。それ以外はHDRパスで描画
        auto gbuffer = Scene::GetObjectPtr<GBuffer>();
        if(gbuffer && !use_forward_)
        {
            // GBufferパスで描画
            SetProc("Draw", [this]() { DrawModel(); }, ProcTiming::Gbuffer, static_cast<ProcPriority>(32768));
        }
        else
        {
            // HDRパスで描画
            SetProc("Draw", [this]() { DrawModel(); }, ProcTiming::HDR, static_cast<ProcPriority>(32768));
        }
    }
}

//! @brief ワールドMatrixの取得
//! @return 他のコンポーネントも含めた位置
const matrix ComponentModel::GetWorldMatrix() const
{
    matrix mat = matrix::identity();

    // AttachedOtherModelの場合はそのコンポーネントもマージする
    if(IsAttachedOtherModel())
    {
        mat = GetAttachMatrix();
        // スケールを自前のものに戻す
        if(!use_model_node_scale_)
        {
            matrix parent_mat(
                float4{normalize(mat.axisX()), 0}, float4{normalize(mat.axisY()), 0}, float4{normalize(mat.axisZ()), 0}, float4{mat.translate(), 1});
            mat = parent_mat;
        }
        mat = mul(GetMatrix(), mat);
        return mat;
    }

    mat = mul(GetMatrix(), mat);
    return mul(mat, GetOwner()->GetWorldMatrix());
}

//! @brief 1フレーム前のワールドMatrixの取得
//! @return 他のコンポーネントも含めた位置

const matrix ComponentModel::GetOldWorldMatrix() const
{
    matrix mat = matrix::identity();

    // AttachedOtherModelの場合はそのコンポーネントもマージする
    if(IsAttachedOtherModel())
    {
        mat = GetAttachMatrix();

        // スケールを自前のものに戻す
        if(!use_model_node_scale_)
        {
            matrix parent_mat(
                float4{normalize(mat.axisX()), 0}, float4{normalize(mat.axisY()), 0}, float4{normalize(mat.axisZ()), 0}, float4{mat.translate(), 1});
            mat = parent_mat;
        }
        mat = mul(GetMatrix(), mat);
        return mat;
    }

    mat = mul(GetMatrix(), mat);
    return mul(mat, GetOwner()->GetOldWorldMatrix());
}

bool ComponentModel::Attach(const std::string_view& model, const std::string_view& node, bool use_model_node_scale)
{
    auto owner = GetOwner();
    {
        if(auto model_comp = owner->GetComponent<ComponentModel>(model))
        {
            target_model_name_ = model;
            target_model_      = model_comp;

            int index = model_comp->GetNodeIndex(node);
            if(index >= 0)
            {
                object_node_index_    = index;
                object_node_name_     = node;
                use_model_node_scale_ = use_model_node_scale;

                SetAttachOtherModel(true);
                return true;
            }
        }
    }
    // モデル名か、ノードが存在しなかった
    return false;
}

void ComponentModel::Detach()
{
    target_model_name_.clear();
    target_model_.reset();

    object_node_name_.clear();
    object_node_index_ = -1;

    SetAttachOtherModel(false);
}

matrix ComponentModel::GetAttachMatrix() const
{
    matrix mat = matrix::identity();

    if(auto target_model = target_model_.lock())
    {
        if(object_node_index_ >= 0)
        {
            mat = target_model->GetNodeMatrix(object_node_index_);
        }
        else
        {
            // ノードがない場合は中心位置へ
            mat = target_model->GetMatrix();
        }
    }

    return mat;
}

CEREAL_CLASS_VERSION(ComponentModel, 2);
