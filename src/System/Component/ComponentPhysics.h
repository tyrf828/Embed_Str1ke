//---------------------------------------------------------------------------
//! @file   ComponentPhysics.h
//! @brief  Physicsコンポーネント
//---------------------------------------------------------------------------
#pragma once

#include <System/Component/Component.h>
#include <System/Component/ComponentTransform.h>

#include <System/Physics/RigidBody.h>
#include <System/Status.h>

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Constraints/SixDOFConstraint.h>

USING_PTR(ComponentPhysics);
USING_PTR(ComponentModel);

//! @brief モデルコンポーネントクラス
class ComponentPhysics final : public Component, public IMatrix<ComponentPhysics>
{
public:
    BP_COMPONENT_DECL(ComponentPhysics, u8"Physics物理機能クラス");

    ComponentPhysics() = default;

    ~ComponentPhysics();

    virtual void Init() override;          //!< 初期化
    virtual void PrePhysics() override;    //!< 更新前
    virtual void PostPhysics() override;   //!< 更新後
    virtual void Draw() override;          //!< 描画
    virtual void Exit() override;          //!< 終了
    virtual void GUI() override;           //!< GUI

    // 移動関数
    void MoveTo(const matrix& mat);
    void MoveToWorldMatrix();

    // 回転軸のロック
    void LockRotateAxis(bool x, bool y, bool z);
    //------------------------------------------------------------------------
    // @name Physics操作
    //------------------------------------------------------------------------
    //@{

    //! Physicsステータスビット
    enum struct PhysicsBit : u64
    {
        Actived = 0,   //!< 物理有効状態
        Initialized,   //!< 初期化終了
        Static,        //!< 固定
        Pause,         //!< 一時停止
        LockRotX,      //!< X軸回転禁止
        LockRotY,      //!< Y軸回転禁止
        LockRotZ,      //!< Z軸回転禁止
    };

    inline void SetPhysicsStatus(PhysicsBit b, bool on)   //!< ステータスの設定
    {
        physics_status_.set(b, on);

        if(b == PhysicsBit::LockRotX || b == PhysicsBit::LockRotY || b == PhysicsBit::LockRotZ)
        {
            LockRotateAxis(GetPhysicsStatus(PhysicsBit::LockRotX), GetPhysicsStatus(PhysicsBit::LockRotY), GetPhysicsStatus(PhysicsBit::LockRotZ));
        }
    }

    inline bool GetPhysicsStatus(PhysicsBit b)   //!< ステータスの取得
    {
        return physics_status_.is(b);
    }

    bool CreateBox(float3 size, float density = shape::default_density);

    bool CreateSphere(float3 center, float radius, float density = shape::default_density);

    bool CreateCapsule(float half_height, float radius, float density = shape::default_density);

    bool CreateCylinder(float half_height, float radius, float density = shape::default_density);

    bool CreateMesh(ComponentModelPtr model, float scale = 1.0);

    std::shared_ptr<physics::RigidBody> GetRigidBody()
    {
        return body_;
    }

    //@}

    bool IsValid() const
    {
        return true;
    }

    //---------------------------------------------------------------------------
    //! @name IMatrixインターフェースの利用するための定義
    //---------------------------------------------------------------------------
    //@{

    matrix& Matrix() override
    {
        return physics_transform_;
    }   //!< マトリクス取得

    const matrix& GetMatrix() const override
    {
        return physics_transform_;
    }   //!< マトリクス取得

    ComponentPhysicsPtr SharedThis()
    {
        return std::dynamic_pointer_cast<ComponentPhysics>(shared_from_this());
    }

    //! @brief ワールドMatrixの取得
    //! @return 他のコンポーネントも含めた位置
    virtual const matrix GetWorldMatrix() const override;

    //! @brief 1フレーム前のワールドMatrixの取得
    //! @return 他のコンポーネントも含めた位置
    virtual const matrix GetOldWorldMatrix() const override;

    void SetPhysicsMatrix(const matrix& world);

    matrix GetPhysicsMatrix() const;
    //@}

    //------------------------------------------------------------------------
    // @name タイプ
    //------------------------------------------------------------------------
    //@{
    static constexpr int CollisionTypeSize = 16;
    enum class CollisionType : u16
    {
        WALL   = 1 << 0,
        GROUND = 1 << 1,
        PLAYER = 1 << 2,
        ENEMY  = 1 << 3,
        WEAPON = 1 << 4,
        ITEM   = 1 << 5,
        CAMERA = 1 << 6,

        ETC   = 1 << 7,
        ETC_0 = ETC,
        ETC_1 = 1 << 8,
        ETC_2 = 1 << 9,
        ETC_3 = 1 << 10,
        ETC_4 = 1 << 11,
        ETC_5 = 1 << 12,
        ETC_6 = 1 << 13,
        ETC_7 = 1 << 14,
        NOHIT = 1 << 15,   //!< これがあると強制的に当たらなくなる

        //ALL = WALL | GROUND | PLAYER | ENEMY | WEAPON | ITEM | CAMERA | ETC_0 | ETC_1 | ETC_2 | ETC_3 | ETC_4 | ETC_5 | ETC_6 | ETC_7;
    };

    //! @brief コリジョンのタイプがどのタイプに当たるかを設定する
    //! @param col 設定するタイプ
    //! @param hit ヒットするタイプの集合体 (NOHITは設定しても当たらない)
    static void SetHitCollision(ComponentPhysics::CollisionType col, u16 hit)
    {
        hit_status_[(int)col] = hit;
    }

    static u16 GetHitCollision(ComponentPhysics::CollisionType col)
    {
        int id = (int)(log2((int)col));
        assert(id >= 0 && id < 16);
        return hit_status_[id];
    }

    void SetOverlapCollision(u16 hit)
    {
        overlap_status_ = hit;
    }

    u16 GetOverlapCollision()
    {
        return overlap_status_;
    }

    void SetCollisionType(ComponentPhysics::CollisionType col)
    {
        type_ = col;
        //if ( body_ )
        //	body_->setLayer( (u16)type_ );
    }

    ComponentPhysics::CollisionType GetCollisionType()
    {
        return type_;
    }

    //@}

    //------------------------------------------------------------------------
    // @name コールバック
    //------------------------------------------------------------------------
    //@{
    //! @brief ヒット情報
    struct HitInfo
    {
        bool hit_                        = false;                //!< ヒットしたか
        ComponentPhysics* collision_     = nullptr;              //!< 自分のコリジョン
        float3 hit_position_             = {0.0f, 0.0f, 0.0f};   //!< 当たった地点
        ComponentPhysics* hit_collision_ = nullptr;              //!< 当たったコリジョン
    };

    //! @brief 当たった時にここに来る (overrideして使用します)
    //! @param hit_info
    virtual void OnHit([[maybe_unused]] const ComponentPhysics::HitInfo& hit_info) {}

    //! @brief オーバーラップ開始
    //! @param hit_info
    virtual void OnBeginOverlap([[maybe_unused]] const ComponentPhysics::HitInfo& hit_info) {}

    //! @brief オーバーラップ終了
    //! @param hit_info
    virtual void OnEndOverlap([[maybe_unused]] const ComponentPhysics::HitInfo& hit_info) {}

    //@}

private:
    //! モデル用のトランスフォーム
    matrix physics_transform_     = matrix::scale(1.0f);
    matrix physics_transform_old_ = matrix::scale(1.0f);

    Status<PhysicsBit> physics_status_;   //!< 状態

    std::shared_ptr<physics::RigidBody> body_;
    matrix world_old_;

    // 重力係数
    float gravity_factor_ = 10.0f;

    // コリジョンのタイプ
    CollisionType type_ = CollisionType::ETC;

    // オーバーラップ情報
    u16 overlap_status_ = 0;

    // コリジョンヒットの情報(共通)
    static u16 hit_status_[CollisionTypeSize];

    // 軸固定コンストレイント
    using SettingsRef = JPH::Ref<JPH::SixDOFConstraintSettings>;

    SettingsRef sSettings;
    JPH::Ref<JPH::SixDOFConstraint> constraint_;

    // デバッグ3D描画用
    int ref_model_ = -1;
    MV1_REF_POLYGONLIST ref_poly_{};

    union ShapeData {
        ShapeData() {}

        ShapeData& operator= (const ShapeData& o) &
        {
            memcpy(this, (void*)&o, sizeof(ShapeData));
            return *this;
        }

        ShapeData(const ShapeData& o)
        {
            *this = o;
        }

        struct
        {
            float3 box_size;
        };

        struct
        {
            float3 sphere_center;
            float sphere_radius;
        };

        struct
        {
            float cylinder_half_height;
            float cylinder_radius;
        };

    } shape_data;

    f32 density_ = 1000.0f;

private:
    //--------------------------------------------------------------------
    //! @name Cereal処理
    //--------------------------------------------------------------------
    //@{
    CEREAL_SAVELOAD(arc, ver)
    {
        arc(cereal::make_nvp("owner", owner_),
            cereal::make_nvp("physics_transform", physics_transform_),
            cereal::make_nvp("physics_status", physics_status_.get()),
            CEREAL_NVP(type_),
            CEREAL_NVP(overlap_status_),
            CEREAL_NVP(gravity_factor_));

        // 現在面倒なので復旧は全員行っている (TODO:別クラスへ移行して一つにするべき)
        for(int i = 0; i < CollisionTypeSize; ++i)
        {
            CEREAL_NVP(hit_status_[i]);
        }

        arc(cereal::make_nvp("Component", cereal::base_class<Component>(this)));
    }

    //@}
};

CEREAL_REGISTER_TYPE(ComponentPhysics)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Component, ComponentPhysics)
