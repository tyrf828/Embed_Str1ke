#pragma once
//===========================================================================
//! @file AttachWeaponParts.h
//! @brief 武器の物件
//===========================================================================

//---------------------------------------------------------------------------
//! @class AttachWeaponParts
//! @brief 武器の物件
//---------------------------------------------------------------------------
class AttachWeaponParts : public Component
{
public:
    BP_COMPONENT_DECL(AttachWeaponParts, u8"武器の物件");

    //! @brief 初期化
    void Init() override;

    //! @brief 更新
    void Update() override;

    //! @brief GUI表示処理
    void GUI() override;

    //! @brief 終了
    void Exit() override;

    //! @brief パーツを追加
    //! @param obj パーツオブジェクト
    void AddPart(ObjectPtr obj);

    //! @brief 武器のパーツを削除
    //! @param obj パーツオブジェクト
    void RemovePart(ObjectPtr obj)
    {
        if(weapon_parts.size() == 0)
        {
            return;
        }
        int index = 0;
        for(auto part : weapon_parts)
        {
            if(part == obj)
            {
                break;
            }
            index++;
        }
        if(index >= weapon_parts.size())
        {
            return;
        }

        weapon_parts.erase(weapon_parts.begin() + index);
    }

    //! @brief 武器のパーツと以降のを削除
    //! @param obj パーツオブジェクト
    void RemoveParts(ObjectPtr obj);

    //! @brief パーツを取得
    //! @return パーツリスト
    ObjectPtrVec GetParts()
    {
        return weapon_parts;
    }

    //! @brief パーツを取得
    //! @param ptr パーツオブジェクト
    //! @return パーツオブジェクト
    ObjectPtr GetPart(ObjectPtr ptr)
    {
        for(int i = 0; i < weapon_parts.size(); i++)
        {
            if(weapon_parts[i] == ptr)
            {
                return weapon_parts[i];
            }
        }
        return nullptr;
    }

    //! @brief 全てのパーツを削除
    void RemoveAllParts()
    {
        weapon_parts.clear();
    }

private:
    ObjectPtrVec weapon_parts;   //!< 武器パーツリスト
};
