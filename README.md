# Embed Str1ke

> **実行についての注意**：容量の大きいファイルを一部除外しているため、このリポジトリをダウンロードしても、そのままではゲームを実行できません。ソースコードとプログラム説明資料は閲覧できます。

**[作品動画を見る（YouTube）](https://youtu.be/RXt3ME39qSA)**

周囲の物を吸着させて武器にし、敵を突破する3Dアクションゲームです。C++とHLSLを使用し、授業で提供されたフレームワークとDXライブラリを基に、1人で制作しました。

**[プログラム説明資料](docs/プログラム説明資料.pdf)**

## 作品概要

プレイヤーは、危険な計画を阻止する組織の新人として、秘密研究を進める企業の施設へ向かいます。周囲の物を武器に吸着と連結するギミックを使い、敵と戦いながら任務を進めます。

| 項目 | 内容 |
| --- | --- |
| ジャンル | 3Dアクション |
| 開発人数 | 1人 |
| 開発期間 | 2025年6月～2026年1月（8か月）、2026年6月～9月（4か月） |
| 使用言語 | C++、HLSL |
| 開発環境 | Visual Studio 2022、DXライブラリ、Blender、授業フレームワーク |
| 動作環境 | Windows 11 |
| 対応入力 | キーボード・マウス、Xboxコントローラー |

## プログラムで工夫した点

### 戦況に応じて行動を選ぶボスAI

プレイヤーとの距離、双方のHP、攻撃の成功・失敗、被弾回数などを基に、各行動のスコアを求めて行動を選びます。

- 関連コード：[BossAI.cpp](src/Game/Component/Enemy/Boss/BossAI.cpp)、[BossActionManager.cpp](src/Game/Component/Enemy/Boss/BossActionManager.cpp)

### 敵同士の衝突を避ける移動

複数の敵がプレイヤーへ接近する際に、互いの移動を妨げないように回避処理を組み込んでいます。

- 関連コード：[ReciprocalVelocityObstacles.cpp](src/Game/Component/ReciprocalVelocityObstacles.cpp)

### メンタルマップによる敵の分散

Recast Navigationのナビメッシュとウェイポイントを基に、移動先をゾーンに分けて管理します。プレイヤーからの見え方や敵同士の距離を考慮して移動先を選び、敵が一か所に密集するのを防ぎます。

- 関連コード：[MentalMap.cpp](src/Game/Object/Enemy/MentalMap.cpp)、[MentalMapBuilder.cpp](src/Game/Object/Tool/MentalMapBuilder.cpp)

